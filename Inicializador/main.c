#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include "../Bitacora/Bitacora.h"
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

int main() {
    sem_t * semaforoBitacora = obtenerSemaforoBitacora();
    escribirBitacora(semaforoBitacora, "--------------------------------------------");
    //CREAR SEMAFORO PRINCIPAL (WRITER)----------------------------------------------------
    sem_t *semaforo;
    semaforo = sem_open("/semaforo_writer", O_CREAT, 0644, 1);
    if (semaforo == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    //SEMAFORO PARA LOS EGOISTAS
    sem_t *semaforoEgoista;
    semaforoEgoista = sem_open("/semaforo_egoista", O_CREAT, 0644, 1);
    if (semaforoEgoista == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }
    sem_init(semaforoEgoista, 1, 0); //inicia bloqueado

    //PEDIR ESPACIO DE MEMORIA COMPARTIDA PRINCIPAL--------------------------------------------
    const char* ruta = "..//..//generadorKey";
    int id_proyecto = 123; // Identificador de proyecto arbitrario
    key_t claveMemoria = obtener_key_t(ruta, id_proyecto);
    if (claveMemoria == -1) {
        perror("Error al obtener la clave de la memoria compartida");
        return 1;
    }
    //printf("La clave obtenida es %d\n", claveMemoria);

    //SOLICITAR CANTIDAD DE LINEAS AL USUARIO---------------------------------------------------
    int cantidadLineas;
    printf("Ingrese la cantidad de líneas de la memoria compartida: ");
    scanf("%d", &cantidadLineas);

    // Calcular el tamaño total de la memoria compartida
    size_t tamanoMemoria = cantidadLineas * sizeof(struct LineaMemoria);

    //CREAR LA MEMORIA COMPARTIDA PRINCIPAL----------------------------------------------------
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

    //-------------------------------------------------------------------------------
    //SOLICITAR MEMORIA COMPARTIDA FINALIZADORA
    key_t claveFinalizador= obtener_key_t("..//..//finalizadorKey", 123);
    if (claveFinalizador == -1) {
        perror("Error al obtener la clave de la memoria compartida");
        return 1;
    }
    // Crear la memoria compartida finalizadora
    int idFinalizador = shmget(claveFinalizador, sizeof(bool), IPC_CREAT | 0666);
    if (idFinalizador == -1) {
        perror("Error al crear la memoria compartida");
        return 1;
    }
    // Adjuntar la memoria compartida a nuestro espacio de direcciones
    bool* terminar = (bool*)shmat(idFinalizador, NULL, 0);
    if (terminar == (bool*)-1) {
        perror("shmat");
        return 1;
    }

    // Establecer la variable compartida
    *terminar = false;

    // Desvincular la memoria compartida
    if (shmdt(terminar) == -1) {
        perror("shmdt");
        return 1;
    }

    printf("Memoria compartida inicializada correctamente.\n");

    return 0;
}