cmake_minimum_required(VERSION 3.24)
project(Finalizador C)

set(CMAKE_C_STANDARD 11)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -lrt")

include_directories("../Bitacora")  # Agregar esta línea para incluir la ruta de Bitacora.h
add_executable(Finalizador main.c  ../Bitacora/Bitacora.c)
