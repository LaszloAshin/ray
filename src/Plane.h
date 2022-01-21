#pragma once

#include "BaseObject.h"

#include <cmath>

class Plane final : public BaseObject {
	float d;

public:
	Plane(const Vec3f &n, float d0, int material) :
		BaseObject(n.norm(), material), d(d0)
	{
	}

	Plane(const Vec3f &r1, const Vec3f &r2, const Vec3f &r3, int material)
	: BaseObject(r1, material)
	{
		Vec3f diff1 = r2 - r1;
		Vec3f diff2 = r3 - r1;
		pos = Vec3f(diff1 % diff2).norm();
		float l = pos * pos;
		d = (l > EPSILON) ? ((pos * r1) / l) : 0.0f;
	}

	~Plane() {}

	std::tuple<float, Vec3f> intersect(const Ray &ray) const {
		const float f = ray.d * pos;
		return std::make_tuple((f > -EPSILON) ? -1.0f : (pos * d - ray.s) * pos / f, pos);
	}
};