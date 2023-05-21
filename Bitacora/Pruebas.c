//
// Created by gera on 20/05/23.
//
#include <stdio.h>
#include <semaphore.h>
#include "Bitacora.h"
#include "stdlib.h"




int main() {
    sem_t * sem = obtenerSemaforoBitacora();
    char* contenidoLeido = leerBitacora(sem);
    printf("%s", contenidoLeido);
    free(contenidoLeido);
    finalizarSemaforoBitacora(sem);

    return 0;
}
