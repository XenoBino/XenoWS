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
			const char *header =
				"HTTP/1.1 200 OK\r\n"
				"connection: Keep-Alive\r\n"
				"content-length: 21\r\n"
				"content-type: text/html; charset=UTF-8\r\n"
				"keep-alive: timeout=5, max=100\r\n"
				"server: XenoWS\r\n";

			const char *body = "<p>Hello, World!</p>";

			const size_t header_len = strlen(header);
			const size_t body_len = strlen(body);

			// REQUEST_LENGTH is capacity, req_size is size
			size_t req_size = 0;
			char *req = (char*)malloc(REQUEST_LENGTH);
			if (!req) oom();

			while (req_size < REQUEST_LENGTH) {
				int r = read(clientfd, &req[req_size], REQUEST_LENGTH - req_size);
				if (r == EINTR) continue;
				if (r == 0) break;
				if (r < 0) die("read(): %s\n", strno());

				req_size += r;
			}

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

			free(req);
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
