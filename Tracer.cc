#include "Tracer.h"

#include "Vector.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <cstdio>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

static volatile int next_block;
static int x_blocks, all_blocks;
static volatile int done_blocks;
static std::mutex mutex_block;
static std::condition_variable cond;

static float halton(int base, int n) {
	float ret = 0.0f;
	int b = base;

	while (n) {
		ret += (float)(n % base) / b;
		b *= base;
		n /= base;
	}
	return ret;
}

Vec3f viewVec(int x0, int y0, float dx, float dy) {
	float x = dx + x0;
	float y = dy + y0;
	return Vec3f(
		(x / (WIDTH / 2.0f)) - 1.0f,
		((HEIGHT / 2.0f) - y) / (WIDTH / 2.0f),
		-0.5f
	).norm();
}

static int getNextBlock(void) {
	std::unique_lock<std::mutex> lk{mutex_block};
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
		if (done_blocks == all_blocks) {
			lk.unlock();
			cond.notify_one();
		}
	}
	return ret;
}

void
Tracer::turboTracer(Tracer *tracer)
{
	int block;

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
				Ray r(Vec3f{}, viewVec(x, y, 0.5f, 0.5f));
				tracer->img->setPixel(x, y, tracer->scene.trace(r, DEPTH_LIMIT, 1.0f));
			}
		} while (++y < y1);

	}
}

void
Tracer::blockTracer(Tracer *tracer)
{
	int block;

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
			Ray r(Vec3f{}, viewVec(x, y, 0.0f, 0.0f));
			up[i] = tracer->scene.trace(r, DEPTH_LIMIT, 1.0f);
		}
		do {
			Ray r(Vec3f{}, viewVec(x0, y, 0.0, 1.0));
			Color left = tracer->scene.trace(r, DEPTH_LIMIT, 1.0f);
			i = 0;
			for (unsigned x = x0; x < x1; ++x, ++i) {
				r = Ray(Vec3f{}, viewVec(x, y, 1.0, 1.0));
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
Tracer::exec(const Scene &scene, Image *img, const char *fname, bool turbo)
{
	const int nthreads = std::thread::hardware_concurrency();
	Vector<Tracer, 32> tracers;
	Vector<std::thread, 32> threads;

	next_block = 0;
	done_blocks = 0;
	x_blocks = (img->getWidth() + BLOCKSIZE - 1) / BLOCKSIZE;
	all_blocks = ((img->getHeight() + BLOCKSIZE - 1) / BLOCKSIZE) * x_blocks;

	fprintf(stderr, "Spawning %d threads...\n", nthreads);
	for (int i = 0; i < nthreads; ++i) {
		tracers.emplace_back(scene, img);
		threads.emplace_back(
			turbo ? Tracer::turboTracer : Tracer::blockTracer,
			&tracers[i]
		);
	}

	for (;;) {
		std::unique_lock<std::mutex> lk(mutex_block);
		float p;

		if (!getProgress(&p)) break;

		fprintf(stderr, "\r%5.2f%% ", p);
		cond.wait_for(lk, std::chrono::seconds(1));
		if (fname) img->write(fname);
	}

	for (int i = 0; i < nthreads; ++i) {
		threads[i].join();
	}

	if (fname) img->write(fname);
}
