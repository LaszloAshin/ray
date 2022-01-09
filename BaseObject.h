#ifndef _BASEOBJECT_H
#define _BASEOBJECT_H	1

#include "Color.h"
#include "VectorMath.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

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

	Color brdf(const Vector &L, const Vector &N, const Vector &V);

	bool isReflective() const { return kr > EPSILON; }
	bool isRefractive() const { return kt > EPSILON; }
};

class BaseObject {
public:
	Vector pos;
	Material *mater;

	BaseObject() {}

	BaseObject(const Vector &p, Material *m) : pos(p), mater(m)
	{
	}

	virtual ~BaseObject() {}

	virtual std::tuple<float, Vector> intersect(const Ray &r) const = 0;
	virtual Color texelAt(const Vector &mp) const { return Color::white; }
};

#endif /* _BASEOBJECT_H */
