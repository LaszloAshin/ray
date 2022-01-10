#pragma once

#include "Scene.h"
#include "Image.h"

class Tracer {

	const Scene &scene;
	Image *img;

	static void turboTracer(Tracer *);
	static void blockTracer(Tracer *);
	static bool getProgress(float *percent);

public:

	static void exec(const Scene &scene, Image *img, const char *fname, bool turbo=false);

	Tracer(const Scene &s, Image *i) : scene(s), img(i) {}

};
