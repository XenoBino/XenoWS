#include "XenoWS/utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int parse_address_port(char *addr_port, struct in_addr *address, port_t *port)
{
	/* ---- Search for ':' in the address argument ---- */
	size_t colon_index = 0;

	// The search loop
	for (size_t i = 0; addr_port[i] != '\0'; i++) {
		if (addr_port[i] == ':') {
			colon_index = i;
			break;
		}
	}

	// If no colon is found,
	//  use the default port
	if (colon_index == 0) {
		if (addr_port[0] != ':') {
			return inet_aton(addr_port, address);
		}
		else {
			return 1;
		}
	}

	// A colon is found, parse the address
	size_t addr_size = colon_index + 1;
	char *addr_temp = (char *)malloc(addr_size);
	memset(addr_temp, 0x00, addr_size);
	strncpy(addr_temp, addr_port, addr_size - 1);

	*port = atoi(addr_port + addr_size);
	inet_aton(addr_temp, address);

	return 0;
}
