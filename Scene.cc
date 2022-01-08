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

	Material *glass = new Material(
		Color::black,
		Color::gray01,
		Color::gray04, 128.0f,
		0.1f, 0.9f, 2.0f
	);
	Material *iron = new Material(
		Color::gray02,
		Color::gray04,
		Color::gray01, 128.0f,
		0.4f, 0.0f, 1.0f
	);
	Material *mirror = new Material(
		Color::black,
		Color::gray01,
		Color::gray08, 128.0f,
		0.2f, 0.0f, 1.0f
	);
	addMaterial(glass);
	addMaterial(iron);
	addMaterial(mirror);
	v = Vector(5.0f, 5.0f, 5.0f);
	addObject(new Ellipsoid(Vector(0.0f, 4.0f, -25.0f), glass, v));
	for (int i = 0; i < 5; ++i) {
		float angle = (float)(i) / (0.5f * 5) * (float)M_PI;
		float sina = sinf(angle);
		float cosa = cosf(angle);
		float x = 10.0f * sina;
		float z = -10.0f * cosa;
		Color c(sina, 0.5f, cosa);
		addLight(new Light(Vector(x, 10.0f, z - 25.0f), c));
		angle += 2.0f * (float)M_PI * frame / (25.0f * 10.0f);
		// 10 sec alatt fordul korbe 25 fps-nel
		sina = sinf(angle);
		cosa = cosf(angle);
		x = 15.0f * sina;
		z = -15.0f * cosa;
		v = Vector(5.0f, 2.0f, 5.0f);
		addObject(new Ellipsoid(
			Vector(x, -4.0f, z - 25.0f),
                        iron, v
		));
	}
	addObject(new Plane(
		Vector(0.0f, 1.0f, 0.0f), -4.5f,
		mirror
	));
	addObject(new Plane(
		Vector(0.0f, -1.0f, 0.0f), -15.0f,
		mirror
	));
	addLight(new Light(
		Vector(0.0f, 10.0f, -25.0f),
		Color::white
	));
}

void
Scene::addLight(Light *p)
{
	p->next = firstLight;
	firstLight = p;
}

void
Scene::addMaterial(Material *p)
{
	p->next = firstMater;
	firstMater = p;
}

void
Scene::addObject(BaseObject *p)
{
	p->next = firstObj;
	firstObj = p;
}

float
Scene::intersect(const Ray &r, Vector &N, BaseObject **O) const
{
	float t = 0.0f;
	bool found = false;
	for (BaseObject *p = firstObj; p != NULL; p = p->next) {
		Vector nv;
		float to = p->intersect(r, nv);
		if (to > 0.0f && (!found || to < t)) {
			found = true;
			t = to;
			N = nv;
			if (O != NULL) *O = p;
		}
	}
	return found ? t : -1.0f;
}

Scene::Scene(int frame) :
	firstLight(NULL),
	firstMater(NULL),
    firstObj(NULL)
{
	build(frame);
}
	
Scene::~Scene()
{
	while (firstObj != NULL) {
		BaseObject *p = firstObj;
		firstObj = p->next;
		delete p;
	}
	while (firstLight != NULL) {
		Light *p = firstLight;
		firstLight = p->next;
		delete p;
	}
	while (firstMater != NULL) {
		Material *p = firstMater;
		firstMater = p->next;
		delete p;
	}
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

	for (Light *l = firstLight; l != NULL; l = l->next) {
		Vector d = mp - l->pos;
		Vector L;
		BaseObject *o = NULL;
		// light source doesn't take effect if we are in shadow
		float sh = intersect(Ray(l->pos, d), L, &o);
		if (o != O && sh > 0.0f && sh < 1.0f)
			continue;

		L = Vector(l->pos - mp).norm();
		Vector V = Vector(r.s - mp).norm();
		float dsq = d * d; // light distance square
		if (dsq > EPSILON) {
			dsq = 200.0f / dsq + 5.0f / sqrtf(dsq);
			ret += l->c * O->mater->brdf(L, N, V) * dsq;
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
