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

	virtual ~Ellipsoid() {}

	// ellipsoid - ray intersection
	virtual float intersect(const Ray &r, Vector &N) const;
	virtual Color texelAt(const Vector &mp) const;
};

#endif /* _ELLIPSOID_H */
