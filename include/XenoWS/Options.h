#pragma once

#include <netinet/in.h>
#include "XenoWS/defs.h"

typedef struct {
	struct in_addr addr;
	port_t port;
	char *Directory;
} XenoWS_Options;
