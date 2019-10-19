#include "Light.h"
#include <math.h>

Light::Light(Vector position = Vector::Zero(), Vector color = Vector::Zero()) : position_(position), color_(color) {}

Vector Light::PhongDiffuseSpecular(const Vector & point, const Vector & normal, const Vector & view, SceneObject::LightingParcel& light_data)
{
	Vector light_direction = GetLightDirection(point);
	Vector halfway = (light_direction + view).Normed();
	float n_dot_l = normal * light_direction;
	float n_dot_h = normal * halfway;

	Vector diffuse_part = light_data.material_scalars[1] * light_data.diffuse_color * fmaxf(0.0f, n_dot_l);
	Vector specular_part = light_data.material_scalars[2] * light_data.specular_color * powf(fmaxf(0.0f, n_dot_h), light_data.specularity);
	Vector sum = diffuse_part + specular_part;

	return Vector(sum[0] * color_[0], sum[1] * color_[1], sum[2] * color_[2]);
}
