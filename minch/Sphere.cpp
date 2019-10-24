#include "Sphere.h"
#include <cmath>
#include "Vector.h"

Vector Sphere::TextureCoordinatesAt(const Vector& point) {
  Vector n = Normal(point);

  float phi = acos(n[0]);
  float theta = atan2(n[2], n[1]);
  float u;
  if (theta < 0.0f) {
    u = (theta + 2.0f * (float)M_PI) / (2.0f * (float)M_PI);
  } else {
    u = theta / (2.0f * (float)M_PI);
  }
  float v = phi / (float)M_PI;
  
  return Vector(u, v, 0.0f);
}

Sphere::Sphere(const Vector& position = Vector::Zero(), float radius = 1.0f,
               const Vector& k_scalars = Vector::One(),
               const Vector& diffuse_color = Vector::One(),
               const Vector& specular_color = Vector::One(),
               float specularity = 10.0f, float opacity = 1.0f, float eta = 0.0f)
    : LitSceneObject(position, k_scalars, diffuse_color, specular_color,
                     specularity, opacity, eta),
      radius_(radius) {}

Sphere::Sphere(const Vector& position, float radius, const Vector& k_scalars,
               const Vector& specular_color, float specularity, float opacity,
               float eta, const char* texture_map)
    : LitSceneObject(position, k_scalars, specular_color, specularity,
                     texture_map, opacity, eta),
      radius_(radius) {}

Sphere::~Sphere() {}

SceneObject* Sphere::RayIntersects(const Vector& dir, const Vector& origin,
                                   float* t1, float* t2) {
  Vector n = origin - position_;
  float dir_dot_n = dir * n;

  float a = dir * dir;
  float b = 2.0f * dir_dot_n;
  float c = n * n - radius_ * radius_;
  float discriminant = b * b - 4.0f * a * c;
  if (discriminant < 0.0f) return nullptr;

  *t1 = (-b - sqrtf(discriminant)) / (2.0f * a);
  *t2 = (-b + sqrtf(discriminant)) / (2.0f * a);
  return this;
}

Vector Sphere::Normal(const Vector& point) {
  return (point - position_).Normed();
}
