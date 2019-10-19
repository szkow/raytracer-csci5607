#include "DirectionalLight.h"
#include <math.h>

DirectionalLight::DirectionalLight(const Vector& direction, const Vector& color) : Light(Vector::Zero(), color), direction_(direction.Normed())
{
}


DirectionalLight::~DirectionalLight()
{
}

Vector DirectionalLight::GetLightDirection(const Vector & point)
{
	return -1.0f * direction_;
}

float DirectionalLight::DistanceTo(const Vector & point)
{
	return INFINITY;
}
