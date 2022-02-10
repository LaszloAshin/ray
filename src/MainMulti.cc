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
#ifdef __amd64__
			"mov %5, %%r10d\n\t"
			"syscall\n\t"
			: "=a"(result)
			: "0"(SYS_wait4), "D"(id), "S"(0), "d"(my__WCHILD), "r"(0)
			: "%rcx", "%r11", "memory", "%r10"
#else
			"int $0x80\n\t"
			: "=a"(result)
			: "0"(SYS_wait4), "b"(id), "c"(0), "d"(my__WCHILD), "S"(0)
			: "memory"
#endif
		);
	}

	static int hardware_concurrency() {
		int result;
#ifdef __amd64__
		uint64_t set;
		__asm __volatile__("\n\t"
			"syscall\n\t"
			"popcnt (%%rdx), %%rax"
			: "=a"(result)
			: "0"(SYS_sched_getaffinity), "D"(0), "S"(sizeof(set)), "d"(&set)
			: "%rcx", "%r11", "memory"
		);
#else
		uint32_t set;
		__asm __volatile__("\n\t"
			"int $0x80\n\t"
			"popcnt (%%edx), %%eax"
			: "=a"(result)
			: "0"(SYS_sched_getaffinity), "b"(0), "c"(sizeof(set)), "d"(&set)
			: "memory"
		);
#endif
		return result;
	}

private:
	int id;
};

#elif _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct MyThreadData {
	void (*func)(void*);
	void *arg;
};

DWORD WINAPI MyThreadThunk( LPVOID lpParam ) {
	MyThreadData* d = static_cast<MyThreadData*>(lpParam);
	(*d->func)(d->arg);
	return 0;
}

struct MyThread {
	MyThread(void (*func)(void*), void* arg) {
		MyThreadData d{ func, arg };
		th = CreateThread(NULL, 0, MyThreadThunk, &d, 0, NULL);
	}

	MyThread(const MyThread&) = delete;
	MyThread& operator=(const MyThread&) = delete;

	~MyThread() {
		myprint(""); // why does this prevent segfault?!
		WaitForSingleObject(th, INFINITE);
		CloseHandle(th);
	}

	static int hardware_concurrency() {
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		return sysinfo.dwNumberOfProcessors;
	}

private:
	HANDLE th;
};

#else

#include <thread>
#include <sys/sysctl.h>

struct MyThread {
	template <typename... Args>
	MyThread(Args&&... args) : th{std::forward<Args>(args)...} {}

	MyThread(const MyThread&) = delete;
	MyThread& operator=(const MyThread&) = delete;

	~MyThread() { th.join(); }

	static int hardware_concurrency() {
		int result;
		size_t len = sizeof(result);
		static const int name[2] = { CTL_HW, HW_NCPU };
		__asm __volatile__("\n\t"
			"movq %4, %%r10\n\t"
			"xor %%r8d, %%r8d\n\t"
			"xor %%r9d, %%r9d\n\t"
			"push %%rdx\n\t"
			"syscall\n\t"
			"pop %%rdx\n\t"
			:
			: "a"(MY_SYSCALL_NR(SYS_sysctl)), "D"(name), "S"(2), "d"(&result), "r"(&len)
			: "%rcx", "%r11", "memory", "%r10", "%r8", "%r9"
		);
		return result;
	}

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
		myprint("frame: "), myprint(frame), myprint("\n");
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

	myprint("Spawning "), myprint(nthreads), myprint(" threads...\n");
	for (int i = 0; i < nthreads; ++i) {
		threads.emplace_back(traceAntialiased, &tracer);
	}
}
