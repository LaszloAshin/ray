#include <cmath>
#include "Ellipsoid.h"

// ellipsoid - ray intersection
std::tuple<float, Vector>
Ellipsoid::intersect(const Ray &r) const
{
	Vector p = r.s - this->pos;
	p = Vector(p.x / radius.x, p.y / radius.y, p.z / radius.z);
	const Vector e(r.d.x / radius.x, r.d.y / radius.y, r.d.z / radius.z);
	const Vector q = e % p;
	const float esq = e * e;
	float D = esq - (q * q);
	if (D < 0.0f) return std::make_tuple(-1.0f, Vector{});
	D = sqrtf(D);
	float t0 = e * p;
	float t1 = (-t0 - D) / esq;
	t0 = (-t0 + D) / esq;
	// sort t0 and t1
	if (t1 < t0) {
		const float tmp = t0;
		t0 = t1;
		t1 = tmp;
	}
	// give back the less positive
	if (t0 < 0.0f) t0 = t1;
	if (t0 < 0.0f) return std::make_tuple(-1.0f, Vector{});
	const Vector mp = r.s + r.d * t0;
	Vector N = (mp - this->pos) * 2.0f;
	N.x /= radius.x * radius.x;
	N.y /= radius.y * radius.y;
	N.z /= radius.z * radius.z;
	N = N.norm();
	return std::make_tuple(t0, N);
}

Color
Ellipsoid::texelAt(const Vector &mp) const
{
	Vector p = mp - pos;
	float v = (float)M_1_PI * acosf(p.z / radius.z);
//	float u = 0.5f * M_1_PI * acosf(p.x / (radius.x * sinf(M_PI * v)));
	float u = 0.5f * (float)M_1_PI * (atan2f(p.y / radius.y, p.x / radius.x) + (float)M_PI);
	int x = (int)(u * 16.0f);
	int y = (int)(v * 16.0f);
	return ((x ^ y) & 1) ? Color(0.0f, 0.0f, 0.0f) : Color(1.0f, 1.0f, 1.0f);
}
