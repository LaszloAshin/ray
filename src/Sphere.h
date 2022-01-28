#pragma once

#include "BaseObject.h"
#include "Geometry.h"
#include "MyMath.h"

#include <cassert>
#include <tuple>

class Sphere final : public BaseObject {
	float r;
	float r2;

public:
	Sphere(const Vec3f &p, int material, float r)
	: BaseObject(p, material)
	, r{r}
	, r2{r * r}
	{
		assert(fabs(r) > EPSILON);
	}

	std::tuple<float, Vec3f> intersect(const Ray &ray) const {
		const Vec3f s{ray.s - pos};
		const Vec3f d{ray.d};
		const float a = d.x * d.x + d.z * d.z + d.y * d.y;
		const float m1 = d.x * s.y - d.y * s.x;
		const float m2 = d.x * s.z - d.z * s.x;
		const float m3 = d.y * s.z - d.z * s.y;
		const float Dlhs = r2 * a;
		const float Drhs = m1 * m1 + m2 * m2 + m3 * m3;
		if (Dlhs < Drhs) return std::make_tuple(-1.0f, Vec3f{});

		const float Dsqrt = mysqrtf(Dlhs - Drhs);
		const float minusb = -(s.x * d.x + s.y * d.y + s.z * d.z);
		float t1 = minusb + Dsqrt;
		if (t1 < 0.0f) return std::make_tuple(-1.0f, Vec3f{});
		float t0 = minusb - Dsqrt;
		if (t0 < 0.0f) t0 = t1;

		t0 /= a;

		const Vec3f mp = ray.s + ray.d * t0;
		Vec3f N = (mp - this->pos) * 2.0f;
		N.x /= r2;
		N.y /= r2;
		N.z /= r2;
		N = N.norm();
		return std::make_tuple(t0, N);
	}

	Color texelAt(const Vec3f &mp) const override {
		Vec3f p = mp - pos;
		float v = (float)M_1_PI * myacosf(p.z / r);
		float u = 0.5f * (float)M_1_PI * (myatan2f(p.y / r, p.x / r) + (float)M_PI);
		int x = (int)(u * 16.0f);
		int y = (int)(v * 16.0f);
		return ((x ^ y) & 1) ? Color(0.0f, 0.0f, 0.0f) : Color(1.0f, 1.0f, 1.0f);
	}
};
