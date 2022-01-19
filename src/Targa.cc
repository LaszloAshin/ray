#define _CRT_SECURE_NO_WARNINGS
#include "Targa.h"

#include "config.h"

#include <cassert>
#include <cstdio>

Targa::Targa(unsigned width, unsigned height) :
	Image(width, height),
	data(width * height * 3)
{
}

struct File {
	File(FILE* fp) : fp{fp} {}
	~File() { if (isValid()) fclose(fp); }
	File(const File&) = delete;
	File& operator=(const File&) = delete;

	bool isValid() const { return fp != NULL; }

	size_t write(const void* buf, size_t count) {
		return fwrite(buf, count, 1, fp);
	}

private:
	FILE* fp;
};

void
Targa::write(const char *fname) const
{
	unsigned char hd[18] = { 0 };
	static const char magic[] = "\0\0\0\0\0\0\0\0TRUEVISION-XFILE.";

	File f(fopen(fname, "wb"));
	if (!f.isValid()) {
		fprintf(stderr, "Targa::Write(): unable to open output file \"%s\"\n", fname);
		return;
	}

//	hd[2] = (bytes < 3) ? 0x03 : 0x02;
	hd[2] = 0x02;
	hd[12] = (unsigned char)width;
	hd[13] = (unsigned char)(width >> 8);
	hd[14] = (unsigned char)height;
	hd[15] = (unsigned char)(height >> 8);
	hd[16] = 24;
//	hd[17] = 0x20;
//	hd[17] |= 0x0f & ((bytes < 3) ? bytes - 1 : bytes - 3);
	hd[17] = 0x20;
	f.write(hd, sizeof(hd));
	f.write(data.data(), data.size());
	f.write(magic, sizeof(magic));

	fprintf(stderr, "Output has been written to \"%s\"\n", fname);
}

void
Targa::setPixel(unsigned x, unsigned y, Color c)
{
	assert(x < width);
	assert(y < height);
	unsigned offs = (y * width + x) * 3;
	c.clamp();

	data[offs++] = static_cast<unsigned char>(c.b * 255.0f);
	data[offs++] = static_cast<unsigned char>(c.g * 255.0f);
	data[offs] = static_cast<unsigned char>(c.r * 255.0f);
}
