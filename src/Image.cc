#include "Image.h"

#include "MyLib.h"

#include <cassert>

Image::Image(const char* fname, int width, int height)
: width{width}
, height{height}
{
	char buf[64];
	const int headerlen = mysnprint(buf, sizeof(buf), "P6\n", width, " ", height, "\n255\n");
	map.emplace(fname, headerlen + width * height * 3);
	data = static_cast<uint8_t*>(map->address());
	for (int i = 0; i < headerlen; ++i) {
		*data++ = buf[i];
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
