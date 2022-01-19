#pragma once

#include "Image.h"

#include <vector>

class Targa : public Image {

	std::vector<unsigned char> data;

public:

	Targa(unsigned width, unsigned height);

	void write(const char *fname) const;
	void setPixel(unsigned x, unsigned y, Color c);

};
