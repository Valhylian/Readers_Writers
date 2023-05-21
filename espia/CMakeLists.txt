cmake_minimum_required(VERSION 3.25)
project(espia C)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
include_directories("../Bitacora")  # Agregar esta línea para incluir la ruta de Bitacora.h

add_executable(espia main.c ../Bitacora/Bitacora.c)
