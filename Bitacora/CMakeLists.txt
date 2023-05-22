cmake_minimum_required(VERSION 3.25)
project(Bitacora C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Bitacora
        Bitacora.c
        Bitacora.h
        Pruebas.c)
