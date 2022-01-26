#include "MyLib.h"

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
