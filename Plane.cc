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

std::tuple<float, Vector>
Plane::intersect(const Ray &ray) const
{
	const float f = ray.d * pos;
	return std::make_tuple((f > -EPSILON) ? -1.0 : (pos * d - ray.s) * pos / f, pos);
}
