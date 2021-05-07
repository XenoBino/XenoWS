#pragma once

#include <netinet/in.h>
#include "XenoWS/defs.h"

typedef struct {
	in_addr_t addr;
	port_t port;
} XenoWS_Options;
