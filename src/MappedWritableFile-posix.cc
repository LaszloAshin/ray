#include "MappedWritableFile.h"

#include "MyLib.h"

#include <cstdint>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#ifdef __linux__

#include <sys/syscall.h>

inline void myexit(int status) {
	__asm __volatile__(
		"syscall;"
		:
		: "a"((long)SYS_exit), "D"(status)
		: "%rcx", "%r11", "memory"
	);
}

inline int myclose(int fd) {
	int result;
	__asm __volatile__(
		"syscall;"
		: "=a"(result)
		: "0"((long)SYS_close), "D"(fd)
		: "%rcx", "%r11", "memory"
	);
	return result;
}

inline int myftruncate(int fd, long length) {
	int result;
	__asm __volatile__(
		"syscall;"
		: "=a"(result)
		: "0"((long)SYS_ftruncate), "D"(fd), "S"(length)
		: "%rcx", "%r11", "memory"
	);
	return result;
}

#else

#include <unistd.h>

#define myexit _exit
#define myclose close
#define myftruncate ftruncate

#endif

MappedWritableFile::MappedWritableFile(const char* fname, int length)
: length_{length}
{
	const int fd = open(fname, O_CREAT | O_RDWR, 0644);
	if (fd == -1) {
		myprint("Fail: open\n");
		myexit(1);
	}
	if (myftruncate(fd, length)) {
		myprint("Fail: ftruncate\n");
		myexit(1);
	}
	address_ = static_cast<uint8_t*>(mmap(NULL, length, PROT_WRITE, MAP_SHARED, fd, 0));
	if (address_ == MAP_FAILED) {
		myprint("Fail: mmap\n");
		myexit(1);
	}
	myclose(fd);
}

MappedWritableFile::~MappedWritableFile() {
	if (munmap(address_, length_)) {
		myprint("Fail: munmap\n");
	}
}
