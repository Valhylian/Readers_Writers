//
// Created by valhylian on 18/05/23.
//
#include "FuncionesGenerales.h"
#ifndef WRITER_ESTADOWRITERS_H
#define WRITER_ESTADOWRITERS_H

// ESTRUCTURA WRITERS
struct Writer {
    int pid;
    char estado[50];
};

//PEDIR MEMORORIA PARA ESTADO WRITERS
// Return -1 = error
int memoriaEstadoWriters(int cantWriters){
    key_t claveWriters = obtener_key_t("..//..//generadorWriters", 123);
    int idMemoria = shmget(claveWriters, sizeof(struct Writer), IPC_CREAT | IPC_EXCL | 0666);
    return idMemoria;
}

#endif //WRITER_ESTADOWRITERS_H
