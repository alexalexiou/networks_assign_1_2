cmake_minimum_required(VERSION 3.14)
project(untitled C)

set(CMAKE_C_STANDARD 99)

add_executable(server server.c)
add_executable(client client.c)
add_executable(helper helper.c)