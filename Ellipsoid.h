#ifndef _ELLIPSOID_H
#define _ELLIPSOID_H	1

#include "VectorMath.h"
#include "BaseObject.h"

#include <cassert>

class Ellipsoid : public BaseObject {
	Vector radius;

public:
	Ellipsoid(const Vector &p, Material *m, const Vector &radius) :
		BaseObject(p, m), radius(radius)
	{
		assert(fabs(radius.x) > EPSILON);
		assert(fabs(radius.y) > EPSILON);
		assert(fabs(radius.z) > EPSILON);
	}

	// ellipsoid - ray intersection
	std::tuple<float, Vector> intersect(const Ray &) const override;
	Color texelAt(const Vector &mp) const override;
};

#endif /* _ELLIPSOID_H */
