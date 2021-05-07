#include <iostream>

#include "XenoWS/XenoWS.hpp"
#include "XenoWS/shell.h"
#include "XenoWS/defs.h"

char *program_name = const_cast<char *>("");

int main(int argc, char **argv)
{
	// Set program_name global variable
	program_name = argv[0];

	// Parse command line arguments
	XenoWS_Options *options = parse_args(argc, argv);

	// Start the server
	XenoWS *server = new XenoWS(options->addr.s_addr, options->port);
	server->Init();
	server->Start();

	return 0;
}
