#pragma once

#include "Color.h"
#include "Geometry.h"

#include "config.h"

#include <cmath>
#include <tuple>

class Material {
public:
	Color ka; // ambient
protected:
	Color kd; // diffuse
	Color ks; // specular
	float n; // shinyness
public:
	float v; // refraction index

	const float kr; // reflection
	const float kt; // refraction

	Material(
		Color ka0, Color kd0, Color ks0, float n0,
		float kr0, float kt0, float v0
	) : ka(ka0), kd(kd0), ks(ks0), kr(kr0), kt(kt0)
	{
		n = n0, v = v0;
	}

	Color brdf(const Vec3f &L, const Vec3f &N, const Vec3f &V) const {
		const float cosLN = L * N;
		if (cosLN < 0.0f)
			return Color::black; // watching from the back

		// specular reflection (Phong)
		const Vec3f R = N * (cosLN * 2.0f) - L;
		const float cosRV = std::max(R * V, 0.0f);

		return kd * cosLN                // diffuse
			+ ks * powf(cosRV, this->n); // specular
	}

	bool isReflective() const { return kr > EPSILON; }
	bool isRefractive() const { return kt > EPSILON; }
};

class BaseObject {
public:
	Vec3f pos;
	int mater;

	BaseObject() {}

	BaseObject(const Vec3f &p, int m) : pos(p), mater(m)
	{
	}

	virtual ~BaseObject() {}

	virtual Color texelAt(const Vec3f &) const { return Color::white; }
};
