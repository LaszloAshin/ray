#ifndef _SCENE_H
#define _SCENE_H	1

class Scene;

#include "Color.h"
#include "Light.h"
#include "BaseObject.h"
#include "KdTree.h"

class Scene {
	Light *firstLight;
	Material *firstMater;
	KdTree *tree;

	void build(int frame);
	void addLight(Light *p);
	void addMaterial(Material *p);

public:
	Scene(int frame);
	~Scene();

	Color trace(const Ray &r, int depth, float weight) const;
};

#endif /* _SCENE_H */
