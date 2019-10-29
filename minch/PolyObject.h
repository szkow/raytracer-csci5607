#ifndef POLY_OBJECT_H
#define POLY_OBJECT_H

#include <vector>
#include "SceneObject.h"

class PolyObject : public LitSceneObject {
 public:
  PolyObject();
  PolyObject(const std::vector<Vector>& verts,
             const std::vector<unsigned int>& indices,
             const std::vector<Vector>& normals,
             const std::vector<unsigned int>& normal_indices,
             const std::vector<Vector>& uvs,
             const std::vector<unsigned int>& uv_indices,
             const Vector& k_scalars, const Vector& diffuse_color,
             const Vector& specular_color, float specularity, float opacity,
             float eta);
  PolyObject(const std::vector<Vector>& verts,
             const std::vector<unsigned int>& indices,
             const std::vector<Vector>& normals,
             const std::vector<unsigned int>& normal_indices,
             const std::vector<Vector>& uvs,
             const std::vector<unsigned int>& uv_indices,
             const Vector& k_scalars, const Vector& specular_color,
             float specularity, float opacity, float eta, const char* texture_map);
  ~PolyObject();

  virtual Vector Normal(const Vector& point);
  virtual SceneObject* RayIntersects(const Vector& dir, const Vector& origin,
                                        float* t1, float* t2);

 protected:
  class Triangle;
  std::vector<Triangle> tris_;
  std::vector<Vector> verts_;
  std::vector<Vector> normals_;
  std::vector<Vector> tex_coords_;

  virtual Vector TextureCoordinatesAt(const Vector& point);
};

class PolyObject::Triangle : public SimpleSceneObject {
 public:
  Triangle(ShadingData* daddy_, Vector* v0, Vector* v1, Vector* v2,
           Vector* n0 = nullptr, Vector* n1 = nullptr, Vector* n2 = nullptr,
           Vector* uv0 = nullptr, Vector* uv1 = nullptr, Vector* uv2 = nullptr);

  static void MakeTris(ShadingData* lighting_daddy, std::vector<Triangle>* tris,
                       std::vector<Vector>& verts,
                       const std::vector<unsigned int>& indices,
                       std::vector<Vector>& normals,
                       const std::vector<unsigned int>& normal_indices,
                       std::vector<Vector>& tex_coords,
                       const std::vector<unsigned int>& tex_coord_indices);
  virtual Vector Normal(const Vector& point);
  virtual SceneObject* RayIntersects(const Vector& dir, const Vector& origin,
                                        float* t1, float* t2);
  void GetAreaticCoordinates(const Vector& point, float* alpha, float* beta,
                             float* gamma);


 protected:
  Vector* v0_;
  Vector* v1_;
  Vector* v2_;
  Vector* n0_;
  Vector* n1_;
  Vector* n2_;
  Vector* uv0_;
  Vector* uv1_;
  Vector* uv2_;

  Vector normal_;
  Vector e1_;
  Vector e2_;
  Vector e3_;
  float area_;

  virtual Vector TextureCoordinatesAt(const Vector& point);
};

#endif
