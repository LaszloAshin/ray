#pragma once

#include "Geometry.h"
#include "Color.h"

struct Light {
	Vec3f pos;
	Color c;

	constexpr Light(const Vec3f &p, const Color &color) : pos{p}, c{color} {}
};
