#ifndef _BASEOBJECT_H
#define _BASEOBJECT_H	1

#include "Color.h"
#include "VectorMath.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

class Material {
public:
	Color ka; // ambient
protected:
	Color kd; // diffuse
	Color ks; // specular
	float n; // shinyness
	float v; // refraction index

	friend class Scene;
	Material *next;

public:
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
	friend class Scene;
	BaseObject *next;
protected:
	Vector pos;

public:
	Material *mater;

	BaseObject() {}

	BaseObject(const Vector &p, Material *m) : pos(p), mater(m)
	{
	}

	virtual ~BaseObject() {}

	virtual bool hasMidPoint() const { return true; }
	virtual Vector midPoint() const { return pos; }
	virtual float intersect(const Ray &r, Vector &N) const = 0;
	virtual Color texelAt(const Vector &mp) const;
};

#endif /* _BASEOBJECT_H */
