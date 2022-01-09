#ifdef __WIN32__
/* This needs to go first before stdio is included. */
# include <windows.h>
# if !defined(__CYGWIN32__)
#  define sleep(x) Sleep((x) * 1000)
# endif
#endif

#include <iostream>
#include <pthread.h>
#include "Tracer.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <cstdio>
#include <unistd.h>
#include <vector>

using namespace std;

int Tracer::next_block;
int Tracer::x_blocks, Tracer::all_blocks;
int Tracer::done_blocks;
pthread_mutex_t Tracer::mutex_block;

float
Tracer::halton(int base, int n)
{
	float ret = 0.0f;
	int b = base;

	while (n) {
		ret += (float)(n % base) / b;
		b *= base;
		n /= base;
	}
	return ret;
}

Vector
Tracer::viewVec(int x0, int y0, float dx, float dy)
{
	float x = dx + x0;
	float y = dy + y0;
	return Vector(
		(x / (WIDTH / 2.0f)) - 1.0f,
		((HEIGHT / 2.0f) - y) / (WIDTH / 2.0f),
		-0.5f
	).norm();
}

int
Tracer::getNextBlock(void)
{
	pthread_mutex_lock(&mutex_block);
	int ret = next_block;
	if (ret < all_blocks) {
		int y = next_block / x_blocks;
		int x = next_block % x_blocks;

		if (!(y & 1)) {
			if (x + 1 == x_blocks)
				next_block += x_blocks;
			else
				++next_block;
		} else {
			if (!x)
				next_block += x_blocks;
			else
				--next_block;
		}
		++done_blocks;
	}
	pthread_mutex_unlock(&mutex_block);
	return ret;
}

void *
Tracer::turboTracer(void *p)
{
	int block;

	Tracer *tracer = (Tracer *)p;

	while ((block = getNextBlock()) < all_blocks) {
		unsigned y  = (block / x_blocks) * BLOCKSIZE;
		unsigned x0 = (block % x_blocks) * BLOCKSIZE;
		unsigned x1 = x0 + BLOCKSIZE;
		unsigned y1 = y + BLOCKSIZE;
		if (x1 > tracer->img->getWidth())
			x1 = tracer->img->getWidth();
		if (y1 > tracer->img->getHeight())
			y1 = tracer->img->getHeight();

		do {
			for (unsigned x = x0; x < x1; ++x) {
				Ray r(Vector{}, viewVec(x, y, 0.5f, 0.5f));
				tracer->img->setPixel(x, y, tracer->scene.trace(r, DEPTH_LIMIT, 1.0f));
			}
		} while (++y < y1);

	}

	pthread_exit(NULL);
	return NULL;
}

void *
Tracer::blockTracer(void *p)
{
	int block;

	Tracer *tracer = (Tracer *)p;

	while ((block = getNextBlock()) < all_blocks) {
		unsigned y  = (block / x_blocks) * BLOCKSIZE;
		unsigned x0 = (block % x_blocks) * BLOCKSIZE;
		unsigned x1 = x0 + BLOCKSIZE;
		unsigned y1 = y + BLOCKSIZE;

		if (x1 > tracer->img->getWidth()) {
			x1 = tracer->img->getWidth();
		}
		if (y1 > tracer->img->getHeight()) {
			y1 = tracer->img->getHeight();
		}

		Color up[BLOCKSIZE + 1];
		int i = 0;
		for (unsigned x = x0; x <= x1; ++x, ++i) {
			Ray r(Vector{}, viewVec(x, y, 0.0f, 0.0f));
			up[i] = tracer->scene.trace(r, DEPTH_LIMIT, 1.0f);
		}
		do {
			Ray r(Vector{}, viewVec(x0, y, 0.0, 1.0));
			Color left = tracer->scene.trace(r, DEPTH_LIMIT, 1.0f);
			i = 0;
			for (unsigned x = x0; x < x1; ++x, ++i) {
				r = Ray(Vector{}, viewVec(x, y, 1.0, 1.0));
				Color right = tracer->scene.trace(r, DEPTH_LIMIT, 1.0f);
				
				Color c;
				if ((up[i].dist(right) < 0.001f) && (up[i+1].dist(left) < 0.001f)) {
					c = (up[i] + up[i+1] + left + right) * 0.25f;
				} else {
					for (int j = 0; j < SAMPLES; ++j) {
						float hx = halton(2, j + 1);
						float hy = halton(3, j + 1);
						r.d = viewVec(x, y, hx, hy);
						c += tracer->scene.trace(r, DEPTH_LIMIT, 1.0f);
					}
					c = c * (1.0f / SAMPLES);
				}
				tracer->img->setPixel(x, y, c);

				up[i] = left;
				left = right;
			}
			up[i] = left;
		} while (++y < y1);

	}

	pthread_exit(NULL);
	return NULL;
}

bool
Tracer::getProgress(float *percent)
{
	if (percent) {
		*percent = (float)done_blocks * 100.0f / all_blocks;
	}
	return next_block < all_blocks;
}

void
Tracer::exec(const Scene &scene, Image *img, const char *fname, int nthreads, bool turbo)
{
	std::vector<std::unique_ptr<Tracer>> tracers(nthreads);
	std::vector<pthread_t> threads(nthreads);
	pthread_attr_t attr;
	int sec;
	int n;

	next_block = 0;
	done_blocks = 0;
	x_blocks = (img->getWidth() + BLOCKSIZE - 1) / BLOCKSIZE;
	all_blocks = ((img->getHeight() + BLOCKSIZE - 1) / BLOCKSIZE) * x_blocks;
	pthread_mutex_init(&mutex_block, NULL);
	sec = 0;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	n = 0;
	for (int i = 0; i < nthreads; ++i) {
		tracers[i] = std::make_unique<Tracer>(i, scene, img);
		int r = pthread_create(
			&threads[i], &attr,
			turbo ? Tracer::turboTracer : Tracer::blockTracer,
			tracers[i].get()
		);
		if (r) {
			cerr << "unable to create thread #" << i << endl;
			tracers[i].reset();
			continue;
		}
		++n;
	}
	pthread_attr_destroy(&attr);

	if (!n) {
		cerr << "There are no usable threads." << endl;
		goto free_it;
	}

	sleep(1), ++sec;
	for (;;) {
		float p;

		if (!getProgress(&p)) break;

		fprintf(stderr, "\r%5.2f%% ", p);
		sleep(1), ++sec;
		if (fname && !(sec % 10) && p < 95.0)
			img->write(fname);
	}

	for (int i = 0; i < nthreads; ++i) {
		if (!tracers[i]) continue;
		int r = pthread_join(threads[i], NULL);
		if (r) {
			cerr << "unable to join thread #" << i << endl;
		}
		tracers[i].reset();
	}

free_it:pthread_mutex_destroy(&mutex_block);

	if (fname) img->write(fname);
}
