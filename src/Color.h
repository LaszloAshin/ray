#pragma once

#include <algorithm> // clamp

struct Color {
	float r{}, g{}, b{};

	constexpr static Color black() { return {}; }
	constexpr static Color gray(float g) { return {g, g, g}; }
	constexpr static Color white() { return gray(1.0f); }

	constexpr Color() = default;
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
