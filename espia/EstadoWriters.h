//
// Created by valhylian on 19/05/23.
//
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#ifndef ESPIA_ESTADOWRITERS_H
// ESTRUCTURA WRITERS

struct Writer {
    int pid;
    int estado;
};

//OBTENER TAMANO DE LA MEMORIA
int obtenerCantWriters (int idMemoria){
    // Obtener información sobre la memoria compartida
    struct shmid_ds info;
    if (shmctl(idMemoria, IPC_STAT, &info) == -1) {
        perror("shmctl");
        return 1;
    }
    // Obtener el tamaño de la memoria compartida
    size_t tamanoMemoria = info.shm_segsz;

    int cant = tamanoMemoria / sizeof(struct Writer);
    return cant;
}

//IMPRIMIR MEMORIA COMPARTIDA
int imprimirEstadoWriters() {
    // Obtener la clave de la memoria compartida
    key_t claveMemoria = ftok("..//..//generadorWriters", 123);
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
    int cantidadLineas = obtenerCantWriters (idMemoria);

    // Recorrer las líneas de la memoria compartida e imprimir sus valores
    struct Writer* writer = (struct Writer*)memoriaCompartida;
    for (int i = 0; i < cantidadLineas; i++) {
        printf("PID %d:\n", writer[i].pid);
        if (writer[i].estado == 0){
            printf("  Estado: Creado\n");
        }
        else if (writer[i].estado == 1){
            printf("  Estado: Bloqueado\n");
        }
        else if (writer[i].estado == 2){
            printf("  Estado: Escribiendo - con acceso a memoria\n");
        }
        else if (writer[i].estado == 3){
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

#define ESPIA_ESTADOWRITERS_H

#endif //ESPIA_ESTADOWRITERS_H
