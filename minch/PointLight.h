#ifndef POINT_LIGHT_H 
#define POINT_LIGHT_H

#include "Light.h"

class PointLight : public Light
{
protected:
	Vector attenuation_;
public:
	PointLight(const Vector& position, const Vector& color, const Vector& attenuation = Vector(1, 0, 0));
	~PointLight();

	virtual Vector PhongDiffuseSpecular(const Vector & point, const Vector & normal, const Vector & view, SceneObject* object);
	virtual Vector GetLightDirection(const Vector& point);
	virtual float DistanceTo(const Vector& point);

};

#endif
