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
	Vector v;

	auto glass = std::make_unique<Material>(
		Color::black,
		Color::gray01,
		Color::gray04, 128.0f,
		0.1f, 0.9f, 2.0f
	);
	auto iron = std::make_unique<Material>(
		Color::gray02,
		Color::gray04,
		Color::gray01, 128.0f,
		0.4f, 0.0f, 1.0f
	);
	auto mirror = std::make_unique<Material>(
		Color::black,
		Color::gray01,
		Color::gray08, 128.0f,
		0.2f, 0.0f, 1.0f
	);
	v = Vector(5.0f, 5.0f, 5.0f);
	addObject(std::make_unique<Ellipsoid>(Vector(0.0f, 4.0f, -25.0f), glass.get(), v));
	for (int i = 0; i < 5; ++i) {
		float angle = (float)(i) / (0.5f * 5) * (float)M_PI;
		float sina = sinf(angle);
		float cosa = cosf(angle);
		float x = 10.0f * sina;
		float z = -10.0f * cosa;
		Color c(sina, 0.5f, cosa);
		addLight(std::make_unique<Light>(Vector(x, 10.0f, z - 25.0f), c));
		angle += 2.0f * (float)M_PI * frame / (25.0f * 10.0f);
		// 10 sec alatt fordul korbe 25 fps-nel
		sina = sinf(angle);
		cosa = cosf(angle);
		x = 15.0f * sina;
		z = -15.0f * cosa;
		v = Vector(5.0f, 2.0f, 5.0f);
		addObject(std::make_unique<Ellipsoid>(
			Vector(x, -4.0f, z - 25.0f),
                        iron.get(), v
		));
	}
	addObject(std::make_unique<Plane>(
		Vector(0.0f, 1.0f, 0.0f), -4.5f,
		mirror.get()
	));
	addObject(std::make_unique<Plane>(
		Vector(0.0f, -1.0f, 0.0f), -15.0f,
		mirror.get()
	));
	addLight(std::make_unique<Light>(
		Vector(0.0f, 10.0f, -25.0f),
		Color::white
	));
	addMaterial(std::move(glass));
	addMaterial(std::move(iron));
	addMaterial(std::move(mirror));
}

void
Scene::addLight(std::unique_ptr<Light> light)
{
	lights.push_back(std::move(light));
}

void
Scene::addMaterial(std::unique_ptr<Material> material)
{
	materials.push_back(std::move(material));
}

void
Scene::addObject(std::unique_ptr<BaseObject> object)
{
	objects.push_back(std::move(object));
}

float
Scene::intersect(const Ray &r, Vector &N, BaseObject **O) const
{
	float t = 0.0f;
	bool found = false;
	for (const auto& object : objects) {
		Vector nv;
		float to = object->intersect(r, nv);
		if (to > 0.0f && (!found || to < t)) {
			found = true;
			t = to;
			N = nv;
			if (O != NULL) *O = object.get();
		}
	}
	return found ? t : -1.0f;
}

Scene::Scene(int frame)
{
	build(frame);
}
	
Color
Scene::trace(const Ray &r, int depth, float weight) const
{
	Color ret = Color(0.0f, 0.0f, 0.0f);
	if (!depth--)
		return ret;

	if (weight < RAY_ERROR)
		return ret; // don't go further if our result doesnt matter

	Vector N;
	BaseObject *O = NULL;
	float t = intersect(r, N, &O);
	if (t < 0.0f) return ret;
	Vector mp = r.s + (r.d * t);
	ret = O->mater->ka;

	for (const auto& light : lights) {
		Vector d = mp - light->pos;
		Vector L;
		BaseObject *o = NULL;
		// light source doesn't take effect if we are in shadow
		float sh = intersect(Ray(light->pos, d), L, &o);
		if (o != O && sh > 0.0f && sh < 1.0f)
			continue;

		L = Vector(light->pos - mp).norm();
		Vector V = Vector(r.s - mp).norm();
		float dsq = d * d; // light distance square
		if (dsq > EPSILON) {
			dsq = 200.0f / dsq + 5.0f / sqrtf(dsq);
			ret += light->c * O->mater->brdf(L, N, V) * dsq;
		}
	}

	Vector V = Vector(r.s - mp).norm();
	float cosVN = V * N;

	// tukor
	Vector R = N * (cosVN * 2.0f) - V;
	Ray rR(mp + (R * EPSILON), R);
	if (O->mater->isReflective())
		ret += trace(rR, depth, weight * O->mater->kr) * O->mater->kr;

	if (O->mater->isRefractive()) {
		// refraction
		float v = O->mater->v;
		float s = 1.0f;
		if (cosVN > 0.0f) {
			v = 1.0f / v;
			s = -1.0f;
		}
		Vector B = (N * cosVN - V) * v;
		float sq = 1 - (B * B);
		if (sq >= 0.0f) { // if there is no full reflection
			R = B + (N * sqrt(sq) * s);
			rR = Ray(mp + (R * EPSILON), R);
		}
	
		ret += trace(rR, depth, weight * O->mater->kt) * O->mater->kt;
	}

	ret = ret * O->texelAt(mp) + ret;
	ret = ret * Color::gray05;

	return ret;
}
