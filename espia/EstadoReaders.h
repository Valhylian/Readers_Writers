//
// Created by valhylian on 21/05/23.
//
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#ifndef ESPIA_ESTADOREADERS_H
#define ESPIA_ESTADOREADERS_H

// ESTRUCTURA WRITERS
struct Reader {
    int pid;
    int estado;
};

//OBTENER TAMANO DE LA MEMORIA
int obtenerCantReaders (int idMemoria){
    // Obtener información sobre la memoria compartida
    struct shmid_ds info;
    if (shmctl(idMemoria, IPC_STAT, &info) == -1) {
        perror("shmctl");
        return 1;
    }
    // Obtener el tamaño de la memoria compartida
    size_t tamanoMemoria = info.shm_segsz;

    int cant = tamanoMemoria / sizeof(struct Reader);
    return cant;
}

//IMPRIMIR MEMORIA COMPARTIDA
int imprimirEstadoReaders() {
    printf("  ESTADO READERS---------------------\n");
    // Obtener la clave de la memoria compartida
    key_t claveMemoria = ftok("..//..//generadorReaders1", 123);
    if (claveMemoria == -1) {
        perror("ftok");
        return 1;
    }

    // Obtener el identificador de la memoria compartida
    int idMemoria = shmget(claveMemoria, 0, 0);
    if (idMemoria == -1) {
        perror("shmget");
        return 1;
    }

    // Adjuntar la memoria compartida a nuestro espacio de direcciones
    void* memoriaCompartida = shmat(idMemoria, NULL, 0);
    if (memoriaCompartida == (void*)-1) {
        perror("shmat");
        return 1;
    }

    // Obtener el número de líneas de la memoria compartida
    int cantidadLineas = obtenerCantReaders(idMemoria);

    // Recorrer las líneas de la memoria compartida e imprimir sus valores
    struct Reader* reader = (struct Reader*)memoriaCompartida;
    for (int i = 0; i < cantidadLineas; i++) {
        printf("PID %d:\n", reader[i].pid);
        if (reader[i].estado == 0){
            printf("  Estado: Creado\n");
        }
        else if (reader[i].estado == 1){
            printf("  Estado: Bloqueado\n");
        }
        else if (reader[i].estado == 2){
            printf("  Estado: Leyendo - con acceso a memoria\n");
        }
        else if (reader[i].estado == 3){
            printf("  Estado: Durmiendo\n");
        }
        else {
            printf("  Estado: Default\n");
        }

    }

    // Desvincular la memoria compartida
    if (shmdt(memoriaCompartida) == -1) {
        perror("shmdt");
        return 1;
    }

    return 0;
}

#endif //ESPIA_ESTADOREADERS_H
