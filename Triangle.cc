#include <cmath>
#include "Triangle.h"

Triangle::Triangle(const Vector &p1, const Vector &p2, const Vector &p3, Material *m) :
	BaseObject(p1, m), pos2(p2), pos3(p3)
{
	Vector diff1(pos2 - pos);
	Vector diff2(pos3 - pos);
	Vector nabs;
	bool isNormNegative;

	n = Vector(diff1 % diff2).norm();
	nabs.x = fabsf(n.x);
	nabs.y = fabsf(n.y);
	nabs.z = fabsf(n.z);
	if (nabs.x > nabs.y) {
		if (nabs.x > nabs.z) {
			// X is the greatest
			masterPlane = axisX;
			a2d = Vector2D(pos.y, pos.z);
			b2d = Vector2D(pos2.y, pos2.z);
			c2d = Vector2D(pos3.y, pos3.z);
			isNormNegative = n.x < 0.0f;
		} else {
			// Z is the greatest
			masterPlane = axisZ;
			a2d = Vector2D(pos.x, pos.y);
			b2d = Vector2D(pos2.x, pos2.y);
			c2d = Vector2D(pos3.x, pos3.y);
			isNormNegative = n.z < 0.0f;
		}
	} else {
		if (nabs.y > nabs.z) {
			// Y is the greatest
			masterPlane = axisY;
			a2d = Vector2D(pos.x, pos.z);
			b2d = Vector2D(pos2.x, pos2.z);
			c2d = Vector2D(pos3.x, pos3.z);
			isNormNegative = n.y < 0.0f;
		} else {
			// Z is the greatest
			masterPlane = axisZ;
			a2d = Vector2D(pos.x, pos.y);
			b2d = Vector2D(pos2.x, pos2.y);
			c2d = Vector2D(pos3.x, pos3.y);
			isNormNegative = n.z < 0.0f;
		}
	}
	if (isNormNegative) {
		Vector2D tmp = b2d;
		b2d = c2d;
		c2d = tmp;
	}
}

float
Triangle::intersect(const Ray &r, Vector &N) const
{
	float f = r.d * n;
	if (f > -EPSILON) return -1.0f;
	float t = (this->pos - r.s) * n / f;
	Vector mp = r.s + r.d * t;
	Vector2D d2d, diff1, diff2;
	switch (masterPlane) {
	case axisX: d2d = Vector2D(mp.y, mp.z); break;
	case axisY: d2d = Vector2D(mp.x, mp.z); break;
	case axisZ: d2d = Vector2D(mp.x, mp.y); break;
	}

	diff1 = a2d - b2d;
	diff2 = d2d - b2d;
	if ((diff1 % diff2) > 0.0f) {
		return -1.0f;
	}

	diff1 = b2d - c2d;
	diff2 = d2d - c2d;
	if ((diff1 % diff2) > 0.0f) {
		return -1.0f;
	}

	diff1 = c2d - a2d;
	diff2 = d2d - a2d;
	if ((diff1 % diff2) > 0.0f) {
		return -1.0f;
	}

	N = n;
	return t;
}
