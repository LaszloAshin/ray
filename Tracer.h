#ifndef _TRACER_H
#define _TRACER_H	1

#include "Scene.h"
#include "Image.h"

class Tracer {

	static int next_block, x_blocks, all_blocks, done_blocks;
	static pthread_mutex_t mutex_block;

	int th;
	const Scene &scene;
	Image *img;

	static float halton(int base, int n);
	static Vector viewVec(int x0, int y0, float dx, float dy);

	static int getNextBlock(void);
	static void *turboTracer(void *p);
	static void *blockTracer(void *p);
	static bool getProgress(float *percent);

public:

	static void exec(const Scene &scene, Image *img, const char *fname, int nthreads, bool turbo=false);

	Tracer(int n, const Scene &s, Image *i) :
		scene(s), img(i)
	{
		th = n;
	}

};

#endif /* _TRACER_H */
