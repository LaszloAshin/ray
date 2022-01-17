#pragma once

#include "Color.h"

class Image {

protected:

	unsigned width, height;

public:

	Image(unsigned w, unsigned h) : width(w), height(h) {}
	virtual ~Image() {}

	virtual void write(const char *fname) const = 0;
	virtual void setPixel(unsigned x, unsigned y, Color c) = 0;
	virtual void computeCorrection(void) = 0;

	unsigned getWidth(void) { return width; }
	unsigned getHeight(void) { return height; }

};
