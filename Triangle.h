#ifndef _TRIANGLE_H
#define _TRIANGLE_H	1

#include "BaseObject.h"

class Triangle : public BaseObject {
	Vector pos2, pos3, n;
	Vector2D a2d, b2d, c2d;
	Axis masterPlane;

public:
	Triangle(const Vector &p1, const Vector &p2, const Vector &p3, Material *m);
	virtual ~Triangle() {}

	virtual float intersect(const Ray &r, Vector &N) const;
	virtual Vector midPoint() const { return (pos + pos2 + pos3) * (1.0f / 3.0f); }
	virtual int onSideOfPlane(Axis axis, float d) const;
};

#endif /* _TRIANGLE_H */
