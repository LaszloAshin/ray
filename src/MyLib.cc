#include "MyLib.h"

int mysnprintOne(char* buf, int size, int value) {
	if (!size) return 0;
	char b[64];
	char *e = b + sizeof(b) - 1;
	*e-- = '\0';
	const bool negative = value < 0;
	if (negative) value = -value;
	if (!value) {
		*e-- = '0';
	} else for (; value; value /= 10) {
		*e-- = '0' + value % 10;
	}
	if (negative) *e-- = '-';
	++e;
	int result = 0;
	while (size && *e) {
		*buf++ = *e++;
		--size;
		++result;
	}
	if (size) {
		*buf = '\0';
	} else {
		buf[-1] = '\0';
		--result;
	}
	return result;
}
