#pragma once

#include "Color.h"
#include "Spheroid.h"
#include "Light.h"
#include "Plane.h"
#include "Vector.h"

#include <memory>

class Scene {
	struct Intersection {
		const BaseObject* nearestObject = nullptr;
		float t = 0.0f;
		Vec3f normal;

		template <class Objects>
		void addObjects(const Ray& ray, const Objects& objects) {
			for (const auto& object : objects) {
				const auto [to, n] = object.intersect(ray);
				if (to > 0.0f && (nearestObject == nullptr || to < t)) {
					nearestObject = &object;
					t = to;
					normal = n;
				}
			}
		}
	};

	Vector<Light, 8> lights;
	Vector<Material, 4> materials;
	Vector<Spheroid, 8> spheroids;
	Vector<Plane, 2> planes;

	int addMaterial(Material material);
	Intersection intersect(const Ray &r) const;

public:
	Scene(int frame);

	Color trace(const Ray &r, int depth, float weight) const;
};
