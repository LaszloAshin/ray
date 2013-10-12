#include <cmath>
#include "Ellipsoid.h"

// ellipsoid - ray intersection
float
Ellipsoid::intersect(const Ray &r, Vector &N) const
{
	if (fabs(c.x) < EPSILON || fabs(c.y) < EPSILON ||
		fabs(c.z) < EPSILON) return -1.0f;

	Vector p = r.s - this->pos;
	p = Vector(p.x / c.x, p.y / c.y, p.z / c.z);
	Vector e(r.d.x / c.x, r.d.y / c.y, r.d.z / c.z);
	Vector q = e % p;
	float esq = e * e;
	float D = esq - (q * q);
	if (D < 0.0f) return -1.0f;
	D = sqrtf(D);
	float t0 = e * p;
	float t1 = (-t0 - D) / esq;
	t0 = (-t0 + D) / esq;
	// sort t0 and t1
	if (t1 < t0) {
		float tmp = t0;
		t0 = t1;
		t1 = tmp;
	}
	// give back the less positive
	if (t0 < 0.0f) t0 = t1;
	if (t0 < 0.0f) return -1.0f;
	Vector mp = r.s + r.d * t0;
	N = (mp - this->pos) * 2.0f;
	N.x /= c.x * c.x;
	N.y /= c.y * c.y;
	N.z /= c.z * c.z;
	N = N.norm();
	return t0;
}

int
Ellipsoid::onSideOfPlane(Axis axis, float d) const
{
	switch (axis) {
		case axisX:
			if (pos.x + c.x < d) return -1;
			if (pos.x - c.x > d) return 1;
			return 0;
		case axisY:
			if (pos.y + c.y < d) return -1;
			if (pos.y - c.y > d) return 1;
			return 0;
		case axisZ:
			if (pos.z + c.z < d) return -1;
			if (pos.z - c.z > d) return 1;
			return 0;
	}
	return 0;
}

Color
Ellipsoid::texelAt(const Vector &mp) const
{
	Vector p = mp - pos;
	float v = (float)M_1_PI * acosf(p.z / c.z);
//	float u = 0.5f * M_1_PI * acosf(p.x / (c.x * sinf(M_PI * v)));
	float u = 0.5f * (float)M_1_PI * (atan2f(p.y / c.y, p.x / c.x) + (float)M_PI);
	int x = (int)(u * 16.0f);
	int y = (int)(v * 16.0f);
	return ((x ^ y) & 1) ? Color(0.0f, 0.0f, 0.0f) : Color(1.0f, 1.0f, 1.0f);
}
