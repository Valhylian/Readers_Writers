//
// Created by valhylian on 19/05/23.
//

#include "FuncionesGenerales.h"
#ifndef READERS_ESTADOREADERS_H
#define READERS_ESTADOREADERS_H

// ESTRUCTURA WRITERS
struct Reader {
    int pid;
    int estado; //1-creacion //2-escriendo //3-durmiendo
};

//PEDIR MEMORORIA PARA ESTADO WRITERS
// Return -1 = error
int memoriaEstadoReaders(int cantWriters){
    key_t claveWriters = obtener_key_t("..//..//generadorReaders", 123);
    size_t tamanoMemoria = cantWriters * sizeof(struct Reader);
    int idMemoria = shmget(claveWriters, tamanoMemoria, IPC_CREAT | IPC_EXCL | 0666);
    return idMemoria;
}

//ADD WRITER A MEMORIA COMPARTIDA
// Agrega un writer en la posición x de la memoria compartida de estado de writers
// Retorna 0 si la operación se realizó correctamente, -1 en caso de error

int agregarReaderEnPosicion(int posicion, struct Reader* writer, int idMemoria) {
    // Adjuntar la memoria compartida a nuestro espacio de direcciones
    void* memoriaCompartida = shmat(idMemoria, NULL, 0);
    if (memoriaCompartida == (void*)-1) {
        perror("Error al adjuntar la memoria compartida");
        return -1;
    }

    // Obtener el puntero al arreglo de writers en la memoria compartida
    struct Reader* readers = (struct Reader*)memoriaCompartida;

    // Copiar el writer en la posición especificada
    readers[posicion] = *writer;

    // Desvincular la memoria compartida
    if (shmdt(memoriaCompartida) == -1) {
        perror("Error al desvincular la memoria compartida");
        return -1;
    }
    return 0;
}
#endif //READERS_ESTADOREADERS_H
