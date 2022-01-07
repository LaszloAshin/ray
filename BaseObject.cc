#include <cmath>
#include "BaseObject.h"

Color
Material::brdf(const Vector &L, const Vector &N, const Vector &V)
{
	Color ret = Color(0.0f, 0.0f, 0.0f);
	float cosLN = L * N;
	if (cosLN < 0.0f)
		return ret; // watching from the back

	// diffuse reflection
	ret += kd * cosLN;

	// specular reflection (Phong)
	Vector R = N * (cosLN * 2.0f) - L;
	float cosRV = R * V;
	if (cosRV < 0.0f) cosRV = 0.0f;
	ret += ks * powf(cosRV, this->n);
	return ret;
}

Color
BaseObject::texelAt(const Vector &) const
{
	return Color::white;
}
