#pragma once

#include "Options.h"
#include "defs.h"

_XENOWS_EXTERN_C_BEGIN
void show_usage(char code);
XenoWS_Options *parse_args(int argc, char **argv);
_XENOWS_EXTERN_C_END
