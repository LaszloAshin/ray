#pragma once

#include "MyMath.h"

#include "config.h"

struct Vec3f {
	float x{}, y{}, z{};

	constexpr Vec3f() = default;
	constexpr Vec3f(float x, float y, float z) : x{x}, y{y}, z{z} {}

	Vec3f& operator+=(const Vec3f &rhs) {
		x += rhs.x, y += rhs.y, z += rhs.z;
		return *this;
	}

	friend Vec3f operator+(Vec3f lhs, const Vec3f &rhs) {
		return lhs += rhs;
	}

	Vec3f& operator-=(const Vec3f &rhs) {
		x -= rhs.x, y -= rhs.y, z -= rhs.z;
		return *this;
	}

	friend Vec3f operator-(Vec3f lhs, const Vec3f& rhs) {
		return lhs -= rhs;
	}

	Vec3f& operator*=(float rhs) {
		x *= rhs, y *= rhs, z *= rhs;
		return *this;
	}

	friend Vec3f operator*(Vec3f lhs, float rhs) {
		return lhs *= rhs;
	}

	friend float operator*(const Vec3f &lhs, const Vec3f &rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	float length() const { return mysqrtf(x * x + y * y + z * z); }

	Vec3f norm() const {
		return *this * (1.0f / length());
	}

	friend Vec3f operator%(const Vec3f &lhs, const Vec3f &rhs) {
		return {
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x
		};
	}
};

struct Ray {
	Vec3f s, d;

	Ray(const Vec3f &source, const Vec3f &dir) :
		s(source), d(dir)
	{
	}
};
