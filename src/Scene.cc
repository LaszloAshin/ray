#include <cmath>
#include "Scene.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

Scene::Scene(int frame)
{
	const int glass = addMaterial({
		Color::black,
		Color::gray01,
		Color::gray04, 128.0f,
		0.1f, 0.9f, 2.0f
	});
	const int iron = addMaterial({
		Color::gray02,
		Color::gray04,
		Color::gray01, 128.0f,
		0.4f, 0.0f, 1.0f
	});
	const int mirror = addMaterial({
		Color::black,
		Color::gray01,
		Color::gray08, 128.0f,
		0.2f, 0.0f, 1.0f
	});
	spheroids.emplace_back(Vec3f(0.0f, 4.0f, -25.0f), glass, 5.0f, 5.0f);
	for (int i = 0; i < 5; ++i) {
		float angle = (float)(i) / (0.5f * 5) * (float)M_PI;
		float sina = sinf(angle);
		float cosa = cosf(angle);
		float x = 10.0f * sina;
		float z = -10.0f * cosa;
		Color c(sina, 0.5f, cosa);
		lights.emplace_back(Vec3f(x, 10.0f, z - 25.0f), c);
		angle += 2.0f * (float)M_PI * frame / (25.0f * 10.0f);
		// 10 sec alatt fordul korbe 25 fps-nel
		sina = sinf(angle);
		cosa = cosf(angle);
		x = 15.0f * sina;
		z = -15.0f * cosa;
		spheroids.emplace_back(Vec3f(x, -4.0f, z - 25.0f), iron, 5.0f, 2.0f);
	}
	planes.emplace_back(Vec3f(0.0f, 1.0f, 0.0f), -4.5f, mirror);
	planes.emplace_back(Vec3f(0.0f, -1.0f, 0.0f), -15.0f, mirror);
	lights.emplace_back(Vec3f(0.0f, 10.0f, -25.0f), Color::white);
}

int
Scene::addMaterial(Material material)
{
	materials.push_back(std::move(material));
	return materials.size() - 1;
}

std::tuple<const BaseObject*, float, Vec3f>
Scene::intersect(const Ray &ray) const
{
	float t = 0.0f;
	Vec3f normal;
	const BaseObject* nearestObject = nullptr;
	for (const auto& object : spheroids) {
		const auto [to, n] = object.intersect(ray);
		if (to > 0.0f && (!nearestObject || to < t)) {
			nearestObject = &object;
			t = to;
			normal = n;
		}
	}
	for (const auto& object : planes) {
		const auto [to, n] = object.intersect(ray);
		if (to > 0.0f && (!nearestObject || to < t)) {
			nearestObject = &object;
			t = to;
			normal = n;
		}
	}
	return std::make_tuple(nearestObject, t, normal);
}

Color
Scene::trace(const Ray &ray, int depth, float weight) const
{
	Color ret(0.0f, 0.0f, 0.0f);
	if (!depth--)
		return ret;

	if (weight < RAY_ERROR)
		return ret; // don't go further if our result doesn't matter

	const auto [O, t, N] = intersect(ray);
	if (O == nullptr) {
		return ret;
	}

	const Material& mater = materials[O->mater];
	const Vec3f mp = ray.s + (ray.d * t);
	ret = mater.ka;

	for (const auto& light : lights) {
		const Vec3f d = mp - light.pos;
		// light source doesn't take effect if we are in shadow
		const auto [o, sh, _] = intersect(Ray(light.pos, d));
		if (o != nullptr && o != O && sh < 1.0f)
			continue;

		const Vec3f L = Vec3f(light.pos - mp).norm();
		const Vec3f V = Vec3f(ray.s - mp).norm();
		float dsq = d * d; // light distance square
		if (dsq > EPSILON) {
			dsq = 200.0f / dsq + 5.0f / sqrtf(dsq);
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
			R = B + (N * sqrt(sq) * s);
			rR = Ray(mp + (R * EPSILON), R);
		}
	
		ret += trace(rR, depth, weight * mater.kt) * mater.kt;
	}

	ret = ret * O->texelAt(mp) + ret;
	ret = ret * Color::gray05;

	return ret;
}
