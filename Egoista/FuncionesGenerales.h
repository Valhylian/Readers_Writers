//
// Created by valhylian on 22/05/23.
//
#include <stdio.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>
#ifndef EGOISTA_FUNCIONESGENERALES_H
#define EGOISTA_FUNCIONESGENERALES_H


// Estructura para almacenar la información de cada línea en la memoria compartida
struct LineaMemoria {
    int pid;
    char horaFecha[50];
    int numLinea;
};

//OBTENER KEY UNICO PARA LA MEMORIA COMPARTIDA
key_t obtener_key_t(const char* ruta, int id_proyecto) {
    key_t clave;
    clave = ftok(ruta, id_proyecto);
    if (clave == -1) {
        perror("ftok");
        // Manejo del error, si es necesario
    }
    return clave;
}

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

//Funcion pa solicitar el id Memoria Principal
int solicitarIDMemoriaPrincipal(){
    const char* ruta = "..//..//generadorKey";
    int id_proyecto = 123; // Identificador de proyecto arbitrario
    key_t claveMemoria = obtener_key_t(ruta, id_proyecto);
    if (claveMemoria == -1) {
        perror("Error al obtener la clave de la memoria compartida");
        return 1;
    }
    printf("La clave obtenida es %d\n", claveMemoria);

    // Obtener el ID de la memoria compartida
    int idMemoria = shmget(claveMemoria, 0, 0);
    if (idMemoria == -1) {
        perror("Error al obtener el ID de la memoria compartida");
        return 1;
    }
    printf("El id obtenido es %d\n", idMemoria);

    return idMemoria;
}
char* obtenerFechaHoraActual() {
    time_t tiempo_actual;
    struct tm* tiempo_local;
    static char buffer[80];

    // Obtener el tiempo actual
    tiempo_actual = time(NULL);
    // Convertir el tiempo a la hora local
    tiempo_local = localtime(&tiempo_actual);
    // Formatear la fecha y la hora en una cadena de caracteres
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tiempo_local);

    return buffer;
}
#endif //EGOISTA_FUNCIONESGENERALES_H
