#pragma once

inline int myatoi(const char* nptr) {
	int result = 0;
	for (; *nptr >= '0' && *nptr <= '9'; ++nptr) {
		result = result * 10 + *nptr - '0';
	}
	return result;
}

char* mygetenv(char* envp[], const char* name);

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

inline int mywrite(const void* p, long size) {
	DWORD wr;
	return WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), p, size, &wr, NULL) ? wr : 0;
}

#else

#include <sys/syscall.h>

#ifdef __APPLE__
#define MY_SYSCALL_NR(N) (0x2000000 | (N))
#else
#define MY_SYSCALL_NR(N) (N)
#endif

inline int mywrite(const void* p, int size) {
	int result;
	int fd = 1;
	__asm __volatile__("\n\t"
#ifdef __amd64__
		"syscall\n\t"
		: "=a"(result)
		: "0"(MY_SYSCALL_NR(SYS_write)), "D"(fd), "S"(p), "d"(size)
		: "%rcx", "%r11", "memory"
#else
		"int $0x80\n\t"
		: "=a"(result)
		: "0"(SYS_write), "b"(fd), "c"(p), "d"(size)
		: "memory"
#endif
	);
	return result;
}

#endif

template <int S>
inline void myprint(const char (&s)[S]) {
	mywrite(s, S - 1);
}

inline void* overwrite_int(void* p, int i) {
	char* q = static_cast<char*>(p);
	for (; i > 0; i /= 10) {
		*q-- = '0' + i % 10;
	}
	return q;
}

inline void myprint(int i) {
	char buf[12];
	char* begin = static_cast<char*>(overwrite_int(buf + sizeof(buf) - 1, i)) + 1;
	mywrite(begin, buf + sizeof(buf) - begin);
}
