#include "MyMath.h"

// https://code.woboq.org/gcc/include/bits/mathinline.h.html
void mysincosf(float x, float* sinx, float* cosx) {
	long double cosr;
	long double sinr;
	unsigned int swtmp;
	__asm __volatile__("\n\t"
		"fsincos\n\t"
		"fnstsw %w2\n\t"
		"testl $0x400, %2\n\t"
		"jz 1f\n\t"
		"fldpi\n\t"
		"fadd %%st(0)\n\t"
		"fxch %%st(1)\n\t"
		"2: fprem1\n\t"
		"fnstsw %w2\n\t"
		"testl $0x400, %2\n\t"
		"jnz 2b\n\t"
		"fstp %%st(1)\n\t"
		"fsincos\n\t"
		"1:\n\t"
		: "=t" (cosr), "=u" (sinr), "=a" (swtmp)
		: "0" (x)
	);
	*sinx = sinr;
	*cosx = cosr;
}

float myatan2f(float y, float x) {
	long double result;
	__asm __volatile__("\n\t"
		"fpatan\n\t"
		: "=t" (result)
		: "0" (x), "u" (y)
		: "st(1)"
	);
	return result;
}
