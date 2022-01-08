#ifndef _TARGA_H
#define _TARGA_H	1

#include "Image.h"

#include <vector>

class Targa : public Image {

	std::vector<unsigned char> data;
	Color min, max;
	bool hdrnorm;

public:

	Targa(unsigned width, unsigned height);

	void write(const char *fname) const;
	void setPixel(unsigned x, unsigned y, Color c);
	void computeCorrection(void);

};

#endif /* _TARGA_H */
