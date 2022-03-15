#include "MappedWritableFile.h"

#include "MyLib.h"

#include <cstdint>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <sys/syscall.h>

#ifdef __APPLE__
#define MY_SYSCALL_NR(N) (0x2000000 | (N))
#else
#define MY_SYSCALL_NR(N) (N)
#endif

inline void myexit(int status) {
	__asm __volatile__("\n\t"
#ifdef __amd64__
		"syscall\n\t"
		:
		: "a"(MY_SYSCALL_NR(SYS_exit)), "D"(status)
		: "%rcx", "%r11", "memory"
#else
		"int $0x80\n\t"
		:
		: "a"(SYS_exit), "b"(status)
		: "memory"
#endif
	);
}

inline int myclose(int fd) {
	int result;
	__asm __volatile__("\n\t"
#ifdef __amd64__
		"syscall\n\t"
		: "=a"(result)
		: "0"(MY_SYSCALL_NR(SYS_close)), "D"(fd)
		: "%rcx", "%r11", "memory"
#else
		"int $0x80\n\t"
		: "=a"(result)
		: "0"(SYS_close), "b"(fd)
		: "memory"
#endif
	);
	return result;
}

inline int myftruncate(int fd, long length) {
	int result;
	__asm __volatile__("\n\t"
#ifdef __amd64__
		"syscall\n\t"
		: "=a"(result)
		: "0"(MY_SYSCALL_NR(SYS_ftruncate)), "D"(fd), "S"(length)
		: "%rcx", "%r11", "memory"
#else
		"int $0x80\n\t"
		: "=a"(result)
		: "0"(SYS_ftruncate), "b"(fd), "c"(length)
		: "memory"
#endif
	);
	return result;
}

inline int myopen(const char* fname, int flags, unsigned mode) {
	int result;
	__asm __volatile__("\n\t"
#ifdef __amd64__
		"syscall\n\t"
		: "=a"(result)
		: "0"(MY_SYSCALL_NR(SYS_open)), "D"(fname), "S"(flags), "d"(mode)
		: "%rcx", "%r11", "memory"
#else
		"int $0x80\n\t"
		: "=a"(result)
		: "0"(SYS_open), "b"(fname), "c"(flags), "d"(mode)
		: "memory"
#endif
	);
	return result;
}

inline int mymunmap(void* addr, long length) {
	int result;
	__asm __volatile__("\n\t"
#ifdef __amd64__
		"syscall\n\t"
		: "=a"(result)
		: "0"(MY_SYSCALL_NR(SYS_munmap)), "D"(addr), "S"(length)
		: "%rcx", "%r11", "memory"
#else
		"int $0x80\n\t"
		: "=a"(result)
		: "0"(SYS_munmap), "b"(addr), "c"(length)
		: "memory"
#endif
	);
	return result;
}

inline void* mymmap(void* addr, long length, int prot, int flags, int fd, long offset) {
	void* result;
	__asm __volatile__("\n\t"
#ifdef __amd64__
		"movl %5, %%r10d\n\t"
		"movl %6, %%r8d\n\t"
		"movq %7, %%r9\n\t"
		"syscall\n\t"
		: "=a"(result)
		: "0"(MY_SYSCALL_NR(SYS_mmap)), "D"(addr), "S"(length), "d"(prot), "r"(flags), "r"(fd), "r"(offset)
		: "%rcx", "%r11", "memory", "%r10", "%r8", "%r9"
#else
		"push %%ebp\n\t"
		"mov %7, %%ebp\n\t"
		"int $0x80\n\t"
		"pop %%ebp\n\t"
		: "=a"(result)
		: "0"(SYS_mmap2), "b"(addr), "c"(length), "d"(prot), "S"(flags), "D"(fd), "r"(offset)
		: "memory"
#endif
	);
	return result;
}

MappedWritableFile::MappedWritableFile(int length)
#ifndef LEAK_RESOURCES_ATEXIT
: length_{length}
#endif
{
	const int fd = myopen("tracement.ppm", O_CREAT | O_RDWR, 0644);
	if (fd < 0) {
		myexit(2);
	}
	if (myftruncate(fd, length)) {
		myexit(3);
	}
	address_ = static_cast<uint8_t*>(mymmap(NULL, length, PROT_WRITE, MAP_SHARED, fd, 0));
	if (reinterpret_cast<unsigned long>(address_) > -4096UL) {
		myexit(4);
	}
#ifndef LEAK_RESOURCES_ATEXIT
	myclose(fd);
#endif
}

#ifndef LEAK_RESOURCES_ATEXIT
MappedWritableFile::~MappedWritableFile() {
	if (mymunmap(address_, length_)) {
		myprint("munmap\n");
	}
}
#endif
