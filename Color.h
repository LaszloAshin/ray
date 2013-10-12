#ifndef	_COLOR_H
#define _COLOR_H	1

#include <iostream>

struct Color {
	float r, g, b;

	static const Color gray01, gray02, gray03, gray04;
	static const Color gray05, gray06, gray07, gray08, gray09;
	static const Color black, white, red, green, blue;

	Color(float r = 0.0f, float g = 0.0f, float b = 0.0f);

	void clamp();
	void set(float r, float g, float b);
	Color operator+=(const Color &c);
	Color operator+(const Color &c) const;
	Color operator-(const Color &c) const;
	Color operator*(float a) const;
	Color operator*(const Color &c) const;
	float dist(const Color &c) const;
};

std::ostream &operator<<(std::ostream &os, const Color &c);

#endif /* _COLOR_H */
