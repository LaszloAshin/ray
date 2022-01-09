#ifndef	_COLOR_H
#define _COLOR_H	1

#include <iostream>

struct Color {
	float r, g, b;

	static const Color gray01, gray02, gray03, gray04;
	static const Color gray05, gray06, gray07, gray08, gray09;
	static const Color black, white, red, green, blue;

	Color(float r = 0.0f, float g = 0.0f, float b = 0.0f) : r{r}, g{g}, b{b} {}

	void clamp();

	Color operator+=(const Color &c) {
		r += c.r, g += c.g, b += c.b;
		return *this;
	}

	friend Color operator+(Color lhs, const Color &rhs) {
		return lhs += rhs;
	}

	Color operator-(const Color &c) const;

	friend Color operator*(const Color& lhs, float rhs) {
		return {lhs.r * rhs, lhs.g * rhs, lhs.b * rhs};
	}

	friend Color operator*(const Color &lhs, const Color& rhs) {
		return {lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b};
	}

	float dist(const Color &c) const;
};

std::ostream &operator<<(std::ostream &os, const Color &c);

#endif /* _COLOR_H */
