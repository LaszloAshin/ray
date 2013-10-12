#include <cmath>
#include "VectorMath.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

const Vector Vector::null(0.0f, 0.0f, 0.0f);

Vector&
Vector::operator+=(const Vector &v) // not const
{
	x += v.x, y += v.y, z += v.z;
	return *this;
}

Vector
Vector::operator+(const Vector &v) const
{
	return Vector(x + v.x, y + v.y, z + v.z);
}

Vector&
Vector::operator-=(const Vector &v) // not const
{
	x -= v.x, y -= v.y, z -= v.z;
	return *this;
}

Vector
Vector::operator-(const Vector &v) const
{
	return Vector(x - v.x, y - v.y, z - v.z);
}

Vector&
Vector::operator*=(float a) // not const
{
	x *= a, y *= a, z *= a;
	return *this;
}

Vector
Vector::operator*(float a) const
{
	return Vector(a * x, a * y, a * z);
}

float
Vector::operator*(const Vector &v) const
{
	return x * v.x + y * v.y + z * v.z;
}

float
Vector::length() const
{
	return sqrtf(x * x + y * y + z * z);
}

Vector
Vector::norm() const
{
	float l = length();
	if (l < EPSILON) return Vector();
	l = 1.0f / l;
	return Vector(x * l, y * l, z * l);
}

Vector
Vector::operator%(const Vector &v) const
{
	return Vector(
		y * v.z - z * v.y,
		z * v.x - x * v.z,
		x * v.y - y * v.x
	);
}

float
Vector2D::operator%(const Vector2D &v) const
{
	return x * v.y - y * v.x;
}

Vector2D
Vector2D::operator-(const Vector2D &v) const
{
	return Vector2D(x - v.x, y - v.y);
}
