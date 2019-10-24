#include "PolyObject.h"
#include <cmath>

#define TRIANGLE_EPSILON 0.0001f
#define TRIANGLE_SELF_INTERSECT_EPSILON 0.001f

PolyObject::PolyObject() : LitSceneObject() {}

PolyObject::PolyObject(const std::vector<Vector>& verts,
                       const std::vector<unsigned int>& indices,
                       const std::vector<Vector>& normals,
                       const std::vector<unsigned int>& normal_indices,
                       const std::vector<Vector>& uvs,
                       const std::vector<unsigned int>& uv_indices,
                       const Vector& k_scalars,
                       const Vector& diffuse_color,
                       const Vector& specular_color, float specularity, float opacity, float eta)
    : LitSceneObject(Vector::Zero(), k_scalars, diffuse_color,
                     specular_color, specularity, opacity, eta),
      verts_(verts),
      normals_(normals),
      tex_coords_(uvs) {
  Triangle::MakeTris(&light_data_, &tris_, verts_, indices, normals_, normal_indices,
                     tex_coords_, uv_indices);
}

PolyObject::PolyObject(const std::vector<Vector>& verts,
                       const std::vector<unsigned int>& indices,
                       const std::vector<Vector>& normals,
                       const std::vector<unsigned int>& normal_indices,
                       const std::vector<Vector>& uvs,
                       const std::vector<unsigned int>& uv_indices,
                       const Vector& k_scalars,
                       const Vector& specular_color, float specularity, float opacity, float eta,
                       const char* texture_map)
    : LitSceneObject(Vector::Zero(), k_scalars, specular_color,
                     specularity, texture_map, opacity, eta),
      verts_(verts),
      normals_(normals),
      tex_coords_(uvs) {
  Triangle::MakeTris(&light_data_, &tris_, verts_, indices, normals_, normal_indices,
                     tex_coords_, uv_indices);
}

PolyObject::~PolyObject() {}

Vector PolyObject::Normal(const Vector& point) { return Vector::Zero(); }

SceneObject* PolyObject::RayIntersects(const Vector& dir, const Vector& origin,
                                       float* t1, float* t2) {
  float min_distance = INFINITY;
  SceneObject* min_tri = nullptr;
  for (std::vector<Triangle>::iterator i = tris_.begin(); i < tris_.end();
       ++i) {
    float local_t1 = -1.0f, local_t2 = -1.0f;

    SceneObject* hit_triangle =
        i->RayIntersects(dir, origin, &local_t1, &local_t2);
    if (local_t1 >= TRIANGLE_SELF_INTERSECT_EPSILON && hit_triangle != nullptr &&
        local_t1 < min_distance) {  // Depth test
      min_distance = local_t1;
      min_tri = hit_triangle;
    }
  }

  if (min_tri != nullptr) {
    *t1 = min_distance;
    *t2 = -1.0f;
    return min_tri;
  }

  *t1 = -1.0f;
  *t2 = -1.0f;
  return nullptr;
}

Vector PolyObject::TextureCoordinatesAt(const Vector& point) {
  return point;
}

PolyObject::Triangle::Triangle(ShadingData* lighting_daddy, Vector* v0,
                               Vector* v1, Vector* v2, Vector* n0, Vector* n1,
                               Vector* n2, Vector* uv0, Vector* uv1,
                               Vector* uv2)
    : SimpleSceneObject(lighting_daddy, (*v0 + *v1 + *v2) / 3.0f),
      v0_(v0),
      v1_(v1),
      v2_(v2),
      n0_(n0),
      n1_(n1),
      n2_(n2),
      uv0_(uv0),
      uv1_(uv1),
      uv2_(uv2) {
  e1_ = *v1 - *v0;
  e2_ = *v2 - *v0;
  e3_ = *v2 - *v1;
  area_ = 0.5f * e1_.Cross(e2_).Length();
  normal_ = (*v1_ - *v0_).Cross(*v2_ - *v0_).Normed();
}

void PolyObject::Triangle::MakeTris(
    ShadingData* lighting_daddy, std::vector<Triangle>* tris,
    std::vector<Vector>& verts, const std::vector<unsigned int>& indices,
    std::vector<Vector>& normals,
    const std::vector<unsigned int>& normal_indices,
    std::vector<Vector>& tex_coords,
    const std::vector<unsigned int>& tex_coord_indices) {
  unsigned int n = indices.size();
  for (unsigned int i = 0; i < n; i += 3) {
    Vector* v0 = &verts[indices[i + 0u]];
    Vector* v1 = &verts[indices[i + 1u]];
    Vector* v2 = &verts[indices[i + 2u]];

    tris->push_back(Triangle(lighting_daddy, v0, v1, v2));
  }

  if (normal_indices.size() == n) {
    for (unsigned int i = 0; i < n; i += 3) {
      (*tris)[i / 3].n0_ = &normals[normal_indices[i + 0u]];
      (*tris)[i / 3].n1_ = &normals[normal_indices[i + 1u]];
      (*tris)[i / 3].n2_ = &normals[normal_indices[i + 2u]];
    }
  }

  if (tex_coord_indices.size() == n) {
    for (unsigned int i = 0; i < n; i += 3) {
      (*tris)[i / 3].uv0_ = &tex_coords[tex_coord_indices[i + 0u]];
      (*tris)[i / 3].uv1_ = &tex_coords[tex_coord_indices[i + 1u]];
      (*tris)[i / 3].uv2_ = &tex_coords[tex_coord_indices[i + 2u]];
    }
  }
}

Vector PolyObject::Triangle::Normal(const Vector& point) {
  if (n0_ == nullptr) {
    return normal_;
  }

  float a, b, c;
  GetAreaticCoordinates(point, &a, &b, &c);
  return (a * *n0_ + b * *n1_ + c * *n2_).Normed();
}

SceneObject* PolyObject::Triangle::RayIntersects(const Vector& dir,
                                                 const Vector& origin,
                                                 float* t1, float* t2) {
  // Find plane intersection
  float denominator = normal_ * dir;
  if (std::abs(denominator) <= TRIANGLE_EPSILON) {
    return nullptr;
  }
  float numerator = (*v0_ - origin) * normal_;
  float t_plane = numerator / denominator;
  if (t_plane < TRIANGLE_EPSILON) {
    return nullptr;
  }

  // Find barycentric coordinates as area of triangle
  float alpha, beta, gamma;
  GetAreaticCoordinates(origin + dir * t_plane, &alpha, &beta, &gamma);

  // Check if barycentric coordinates dictate p is in the triangle
  if (std::abs(alpha + beta + gamma - area_) < TRIANGLE_EPSILON) {
    *t1 = t_plane;
    *t2 = -1.0f;
    return this;
  }

  return nullptr;
}

void PolyObject::Triangle::GetAreaticCoordinates(const Vector& point,
                                                 float* alpha, float* beta,
                                                 float* gamma) {
  // Define plane intersection relative to v0_
  Vector v0_to_p = point - *v0_;

  // Define plane intersection relative to v1_
  Vector v1_to_p = point - *v1_;

  // Find barycentric coordinates as area of triangle
  *alpha = 0.5f * v1_to_p.Cross(e3_).Length();
  *beta = 0.5f * v0_to_p.Cross(e2_).Length();
  *gamma = 0.5f * v0_to_p.Cross(e1_).Length();
}

Vector PolyObject::Triangle::TextureCoordinatesAt(const Vector& point) {
  float a, b, c;
  GetAreaticCoordinates(point, &a, &b, &c);

  return (a * *uv0_ + b * *uv1_ + c * *uv2_) / (3.0f * area_);
}
