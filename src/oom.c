#include "oom.h"

ssize_t write_uint64(int fd, uint64_t val) {
	if (val == 0) return write(fd, "0", 1);

	char buf[24];
	uint8_t index = 0;

	while (val != 0) {
		buf[23 - index] = '0' + (val % 10);
		val /= 10;
		index++;
	}

	return write(fd, &buf[index], index);
}
