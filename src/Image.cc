#include "Image.h"

#include "MyLib.h"

#include <cassert>

Image::Image(int width, int height)
: width{width}
, height{height}
{
	static const char header_template[] = "P6\n                     \n255\n";
	const int headerlen = sizeof(header_template) - 1;
	map.emplace(headerlen + width * height * 3);
	data = static_cast<uint8_t*>(map->address());
	for (int i = 0; i < headerlen; ++i) {
		data[i] = header_template[i];
	}
	overwrite_int(data + 12, width);
	overwrite_int(data + 23, height);
	data += headerlen;
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
