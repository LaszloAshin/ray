#include "Targa.h"
#include "Scene.h"
#include "MultithreadedTracer.h"

#include "config.h"

#include <cstdio>
#include <cstdlib> // atoi()

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
	MultithreadedTracer{scene, &img}.exec("tracement.tga");
}
