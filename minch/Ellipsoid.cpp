#include "Ellipsoid.h"
#include "cmath"

Ellipsoid::Ellipsoid(const Vector & position = Vector::Zero(), const Vector & coefficients = Vector::One(), const Vector & material_scalars = Vector::One(), const Vector & diffuse_color = Vector::One(), const Vector & specular_color = Vector::One(), int specularity = 10) :
	LitSceneObject(position, material_scalars, diffuse_color, specular_color, specularity), coefficients_(coefficients), squared_coefficients_(Vector(coefficients[0] * coefficients[0], coefficients[1] * coefficients[1], coefficients[2] * coefficients[2]))
{
}

Ellipsoid::~Ellipsoid()
{
}

SceneObject* Ellipsoid::RayIntersects(const Vector & dir, const Vector & origin, float * t1, float * t2)
{
	Vector A2d = Vector(squared_coefficients_[0] * dir[0], squared_coefficients_[1] * dir[1], squared_coefficients_[2] * dir[2]);
	Vector Ad = Vector(coefficients_[0] * dir[0], coefficients_[1] * dir[1], coefficients_[2] * dir[2]);
	Vector n = origin - position_;
	Vector An = Vector(coefficients_[0] * n[0], coefficients_[1] * n[1], coefficients_[2] * n[2]);
	
	float a = Ad * Ad;
	float b = 2.0f * n * A2d;
	float c = An * An - 1;
	float discriminant = b * b - 4.0f * a * c;
	
	if (discriminant < 0) return nullptr;

	*t1 = -b + sqrt(discriminant) / (2.0f * a);
	*t2 = -b - sqrt(discriminant) / (2.0f * a);
	return this;
}

Vector Ellipsoid::
Normal(const Vector & point)
{
	Vector diff_point = point - position_;
	return Vector(diff_point[0] / squared_coefficients_[0], diff_point[1] / squared_coefficients_[1], diff_point[2] / squared_coefficients_[2]).Normed();
}
