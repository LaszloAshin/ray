#pragma once

#include "BaseObject.h"

class Plane final : public BaseObject {
	float d;

public:
	Plane(const Vec3f &n, float d0, int material) :
		BaseObject(n.norm(), material), d(d0)
	{
	}

	Plane(const Vec3f &r1, const Vec3f &r2, const Vec3f &r3, int material);
	~Plane() {}

	std::tuple<float, Vec3f> intersect(const Ray &) const override;
};
