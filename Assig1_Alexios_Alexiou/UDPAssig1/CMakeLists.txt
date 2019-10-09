cmake_minimum_required(VERSION 3.14)
project(UDPAssig1 C)

set(CMAKE_C_STANDARD 99)

add_executable(Server server.c)
add_executable(Client client.c)
add_executable(Helper helper.c)