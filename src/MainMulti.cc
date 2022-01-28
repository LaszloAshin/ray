#include "Image.h"
#include "MyLib.h"
#include "Scene.h"
#include "Tracer.h"
#include "Vector.h"

#include "config.h"

#include <thread>

struct MyThread {
	template <class F>
	MyThread(F&& f) : th{std::forward<F>(f)} {}

	MyThread(const MyThread&) = delete;
	MyThread& operator=(const MyThread&) = delete;

	~MyThread() { th.join(); }

	static int hardware_concurrency() { return std::thread::hardware_concurrency(); }

private:
	std::thread th;
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
	Tracer tracer{scene, &img};
	const int nthreads = MyThread::hardware_concurrency();
	Vector<MyThread, 32> threads;

	myprint("Spawning ", nthreads, " threads...\n");
	for (int i = 0; i < nthreads; ++i) {
		threads.emplace_back([&]{ tracer.traceAntialiased(); });
	}
}
