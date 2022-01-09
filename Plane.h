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

	std::tuple<float, Vector> intersect(const Ray &) const override;
};

#endif /* _PLANE_H */
