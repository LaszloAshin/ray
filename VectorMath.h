#ifndef _VECTORMATH_H
#define _VECTORMATH_H	1

struct Vector {
	float x, y, z;

	static const Vector null;

	Vector(float x0 = 0.0f, float y0 = 0.0f, float z0 = 0.0f) :
		x(x0), y(y0), z(z0)
	{
	}

	Vector&	operator+=(const Vector &v);
	Vector operator+(const Vector &v) const;
	Vector& operator-=(const Vector &v);
	Vector operator-(const Vector &v) const;
	Vector& operator*=(float a);
	Vector operator*(float a) const;
	float operator*(const Vector &v) const;
	float length() const;
	Vector norm() const;
	Vector operator%(const Vector &v) const;
};

struct Vector2D {
	float x, y;

	Vector2D(float x0 = 0.0f, float y0 = 0.0f) :
		x(x0), y(y0)
	{
	}

	float operator%(const Vector2D &v) const;
	Vector2D operator-(const Vector2D &v) const;
};

struct Ray {
	Vector s, d;

	Ray(const Vector &source, const Vector &dir) :
		s(source), d(dir)
	{
	}
};

class Matrix {

	float e[16];

public:
	Matrix()
	{
		for (int i = 0; i < 16; ++i) {
			e[i] = 0.0f;
		}
	}

};

#endif /* _VECTORMATH_H */
