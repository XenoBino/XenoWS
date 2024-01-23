#include "XenoWS.hpp"
#include "shell.h"
#include "defs.h"

char *program_name;

int main(int argc, char **argv)
{
	// Set program_name global variable
	program_name = argv[0];

	// Parse command line arguments
	XenoWS_Options *options = parse_args(argc, argv);

	// Start the server
	XenoWS *server = new XenoWS(options->addr.s_addr, options->port, options->Directory);
	server->Init();
	server->Start();

	return 0;
}
