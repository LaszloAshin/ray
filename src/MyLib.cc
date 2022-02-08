#include "MyLib.h"

#include <cassert>

int mysnprintOne(char* buf, int size, int value) {
	assert(size > 0);
	char b[12];
	char *e = b + sizeof(b) - 1;
	*e-- = '\0';
	const bool negative = value < 0;
	if (negative) value = -value;
	do {
		*e-- = '0' + value % 10;
		value /= 10;
	} while (value);
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
