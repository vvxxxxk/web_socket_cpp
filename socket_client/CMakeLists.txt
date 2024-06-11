# CMake minimum version
cmake_minimum_required(VERSION 3.11)

# project info
project(socket_client VERSION 0.1 DESCRIPTION "TCP/IP CPP SOCKET CLIENT EXAMPLE" LANGUAGES CXX)

# 실행파일
add_executable(socket_client socket_client.cpp)