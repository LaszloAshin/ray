#include "MappedWritableFile.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

MappedWritableFile::MappedWritableFile(const char* fname, int length)
: length_{length}
{
	HANDLE fh = CreateFileA(fname, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fh == INVALID_HANDLE_VALUE) {
		ExitProcess(1);
	}
	HANDLE mh = CreateFileMappingA(fh, NULL, PAGE_READWRITE, 0, length, NULL);
	CloseHandle(fh);
	if (mh == NULL) {
		ExitProcess(1);
	}
	address_ = static_cast<uint8_t*>(MapViewOfFile(mh, FILE_MAP_WRITE, 0, 0, 0));
	CloseHandle(mh);
	if (address_ == NULL) {
		ExitProcess(1);
	}
}

MappedWritableFile::~MappedWritableFile() {
	UnmapViewOfFile(address_);
}
