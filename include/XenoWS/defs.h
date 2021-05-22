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

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifdef BIT
#undef BIT
#endif

#define BIT(x)	(1 << x)

/* Error codes */
#define XENOWS_OK	BIT(0)
#define XENOWS_ERR_HTTP_INVALID_REQUEST		BIT(1)
#define XENOWS_ERR_HTTP_INVALID_REQUEST_PATH	BIT(2)
#define XENOWS_ERR_HTTP_INVALID_SIGNATURE	BIT(3)
#define XENOWS_ERR_HTTP_INVALID_METHOD		BIT(4)
#define XENOWS_ERR_HTTP_INVALID_VERSION		BIT(5)
#define XENOWS_ERR_PARAM_TOO_LONG		BIT(6)
#define XENOWS_ERR_PARAM_TOO_SHORT		BIT(7)

/* HTTP Methods */
#define XENOWS_HTTP_GET		BIT(1)
#define XENOWS_HTTP_POST	BIT(2)
#define XENOWS_HTTP_PUT		BIT(3)
#define XENOWS_HTTP_PATCH	BIT(4)
#define XENOWS_HTTP_DELETE	BIT(5)
