#pragma once

#include "XenoWS/defs.h"
#include <netinet/in.h>

_XENOWS_EXTERN_C_BEGIN
int parse_address_port(char *addr_port, struct in_addr *address, port_t *port);
_XENOWS_EXTERN_C_END
