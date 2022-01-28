#include "Image.h"
#include "MyLib.h"
#include "Scene.h"
#include "Tracer.h"
#include "Vector.h"

#include "config.h"

#include <thread>

struct MultithreadedTracer : public Tracer {
	using Tracer::Tracer;

private:
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
};

int
main(int argc, char *argv[], char *envp[])
{
	int frame = 0;

	if (argc > 1) {
		frame = myatoi(argv[1]);
		myprint("frame: ", frame, "\n");
	}
	Scene scene(frame);
	int width = WIDTH;
	if (const char* p = mygetenv(envp, "WIDTH")) {
		width = myatoi(p);
	}
	int height = HEIGHT;
	if (const char* p = mygetenv(envp, "HEIGHT")) {
		height = myatoi(p);
	}
	Image img("tracement.ppm", width, height);
	MultithreadedTracer{scene, &img}.exec();
}
