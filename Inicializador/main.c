#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/time.h>

// Estructura para almacenar la información de cada línea en la memoria compartida
struct LineaMemoria {
    int pid;
    char horaFecha[50];
    int numLinea;
};


key_t obtener_key_t(const char* ruta, int id_proyecto) {
    key_t clave;

    clave = ftok(ruta, id_proyecto);
    if (clave == -1) {
        perror("ftok");
        // Manejo del error, si es necesario
    }
    return clave;
}


int main() {
    const char* ruta = "..//..//generadorKey";
    int id_proyecto = 123; // Identificador de proyecto arbitrario
    key_t claveMemoria = obtener_key_t(ruta, id_proyecto);
    if (claveMemoria == -1) {
        perror("Error al obtener la clave de la memoria compartida");
        return 1;
    }
    printf("La clave obtenida es %d\n", claveMemoria);


    int cantidadLineas;

    // Solicitar al usuario la cantidad de líneas de la memoria compartida
    printf("Ingrese la cantidad de líneas de la memoria compartida: ");
    scanf("%d", &cantidadLineas);


    // Calcular el tamaño total de la memoria compartida
    size_t tamanoMemoria = cantidadLineas * sizeof(struct LineaMemoria);

    // Crear la memoria compartida
    int idMemoria = shmget(claveMemoria, tamanoMemoria, IPC_CREAT | IPC_EXCL | 0666);
    if (idMemoria == -1) {
        perror("Error al crear la memoria compartida");
        return 1;
    }

    // Adjuntar la memoria compartida a nuestro espacio de direcciones
    void* memoriaCompartida = shmat(idMemoria, NULL, 0);
    if (memoriaCompartida == (void*)-1) {
        perror("Error al adjuntar la memoria compartida");
        // Eliminar la memoria compartida creada
        shmctl(idMemoria, IPC_RMID, NULL);
        return 1;
    }

    // Inicializar la memoria compartida vacía
    struct LineaMemoria* lineas = (struct LineaMemoria*)memoriaCompartida;
    for (int i = 0; i < cantidadLineas; i++) {
        lineas[i].pid = 0;
        lineas[i].numLinea = i + 1;
    }

    // Desvincular la memoria compartida
    if (shmdt(memoriaCompartida) == -1) {
        perror("Error al desvincular la memoria compartida");
        // Eliminar la memoria compartida creada
        shmctl(idMemoria, IPC_RMID, NULL);
        return 1;
    }

    printf("Memoria compartida inicializada correctamente.\n");

    return 0;
}