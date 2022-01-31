#pragma once

#include "Color.h"
#include "Geometry.h"

#include "config.h"

#include <tuple>

struct Material {
	Color ka; // ambient
	Color kd; // diffuse
	Color ks; // specular
	int n; // shinyness
	const float kr; // reflection
	const float kt; // refraction
	float v; // refraction index

	Color brdf(const Vec3f &L, const Vec3f &N, const Vec3f &V) const {
		const float cosLN = L.x * N.x + L.y * N.y + L.z * N.z;
		if (cosLN < 0.0f)
			return {}; // watching from the back

		// specular reflection (Phong)
		const float cosLN2 = cosLN * 2.0f;
		const Vec3f R{
			N.x * cosLN2 - L.x,
			N.y * cosLN2 - L.y,
			N.z * cosLN2 - L.z,
		};
		const float cosRV = std::max(R.x * V.x + R.y * V.y + R.z * V.z, 0.0f);
		const float spec = powi(cosRV, n);
		return {
			kd.r * cosLN + ks.r * spec,
			kd.g * cosLN + ks.g * spec,
			kd.b * cosLN + ks.b * spec,
		};
	}

	bool isReflective() const { return kr > EPSILON; }
	bool isRefractive() const { return kt > EPSILON; }

private:
	static float powi(float x, int y) {
		float result = 1.0f;
		for (; y > 0; y >>= 1) {
			if (y & 1) {
				result *= x;
			}
			x *= x;
		}
		return result;
	}
};

struct BaseObject {
	using ComputeIntersectionDetails = std::tuple<Vec3f, Color> (*)(const BaseObject* o, const Vec3f&);

	Vec3f pos;
	int mater;
	ComputeIntersectionDetails computeIntersectionDetails;

	template <class T>
	static std::tuple<Vec3f, Color> cid(const BaseObject* o, const Vec3f& mp) {
		return static_cast<const T*>(o)->computeIntersectionDetails(mp);
	}
};
