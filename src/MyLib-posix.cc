#include "MyLib.h"

#include <cstddef>

char** environ;

static char* mygetenv(const char* name) {
	for (char** envp = environ; *envp; ++envp) {
		char* line = *envp;
		const char* n = name;
		while (*n && *n == *line) ++n, ++line;
		if (!*n && *line == '=') {
			return line + 1;
		}
	}
	return nullptr;
}

int mygetintfromenv(const char* name, int default_) {
	const char* p = mygetenv(name);
	return p ? myatoi(p) : default_;
}

extern "C" void __bzero(void* p, size_t len) {
	__asm __volatile__("rep stosb" :: "a"(0), "D"(p), "c"(len) : "memory");
}
