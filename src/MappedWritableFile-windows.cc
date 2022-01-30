#include "MappedWritableFile.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

MappedWritableFile::MappedWritableFile(const char* fname, int length)
#ifndef LEAK_RESOURCES_ATEXIT
: length_{length}
#endif
{
	HANDLE fh = CreateFileA(fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fh == INVALID_HANDLE_VALUE) {
		ExitProcess(1);
	}
	HANDLE mh = CreateFileMappingA(fh, NULL, PAGE_READWRITE, 0, length, NULL);
	if (mh == NULL) {
		ExitProcess(1);
	}
	address_ = MapViewOfFile(mh, FILE_MAP_WRITE, 0, 0, 0);
#ifndef LEAK_RESOURCES_ATEXIT
	CloseHandle(fh);
	CloseHandle(mh);
#endif
	if (address_ == NULL) {
		ExitProcess(1);
	}
}

#ifndef LEAK_RESOURCES_ATEXIT
MappedWritableFile::~MappedWritableFile() {
	UnmapViewOfFile(address_);
}
#endif
