#include "util.h"
#include <string.h>

char *uintcpy(char *str, uint64_t val) {
	if (val == 0) return stpcpy(str, "0");

	char buf[24];
	uint8_t index = 0;

	buf[23] = '\0';

	while (val) {
		buf[22 - index] = '0' + (val % 10);
		val /= 10;
		index++;
	}

	return stpcpy(str, &buf[22 - index]);
}
