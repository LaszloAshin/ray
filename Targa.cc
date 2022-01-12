#include "Targa.h"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

Targa::Targa(unsigned width, unsigned height) :
	Image(width, height),
	data(width * height * 3)
{
	memset(data.data(), 0xff, data.size());
	min = Color(1.0f, 1.0f, 1.0f);
	max = Color(0.0f, 0.0f, 0.0f);
	hdrnorm = false;
}

struct File {
	File(int fd) : fd{fd} {}
	~File() { if (isValid()) close(fd); }
	File(const File&) = delete;
	File& operator=(const File&) = delete;

	bool isValid() const { return fd != -1; }

	ssize_t write(const void* buf, size_t count) {
		return ::write(fd, buf, count);
	}

private:
	int fd;
};

void
Targa::write(const char *fname) const
{
	unsigned char hd[18];
	static const char magic[] = "\0\0\0\0\0\0\0\0TRUEVISION-XFILE.";

	File f(open(fname, O_CREAT | O_WRONLY | O_TRUNC, 0644));
	if (!f.isValid()) {
		fprintf(stderr, "Targa::Write(): unable to open output file \"%s\"\n", fname);
		return;
	}

	memset(hd, 0, sizeof(hd));

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
#ifdef DEBUG
	assert(x >= 0);
	assert(x < width);
	assert(y >= 0);
	assert(y < height);
#endif /* DEBUG */
	unsigned offs = (y * width + x) * 3;
	if (!hdrnorm) {
		if (c.r < min.r) min.r = c.r;
		if (c.g < min.g) min.g = c.g;
		if (c.b < min.b) min.b = c.b;
		if (c.r > max.r) max.r = c.r;
		if (c.g > max.g) max.g = c.g;
		if (c.b > max.b) max.b = c.b;
	} else {
/*		c.r = (c.r - min.r) * max.r;
		c.g = (c.g - min.g) * max.g;
		c.b = (c.b - min.b) * max.b;*/
		c.r = log10(((c.r - min.r) * max.r * 9.0f) + 1.0f);
		c.g = log10(((c.g - min.g) * max.g * 9.0f) + 1.0f);
		c.b = log10(((c.b - min.b) * max.b * 9.0f) + 1.0f);
	}
	c.clamp();

	data[offs++] = c.b * 255.0f;
	data[offs++] = c.g * 255.0f;
	data[offs] = c.r * 255.0f;
}

void
Targa::computeCorrection(void)
{
	if (min.r < 0.0f) min.r = 0.0f;
	if (min.g < 0.0f) min.g = 0.0f;
	if (min.b < 0.0f) min.b = 0.0f;
	max = max - min;
	max.r = 1.0f / max.r;
	max.g = 1.0f / max.g;
	max.b = 1.0f / max.b;
	hdrnorm = true;
}
