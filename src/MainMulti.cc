#define _CRT_SECURE_NO_WARNINGS
#include "Image.h"
#include "MyLib.h"
#include "Scene.h"
#include "Tracer.h"
#include "Vector.h"

#include "config.h"

#include <cstdio>
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

		myprint("Spawning ", nthreads, " threads...\n");
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
		frame = myatoi(argv[1]);
		myprint("frame: ", frame, "\n");
	}
	Scene scene(frame);
	int width = WIDTH;
	if (const char* p = std::getenv("WIDTH")) {
		width = myatoi(p);
	}
	int height = HEIGHT;
	if (const char* p = std::getenv("HEIGHT")) {
		height = myatoi(p);
	}
	Image img("tracement.ppm", width, height);
	MultithreadedTracer{scene, &img}.exec();
}
