#!/usr/bin/bash

cmake . -BBuild/
cmake --build Build/
echo Build/Sockets > .type
