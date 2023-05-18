cmake_minimum_required(VERSION 3.25)
project(Finalizador C)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -lrt")

add_executable(Finalizador main.c)
