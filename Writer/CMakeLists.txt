cmake_minimum_required(VERSION 3.24)
project(Writer C)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
add_executable(Writer main.c)
