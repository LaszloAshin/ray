#pragma once

struct MappedWritableFile {
	MappedWritableFile(const char* fname, int length);
	MappedWritableFile(const MappedWritableFile&) = delete;
	MappedWritableFile& operator=(const MappedWritableFile&) = delete;
	~MappedWritableFile();

	void* address() { return address_; }

private:
	void* address_{};
	int length_;
};
