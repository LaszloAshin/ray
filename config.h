#ifndef _CONFIG_H
#define _CONFIG_H	1

// resolution of the output image
#define WIDTH		1280
#define HEIGHT		800

// number of samples per pixel
#define SAMPLES		16

#define BLOCKSIZE	32

#define EPSILON		0.001f

// depth limit of tracer recursion
#define DEPTH_LIMIT	16
#define RAY_ERROR	0.002f

#endif /* _CONFIG_H */
