#pragma once

#include "Color.h"
#include "Spheroid.h"
#include "Light.h"
#include "Plane.h"

struct Scene {
	Scene(int frame);

	Color trace(const Ray &r, int depth = DEPTH_LIMIT, float weight = 1.0f) const;

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
	void traceLights(const BaseObject* O, const Vec3f& mp, const Vec3f& N, const Vec3f& V, Color& rgb) const;

	Spheroid spheroids[6];
	Light lights[6];
};
