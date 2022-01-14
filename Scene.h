#pragma once

#include "BaseObject.h"
#include "Color.h"
#include "Light.h"
#include "Vector.h"

#include <memory>

class Scene {
	Vector<Light, 8> lights;
	Vector<Material, 4> materials;
	Vector<std::unique_ptr<BaseObject>, 8> objects;

	void build(int frame);
	int addMaterial(Material material);
	void addObject(std::unique_ptr<BaseObject> object);
	std::tuple<BaseObject*, float, Vec3f> intersect(const Ray &r) const;

public:
	Scene(int frame);

	Color trace(const Ray &r, int depth, float weight) const;
};
