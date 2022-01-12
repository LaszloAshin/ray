#pragma once

#include <vector>
#include <memory>

class Scene;

#include "Color.h"
#include "Light.h"
#include "BaseObject.h"

class Scene {
	std::vector<Light> lights;
	std::vector<Material> materials;
	std::vector<std::unique_ptr<BaseObject>> objects;

	void build(int frame);
	void addLight(Light light);
	int addMaterial(Material material);
	void addObject(std::unique_ptr<BaseObject> object);
	std::tuple<BaseObject*, float, Vector> intersect(const Ray &r) const;

public:
	Scene(int frame);

	Color trace(const Ray &r, int depth, float weight) const;
};
