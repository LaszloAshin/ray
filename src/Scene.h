#pragma once

#include "Color.h"
#include "Spheroid.h"
#include "Light.h"
#include "Plane.h"
#include "Vector.h"

#include <memory>

class Scene {
	Vector<Light, 8> lights;
	Vector<Material, 4> materials;
	Vector<Spheroid, 8> spheroids;
	Vector<Plane, 2> planes;

	int addMaterial(Material material);
	std::tuple<const BaseObject*, float, Vec3f> intersect(const Ray &r) const;

public:
	Scene(int frame);

	Color trace(const Ray &r, int depth, float weight) const;
};
