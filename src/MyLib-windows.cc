#include "MyLib.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static char* mygetenv(const char* name) {
	char buf[128];
	return GetEnvironmentVariable(name, buf, sizeof(buf)) ? buf : nullptr;
}

int mygetintfromenv(const char* name, int default_) {
	const char* p = mygetenv(name);
	return p ? myatoi(p) : default_;
}

void operator delete(void*) {}

extern "C" const int _fltused = 0;
