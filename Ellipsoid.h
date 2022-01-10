#pragma once

#include "VectorMath.h"
#include "BaseObject.h"

#include <cassert>

class Ellipsoid : public BaseObject {
	Vector radius;
	float rxsq, rysq, rzsq;
	float rxysq, rxzsq, ryzsq;

public:
	Ellipsoid(const Vector &p, Material *m, const Vector &radius)
	: BaseObject(p, m)
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
	std::tuple<float, Vector> intersect(const Ray &) const override;
	Color texelAt(const Vector &mp) const override;
};
