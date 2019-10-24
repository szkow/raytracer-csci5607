#include "PointLight.h"



PointLight::PointLight(const Vector& position, const Vector& color, const Vector& attenuation) : Light(position, color), attenuation_(attenuation)
{
}


PointLight::~PointLight()
{
}

Vector PointLight::PhongDiffuseSpecular(const Vector & point, const Vector & normal, const Vector & view, SceneObject* object)
{
	// Calculate lighting as normal
	Vector regular_phong = Light::PhongDiffuseSpecular(point, normal, view, object);

	// If we don't have an attenuated light, don't do anything
	if (attenuation_ == Vector(1, 0, 0)) return regular_phong;

	// If we do need attenuation, calculate it
	float d = DistanceTo(point);
	float attenuation = 1.0f / (attenuation_ * Vector(1.0f, d, d * d));
	return (attenuation > 0.0f) ? attenuation * regular_phong : regular_phong;
}

Vector PointLight::GetLightDirection(const Vector& point)
{
	return (position_ - point).Normed();
}

float PointLight::DistanceTo(const Vector & point)
{
	return (position_ - point).Length();
}
