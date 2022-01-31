#include "Scene.h"

#include "MyMath.h"

#include "config.h"

static const Material materials[] = {
	{
		Color::black(), Color::gray(0.1f), Color::gray(0.4f),
		128, 0.1f, 0.9f, 2.0f
	},
	{
		Color::gray(0.2f), Color::gray(0.4f), Color::gray(0.1f),
		128, 0.4f, 0.0f, 1.0f
	},
	{
		Color::black(), Color::gray(0.1f), Color::gray(0.8f),
		128, 0.2f, 0.0f, 1.0f
	},
};

enum MaterialIds {
	MATERIAL_GLASS,
	MATERIAL_IRON,
	MATERIAL_MIRROR,
};

static const Sphere spheres[] = {
	{Vec3f{0.0f, 4.0f, -25.0f}, MATERIAL_GLASS, 5.0f},
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
	planes.emplace_back(Vec3f(0.0f, 1.0f, 0.0f), -4.5f, MATERIAL_MIRROR);
	planes.emplace_back(Vec3f(0.0f, -1.0f, 0.0f), -15.0f, MATERIAL_MIRROR);
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

Color
Scene::trace(const Ray &ray, int depth, float weight) const
{
	Color ret(0.0f, 0.0f, 0.0f);
	if (!depth--)
		return ret;

	if (weight < RAY_ERROR)
		return ret; // don't go further if our result doesn't matter

	const auto [O, t] = intersect(ray);
	if (O == nullptr) {
		return ret;
	}

	const Material& mater = materials[O->mater];
	const Vec3f mp = ray.s + (ray.d * t);
	ret = mater.ka;

	const auto [N, texel] = O->computeIntersectionDetails(O, mp);

	for (const auto& light : lights) {
		const Vec3f d = mp - light.pos;
		// light source doesn't take effect if we are in shadow
		const auto [o, sh] = intersect(Ray(light.pos, d));
		if (o != nullptr && o != O && sh < 1.0f)
			continue;

		const Vec3f L = Vec3f(light.pos - mp).norm();
		const Vec3f V = Vec3f(ray.s - mp).norm();
		float dsq = d * d; // light distance square
		if (dsq > EPSILON) {
			dsq = 200.0f / dsq + 5.0f / mysqrtf(dsq);
			ret += light.c * mater.brdf(L, N, V) * dsq;
		}
	}

	const Vec3f V = Vec3f(ray.s - mp).norm();
	const float cosVN = V * N;

	// tukor
	Vec3f R = N * (cosVN * 2.0f) - V;
	Ray rR(mp + (R * EPSILON), R);
	if (mater.isReflective())
		ret += trace(rR, depth, weight * mater.kr) * mater.kr;

	if (mater.isRefractive()) {
		// refraction
		float v = mater.v;
		float s = 1.0f;
		if (cosVN > 0.0f) {
			v = 1.0f / v;
			s = -1.0f;
		}
		const Vec3f B = (N * cosVN - V) * v;
		const float sq = 1 - (B * B);
		if (sq >= 0.0f) { // if there is no full reflection
			R = B + (N * mysqrtf(sq) * s);
			rR = Ray(mp + (R * EPSILON), R);
		}
	
		ret += trace(rR, depth, weight * mater.kt) * mater.kt;
	}

	ret = ret * texel + ret;
	ret = ret * Color::gray(0.5f);

	return ret;
}
