#pragma once

#include <algorithm> // clamp

struct Color {
	float r{}, g{}, b{};

	static const Color gray01, gray02, gray03, gray04;
	static const Color gray05, gray06, gray07, gray08, gray09;
	static const Color black, white, red, green, blue;

	constexpr Color() {}
	constexpr Color(float r, float g, float b) : r{r}, g{g}, b{b} {}

	void clamp() {
		r = std::clamp(r, 0.0f, 1.0f);
		g = std::clamp(g, 0.0f, 1.0f);
		b = std::clamp(b, 0.0f, 1.0f);
	}

	Color operator+=(const Color &c) {
		r += c.r, g += c.g, b += c.b;
		return *this;
	}

	friend Color operator+(Color lhs, const Color &rhs) {
		return lhs += rhs;
	}

	Color operator-(const Color &c) const {
		return Color(r - c.r, g - c.g, b - c.b);
	}

	friend Color operator*(const Color& lhs, float rhs) {
		return {lhs.r * rhs, lhs.g * rhs, lhs.b * rhs};
	}

	friend Color operator*(const Color &lhs, const Color& rhs) {
		return {lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b};
	}

	float dist(const Color &c) const {
		return (r - c.r) * (r - c.r) + (g - c.g) * (g - c.g) + (b - c.b) * (b - c.b);
	}
};
