#pragma once

#include "VectorMath.h"
#include "Color.h"

class Light {
public:
	const Vec3f pos;
	const Color c;

	Light(const Vec3f &p, const Color &color) :
		pos(p), c(color)
	{
	}
};
