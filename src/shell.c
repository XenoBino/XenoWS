#include "shell.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <arpa/inet.h>
#include <getopt.h>

void show_usage(char code)
{
	fprintf(stderr, "Usage: %s [-A ip:port] -D [PATH]\n", program_name);
	exit(code);
}

void show_version(char code)
{
	fprintf(stderr, "%s 1.0.0\n", program_name);
	exit(code);
}

#define	XenoWS_OPTS	"-A:a:D:d:hv"
static struct option XenoWS_OPTS_long[] = {
	{"address", required_argument, NULL, 'a'},
	{"directory", required_argument, NULL, 'd'},
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'h'},
	{NULL, 0, NULL, '\0'}
};

XenoWS_Options *parse_args(int argc, char **argv)
{
	if (argc < 3) {
		show_usage(1);
	}

	// Alocate a XenoWS_Options
	XenoWS_Options *ret = (XenoWS_Options *)malloc(sizeof(XenoWS_Options));
	memset(ret, 0x00, sizeof(XenoWS_Options));

	char *dir = NULL;

	int opt = 0;
	while ((opt = getopt_long(argc, argv, XenoWS_OPTS, XenoWS_OPTS_long, NULL)) != -1) {
		switch (opt) {
		case 1:
		case '?':
			show_usage(1);
			break;

		case 'A':
		case 'a':
			if (parse_address_port(optarg, &ret->addr, &ret->port) != 0) {
				fprintf(stderr, "%s: %s: Invalid address\n", program_name, optarg);
			}
			break;

		case 'D':
		case 'd':
			dir = optarg;
			break;

		case 'h':
			show_usage(0);
			break;

		case 'v':
			show_version(0);
			break;
		}
	}

	if (dir == NULL)
		show_usage(1);

	ret->Directory = dir;

	return ret;
}
