#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include "die.h"
#include "date.h"
#include "oom.h"
#include "hashmap.h"
#include "http.h"

#define REQUEST_LENGTH	2047

struct {
	struct in_addr addr;
	uint32_t port;
	char *directory;
} Options;

void parse_args(int argc, char **argv);
static char *program_name;

int main(int argc, char **argv)
{
	program_name = basename(argv[0]);
	parse_args(argc, argv);

	const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		die("socket(): %s\n", strno());

	struct sockaddr_in listen_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(Options.port),
		.sin_addr = Options.addr,
		.sin_zero = {0},
	};

	if (bind(sockfd, (struct sockaddr*)&listen_addr, sizeof(struct sockaddr_in)) < 0)
		die("bind(): %s\n", strno());

	if (listen(sockfd, 255) < 0)
		die("listen(): %s\n", strno());

	printf("Listening on %s:%d\n", inet_ntoa(Options.addr), Options.port);

	while (1) {
		struct sockaddr_in client = {0};
		socklen_t client_size = 0;
		const int clientfd = accept(sockfd, (struct sockaddr*)&client, &client_size);
		if (clientfd < 0)
			die("accept(): %s\n", strno());

		pid_t pid = fork();
		if (pid < 0) {
			perror("fork");
			exit(1);
		}

		if (pid == 0) {
			size_t req_line_size = 0;
			size_t req_line_extra = 0;
			const size_t req_line_cap = 2047;
			char *req_line = (char*)malloc(req_line_cap + 1);
			if (!req_line) oom();

			char *req_line_end = NULL;
			while (req_line_size < req_line_cap) {
				int r = read(clientfd, &req_line[req_line_size], req_line_cap - req_line_size);
				if (r == EINTR) continue;
				if (r == 0) break;
				if (r < 0) die("read(): %s\n", strno());

				char *cr = memchr(&req_line[req_line_size], '\r', r);
				req_line_size += r;

				if (cr && cr[1] == '\n') {
					req_line_end = cr;
					req_line_extra = &req_line[req_line_size - 1] - &cr[1];
					break;
				}
			}

			if (!req_line_end) {
				const char *res = "HTTP/1.1 200 OK";
				write(clientfd, res, strlen(res));
			}

			req_line[req_line_size] = '\0';

			char *saveptr = NULL;

			char *method   = strtok_r(req_line, " ", &saveptr);
			char *path     = strtok_r(NULL, " ", &saveptr);
			char *http_ver = strtok_r(NULL, "\r\n", &saveptr);

			HttpRequest *request = http_req_create();

			// Not an http request
			if (!method || !path || !http_ver || strncmp(http_ver, "HTTP/", 5)) {
				free(req_line);
				close(clientfd);
				exit(0);
			}

			if (strcmp(&http_ver[5], "1.1")) {
				const char *res = "HTTP/1.1 400 Bad Request\r\nServer: XenoWS\r\n\r\nInvalid protocol Version.\r\n";
				write(clientfd, res, strlen(res));
				free(req_line);
				close(clientfd);
				exit(0);
			}

			request->version.major = 1;
			request->version.minor = 1;

			if (strcmp(method, "GET")) {
				request->method = REQ_METHOD_GET;
			}
			else if (strcmp(method, "POST")) {
				request->method = REQ_METHOD_POST;
			}
			else if (strcmp(method, "PUT")) {
				request->method = REQ_METHOD_PUT;
			}
			else if (strcmp(method, "PATCH")) {
				request->method = REQ_METHOD_PATCH;
			}
			else if (strcmp(method, "DELETE")) {
				request->method = REQ_METHOD_OPTIONS;
			}
			else {
				const char *res = "HTTP/1.1 405 Method Not Allowed\r\nServer: XenoWS\r\n\r\nInvalid HTTP method.\r\n";
				write(clientfd, res, strlen(res));
				free(req_line);
				close(clientfd);
				exit(0);
			}

			if (*path != '/') {
				const char *res = "HTTP/1.1 400 Bad Request\r\nServer: XenoWS\r\n\r\nMalformed request path.\r\n";
				write(clientfd, res, strlen(res));
				free(req_line);
				close(clientfd);
				exit(0);
			}

			request->path = path;

			free(req_line);
			http_req_destroy(request);

			const char *header =
				"HTTP/1.1 200 OK\r\n"
				"Connection: Keep-Alive\r\n"
				"Content-length: 21\r\n"
				"Content-type: text/html; charset=UTF-8\r\n"
				"Keep-alive: timeout=5, max=100\r\n"
				"Server: XenoWS\r\n";

			const char *body = "<p>Hello, World!</p>";

			const size_t header_len = strlen(header);
			const size_t body_len = strlen(body);

			size_t date_len = 0;
			char *date = get_date_header_string(&date_len);

			size_t res_len = header_len + 4 + body_len + date_len + 1;
			char *res = (char *)calloc(res_len, sizeof(*res));
			if (!res) oom();

			char *end = res;
			end = stpcpy(end, header);
			end = stpcpy(end, date);
			end = stpcpy(end, "\r\n");
			end = stpcpy(end, body);

			write(clientfd, res, end - res);

			free(date);
			free(res);

			close(clientfd);
			exit(0);
		}
	}

	close(sockfd);
	return 0;
}

void show_usage(int code)
{
	printf("Usage: %s -l ip:port -d directory\n", program_name);
	puts("");
	puts("Options:");
	puts("  -l --listen    Listen on <ip:port>. Defaults to 0.0.0.0:0");
	puts("  -d --directory Serve files from this directory");
	puts("  -h --help      Show this help message");
	puts("  -v --version   Show program version and exit");
	puts("");
	exit(code);
}

void show_version()
{
	printf("%s 1.0.0\n", program_name);
	exit(0);
}

void parse_args(int argc, char **argv)
{
	memset(&Options, 0x00, sizeof(Options));
	const char *shortopts = ":a:p:t:hv";

	struct option longopts[] = {
		{ "address", required_argument, NULL, 'a' },
		{ "port", required_argument, NULL, 'p' },
		{ "target", required_argument, NULL, 't' },
		{ "help", no_argument, NULL, 'h' },
		{ "version", no_argument, NULL, 'v' },
		{ NULL, 0, NULL, 0 }
	};

	while (1) {
		int c = getopt_long(argc, argv, shortopts, longopts, NULL);
		if (c == -1) break;

		switch (c) {
		case 'a': {
			struct in_addr addr;
			int ret = inet_aton(optarg, &addr);
			if (!ret) die("Invalid address: %s\n", optarg);

			Options.addr = addr;
			break;
		}

		case 'p': {
			char *end = NULL;
			int port = strtol(optarg, &end, 10);
			if (*end) {
				die("Invalid port number: %s\n", optarg);
			}
			Options.port = port;
			break;
		}

		case 'd':
			Options.directory = strdup(optarg);
			break;

		case 'h':
			show_usage(0);
			break;

		case 'v':
			show_version();
			break;

		case '?':
			fprintf(stderr, "%s: Invalid option -- '-%c'\n", program_name, optopt);
			show_usage(1);
			break;

		case ':':
			fprintf(stderr, "%s: Missing argument -- '-%c'\n", program_name, optopt);
			show_usage(1);
			break;

		default:
			die("getopt_long(): Uncaught case: '-%c'\n", optopt);
			break;
		}
	}
}
