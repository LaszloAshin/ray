#include "Color.h"

#include <algorithm> // clamp

const Color Color::gray01(0.1f, 0.1f, 0.1f);
const Color Color::gray02(0.2f, 0.2f, 0.2f);
const Color Color::gray03(0.3f, 0.3f, 0.3f);
const Color Color::gray04(0.4f, 0.4f, 0.4f);
const Color Color::gray05(0.5f, 0.5f, 0.5f);
const Color Color::gray06(0.6f, 0.6f, 0.6f);
const Color Color::gray07(0.7f, 0.7f, 0.7f);
const Color Color::gray08(0.8f, 0.8f, 0.8f);
const Color Color::gray09(0.9f, 0.9f, 0.9f);
const Color Color::black(0.0f, 0.0f, 0.0f);
const Color Color::white(1.0f, 1.0f, 1.0f);
const Color Color::red(1.0f, 0.0f, 0.0f);
const Color Color::green(0.0f, 1.0f, 0.0f);
const Color Color::blue(0.0f, 0.0f, 1.0f);

void
Color::clamp()
{
	r = std::clamp(r, 0.0f, 1.0f);
	g = std::clamp(g, 0.0f, 1.0f);
	b = std::clamp(b, 0.0f, 1.0f);
}

Color
Color::operator-(const Color &c) const
{
	return Color(r - c.r, g - c.g, b - c.b);
}

float
Color::dist(const Color &c) const
{
	return (r - c.r) * (r - c.r) + (g - c.g) * (g - c.g) + (b - c.b) * (b - c.b);
}

std::ostream &
operator<<(std::ostream &os, const Color &c)
{
	os << '(' << c.r << ", " << c.g << ", " << c.b << ')';
	return os;
}

