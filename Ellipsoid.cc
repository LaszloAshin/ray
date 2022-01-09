#include <cmath>
#include "Ellipsoid.h"

// ellipsoid - ray intersection
std::tuple<float, Vector>
Ellipsoid::intersect(const Ray &r) const
{
	const Vector s{r.s - pos};
	const Vector d{r.d};
	const float a = d.x * d.x * ryzsq + d.y * d.y * rxzsq + d.z * d.z * rxysq;
	const float m1 = d.x * s.y - d.y * s.x;
	const float m2 = d.x * s.z - d.z * s.x;
	const float m3 = d.y * s.z - d.z * s.y;
	const float preD = m1 * m1 * rzsq + m2 * m2 * rysq + m3 * m3 * rxsq;
	if (a < preD) return std::make_tuple(-1.0f, Vector{});

	const float Dsqrt = sqrtf(rxsq * rysq * rzsq * (a - preD));
	const float b = s.x * d.x * ryzsq + s.y * d.y * rxzsq + s.z * d.z * rxysq;
	float t0 = (-b + Dsqrt) / a;
	float t1 = (-b - Dsqrt) / a;
	if (t1 < t0) {
		const float tmp = t0;
		t0 = t1;
		t1 = tmp;
	}
	// return the smaller positive
	if (t0 < 0.0f) t0 = t1;
	if (t0 < 0.0f) return std::make_tuple(-1.0f, Vector{});

	const Vector mp = r.s + r.d * t0;
	Vector N = (mp - this->pos) * 2.0f;
	N.x /= rxsq;
	N.y /= rysq;
	N.z /= rzsq;
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
