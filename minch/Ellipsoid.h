#ifndef ELLIPSOID_H
#define ELlIPSOID_H

#include "SceneObject.h"

class Ellipsoid : public LitSceneObject
{
protected:
	Vector coefficients_;
	Vector squared_coefficients_;
public:
	Ellipsoid(const Vector& position, const Vector& coefficients, const Vector& k_scalars, const Vector& diffuse_color, const Vector& specular_color, int specularity);
	~Ellipsoid();

	SceneObject* RayIntersects(const Vector & dir, const Vector & origin, float* t1, float* t2);
	Vector Normal(const Vector& point);
};

#endif