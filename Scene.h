#pragma once

#include <vector>
#include <memory>

class Scene;

#include "Color.h"
#include "Light.h"
#include "BaseObject.h"

class Scene {
	std::vector<std::unique_ptr<Light>> lights;
	std::vector<std::unique_ptr<Material>> materials;
	std::vector<std::unique_ptr<BaseObject>> objects;

	void build(int frame);
	void addLight(std::unique_ptr<Light> light);
	int addMaterial(std::unique_ptr<Material> material);
	void addObject(std::unique_ptr<BaseObject> object);
	std::tuple<BaseObject*, float, Vector> intersect(const Ray &r) const;

public:
	Scene(int frame);

	Color trace(const Ray &r, int depth, float weight) const;
};
