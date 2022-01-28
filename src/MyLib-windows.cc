#include "MyLib.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

char* mygetenv(char*[], const char* name) {
	char buf[128] = {0};
	return GetEnvironmentVariable(name, buf, sizeof(buf)) ? buf : nullptr;
}

void operator delete(void*) {}
