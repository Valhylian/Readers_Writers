//
// Created by valhylian on 18/05/23.
//
#include "FuncionesGenerales.h"
#ifndef WRITER_ESTADOWRITERS_H
#define WRITER_ESTADOWRITERS_H

// ESTRUCTURA WRITERS
struct Writer {
    int pid;
    int estado; //1-creacion //2-escriendo //3-durmiendo
};

//PEDIR MEMORORIA PARA ESTADO WRITERS
// Return -1 = error
int memoriaEstadoWriters(int cantWriters){
    key_t claveWriters = obtener_key_t("..//..//generadorWriters", 123);
    size_t tamanoMemoria = cantWriters * sizeof(struct Writer);
    int idMemoria = shmget(claveWriters, tamanoMemoria, IPC_CREAT | IPC_EXCL | 0666);
    return idMemoria;
}

//ADD WRITER A MEMORIA COMPARTIDA
// Agrega un writer en la posición x de la memoria compartida de estado de writers
// Retorna 0 si la operación se realizó correctamente, -1 en caso de error

int agregarWriterEnPosicion(int posicion, struct Writer* writer, int idMemoria) {
    // Adjuntar la memoria compartida a nuestro espacio de direcciones
    void* memoriaCompartida = shmat(idMemoria, NULL, 0);
    if (memoriaCompartida == (void*)-1) {
        perror("Error al adjuntar la memoria compartida");
        return -1;
    }

    // Obtener el puntero al arreglo de writers en la memoria compartida
    struct Writer* writers = (struct Writer*)memoriaCompartida;

    // Copiar el writer en la posición especificada
    writers[posicion] = *writer;

    // Desvincular la memoria compartida
    if (shmdt(memoriaCompartida) == -1) {
        perror("Error al desvincular la memoria compartida");
        return -1;
    }
    printf("esooo\n");
    return 0;
}

#endif //WRITER_ESTADOWRITERS_H
