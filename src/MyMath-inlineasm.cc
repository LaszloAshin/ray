#include "MyMath.h"

// https://code.woboq.org/gcc/include/bits/mathinline.h.html
void mysincosf(float x, float* sinx, float* cosx) {
	long double cosr;
	long double sinr;
	unsigned int swtmp;
	__asm __volatile__(
		"fsincos;"
		"fnstsw %w2;"
		"testl $0x400, %2;"
		"jz 1f;"
		"fldpi;"
		"fadd %%st(0);"
		"fxch %%st(1);"
		"2: fprem1;"
		"fnstsw %w2;"
		"testl $0x400, %2;"
		"jnz 2b;"
		"fstp %%st(1);"
		"fsincos;"
		"1:"
		: "=t" (cosr), "=u" (sinr), "=a" (swtmp)
		: "0" (x)
	);
	*sinx = sinr;
	*cosx = cosr;
}
