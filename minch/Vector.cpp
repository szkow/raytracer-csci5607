#include "Vector.h"
#include <cmath>
#include <cstdio>
#include <random>

Vector::Vector(float x, float y, float z)
{
	x_ = x;
	y_ = y;
	z_ = z;
	arr_[0] = x;
	arr_[1] = y;
	arr_[2] = z;
}

Vector::Vector(const float components[3]) : Vector(components[0], components[1], components[2]) {}


Vector::~Vector()
{
}

const Vector Vector::Zero()
{
	return Vector(0.0f, 0.0f, 0.0f);
}

const Vector Vector::One()
{
	return Vector(1.0f, 1.0f, 1.0f);
}

Vector Vector::Random()
{
	return Vector((float)rand(), (float)rand(), (float)rand()).Normed();
}

float Vector::Length() const
{
	return sqrt(x_ * x_ + y_ * y_ + z_ * z_); }

Vector Vector::ProjectOnto(const Vector& other) {
  return ((*this * other) * other);
}

Vector Vector::Normed() const
{
	float len = Length();
	return Vector(x_ / len, y_ / len, z_ / len);
}

Vector Vector::Cross(const Vector & other) const
{
	return Vector(y_ * other.z_ - z_ * other.y_, z_ * other.x_ - x_ * other.z_, x_ * other.y_ - y_ * other.x_);
}

Vector Vector::Clamped(float x_clamp, float y_clamp, float z_clamp)
{
	
	return Vector(fminf(x_clamp, x_), fminf(y_clamp, y_), fminf(z_clamp, z_));
}

Vector Vector::Clamped(float c)
{
	return Clamped(c, c, c);
}

Vector Vector::operator/(const float scalar) const
{
	return Vector(x_ / scalar, y_ / scalar, z_ / scalar);
}

float Vector::operator*(const Vector& other) const {
	return x_ * other.x_ + y_ * other.y_ + z_ * other.z_;
}
Vector Vector::operator+(const Vector& other) const {
	return Vector(x_ + other.x_, y_ + other.y_, z_ + other.z_);
}

Vector Vector::operator-(const Vector & other) const
{
	return Vector(x_ - other.x_, y_ - other.y_, z_ - other.z_);
}

Vector Vector::operator-() const {
  return Vector(-x_, -y_, -z_);
}

float Vector::operator[](const unsigned int i) const
{
	return arr_[i];
}

bool Vector::operator==(const Vector & other) const
{
	return x_ == other[0] && y_ == other[1] && z_ == other[2];
}

Vector operator*(const float scalar, const Vector& v) {
	return Vector(v.x_ * scalar, v.y_ * scalar, v.z_ * scalar);
}

Vector operator*(const Vector& v, const float scalar) {
	return scalar * v;
}
