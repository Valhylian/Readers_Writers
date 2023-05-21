//
// Created by gera on 20/05/23.
//
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "Bitacora.h"
#include "stdlib.h"
sem_t semaforo;



int main() {
    finalizarSemaforoBitacora();
    char* contenidoLeido = leerBitacora();
    if (contenidoLeido != NULL) {
        printf("Contenido leído del archivo:\n%s\n", contenidoLeido);
        free(contenidoLeido);
    }

    finalizarSemaforoBitacora();

    return 0;
}
