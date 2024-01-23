#ifndef XENO_XENOWS_OOM_H_
#define XENO_XENOWS_OOM_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

ssize_t write_uint64(int fd, uint64_t val);

#define oom() {\
	int err = fileno(stderr);\
	write(err, "Out of memory!\n", 15);\
	write(err, __FILE__, strlen(__FILE__));\
	write(err, "::", 2);\
	write_uint64(err, __LINE__);\
	write(err, " at ", 4);\
	write(err, __func__, strlen(__func__));\
	write(err, "\n", 1);\
	abort();\
	_exit(-1);\
}

#endif
