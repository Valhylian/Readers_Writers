//
// Created by gera on 20/05/23.
//
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "Bitacora.h"
#include "stdlib.h"
sem_t semaforo;

void* hiloPrueba(void* arg) {
    char contenido[50] ;
    strcpy(contenido, "Prueba numero 1");
    escribirBitacora(contenido);
    return NULL;
}

int main() {


    // Crear un hilo para realizar pruebas adicionales
    pthread_t hilo;
    if (pthread_create(&hilo, NULL, hiloPrueba, NULL) != 0) {
        printf("Error al crear el hilo.\n");
        return 1;
    }

    // Realizar prueba de lectura y escritura en el archivo principal
    char* nombreArchivo = "archivo.txt";
    char* contenido = "Hola, este es un ejemplo de prueba de archivo.";
    escribirBitacora( contenido);

    char* contenidoLeido = leerBitacora();
    if (contenidoLeido != NULL) {
        printf("Contenido leído del archivo:\n%s\n", contenidoLeido);
        free(contenidoLeido);
    }

    // Esperar a que el hilo finalice
    if (pthread_join(hilo, NULL) != 0) {
        printf("Error al esperar al hilo.\n");
        return 1;
    }

    // Destruir el semáforo
    sem_destroy(&semaforo);

    return 0;
}