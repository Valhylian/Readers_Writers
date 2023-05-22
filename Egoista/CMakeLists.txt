cmake_minimum_required(VERSION 3.25)
project(Egoista C)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
include_directories("../Bitacora")
add_executable(Egoista main.c ../Bitacora/Bitacora.c)
