//
// Created by valhylian on 22/05/23.
//
#include "FuncionesGenerales.h"

#ifndef EGOISTA_ESTADOEGOISTA_H
#define EGOISTA_ESTADOEGOISTA_H

// ESTRUCTURA WRITERS
struct Egoista {
    int pid;
    int estado; //1-creacion //2-escriendo //3-durmiendo
};

//ADD WRITER A MEMORIA COMPARTIDA
// Agrega un writer en la posición x de la memoria compartida de estado de writers
// Retorna 0 si la operación se realizó correctamente, -1 en caso de error

int agregarEgoistaEnPosicion(int posicion, struct Egoista* egoista, int idMemoria) {
    // Adjuntar la memoria compartida a nuestro espacio de direcciones
    void* memoriaCompartida = shmat(idMemoria, NULL, 0);
    if (memoriaCompartida == (void*)-1) {
        perror("Error al adjuntar la memoria compartida");
        return -1;
    }

    // Obtener el puntero al arreglo de writers en la memoria compartida
    struct Egoista* egoistas = (struct Egoista*)memoriaCompartida;

    // Copiar el writer en la posición especificada
    egoista[posicion] = *egoista;

    // Desvincular la memoria compartida
    if (shmdt(memoriaCompartida) == -1) {
        perror("Error al desvincular la memoria compartida");
        return -1;
    }
    return 0;
}

//ACTUALIZAR ESTADO DEL READER
int actulizarEstadoReader(int estado, sem_t *semaforoEstadoReaders, struct Egoista* estadoEgoista, int idMemoriaEstadoReaders){
    sem_wait(semaforoEstadoReaders);
    estadoEgoista->estado = estado;
    agregarEgoistaEnPosicion(estadoEgoista->pid-1, estadoEgoista, idMemoriaEstadoReaders);
    sem_post(semaforoEstadoReaders);
}

//PEDIR MEMORORIA PARA ESTADO WRITERS
// Return -1 = error
int memoriaEstadoEgoistas(int cantReaders){
    key_t claveReaders = obtener_key_t("..//..//generadorEgoista", 123);
    printf("La clave Egoistas obtenida es %d\n", claveReaders);

    size_t tamanoMemoria = cantReaders * sizeof(struct Egoista);
    int idMemoria = shmget(claveReaders, tamanoMemoria, IPC_CREAT | IPC_EXCL | 0666);

    if (idMemoria == -1){
        printf("errooooooooooooor\n");
    }
    return idMemoria;
}


#endif //EGOISTA_ESTADOEGOISTA_H
