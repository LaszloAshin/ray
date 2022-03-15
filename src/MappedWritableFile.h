#pragma once

#include "config.h"

struct MappedWritableFile {
	MappedWritableFile(int length);
	MappedWritableFile(const MappedWritableFile&) = delete;
	MappedWritableFile& operator=(const MappedWritableFile&) = delete;
#ifndef LEAK_RESOURCES_ATEXIT
	~MappedWritableFile();
#endif

	void* address() { return address_; }

private:
	void* address_;
#ifndef LEAK_RESOURCES_ATEXIT
	int length_;
#endif
};
