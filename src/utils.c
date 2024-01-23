#include "utils.h"
#include "defs.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <arpa/inet.h>

int parse_address_port(char *addr_port, struct in_addr *address, port_t *port)
{
	/* ---- Search for ':' in the address argument ---- */
	char *colon = strchr(addr_port, ':');

	// If no colon is found,
	//  use the default port
	if (colon == NULL) {
		if (addr_port[0] != ':') {
			return inet_aton(addr_port, address);
		}
		else {
			return 1;
		}
	}

	size_t colon_index = colon - addr_port;

	// A colon is found, parse the address
	size_t addr_size = colon_index + 1;
	char *addr_temp = (char *)malloc(addr_size);
	memset(addr_temp, 0x00, addr_size);
	strncpy(addr_temp, addr_port, addr_size - 1);

	*port = atoi(addr_port + addr_size);
	inet_aton(addr_temp, address);

	return 0;
}

int parse_request_info(char *str, size_t length, char *method, char **path, char *http_version)
{
	size_t var_len = 0;

	// If length is NULL, find the length
	if (length == 0) {
		char *line_end = NULL;
		if ((line_end = strchr(str, '\n')) == NULL) {
			length = strlen(str);
		}
		else {
			length = line_end - str;
		}
	}

	// A request info header can't be less than 14 characters
	if (length < 14)
		return XENOWS_ERR_PARAM_TOO_SHORT;

	var_len = length;

	// Parse the request method
	int method_res = 0;
	unsigned char method_end = 0;
	if ((method_res = parse_HTTP_method(str, 6, &method_end, method)) != XENOWS_OK) {
		return method_res;
	}

	var_len -= method_end;

	// Find the beginning of request path
	char *path_begin = NULL;
	if ((path_begin = strchr(str, ' ')) == NULL)
		return XENOWS_ERR_HTTP_INVALID_REQUEST;
	else if (path_begin[1] != '/')
		return XENOWS_ERR_HTTP_INVALID_REQUEST_PATH;

	path_begin++;

	// Find the end of request path
	char *path_end = NULL;
	char *raw_path_end = NULL;

	if ((raw_path_end = strchr(path_begin, ' ')) == NULL)
		return XENOWS_ERR_HTTP_INVALID_REQUEST_PATH;

	size_t raw_path_len = raw_path_end - path_begin + 1;
	size_t path_len = 0;
	char *raw_get_params = NULL;

	if ((path_end = memchr(path_begin, '?', raw_path_len)) == NULL) {
		path_end = raw_path_end;
		path_len = raw_path_len;
	}
	else
		path_len = path_end - path_begin + 1;

	*path = (char *)malloc(path_len);
	memset(*path, 0x00, path_len);
	strncpy(*path, path_begin, path_len - 1);

	var_len -= raw_path_len + 1;

	// Parse HTTP Signature
	char *http = raw_path_end + 1;

	if (strncmp(http, "HTTP", 4) != 0) {
		return XENOWS_ERR_HTTP_INVALID_SIGNATURE;
	}

	http += 4;
	var_len -= 4;
	if (var_len < 2)
		return XENOWS_ERR_PARAM_TOO_SHORT;

	if (http[0] != '/')
		return XENOWS_ERR_HTTP_INVALID_SIGNATURE;

	if (!isdigit(*(++http)))
		return XENOWS_ERR_HTTP_INVALID_VERSION;

	http_version[0] = http[0] - '0';
	var_len -= 2;

	if (var_len > 2)
		return XENOWS_ERR_PARAM_TOO_LONG;
	else if (var_len == 1)
		return XENOWS_ERR_HTTP_INVALID_VERSION;
	else if (var_len == 2) {
		http_version[1] = http[2] - '0';
		var_len -= 2;
	}
	else
		http_version[1] = 0;

//	printf("str: %p, len: %lu, var_len: %lu, ss: %.*s\n", str, length, var_len, length - var_len, &str[length - var_len]);

	return XENOWS_OK;
}

int parse_HTTP_method(char *str, size_t length, unsigned char *end, char *method) {
	if (length < 3)
		return XENOWS_ERR_PARAM_TOO_SHORT;

	if (*str != 'D' && *str != 'G' && *str != 'P') {
		return XENOWS_ERR_HTTP_INVALID_METHOD;
	}

	if (strncmp(str, "GET", 3) == 0) {
		*end = 3;
		*method = XENOWS_HTTP_GET;
		return XENOWS_OK;
	}
	else if (strncmp(str, "POST", 4) == 0) {
		*end = 4;
		*method = XENOWS_HTTP_POST;
		return XENOWS_OK;
	}
	else if (strncmp(str, "PUT", 3) == 0) {
		*end = 3;
		*method = XENOWS_HTTP_PUT;
		return XENOWS_OK;
	}
	else if (strncmp(str, "PATCH", 5) == 0) {
		*end = 5;
		*method = XENOWS_HTTP_PATCH;
		return XENOWS_OK;
	}
	else if (strncmp(str, "DELETE", 6) == 0) {
		*end = 6;
		*method = XENOWS_HTTP_DELETE;
		return XENOWS_OK;
	}

	return XENOWS_ERR_HTTP_INVALID_METHOD;
}
