#include "XenoWS/shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

void show_usage(char code)
{
	printf("Usage: %s ip port\n", program_name);
	exit(code);
}

XenoWS_Options *parse_args(int argc, char **argv)
{
	if (argc < 3) {
		show_usage(1);
	}

	// Alocate a XenoWS_Options
	XenoWS_Options *ret = (XenoWS_Options *)malloc(sizeof(XenoWS_Options));
	memset(ret, 0x00, sizeof(XenoWS_Options));

	struct in_addr *address = (struct in_addr*)malloc(sizeof(struct in_addr));
	memset(address, 0x00, sizeof(struct in_addr));

	if (!inet_aton(argv[1], address)) {
		printf("%s: inet_aton(): %s: invalid address!\n", argv[0], argv[1]);
		exit(1);
	}

	ret->addr = address->s_addr;
	ret->port = atoi(argv[2]);

	return ret;
}
