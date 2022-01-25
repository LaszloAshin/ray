#include "MappedWritableFile.h"

#include "MyLib.h"

#include <cstdint>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

MappedWritableFile::MappedWritableFile(const char* fname, int length)
: length_{length}
{
	const int fd = open(fname, O_CREAT | O_RDWR, 0644);
	if (fd == -1) {
		myprint("Fail: open\n");
		_exit(1);
	}
	if (ftruncate(fd, length)) {
		myprint("Fail: ftruncate\n");
		_exit(1);
	}
	address_ = static_cast<uint8_t*>(mmap(NULL, length, PROT_WRITE, MAP_SHARED, fd, 0));
	if (address_ == MAP_FAILED) {
		myprint("Fail: mmap\n");
		_exit(1);
	}
	close(fd);
}

MappedWritableFile::~MappedWritableFile() {
	if (munmap(address_, length_)) {
		myprint("Fail: munmap\n");
	}
}
