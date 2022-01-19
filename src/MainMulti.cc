#include "Image.h"
#include "Scene.h"
#include "Tracer.h"
#include "Vector.h"

#include "config.h"

#include <cstdio>
#include <cstdlib> // atoi()
#include <mutex>
#include <thread>

struct MultithreadedTracer : public Tracer {
	using Tracer::Tracer;

private:
	int getNextBlock() override {
		std::unique_lock<std::mutex> lk{mutex_block};
		return Tracer::getNextBlock();
	}

	void consumeBlocks(bool turbo) override {
		const int nthreads = std::thread::hardware_concurrency();
		Vector<std::thread, 32> threads;

		fprintf(stderr, "Spawning %d threads...\n", nthreads);
		for (int i = 0; i < nthreads; ++i) {
			threads.emplace_back([this, turbo]{ Tracer::consumeBlocks(turbo); });
		}

		for (int i = 0; i < nthreads; ++i) {
			threads[i].join();
		}
	}

	std::mutex mutex_block;
};

int
main(int argc, char *argv[])
{
	int frame = 0;

	if (argc > 1) {
		frame = atoi(argv[1]);
		fprintf(stderr, "frame: %d\n", frame);
	}
	Scene scene(frame);
	Image img(WIDTH, HEIGHT);
	MultithreadedTracer{scene, &img}.exec("tracement.ppm");
}
