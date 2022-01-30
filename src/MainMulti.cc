#include "Image.h"
#include "MyLib.h"
#include "Scene.h"
#include "Tracer.h"
#include "Vector.h"

#include "config.h"

#ifdef __linux__

extern "C" int create_thread(void (*func)(void*), void* arg);

struct MyThread {
	MyThread(void (*func)(void*), void* arg) : id{create_thread(func, arg)} {}

	MyThread(const MyThread&) = delete;
	MyThread& operator=(const MyThread&) = delete;

	~MyThread() {
		int result;
		const unsigned my__WCHILD = 0x80000000;
		__asm __volatile__("\n\t"
			"mov %5, %%r10d\n\t"
			"syscall\n\t"
			: "=a"(result)
			: "0"(SYS_wait4), "D"(id), "S"(0), "d"(my__WCHILD), "r"(0)
			: "%rcx", "%r11", "memory", "%r10"
		);
	}

	static int hardware_concurrency() {
		int result;
		uint64_t set;
		__asm __volatile__("\n\t"
			"syscall\n\t"
			"popcnt (%%rdx), %%rax"
			: "=a"(result)
			: "0"(SYS_sched_getaffinity), "D"(0), "S"(sizeof(set)), "d"(&set)
			: "%rcx", "%r11", "memory"
		);
		return result;
	}

private:
	int id;
};

#else // !__linux__

#include <thread>

struct MyThread {
	template <typename... Args>
	MyThread(Args&&... args) : th{std::forward<Args>(args)...} {}

	MyThread(const MyThread&) = delete;
	MyThread& operator=(const MyThread&) = delete;

	~MyThread() { th.join(); }

	static int hardware_concurrency() { return std::thread::hardware_concurrency(); }

private:
	std::thread th;
};

#endif

static void traceAntialiased(void* t) { static_cast<Tracer*>(t)->traceAntialiased(); }

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
		threads.emplace_back(traceAntialiased, &tracer);
	}
}
