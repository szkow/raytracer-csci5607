#ifndef LIGHT_H
#define LIGHT_H

#include "SceneObject.h"
#include "Vector.h"

class Light
{
protected:
	Vector position_;
	Vector color_;
public:
	Light(Vector position, Vector color);
	virtual ~Light() {}
	virtual Vector PhongDiffuseSpecular(const Vector& point, const Vector& normal, const Vector& view, SceneObject::LightingParcel& light_data);
	virtual Vector GetLightDirection(const Vector& point) = 0;
	virtual float DistanceTo(const Vector& point) = 0;
};

#endif