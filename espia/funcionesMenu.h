//
// Created by gera on 21/05/23.
//

#ifndef ESPIA_MENUPRINCIPAL_H
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#include "../Bitacora/Bitacora.h"
#define ESPIA_MENUPRINCIPAL_H
struct LineaMemoria {
    int pid;
    char horaFecha[50];
    int numLinea;
};
void imprimirBitacora();
int imprimirMemoria();
int obtenerCantLineas (int idMemoria);
int menuPrincipal();
void cargarSemaforoBitacora(sem_t * semaforo);
#endif //ESPIA_MENUPRINCIPAL_H
