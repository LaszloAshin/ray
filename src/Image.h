#pragma once

#include "Color.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>

struct Image {
	Image(int width, int height)
	: data{static_cast<uint8_t*>(malloc(width * height * 3))}
	, width{width}
	, height{height}
	{
		assert(data != nullptr);
	}

	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;
	~Image() { free(data); }

	void write(const char* fname) const;
	void setPixel(int x, int y, Color c);

	int getWidth() const { return width; }
	int getHeight() const { return height; }

private:
	uint8_t* data;
	int width;
	int height;
};
