#pragma once

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
