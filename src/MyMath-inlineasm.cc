#include "MyMath.h"

// https://code.woboq.org/gcc/include/bits/mathinline.h.html
void mysincosf(float x, float* sinx, float* cosx) {
	__asm __volatile__("\n\t"
		"fsincos\n\t"
		"fnstsw %%ax\n\t"
		"test $0x400, %%ax\n\t"
		"jz 1f\n\t"
		"fldpi\n\t"
		"fadd %%st(0)\n\t"
		"fxch %%st(1)\n\t"
		"2: fprem1\n\t"
		"fnstsw %%ax\n\t"
		"test $0x400, %%ax\n\t"
		"jnz 2b\n\t"
		"fstp %%st(1)\n\t"
		"fsincos\n\t"
		"1:\n\t"
		: "=t" (*cosx), "=u" (*sinx)
		: "0" (x)
		: "%ax"
	);
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
