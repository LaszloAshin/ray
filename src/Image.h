#pragma once

#include "Color.h"
#include "MappedWritableFile.h"

#include <cstdint>

struct Image {
	Image(const char* fname, int width, int height);

	void setPixel(int x, int y, Color c);

	int getWidth() const { return width; }
	int getHeight() const { return height; }

private:
	struct NetpbmHeader {
		NetpbmHeader(int width, int height);

		int size() const;
		const char* data() const { return buf; }

	private:
		char buf[64];
	};

	NetpbmHeader header;
	MappedWritableFile map;
	uint8_t* data;
	int width;
	int height;
};
