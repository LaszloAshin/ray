#ifndef _PLANE_H
#define _PLANE_H	1

#include "BaseObject.h"

class Plane : public BaseObject {
	float d;

public:
	Plane(const Vector &n, float d0, Material *m) :
		BaseObject(n.norm(), m), d(d0)
	{
	}

	Plane(const Vector &r1, const Vector &r2, const Vector &r3, Material *m);
	~Plane() {}

	virtual bool hasMidPoint() const { return false; }

	virtual float intersect(const Ray &r, Vector &N) const;
};

#endif /* _PLANE_H */
