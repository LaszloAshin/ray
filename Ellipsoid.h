#ifndef _ELLIPSOID_H
#define _ELLIPSOID_H	1

#include "VectorMath.h"
#include "BaseObject.h"

class Ellipsoid : public BaseObject {
	Vector c;
public:
	Ellipsoid(const Vector &p, Material *m, const Vector &center) :
		BaseObject(p, m), c(center)
	{
	}

	// ellipsoid - ray intersection
	std::tuple<float, Vector> intersect(const Ray &) const override;
	Color texelAt(const Vector &mp) const override;
};

#endif /* _ELLIPSOID_H */
