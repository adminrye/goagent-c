cmake_minimum_required(VERSION 2.6)
project(GOAGENT-C)


include_directories(zlib)
#include_directories($ENV{HOME}/include)
#link_directories($ENV{HOME}/lib)

#set(CMAKE_VERBOSE_MAKEFILE on)

find_package(Threads REQUIRED)

exec_program("${CMAKE_CURRENT_SOURCE_DIR}/get-zlib")

aux_source_directory(src GOAGENT-C_SRCLIST)
aux_source_directory(zlib GOAGENT-C_SRCLIST)
add_executable(goagent-c ${GOAGENT-C_SRCLIST})
target_link_libraries(goagent-c ${CMAKE_THREAD_LIBS_INIT})
