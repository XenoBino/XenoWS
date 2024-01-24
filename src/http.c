#include "http.h"
#include <stdlib.h>
#include "oom.h"
#include "util.h"

static const char *http_status_str[] = {
	[ HTTP_CONTINUE            ] = "Continue",
	[ HTTP_SWITCHING_PROTOCOLS ] = "Switching Protocols",
	[ HTTP_PROCESSING          ] = "Processing",
	[ HTTP_EARLY_HINTS         ] = "Early Hints",

	[ HTTP_OK                            ] = "OK",
	[ HTTP_CREATED                       ] = "Created",
	[ HTTP_ACCEPTED                      ] = "Accepted",
	[ HTTP_NON_AUTHORITATIVE_INFORMATION ] = "Non-Authoritative Information",
	[ HTTP_NO_CONTENT                    ] = "No Content",
	[ HTTP_RESET_CONTENT                 ] = "Reset Content",
	[ HTTP_PARTIAL_CONTENT               ] = "Partial Content",
	[ HTTP_MULTI_STATUS                  ] = "Multi-Status",
	[ HTTP_ALREADY_REPORTED              ] = "Already Reported",
	[ HTTP_IM_USED                       ] = "IM Used",

	[ HTTP_MULTIPLE_CHOISES   ] = "Multiple Choises",
	[ HTTP_MOVED_PERMANENTLY  ] = "Moved Permanently",
	[ HTTP_FOUND              ] = "Found",
	[ HTTP_SEE_OTHER          ] = "See Other",
	[ HTTP_NOT_MODIFIED       ] = "Not Modified",
	[ HTTP_TEMPORARY_REDIRECT ] = "Temporary Redirect",
	[ HTTP_PERMANENT_REDIRECT ] = "Permanent Redirect",

	[ HTTP_BAD_REQUEST                     ] = "Bad Request",
	[ HTTP_UNAUTHORIZED                    ] = "Unauthorized",
	[ HTTP_PAYMENT_REQUIRED                ] = "Payment Required",
	[ HTTP_FORBIDDEN                       ] = "Forbidden",
	[ HTTP_NOT_FOUND                       ] = "Not Found",
	[ HTTP_METHOD_NOT_ALLOWED              ] = "Method Not Allowed",
	[ HTTP_NOT_ACCEPTABLE                  ] = "Not Acceptable",
	[ HTTP_PROXY_AUTHENTICATION_REQUIRED   ] = "Proxy Authentication Required",
	[ HTTP_REQUEST_TIMEOUT                 ] = "Request Timeout",
	[ HTTP_CONFLICT                        ] = "Conflict",
	[ HTTP_GONE                            ] = "Gone",
	[ HTTP_LENGTH_REQUIRED                 ] = "Length Required",
	[ HTTP_PRECONDITION_FAILED             ] = "Precondition Failed",
	[ HTTP_PAYLOAD_TOO_LARGE               ] = "Payload Too Large",
	[ HTTP_URI_TOO_LONG                    ] = "URI Too Long",
	[ HTTP_UNSUPPORTED_MEDIA_TYPE          ] = "Unsupported Media Type",
	[ HTTP_RANGE_NOT_SATISFIABLE           ] = "Range Not Satisfiable",
	[ HTTP_EXPECTATION_FAILED              ] = "Expectation Failed",
	[ HTTP_IM_A_TEAPOT                     ] = "I'm a teapot",
	[ HTTP_MISDIRECTED_REQUEST             ] = "Misdirected Request",
	[ HTTP_UNPROCESSABLE_CONTENT           ] = "Unprocessable Content",
	[ HTTP_LOCKED                          ] = "Locked",
	[ HTTP_FAILED_DEPENDENCY               ] = "Failed Dependency",
	[ HTTP_TOO_EARLY                       ] = "Too Early",
	[ HTTP_UPGRADE_REQUIRED                ] = "Upgrade Required",
	[ HTTP_PRECONDITION_REQUIRED           ] = "Precondition Required",
	[ HTTP_TOO_MANY_REQUESTS               ] = "Too Many Requests",
	[ HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE ] = "Request Header Fields Too Large",
	[ HTTP_UNAVAILABLE_FOR_LEGAL_REASONS   ] = "Unavailable For Legal Reasons",

	[ HTTP_INTERNAL_SERVER_ERROR           ] = "Internal Server Error",
	[ HTTP_NOT_IMPLEMENTED                 ] = "Not Implemented",
	[ HTTP_BAD_GATEWAY                     ] = "Bad Gateway",
	[ HTTP_SERVICE_UNAVAILABLE             ] = "Service Unavailable",
	[ HTTP_GATEWAY_TIMEOUT                 ] = "Gateway Timeout",
	[ HTTP_HTTP_VERSION_NOT_SUPPORTED      ] = "Http Version Not Supported",
	[ HTTP_VARIANT_ALSO_NEGOTIATES         ] = "Variant Also Negotiates",
	[ HTTP_INSUFFICIENT_STORAGE            ] = "Insufficient Storage",
	[ HTTP_LOOP_DETECTED                   ] = "Loop Detected",
	[ HTTP_NOT_EXTENDED                    ] = "Not Extended",
	[ HTTP_NETWORK_AUTHENTICATION_REQUIRED ] = "Network Authentication Required",
};

HttpRequest *http_req_create(void) {
	HttpRequest *req = calloc(sizeof(*req), 1);
	if (!req) oom();

	req->headers = hashmap_create(0, 0);
	return req;
}

void http_req_destroy(HttpRequest *req) {
	hashmap_destroy(req->headers);
	free(req);
}

HttpResponse *http_res_create(void) {
	HttpResponse *res = malloc(sizeof(*res));
	if (!res) oom();

	res->version.major = 1;
	res->version.minor = 1;
	res->headers = hashmap_create(0, 0);
	res->body = NULL;
	res->status_code = HTTP_OK;

	return res;
}

void http_res_send(HttpResponse *res, int fd) {
	char *res_line = malloc(256);
	if (!res_line) oom();

	char *end = res_line;
	end = stpcpy(end, "HTTP/");
	end = uintcpy(end, res->version.major);
	end = stpcpy(end, ".");
	end = uintcpy(end, res->version.minor);
	end = stpcpy(end, " ");
	end = uintcpy(end, res->status_code);
	end = stpcpy(end, " ");
	end = stpcpy(end, http_status_str[res->status_code]);

	write(fd, res_line, end - res_line);
	free(res_line);
}

void http_res_destroy(HttpResponse *res) {
	hashmap_destroy(res->headers);
	free(res);
}
