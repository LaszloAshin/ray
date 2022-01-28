#include "Image.h"
#include "MyLib.h"
#include "Scene.h"
#include "Tracer.h"
#include "Vector.h"

#include "config.h"

#ifdef __linux__

#include <thread>

struct MyThread {
	template <class F>
	MyThread(F&& f) : th{f} {}

	MyThread(const MyThread&) = delete;
	MyThread& operator=(const MyThread&) = delete;

	~MyThread() { th.join(); }

	static int hardware_concurrency() {
		int result;
		uint64_t set;
		__asm __volatile__("\n\t"
			"syscall\n\t"
			"popcnt (%%rdx), %%rax"
			: "=a"(result)
			: "0"((long)SYS_sched_getaffinity), "D"(0), "S"(sizeof(set)), "d"(&set)
			: "%rcx", "%r11", "memory"
		);
		return result;
	}

private:
	std::thread th;
};

#else // !__linux__

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

#endif

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
	Vector<MyThread, 64> threads;

	myprint("Spawning ", nthreads, " threads...\n");
	for (int i = 0; i < nthreads; ++i) {
		threads.emplace_back([&]{ tracer.traceAntialiased(); });
	}
}
