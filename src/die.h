#ifndef XENO_XENOWS_DIE_H_
#define XENO_XENOWS_DIE_H_

__attribute__((__noreturn__, __format__(printf, 1, 2))) void die(const char *fmt, ...);
const char *strno(void);

#endif
