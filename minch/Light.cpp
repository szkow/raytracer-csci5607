#include "Light.h"
#include <math.h>

Light::Light(Vector position = Vector::Zero(), Vector color = Vector::Zero()) : position_(position), color_(color) {}

Vector Light::PhongDiffuseSpecular(const Vector & point, const Vector & normal, const Vector & view, SceneObject* object)
{
	Vector light_direction = GetLightDirection(point);
	Vector halfway = (light_direction + view).Normed();
	float n_dot_l = normal * light_direction;
	float n_dot_h = normal * halfway;

  Vector k = object->KScalars();
  Vector diffuse = object->DiffuseColor(point);
  Vector specular = object->SpecularColor();
  float specularity = object->Specularity();

	Vector diffuse_part = k[1] * diffuse * fmaxf(0.0f, n_dot_l);
	Vector specular_part = k[2] * specular * powf(fmaxf(0.0f, n_dot_h), specularity);
	Vector sum = diffuse_part + specular_part;

	return Vector(sum[0] * color_[0], sum[1] * color_[1], sum[2] * color_[2]);
}
