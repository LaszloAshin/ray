#include <cmath>
#include "Plane.h"


Plane::Plane(const Vector &r1, const Vector &r2, const Vector &r3, Material *m) :
	BaseObject(r1, m)
{
	Vector diff1 = r2 - r1;
	Vector diff2 = r3 - r1;
	pos = Vector(diff1 % diff2).norm();
	float l = pos * pos;
	d = (l > EPSILON) ? ((pos * r1) / l) : 0.0f;
}

float
Plane::intersect(const Ray &r, Vector &N) const
{
	float f = r.d * pos;
	if (f > -EPSILON) return -1.0f;
	N = pos;
	return (pos * d - r.s) * pos / f;
}

int
Plane::onSideOfPlane(Axis axis, float d) const
{
	switch (axis) {
		case axisX:
			if (fabs(pos.y) > EPSILON) return 0;
			if (fabs(pos.z) > EPSILON) return 0;
			if (this->d < d - EPSILON) return -1;
			if (this->d > d + EPSILON) return 1;
			return 0;
		case axisY:
			if (fabs(pos.x) > EPSILON) return 0;
			if (fabs(pos.z) > EPSILON) return 0;
			if (this->d < d - EPSILON) return -1;
			if (this->d > d + EPSILON) return 1;
			return 0;
		case axisZ:
			if (fabs(pos.x) > EPSILON) return 0;
			if (fabs(pos.y) > EPSILON) return 0;
			if (this->d < d - EPSILON) return -1;
			if (this->d > d + EPSILON) return 1;
			return 0;
	}
	return 0;
}
