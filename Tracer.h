#pragma once

#include "Image.h"
#include "Scene.h"

struct Tracer {
	Tracer(const Scene &s, Image *i) : scene(s), img(i) {}

	void exec(const char *fname, bool turbo=false);

private:
	void turboTracer();
	void blockTracer();
	static bool getProgress(float *percent);

	const Scene &scene;
	Image *img;
};
