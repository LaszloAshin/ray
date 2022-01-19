#pragma once

#include "Color.h"

#include <cstdint>
#include <vector>

struct Image {
	Image(int width, int height) : data(width * height * 3), width{width} {}

	void write(const char* fname) const;
	void setPixel(int x, int y, Color c);

	int getWidth() const { return width; }
	int getHeight() const { return static_cast<int>(data.size()) / (width * 3); }

private:
	std::vector<uint8_t> data;
	int width;
};
