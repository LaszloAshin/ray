#include "Scene.h"

#include "MyMath.h"

#include "config.h"

static const Material materials[] = {
	{
		Color::black(), Color::gray(0.1f), Color::gray(0.4f),
		7, 0.1f, 0.9f, 2.0f
	},
	{
		Color::gray(0.2f), Color::gray(0.4f), Color::gray(0.1f),
		7, 0.4f, 0.0f, 1.0f
	},
	{
		Color::black(), Color::gray(0.1f), Color::gray(0.8f),
		7, 0.2f, 0.0f, 1.0f
	},
};

enum MaterialIds : short {
	MATERIAL_GLASS,
	MATERIAL_IRON,
	MATERIAL_MIRROR,
};

static const Sphere spheres[] = {
	{Vec3f{0.0f, 4.0f, -25.0f}, MATERIAL_GLASS, 5.0f},
};

static const Plane planes[] = {
	{Vec3f{0.0f, 1.0f, 0.0f}, -4.5f, MATERIAL_MIRROR},
	{Vec3f{0.0f, -1.0f, 0.0f}, -15.0f, MATERIAL_MIRROR},
};

Scene::Scene(int frame)
{
	for (int i = 0; i < 5; ++i) {
		float angle = (float)(i) / (0.5f * 5) * (float)M_PI;
		float sina, cosa;
		mysincosf(angle, &sina, &cosa);
		float x = 10.0f * sina;
		float z = -10.0f * cosa;
		Color c(sina, 0.5f, cosa);
		lights.emplace_back(Vec3f(x, 10.0f, z - 25.0f), c);
		angle += 2.0f * (float)M_PI * frame / (25.0f * 10.0f);
		// 10 sec alatt fordul korbe 25 fps-nel
		mysincosf(angle, &sina, &cosa);
		x = 15.0f * sina;
		z = -15.0f * cosa;
		spheroids.emplace_back(Vec3f(x, -4.0f, z - 25.0f), MATERIAL_IRON, 5.0f, 2.0f);
	}
	lights.emplace_back(Vec3f(0.0f, 10.0f, -25.0f), Color::white());
}

Scene::Intersection
Scene::intersect(const Ray &ray) const
{
	Intersection result;
	result.addObjects(ray, spheres);
	result.addObjects(ray, spheroids);
	result.addObjects(ray, planes);
	return result;
}

static const BaseObject::ComputeIntersectionDetails cid[] = {
	BaseObject::cid<Plane>,
	BaseObject::cid<Sphere>,
	BaseObject::cid<Spheroid>,
};

Color
Scene::trace(const Ray &ray, int depth, float weight) const
{
	if (!depth--)
		return {};

	if (weight < RAY_ERROR)
		return {}; // don't go further if our result doesn't matter

	const auto [O, t] = intersect(ray);
	if (O == nullptr) {
		return {};
	}

	const Material& mater = materials[O->mater];
	const Vec3f mp{ray.s.x + t * ray.d.x, ray.s.y + t * ray.d.y, ray.s.z + t * ray.d.z};
	auto [r, g, b] = mater.ka;

	const auto [N, texel] = cid[O->type](O, mp);
	const Vec3f V = Vec3f{ray.s.x - mp.x, ray.s.y - mp.y, ray.s.z - mp.z}.norm();

	for (const auto& light : lights) {
		const Vec3f d{mp.x - light.pos.x, mp.y - light.pos.y, mp.z - light.pos.z};
		// light source doesn't take effect if we are in shadow
		const auto [o, sh] = intersect(Ray(light.pos, d));
		if (o != nullptr && o != O && sh < 1.0f)
			continue;

		float dsq = d.x * d.x + d.y * d.y + d.z * d.z;
		if (dsq > EPSILON) {
			dsq = 200.0f / dsq + 5.0f / mysqrtf(dsq);
			const Vec3f L = Vec3f{-d.x, -d.y, -d.z}.norm();
			const Color brdf = mater.brdf(L, N, V);
			r += light.c.r * brdf.r * dsq;
			g += light.c.g * brdf.g * dsq;
			b += light.c.b * brdf.b * dsq;
		}
	}

	const float cosVN = V.x * N.x + V.y * N.y + V.z * N.z;
	const float cosVN2 = cosVN * 2.0f;

	// reflection
	const Vec3f R{N.x * cosVN2 - V.x, N.y * cosVN2 - V.y, N.z * cosVN2 - V.z};
	Ray rR({mp.x + EPSILON * R.x, mp.y + EPSILON * R.y, mp.z + EPSILON * R.z}, R);
	if (mater.isReflective()) {
		const Color c = trace(rR, depth, weight * mater.kr);
		r += c.r * mater.kr;
		g += c.g * mater.kr;
		b += c.b * mater.kr;
	}

	if (mater.isRefractive()) {
		// refraction
		float v = mater.v;
		float s = 1.0f;
		if (cosVN > 0.0f) {
			v = 1.0f / v;
			s = -1.0f;
		}
		const Vec3f B{(N.x * cosVN - V.x) * v, (N.y * cosVN - V.y) * v, (N.z * cosVN - V.z) * v};
		const float sq = 1.0f - (B.x * B.x + B.y * B.y + B.z * B.z);
		if (sq >= 0.0f) { // if there is no full reflection
			const float m = mysqrtf(sq) * s;
			const Vec3f R2{B.x + m * N.x, B.y + m * N.y, B.z + m * N.z};
			rR = Ray({mp.x + EPSILON * R2.x, mp.y + EPSILON * R2.y, mp.z + EPSILON * R2.z}, R2);
		}
	
		const Color c = trace(rR, depth, weight * mater.kt);
		r += c.r * mater.kt;
		g += c.g * mater.kt;
		b += c.b * mater.kt;
	}

	return {
		(r * texel.r + r) * 0.5f,
		(g * texel.g + g) * 0.5f,
		(b * texel.b + b) * 0.5f
	};
}
