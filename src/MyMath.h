#pragma once

#include <cmath>

extern "C" {

void mysincosf(float a, float* s, float* c);
float myatan2f(float y, float x);

inline float myacosf(float x) {
	return myatan2f(sqrtf(1.0f - x * x), x);
}

} // extern "C"
