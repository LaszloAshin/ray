#pragma once

#include "Tracer.h"

#include <thread>
#include <mutex>

struct MultithreadedTracer : public Tracer {
	using Tracer::Tracer;

private:
	int getNextBlock() override;
	void consumeBlocks(bool turbo) override;

	std::mutex mutex_block;
};
