
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
#include "EstadoWriters.h"
#include "EstadoReaders.h"


#include "funcionesMenu.h"


// Estructura para almacenar la información de cada línea en la memoria compartida




int main() {
    sem_t * semaforoBitacora;
    semaforoBitacora = obtenerSemaforoBitacora();
   cargarSemaforoBitacora(semaforoBitacora);

    int salida = 0;
    do {
        printf("\n------- Menú principal -------\n\n");
        salida = menuPrincipal();
    }while(salida == 0);
    return 0;
}
