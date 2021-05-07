#include "XenoWS/XenoWS.hpp"
#include "XenoWS/Date.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define REQUEST_LENGTH	2047

static void doProcessing(int fd, sockaddr_in *address);

XenoWS::XenoWS(in_addr_t address, port_t port) : m_FD(0), m_Address(address), m_Port(port), m_ADDR(nullptr)
{
	m_FD = socket(AF_INET, SOCK_STREAM, 0);
	if (m_FD < 0) {
		perror("socket");
		exit(1);
	}

	m_ADDR = new sockaddr_in;
	memset(m_ADDR, 0x00, sizeof(sockaddr_in));
}

XenoWS::~XenoWS()
{
	close(m_FD);
}

void XenoWS::Init()
{
	// Set family, port and address
	m_ADDR->sin_family = AF_INET;
	m_ADDR->sin_addr.s_addr = m_Address;
	m_ADDR->sin_port = htons(m_Port);

	if (bind(m_FD, (const sockaddr*)m_ADDR, sizeof(sockaddr_in)) < 0) {
		perror("bind");
		exit(1);
	}

	listen(m_FD, 255);
	std::cout << "Listening on " << inet_ntoa(m_ADDR->sin_addr) << ':' << m_Port << std::endl;
}

void XenoWS::Start()
{
	sockaddr_in client;
	socklen_t client_size;
	memset(&client, 0x00, sizeof(client));

	while (true) {
		int client_fd = accept(m_FD, (sockaddr*)&client, &client_size);

		pid_t pid = fork();
		if (pid < 0) {
			perror("fork");
			exit(1);
		}

		if (pid == 0) {
			close(m_FD);
			doProcessing(client_fd, &client);
			exit(0);
		}

		close(client_fd);
	}
}

static void doProcessing(int fd, sockaddr_in *address)
{
	static const char *header= "HTTP/1.1 200 OK\n"
				"connection: Keep-Alive\n"
				"content-length: 21\n"
				"content-type: text/html; charset=UTF-8\n"
				"keep-alive: timeout=5, max=100\n"
				"server: XenoWS\n";

	static const char *body = "\n<p>Hello, World!</p>\n";

	static const size_t header_len = strlen(header);
	static const size_t body_len = strlen(body);
	size_t date_len = 0, final_len = 0;
	char *date = NULL, *final_msg = NULL;

	std::cout << "-------- Fork of a connection --------" << std::endl;
	char *req = (char*)malloc(REQUEST_LENGTH);
	memset(req, 0x00, REQUEST_LENGTH);

	puts("Reading... ");
	int req_len = read(fd, req, REQUEST_LENGTH);
	if (req_len < 0) {
		perror("read");
		exit(1);
	}

	puts("Writing... \n");
	puts(req);

	puts("Geting Date... ");
	date = GetDateHeaderString(&date_len);

	final_len = header_len + body_len + date_len + 1;
	final_msg = (char *)malloc(final_len);

	puts("Constructing response... ");
	memset(final_msg, 0x00, final_len);
	memcpy(final_msg			, header,	header_len);
	memcpy(final_msg + header_len		, date,		date_len);
	memcpy(final_msg + header_len + date_len, body,		body_len);

	puts("Responding... ");
	write(fd, final_msg, final_len - 1);

	puts("Cleaning... \n");
	/* End */
	close(fd);

	free(req);
	free(date);
	free(final_msg);

	std::cout << "--------     End of fork     --------" << std::endl;
}
