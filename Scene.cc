#include <cmath>
#include "Scene.h"

#include "Ellipsoid.h"
#include "Plane.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

void
Scene::build(int frame)
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
	addObject(std::make_unique<Ellipsoid>(Vector(0.0f, 4.0f, -25.0f), glass, Vector{5.0f, 5.0f, 5.0f}));
	for (int i = 0; i < 5; ++i) {
		float angle = (float)(i) / (0.5f * 5) * (float)M_PI;
		float sina = sinf(angle);
		float cosa = cosf(angle);
		float x = 10.0f * sina;
		float z = -10.0f * cosa;
		Color c(sina, 0.5f, cosa);
		addLight({Vector(x, 10.0f, z - 25.0f), c});
		angle += 2.0f * (float)M_PI * frame / (25.0f * 10.0f);
		// 10 sec alatt fordul korbe 25 fps-nel
		sina = sinf(angle);
		cosa = cosf(angle);
		x = 15.0f * sina;
		z = -15.0f * cosa;
		addObject(std::make_unique<Ellipsoid>(
			Vector(x, -4.0f, z - 25.0f),
                        iron, Vector{5.0f, 2.0f, 5.0f}
		));
	}
	addObject(std::make_unique<Plane>(
		Vector(0.0f, 1.0f, 0.0f), -4.5f,
		mirror
	));
	addObject(std::make_unique<Plane>(
		Vector(0.0f, -1.0f, 0.0f), -15.0f,
		mirror
	));
	addLight({
		Vector(0.0f, 10.0f, -25.0f),
		Color::white
	});
}

void
Scene::addLight(Light light)
{
	lights.push_back(std::move(light));
}

int
Scene::addMaterial(Material material)
{
	materials.push_back(std::move(material));
	return materials.size() - 1;
}

void
Scene::addObject(std::unique_ptr<BaseObject> object)
{
	objects.push_back(std::move(object));
}

std::tuple<BaseObject*, float, Vector>
Scene::intersect(const Ray &ray) const
{
	float t = 0.0f;
	Vector normal;
	BaseObject* nearestObject = nullptr;
	for (const auto& object : objects) {
		const auto [to, n] = object->intersect(ray);
		if (to > 0.0f && (!nearestObject || to < t)) {
			nearestObject = object.get();
			t = to;
			normal = n;
		}
	}
	return std::make_tuple(nearestObject, t, normal);
}

Scene::Scene(int frame)
{
	build(frame);
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
	const Vector mp = ray.s + (ray.d * t);
	ret = mater.ka;

	for (const auto& light : lights) {
		const Vector d = mp - light.pos;
		// light source doesn't take effect if we are in shadow
		const auto [o, sh, _] = intersect(Ray(light.pos, d));
		if (o != nullptr && o != O && sh < 1.0f)
			continue;

		const Vector L = Vector(light.pos - mp).norm();
		const Vector V = Vector(ray.s - mp).norm();
		float dsq = d * d; // light distance square
		if (dsq > EPSILON) {
			dsq = 200.0f / dsq + 5.0f / sqrtf(dsq);
			ret += light.c * mater.brdf(L, N, V) * dsq;
		}
	}

	const Vector V = Vector(ray.s - mp).norm();
	const float cosVN = V * N;

	// tukor
	Vector R = N * (cosVN * 2.0f) - V;
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
		const Vector B = (N * cosVN - V) * v;
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
