#ifndef _LIGHT_H
#define _LIGHT_H	1

#include "VectorMath.h"
#include "Color.h"

class Light {
public:
	const Vector pos;
	const Color c;

	Light(const Vector &p, const Color &color) :
		pos(p), c(color)
	{
	}
};

#endif /* _LIGHT_H */
