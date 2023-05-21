//
// Created by gera on 20/05/23.
//

#ifndef READERS_WRITERS_BITACORA_H
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#define READERS_WRITERS_BITACORA_H

void estadoToString(char * estado ,int i);
void escribirBitacora(sem_t * semaforo, char * texto);
char * leerBitacora(sem_t * semaforo);
void finalizarSemaforoBitacora(sem_t * semaforo);
sem_t* obtenerSemaforoBitacora();
#endif //READERS_WRITERS_BITACORA_H
