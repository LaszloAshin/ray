#include "MultithreadedTracer.h"

#include "Vector.h"

int
MultithreadedTracer::getNextBlock() {
	std::unique_lock<std::mutex> lk{mutex_block};
	return Tracer::getNextBlock();
}

void
MultithreadedTracer::consumeBlocks(bool turbo) {
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
