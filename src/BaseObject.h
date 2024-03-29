#pragma once

#include "Color.h"
#include "Geometry.h"

#include "config.h"

#include <tuple>

struct Material {
	float ka; // ambient
	float kd; // diffuse
	float ks; // specular
	float kr; // reflection
	float kt; // refraction
	float v; // refraction index
	char n; // shinyness

	float brdf(const Vec3f &L, const Vec3f &N, const Vec3f &V) const {
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
		return kd * cosLN + ks * spec;
	}

	bool isReflective() const { return kr > EPSILON; }
	bool isRefractive() const { return kt > EPSILON; }

private:
	static float powi(float x, int n) {
		for (; n > 0; --n) {
			x *= x;
		}
		return x;
	}
};

enum ObjectType : short {
	OT_PLANE,
	OT_SPHEROID,
};

struct BaseObject {
	using ComputeIntersectionDetails = std::tuple<Vec3f, Color> (*)(const BaseObject* o, const Vec3f&);

	ObjectType type;
	short mater;
	Vec3f pos;

	template <class T>
	static std::tuple<Vec3f, Color> cid(const BaseObject* o, const Vec3f& mp) {
		return static_cast<const T*>(o)->computeIntersectionDetails(mp);
	}
};
