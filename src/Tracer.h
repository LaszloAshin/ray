#pragma once

#include "Image.h"
#include "Scene.h"

struct Tracer {
	Tracer(const Scene &s, Image *i) : scene(s), img(i) {}

	void exec(const char *fname, bool turbo=false);

protected:
	virtual int getNextBlock();
	virtual void consumeBlocks(bool turbo);

private:
	void turboTracer();
	void blockTracer();

	const Scene &scene;
	Image *img;
	volatile int next_block;
	int x_blocks, all_blocks;
	volatile int done_blocks;
};
