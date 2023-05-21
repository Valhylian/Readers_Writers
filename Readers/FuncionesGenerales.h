//
// Created by valhylian on 19/05/23.
//
#include <stdio.h>
#include <sys/shm.h>


#ifndef READERS_FUNCIONESGENERALES_H
#define READERS_FUNCIONESGENERALES_H

//OBTENER KEY UNICO PARA LA MEMORIA COMPARTIDA
key_t obtener_key_t(const char* ruta, int id_proyecto) {
    key_t clave;
    clave = ftok(ruta, id_proyecto);
    if (clave == -1) {
        perror("ftok");
        // Manejo del error, si es necesario
    }
    return clave;
}

#endif //READERS_FUNCIONESGENERALES_H
