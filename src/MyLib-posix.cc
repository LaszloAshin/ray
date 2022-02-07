#include "MyLib.h"

#include <cstddef>

char* mygetenv(char* envp[], const char* name) {
	for (; *envp; ++envp) {
		char* line = *envp;
		const char* n = name;
		while (*n && *n == *line) ++n, ++line;
		if (!*n && *line == '=') {
			return line + 1;
		}
	}
	return nullptr;
}

extern "C" void __bzero(void* p, size_t len) {
	__asm __volatile__("rep stosb" :: "a"(0), "D"(p), "c"(len) : "memory");
}
