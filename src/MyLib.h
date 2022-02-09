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
inline int mysnprintOne(char* buf, int size, const char (&s)[S]) {
	if (S < size) size = S;
	for (int i = 0; i < size; ++i) {
		*buf++ = s[i];
	}
	return size - 1;
}

int mysnprintOne(char* buf, int size, int value);

inline int mysnprint(char*, int) { return 0; }

template <typename T, typename... Args>
int mysnprint(char* buf, int size, const T& value, const Args&... args) {
	const auto n = mysnprintOne(buf, size, value);
	return n + mysnprint(buf + n, size - n, args...);
}

#define MYPRINT(...) do { \
		char buf[256]; \
		const int n = mysnprint(buf, sizeof(buf), __VA_ARGS__); \
		mywrite(buf, n); \
	} while (0)
