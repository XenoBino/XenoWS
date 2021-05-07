cmake_minimum_required(VERSION 3.10)

project(Sockets)

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
)

file(GLOB SRCS
	"${PROJECT_SOURCE_DIR}/src/*.c"
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.cxx"
	"${PROJECT_SOURCE_DIR}/src/*.c++"
)

add_executable(Sockets ${SRCS})
