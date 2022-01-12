#pragma once

#include "VectorMath.h"
#include "BaseObject.h"

#include <cassert>

class Ellipsoid final : public BaseObject {
	Vec3f radius;
	float rxsq, rysq, rzsq;
	float rxysq, rxzsq, ryzsq;

public:
	Ellipsoid(const Vec3f &p, int material, const Vec3f &radius)
	: BaseObject(p, material)
	, radius(radius)
	, rxsq{radius.x * radius.x}
	, rysq{radius.y * radius.y}
	, rzsq{radius.z * radius.z}
	, rxysq{rxsq * rysq}
	, rxzsq{rxsq * rzsq}
	, ryzsq{rysq * rzsq}
	{
		assert(fabs(radius.x) > EPSILON);
		assert(fabs(radius.y) > EPSILON);
		assert(fabs(radius.z) > EPSILON);
	}

	// ellipsoid - ray intersection
	std::tuple<float, Vec3f> intersect(const Ray &) const override;
	Color texelAt(const Vec3f &mp) const override;
};
