#include <cmath>
#include "Plane.h"


Plane::Plane(const Vec3f &r1, const Vec3f &r2, const Vec3f &r3, int material) :
	BaseObject(r1, material)
{
	Vec3f diff1 = r2 - r1;
	Vec3f diff2 = r3 - r1;
	pos = Vec3f(diff1 % diff2).norm();
	float l = pos * pos;
	d = (l > EPSILON) ? ((pos * r1) / l) : 0.0f;
}

std::tuple<float, Vec3f>
Plane::intersect(const Ray &ray) const
{
	const float f = ray.d * pos;
	return std::make_tuple((f > -EPSILON) ? -1.0 : (pos * d - ray.s) * pos / f, pos);
}
