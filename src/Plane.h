#pragma once

#include "BaseObject.h"

#include <tuple>

class Plane final : public BaseObject {
	float d;

public:
	constexpr Plane(const Vec3f &n, float d0, int material)
	: BaseObject{n, material, cid<Plane>}
	, d{d0}
	{}

	Plane(const Vec3f &r1, const Vec3f &r2, const Vec3f &r3, int material)
	: BaseObject{r1, material, cid<Plane>}
	{
		Vec3f diff1 = r2 - r1;
		Vec3f diff2 = r3 - r1;
		pos = Vec3f(diff1 % diff2).norm();
		float l = pos * pos;
		d = (l > EPSILON) ? ((pos * r1) / l) : 0.0f;
	}

	float intersect(const Ray &r) const {
		const float f = r.d.x * pos.x + r.d.y * pos.y + r.d.z * pos.z;
		if (f > -EPSILON) return -1.0f;
		const float x = pos.x * d - r.s.x;
		const float y = pos.y * d - r.s.y;
		const float z = pos.z * d - r.s.z;
		return (x * pos.x + y * pos.y + z * pos.z) / f;
	}

	std::tuple<Vec3f, Color> computeIntersectionDetails(const Vec3f&) const {
		return std::make_tuple(pos, Color::white());
	}
};
