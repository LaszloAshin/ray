#pragma once

#include "VectorMath.h"
#include "BaseObject.h"

#include <cassert>
#include <cmath>

class Spheroid final : public BaseObject {
	float rxz;
	float ry;
	float rxz2;
	float ry2;
	float ry4;
	float rxz2ry2;
	float rxz2ry4;
	float rxz4ry2;

public:
	Spheroid(const Vec3f &p, int material, float rxz, float ry)
	: BaseObject(p, material)
	, rxz{rxz}
	, ry{ry}
	, rxz2{rxz * rxz}
	, ry2{ry * ry}
	, ry4{ry2 * ry2}
	, rxz2ry2{rxz2 * ry2}
	, rxz2ry4{rxz2ry2 * ry2}
	, rxz4ry2{rxz2ry2 * rxz2}
	{
		assert(fabs(rxz) > EPSILON);
		assert(fabs(ry) > EPSILON);
	}

	std::tuple<float, Vec3f> intersect(const Ray &r) const {
		const Vec3f s{r.s - pos};
		const Vec3f d{r.d};
		const float m1 = d.x * s.y - d.y * s.x;
		const float m2 = d.x * s.z - d.z * s.x;
		const float m3 = d.y * s.z - d.z * s.y;
		const float Dlhs = rxz2ry4 * (d.x * d.x + d.z * d.z) + rxz4ry2 * d.y * d.y;
		const float Drhs = ry4 * m2 * m2 + rxz2ry2 * (m1 * m1 + m3 * m3);
		if (Dlhs < Drhs) return std::make_tuple(-1.0f, Vec3f{});

		const float Dsqrt = sqrtf(Dlhs - Drhs);
		const float a = (d.x * d.x + d.z * d.z) * ry2 + d.y * d.y * rxz2;
		const float minusb = -((s.x * d.x + s.z * d.z) * ry2 + s.y * d.y * rxz2);
		float t1 = minusb + Dsqrt;
		if (t1 < 0.0f) return std::make_tuple(-1.0f, Vec3f{});
		float t0 = minusb - Dsqrt;
		if (t0 < 0.0f) t0 = t1;

		t0 /= a;

		const Vec3f mp = r.s + r.d * t0;
		Vec3f N = (mp - this->pos) * 2.0f;
		N.x /= rxz2;
		N.y /= ry2;
		N.z /= rxz2;
		N = N.norm();
		return std::make_tuple(t0, N);
	}

	Color texelAt(const Vec3f &mp) const override {
		Vec3f p = mp - pos;
		float v = (float)M_1_PI * acosf(p.z / rxz);
	//	float u = 0.5f * M_1_PI * acosf(p.x / (radius.x * sinf(M_PI * v)));
		float u = 0.5f * (float)M_1_PI * (atan2f(p.y / ry, p.x / rxz) + (float)M_PI);
		int x = (int)(u * 16.0f);
		int y = (int)(v * 16.0f);
		return ((x ^ y) & 1) ? Color(0.0f, 0.0f, 0.0f) : Color(1.0f, 1.0f, 1.0f);
	}
};
