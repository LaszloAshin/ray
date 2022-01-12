#pragma once

#include "BaseObject.h"

class Plane final : public BaseObject {
	float d;

public:
	Plane(const Vector &n, float d0, int material) :
		BaseObject(n.norm(), material), d(d0)
	{
	}

	Plane(const Vector &r1, const Vector &r2, const Vector &r3, int material);
	~Plane() {}

	std::tuple<float, Vector> intersect(const Ray &) const override;
};
