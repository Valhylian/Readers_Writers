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

// Estructura para almacenar la información de cada línea en la memoria compartida
struct LineaMemoria {
    int pid;
    char horaFecha[50];
    int numLinea;
};

//OBTENER TAMANO DE LA MEMORIA
int obtenerCantLineas (int idMemoria){
    // Obtener información sobre la memoria compartida
    struct shmid_ds info;
    if (shmctl(idMemoria, IPC_STAT, &info) == -1) {
        perror("shmctl");
        return 1;
    }
    // Obtener el tamaño de la memoria compartida
    size_t tamanoMemoria = info.shm_segsz;
    int lineas = tamanoMemoria / sizeof(struct LineaMemoria);
    return lineas;
}

//IMPRIMIR MEMORIA COMPARTIDA
int imprimirMemoria() {
    // Obtener la clave de la memoria compartida
    key_t claveMemoria = ftok("..//..//generadorKey", 123);
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
    int cantidadLineas = obtenerCantLineas (idMemoria);

    // Recorrer las líneas de la memoria compartida e imprimir sus valores
    struct LineaMemoria* lineas = (struct LineaMemoria*)memoriaCompartida;
    for (int i = 0; i < cantidadLineas; i++) {
        printf("Linea %d:\n", lineas[i].numLinea);
        if ( lineas[i].pid == 0){
            printf("    ---------------------\n");
        }
        else{
            printf("  PID: %d\n", lineas[i].pid);
            printf("  Hora y fecha: %s\n", lineas[i].horaFecha);
        }

    }

    // Desvincular la memoria compartida
    if (shmdt(memoriaCompartida) == -1) {
        perror("shmdt");
        return 1;
    }

    return 0;
}


int main() {
    imprimirMemoria();
    return 0;
}
