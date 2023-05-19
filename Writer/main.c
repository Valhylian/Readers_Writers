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

// Estructura para almacenar la información de cada línea en la memoria compartida
struct LineaMemoria {
    int pid;
    char horaFecha[50];
    int numLinea;
};

//Estructura para pasar paramtros al hilo Writer
struct ParametrosHilo {
    int idMemoria;
    int idMemoriaEstadoWriters;
    int cantidadLineas;
    int pid;
    sem_t *semaforo;
    sem_t *semaforoEstadoWriters;
    int segSleep;
    int segEscritura;
    struct Writer *estadoWriter;
};

//FUNCION retorna fecha y hora
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

//PROCESOS ESCRITORES--------------------------------------
void* procesoWriter(void* argumento) {
    struct ParametrosHilo* parametros = (struct ParametrosHilo*)argumento;
    int idMemoria = parametros->idMemoria;
    int idMemoriaEstadoWriters = parametros->idMemoriaEstadoWriters;
    int cantidadLineas = parametros->cantidadLineas;
    int pid = parametros->pid;
    sem_t *semaforo = parametros->semaforo;
    sem_t *semaforoEstadoWriters = parametros->semaforoEstadoWriters;
    int segSleep = parametros->segSleep;
    int segEscritura = parametros->segEscritura;
    struct Writer *estadoWriter = parametros->estadoWriter;
    printf("Proceso: %d\n", pid);

    //SOLICITAR MEMORIA COMPARTIDA FINALIZADORA----------------------------------------------
    key_t claveFinalizador= obtener_key_t("..//..//finalizadorKey", 123);

    // Crear la memoria compartida
    int idFinalizador = shmget(claveFinalizador, sizeof(bool), IPC_CREAT | 0666);

    // Adjuntar la memoria  a nuestro espacio de direcciones
    bool* terminar = (bool*)shmat(idFinalizador, NULL, 0);
    bool finalizar = *terminar;

    printf("El valor es: %d\n", finalizar);

    while(!*terminar){
        //1
        finalizar = *terminar;
        if (finalizar){
            break;
        }

        //ESPERAR SEMAFORO

        //actualizar estado de writer
        strcpy(estadoWriter->estado,  "Bloqueado");
        printf("Proceso: %d Esperando semaforo\n", pid);
        // agregar writer a la memoria compartida

        sem_wait(semaforoEstadoWriters);
        agregarWriterEnPosicion(estadoWriter->pid, &estadoWriter, idMemoriaEstadoWriters);
        printf("Proceso: %d \n", pid);
        sem_post(semaforoEstadoWriters);
        sem_wait(semaforo);

        //2
        finalizar = *terminar;
        if (finalizar){
            sem_post(semaforo); //libera semaforo
            break;
        }

        // Adjuntar la memoria compartida a nuestro espacio de direcciones
        void* memoriaCompartida = shmat(idMemoria, NULL, 0);
        if (memoriaCompartida == (void*)-1) {
            perror("Error al adjuntar la memoria compartida");
            break;
        }
        // Castear la memoria compartida a un array de struct LineaMemoria
        struct LineaMemoria* lineas = (struct LineaMemoria*)memoriaCompartida;

        // Buscar la próxima línea vacía en la memoria compartida
        int lineaVacia = -1;
        for (int i = 0; i < cantidadLineas; i++) {
            if (lineas[i].pid == 0) {
                lineaVacia = i;
                break;
            }
        }
        if (lineaVacia != -1) {
            //TIEMPO DE ESCRITURA
            printf("Proceso: %d Escribiendo\n", pid);
            usleep( segEscritura*1000000);

            // Escribir en la próxima línea vacía
            lineas[lineaVacia].pid = pid;
            char* fecha_hora_actual = obtenerFechaHoraActual();
            strcpy(lineas[lineaVacia].horaFecha, fecha_hora_actual);
            lineas[lineaVacia].numLinea = lineaVacia;

            printf("Escritura exitosa en la línea %d\n", lineaVacia);
        } else {
            printf("No hay líneas vacías disponibles\n");
        }

        // Desvincular la memoria compartida
        if (shmdt(memoriaCompartida) == -1) {
            perror("Error al desvincular la memoria compartida");
        }

        //LIBERAR SEMAFORO
        printf("Proceso: %d Libera el semaforo\n", pid);
        sem_post(semaforo);

        //TIEMPO DORMIDO
        printf("Proceso: %d durmiendo\n", pid);
        usleep( segSleep*1000000);
    }
    printf("Proceso: %d sale\n", pid);
    // Desvincular la memoria compartida
    shmdt(terminar);
    pthread_exit(NULL);
}


int main() {
    //SEMAFORO
    sem_t *semaforo;
    semaforo = sem_open("/semaforo_writer", O_CREAT, 0644, 1);
    if (semaforo == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    //INICIAR MEMORIA-ESTADO WRITERS

    int cantidadWriters;
    int segEscritura;
    int segSleep;

    // Solicitar al usuario la informacion necesaria
    printf("Ingrese la cantidad de procesos Writers: ");
    scanf("%d", &cantidadWriters);

    printf("Ingrese los segundos de escritura: ");
    scanf("%d", &segEscritura);

    printf("Ingrese los segundos dormidos: ");
    scanf("%d", &segSleep);

    //arreglo de hilos escritores
    pthread_t hilo_writers[cantidadWriters];

    //obtener key de memoria compartida
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
    int cantLineas = obtenerCantLineas (idMemoria);
    printf("cantLineas %d\n", cantLineas);

    struct ParametrosHilo parametros[cantidadWriters];

    //pedir memoria compartida para los estados de los writers
    int idmemoriaWriters = memoriaEstadoWriters(cantidadWriters);
    if (idmemoriaWriters== -1){
        printf("Error al pedir memoria de Writers");
        return 0;
    }

    //pedir semaforo para los estados de los writers
    sem_t *semaforoEstadoWriter;
    semaforoEstadoWriter = sem_open("/semaforo_estadoWriter", O_CREAT, 0644, 1);
    if (semaforoEstadoWriter == SEM_FAILED) {
        printf("Error al pedir el semaforo de estado de Writers");
    }

    for (int i=0; i<cantidadWriters; i++){
        // Crear estado Writer
        struct Writer *estadoWriter;
        estadoWriter->pid = i+1;
        strcpy(estadoWriter->estado,  "En creacion");
        // agregar writer a la memoria compartida
        agregarWriterEnPosicion(i, estadoWriter, idmemoriaWriters);

        // Crear una instancia de la estructura de parámetros
        parametros[i].idMemoria = idMemoria;
        parametros[i].cantidadLineas = cantLineas;
        parametros[i].pid = i+1;
        parametros[i].semaforo = semaforo;
        parametros[i].segSleep = segSleep;
        parametros[i].segEscritura = segEscritura;
        parametros[i].estadoWriter = &estadoWriter;
        parametros[i].idMemoriaEstadoWriters = idmemoriaWriters;
        parametros[i].semaforoEstadoWriters = semaforoEstadoWriter;

        if (pthread_create(&hilo_writers[i], NULL, procesoWriter, (void*)&parametros[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // Esperar a que todos los hilos terminen su ejecución
    for (int i = 0; i < cantidadWriters; i++) {
        if (pthread_join(hilo_writers[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }

    return 0;
}

