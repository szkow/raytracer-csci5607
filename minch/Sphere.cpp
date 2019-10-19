#include "Sphere.h"
#include "Vector.h"
#include <cmath>


Sphere::Sphere(const Vector& position = Vector::Zero(), float radius = 1.0f,
               const Vector& material_scalars = Vector::One(),
               const Vector& diffuse_color = Vector::One(),
               const Vector& specular_color = Vector::One(),
               int specularity = 10)
    : LitSceneObject(position, material_scalars, diffuse_color, specular_color,
                     specularity),
      radius_(radius) {}

Sphere::Sphere(const Vector& position, float radius,
               const Vector& material_scalars, const Vector& specular_color,
               int specularity, const char* texture_map)
    : LitSceneObject(position, material_scalars, specular_color, specularity,
                     texture_map),
      radius_(radius) {}

Sphere::~Sphere()
{
}

SceneObject* Sphere::RayIntersects(const Vector& dir, const Vector& origin, float* t1, float* t2)
{
	Vector n = origin - position_;
	float dir_dot_n = dir * n;

	float a = dir * dir;
	float b = 2.0f * dir_dot_n;
	float c = n * n - radius_ * radius_;
	float discriminant = b * b - 4.0f * a * c;
	if (discriminant < 0.0f)	return nullptr;

	*t1 = (-b - sqrtf(discriminant)) / (2.0f * a);
	*t2 = (-b + sqrtf(discriminant)) / (2.0f * a);
	return this;
}

Vector Sphere::Normal(const Vector & point)
{
	return (point - position_).Normed();
}

SceneObject::LightingParcel Sphere::LightData(const Vector& point) {
  if (!textured_) {
    return light_data_;
  }
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

  light_data_.diffuse_color = texture_map_.ColorAt(u, v);
  return light_data_;
}
