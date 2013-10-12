#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstring>
#include <cstdlib>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "Targa.h"

using namespace std;

Targa::Targa(unsigned width, unsigned height) :
	Image(width, height)
{
	data = new unsigned char[width * height * 3];
	memset(data, 0xff, width * height * 3);
	min = Color(1.0f, 1.0f, 1.0f);
	max = Color(0.0f, 0.0f, 0.0f);
	hdrnorm = false;
}

Targa::~Targa()
{
	delete[] data;
}

void
Targa::write(const char *fname) const
{
	unsigned char hd[18];
	static const char magic[] = "\0\0\0\0\0\0\0\0TRUEVISION-XFILE.";

	ofstream f(fname, ios::out | ios::binary);
	if (!f) {
		cerr << "Targa::Write(): unable to open output file ";
		cerr << fname << endl;
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
	f.write((char *)hd, sizeof(hd));
	f.write((char *)data, width * height * 3);
	f.write(magic, sizeof(magic));

	f.close();
	cerr << "Output has been written to " << fname << endl;
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

	const float m = 255.0f - EPSILON;
	unsigned char r = (unsigned char)(c.r * m);
	if ((rand() & 0xffff) < 65536.0f * (255.0f * c.r - r))
		++r;

	unsigned char g = (unsigned char)(c.g * m);
	if ((rand() & 0xffff) < 65536.0f * (255.0f * c.g - g))
		++g;

	unsigned char b = (unsigned char)(c.b * m);
	if ((rand() & 0xffff) < 65536.0f * (255.0f * c.b - b))
		++b;

	data[offs++] = b;
	data[offs++] = g;
	data[offs] = r;
}

void
Targa::computeCorrection(void)
{
	cerr << "HDR correction values:" << endl;
	cerr << "min: " << min << endl;
	cerr << "max: " << max << endl;
	if (min.r < 0.0f) min.r = 0.0f;
	if (min.g < 0.0f) min.g = 0.0f;
	if (min.b < 0.0f) min.b = 0.0f;
	max = max - min;
	max.r = 1.0f / max.r;
	max.g = 1.0f / max.g;
	max.b = 1.0f / max.b;
	cerr << "corr: " << max << endl;
	hdrnorm = true;
}
