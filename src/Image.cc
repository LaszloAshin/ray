#define _CRT_SECURE_NO_WARNINGS // silence msvc about fopen
#include "Image.h"

#include <cassert>
#include <cstdio>

void
Image::write(const char *fname) const
{
	FILE* fp = fopen(fname, "wb");
	if (fp == NULL) {
		fprintf(stderr, "Image::write(): unable to open output file \"%s\"\n", fname);
		return;
	}

	fprintf(fp, "P6\n%d %d\n255\n", getWidth(), getHeight());
	fwrite(data.data(), data.size(), 1, fp);

	fprintf(stderr, "Output has been written to \"%s\"\n", fname);
	fclose(fp);
}

void
Image::setPixel(int x, int y, Color c)
{
	assert(x >= 0);
	assert(x < width);
	int offs = (y * width + x) * 3;
	assert(offs >= 0);
	assert(offs < data.size());
	c.clamp();

	data[offs++] = static_cast<unsigned char>(c.r * 255.0f);
	data[offs++] = static_cast<unsigned char>(c.g * 255.0f);
	data[offs] = static_cast<unsigned char>(c.b * 255.0f);
}
