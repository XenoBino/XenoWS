#pragma once

#include "defs.h"
#include <netinet/in.h>
#include <stddef.h>

_XENOWS_EXTERN_C_BEGIN
int parse_address_port(char *addr_port, struct in_addr *address, port_t *port);
int parse_request_info(char *str, size_t length, char *method, char **path, char *version_arr);
int parse_HTTP_method(char *str, size_t length, unsigned char *end, char *method);
_XENOWS_EXTERN_C_END
