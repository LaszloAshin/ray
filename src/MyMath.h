#pragma once

#include <cmath>

extern "C" {

void mysincosf(float a, float* s, float* c);
float myatan2f(float y, float x);

#ifdef _WIN32
#include <immintrin.h>

inline float mysqrtf(float f) {
       float result;
       _mm_store_ss(&result, _mm_sqrt_ss(_mm_load_ss(&f)));
       return result;
}
#else
#define mysqrtf sqrtf
#endif

inline float myacosf(float x) {
	return myatan2f(mysqrtf(1.0f - x * x), x);
}

} // extern "C"
