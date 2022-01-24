#include "Image.h"

#include <cassert>
#include <cstdio>
#include <cstring>

Image::NetpbmHeader::NetpbmHeader(int width, int height) {
	snprintf(buf, sizeof(buf), "P6\n%5d %5d\n255\n", width, height);
}

int Image::NetpbmHeader::size() const {
	return static_cast<int>(strlen(buf));
}

Image::Image(const char* fname, int width, int height)
: header{width, height}
, map{fname, header.size() + width * height * 3}
, width{width}
, height{height}
{
	data = static_cast<uint8_t*>(map.address());
	for (int i = 0; i < header.size(); ++i) {
		*data++ = header.data()[i];
	}
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

	data[offs++] = static_cast<uint8_t>(c.r * 255.0f);
	data[offs++] = static_cast<uint8_t>(c.g * 255.0f);
	data[offs] = static_cast<uint8_t>(c.b * 255.0f);
}
