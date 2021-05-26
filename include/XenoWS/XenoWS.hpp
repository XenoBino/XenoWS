#pragma once

#include <netdb.h>
#include "XenoWS/defs.h"

class XenoWS {
public:
	XenoWS(in_addr_t address, port_t port, char *Directory);
	~XenoWS();

	void Init();
	void Start();
private:
	sockaddr_in *m_ADDR;
	char *m_Dir;
	int m_FD;
	in_addr_t m_Address;
	port_t m_Port;

};
