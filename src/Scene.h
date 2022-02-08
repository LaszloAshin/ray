#pragma once

#include "Color.h"
#include "Sphere.h"
#include "Spheroid.h"
#include "Light.h"
#include "Plane.h"

struct Scene {
	Scene(int frame);

	Color trace(const Ray &r, int depth, float weight) const;

private:
	struct Intersection {
		const BaseObject* nearestObject = nullptr;
		float t = 0.0f;

		template <class Objects>
		void addObjects(const Ray& ray, const Objects& objects) {
			for (const auto& object : objects) {
				const auto to = object.intersect(ray);
				if (to > 0.0f && (nearestObject == nullptr || to < t)) {
					nearestObject = &object;
					t = to;
				}
			}
		}
	};

	Intersection intersect(const Ray &r) const;

	Spheroid spheroids[5];
	Light lights[6];
};
