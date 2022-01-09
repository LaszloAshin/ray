#ifndef _VECTORMATH_H
#define _VECTORMATH_H	1

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <cmath>

struct Vector {
	float x, y, z;

	Vector() : x{}, y{}, z{} {}

	Vector(float x0, float y0, float z0) : x{x0}, y{y0}, z{z0} {}

	Vector&	operator+=(const Vector &rhs) {
		x += rhs.x, y += rhs.y, z += rhs.z;
		return *this;
	}

	friend Vector operator+(Vector lhs, const Vector &rhs) {
		return lhs += rhs;
	}

	Vector& operator-=(const Vector &rhs) {
		x -= rhs.x, y -= rhs.y, z -= rhs.z;
		return *this;
	}

	friend Vector operator-(Vector lhs, const Vector& rhs) {
		return lhs -= rhs;
	}

	Vector& operator*=(float rhs) {
		x *= rhs, y *= rhs, z *= rhs;
		return *this;
	}

	friend Vector operator*(Vector lhs, float rhs) {
		return lhs *= rhs;
	}

	friend float operator*(const Vector &lhs, const Vector &rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	float length() const { return sqrtf(x * x + y * y + z * z); }

	Vector norm() const {
		// maybe http://rrrola.wz.cz/inv_sqrt.html ?
		const float len = length();
		return (len > EPSILON) ? (*this * (1.0f / len)) : Vector{};
	}

	friend Vector operator%(const Vector &lhs, const Vector &rhs) {
		return {
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		};
	}
};

struct Ray {
	Vector s, d;

	Ray(const Vector &source, const Vector &dir) :
		s(source), d(dir)
	{
	}
};

#endif /* _VECTORMATH_H */
