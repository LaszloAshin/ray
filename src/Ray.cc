#include "Image.h"
#include "Targa.h"
#include "Scene.h"
#include "MultithreadedTracer.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <cstdio>
#include <cstdlib> // atoi()

// Enable HDR (High Dynamic Range) computations
//#define USE_HDR 1

#define FNAME		"tracement.tga"

int
main(int argc, char *argv[])
{
	int frame = 0;

	if (argc > 1) {
		frame = atoi(argv[1]);
		fprintf(stderr, "frame: %d\n", frame);
	}
	Scene scene(frame);
	Targa img(WIDTH, HEIGHT);

	MultithreadedTracer tracer{scene, &img};
#ifdef USE_HDR
	fprintf(stderr, "Starting pass #1\n");
	tracer.exec(FNAME, true);
	img.computeCorrection();
	fprintf(stderr, "Starting pass #2\n");
#endif /* USE_HDR */
	tracer.exec(FNAME);
}
