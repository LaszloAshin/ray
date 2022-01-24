#pragma once

inline int myatoi(const char* nptr) {
	int result = 0;
	for (; *nptr >= '0' && *nptr <= '9'; ++nptr) {
		result = result * 10 + *nptr - '0';
	}
	return result;
}
