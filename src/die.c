#include "die.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

__attribute__((__noreturn__, __format__(printf, 1, 2))) void die(const char *fmt, ...) {
	va_list list;
	va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	va_end(list);
	exit(1);
	_exit(1);
}

const char *strno(void) {
	return strerror(errno);
}
