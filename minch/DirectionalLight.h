#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include "Light.h"

class DirectionalLight :
	public Light
{
private:
	Vector direction_;
public:
	DirectionalLight(const Vector& direction, const Vector& color);
	~DirectionalLight();

	virtual Vector GetLightDirection(const Vector& point);
	virtual float DistanceTo(const Vector& point);
};

#endif

