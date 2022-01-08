/**
 * Ray.cpp
 */

#include <iostream>
#include <stdlib.h> /* atoi() */
using namespace std;

#include "Image.h"
#include "Targa.h"
#include "Scene.h"
#include "Tracer.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

// Enable HDR (High Dynamic Range) computations
//#define USE_HDR 1

#define FNAME		"tracement.tga"

#include <sstream>

int
main(int argc, char *argv[])
{
	int frame = 0;

	srand(time(NULL));

	if (argc > 1) {
		frame = atoi(argv[1]);
		cerr << "frame: " << frame << endl;
	}
	Scene scene(frame);
	Targa img(WIDTH, HEIGHT);

#ifdef USE_HDR
	cerr << "Starting pass #1" << endl;
	Tracer::exec(scene, &img, FNAME, THREADS, true);
	output.computeCorrection();
	cerr << "Starting pass #2" << endl;
#endif /* USE_HDR */
	Tracer::exec(scene, &img, FNAME, THREADS);
}
