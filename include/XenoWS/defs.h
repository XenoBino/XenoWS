#pragma once

#ifdef __cplusplus
#define _XENOWS_EXTERN_C_BEGIN	extern "C" {
#else
#define _XENOWS_EXTERN_C_BEGIN
#endif

#ifdef __cplusplus
#define _XENOWS_EXTERN_C_END	}
#else
#define _XENOWS_EXTERN_C_END
#endif

extern char *program_name;

typedef unsigned short port_t;
