#ifndef _TARGA_H
#define _TARGA_H	1

#include "Image.h"

class Targa : public Image {

	unsigned char *data;
	Color min, max;
	bool hdrnorm;

public:

	Targa(unsigned width, unsigned height);
	~Targa();

	void write(const char *fname) const;
	void setPixel(unsigned x, unsigned y, Color c);
	void computeCorrection(void);

};

#endif /* _TARGA_H */
