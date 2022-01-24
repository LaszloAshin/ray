#pragma once

#include "Color.h"
#include "MappedWritableFile.h"

#include <cstdint>
#include <optional>

struct Image {
	Image(const char* fname, int width, int height);

	void setPixel(int x, int y, Color c);

	int getWidth() const { return width; }
	int getHeight() const { return height; }

private:
	std::optional<MappedWritableFile> map;
	uint8_t* data;
	int width;
	int height;
};
