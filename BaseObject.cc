#include <cmath>
#include "BaseObject.h"

Color
Material::brdf(const Vector &L, const Vector &N, const Vector &V)
{
	const float cosLN = L * N;
	if (cosLN < 0.0f)
		return Color::black; // watching from the back

	// specular reflection (Phong)
	const Vector R = N * (cosLN * 2.0f) - L;
	const float cosRV = std::max(R * V, 0.0f);

	return kd * cosLN                // diffuse
		+ ks * powf(cosRV, this->n); // specular
}
