#pragma once

#include "BaseObject.h"

#include <tuple>

class Plane final : public BaseObject {
	float d;

	static std::tuple<Vec3f, Color> cid(const BaseObject* o, const Vec3f& mp) {
		return static_cast<const Plane*>(o)->computeIntersectionDetails(mp);
	}

public:
	Plane(const Vec3f &n, float d0, int material) :
		BaseObject(n.norm(), material, cid), d(d0)
	{
	}

	Plane(const Vec3f &r1, const Vec3f &r2, const Vec3f &r3, int material)
	: BaseObject(r1, material, cid)
	{
		Vec3f diff1 = r2 - r1;
		Vec3f diff2 = r3 - r1;
		pos = Vec3f(diff1 % diff2).norm();
		float l = pos * pos;
		d = (l > EPSILON) ? ((pos * r1) / l) : 0.0f;
	}

	~Plane() {}

	float intersect(const Ray &ray) const {
		const float f = ray.d * pos;
		return (f > -EPSILON) ? -1.0f : (pos * d - ray.s) * pos / f;
	}

	std::tuple<Vec3f, Color> computeIntersectionDetails(const Vec3f&) const {
		return std::make_tuple(pos, Color::white());
	}
};
