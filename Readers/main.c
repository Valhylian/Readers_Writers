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
#include "EstadoReaders.h"
#include "../Bitacora/Bitacora.h"
//VARIABLE GLOABL PARA CONTROLAR LA CANTIDAD O COLA DE LECTORES
int readCnt = 0;
sem_t  * semaforoBitacora;
// Estructura para almacenar la información de cada línea en la memoria compartida
struct LineaMemoria {
    int pid;
    char horaFecha[50];
    int numLinea;
};
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
//Estructura para pasar paramtros al hilo Writer
struct ParametrosHilo {
    int idMemoria;
    int idMemoriaEstadoReaders;
    int cantidadLineas;
    int pid;
    sem_t *semaforo;
    sem_t *semaforoEstadoReaders;
    int segSleep;
    int segEscritura;
    struct Reader *estadoReader;
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
void* procesoReader(void* argumento) {
    int lineaLectura = 0;

    //pedir semaforo para readCnt
    sem_t *semaforoCnt;
    semaforoCnt = sem_open("/semaforo_readCnt", O_CREAT, 0644, 1);

    if (semaforoCnt == SEM_FAILED) {
        perror("sem_open");
    }

    struct ParametrosHilo *parametros = (struct ParametrosHilo *) argumento;
    int idMemoria = parametros->idMemoria;
    int idMemoriaEstadoReaders = parametros->idMemoriaEstadoReaders;
    int cantidadLineas = parametros->cantidadLineas;
    int pid = parametros->pid;
    sem_t *semaforo = parametros->semaforo;
    sem_t *semaforoEstadoReaders = parametros->semaforoEstadoReaders;
    int segSleep = parametros->segSleep;
    int segEscritura = parametros->segEscritura;
    struct Reader *estadoReader = parametros->estadoReader;
    printf("Reader: %d\n", pid);

    //SOLICITAR MEMORIA COMPARTIDA FINALIZADORA----------------------------------------------
    key_t claveFinalizador= obtener_key_t("..//..//finalizadorKey", 123);

    // Crear la memoria compartida
    int idFinalizador = shmget(claveFinalizador, sizeof(bool), IPC_CREAT | 0666);

    // Adjuntar la memoria  a nuestro espacio de direcciones
    bool* terminar = (bool*)shmat(idFinalizador, NULL, 0);

    while(!*terminar) {
        actulizarEstadoReader(1, semaforoEstadoReaders, estadoReader, idMemoriaEstadoReaders);

        sem_wait(semaforoCnt);
        readCnt++;

        if (readCnt == 1) {
            sem_wait(semaforo);
        }
        sem_post(semaforoCnt);


        // Adjuntar la memoria compartida a nuestro espacio de direcciones
        void *memoriaCompartida = shmat(idMemoria, NULL, 0);

        // Castear la memoria compartida a un array de struct LineaMemoria
        struct LineaMemoria *lineas = (struct LineaMemoria *) memoriaCompartida;

        actulizarEstadoReader(2, semaforoEstadoReaders, estadoReader, idMemoriaEstadoReaders);

        if (lineas[lineaLectura].pid == 0){
            printf("Reader: %dlinea: %d Vacia\n ", pid,lineaLectura);
        }
        else{
            //lea
            printf("Reader: %d Leyendo...\n", pid);
            sleep(segEscritura);
            printf("Reader: %d Lee...\n", pid);
            printf("Linea %d:\n", lineas[lineaLectura].numLinea);
            printf("  PID: %d\n", lineas[lineaLectura].pid);
            printf("  Hora y fecha: %s\n", lineas[lineaLectura].horaFecha);
            char buffer[1000]= "";
            char bufferMensaje[1000]= "";
            char convertido[10]= "";
 
            strcat(bufferMensaje, " -Linea: ");
            sprintf(convertido,"%d", lineas[lineaLectura].numLinea);
            strcat(bufferMensaje, convertido);
            strcat(bufferMensaje, " -PID: ");
            sprintf(convertido, "%d",lineas[lineaLectura].pid );
            strcat(bufferMensaje, convertido);
            strcat(bufferMensaje, " -Hora y fecha: ");
            strcat(bufferMensaje,lineas[lineaLectura].horaFecha);
            char * fechaHora = obtenerFechaHoraActual();
            parsearInfoBitacora(buffer,pid, "Reader",fechaHora,"Leyendo",bufferMensaje);
            escribirBitacora(semaforoBitacora, buffer);
            lineaLectura++;

            if(lineaLectura>=cantidadLineas){
                lineaLectura = 0;
            }

        }



        sem_wait(semaforoCnt);
        readCnt--;
        if (readCnt == 0){
            sem_post(semaforo);
        }
        sem_post(semaforoCnt);

        actulizarEstadoReader(3, semaforoEstadoReaders, estadoReader,  idMemoriaEstadoReaders);
        printf("Reader: %d Durmiendo...\n", pid);
        sleep(segSleep);

    }

    printf("Proceso: %d sale\n", pid);
    // Desvincular la memoria compartida
    shmdt(terminar);

    pthread_exit(NULL);
}

int main() {
    sem_t *semaforo;
    semaforoBitacora = obtenerSemaforoBitacora();
    semaforo = sem_open("/semaforo_writer", O_CREAT, 0644, 1);
    if (semaforo == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    //SOLICITAR CANT DE READERS AL USUARIO
    int cantidadReaders;
    int segEscritura;
    int segSleep;

    //Solicitar al usuario la informacion necesaria
    printf("Ingrese la cantidad de procesos readers: ");
    scanf("%d", &cantidadReaders);

    printf("Ingrese los segundos de escritura: ");
    scanf("%d", &segEscritura);

    printf("Ingrese los segundos dormidos: ");
    scanf("%d", &segSleep);

    //arreglo de hilos lectores
    pthread_t hilo_Readers [cantidadReaders];
    struct ParametrosHilo parametros[cantidadReaders];
    struct Reader arregloEstadosReader[cantidadReaders];

    //Solicita memoria principal
    int idMemoriaPrincipal = solicitarIDMemoriaPrincipal();
    int cantLineas = obtenerCantLineas (idMemoriaPrincipal);

    //Solicita memoria finalizadora
    key_t claveFinalizador= obtener_key_t("..//..//finalizadorKey", 123);
    int idFinalizador = shmget(claveFinalizador, sizeof(bool), IPC_CREAT | 0666);

    //Solicita memoria estados Readers
    int idmemoriaWriters = memoriaEstadoReaders(cantidadReaders);

    //Pedir semaforo para estado readers
    sem_t *semaforoEstadoReader;
    semaforoEstadoReader = sem_open("/semaforo_estadoReader", O_CREAT, 0644, 1);
    if (semaforoEstadoReader == SEM_FAILED) {
        printf("Error al pedir el semaforo de estado de readers");
    }

    for (int i=0; i<cantidadReaders; i++){
        // Crear estado Writer
        arregloEstadosReader[i].pid = i+1;
        arregloEstadosReader[i].estado = 0;

        agregarReaderEnPosicion(i, &arregloEstadosReader[i], idmemoriaWriters);

        // Crear una instancia de la estructura de parámetros
        parametros[i].idMemoria = idMemoriaPrincipal;
        parametros[i].cantidadLineas = cantLineas;
        parametros[i].pid = i+1;
        parametros[i].semaforo = semaforo;
        parametros[i].segSleep = segSleep;
        parametros[i].segEscritura = segEscritura;
        parametros[i].estadoReader = &arregloEstadosReader[i];
        parametros[i].idMemoriaEstadoReaders = idmemoriaWriters;
        parametros[i].semaforoEstadoReaders = semaforoEstadoReader;

        if (pthread_create(&hilo_Readers[i], NULL, procesoReader, (void*)&parametros[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // Esperar a que todos los hilos terminen su ejecución
    for (int i = 0; i < cantidadReaders; i++) {
        if (pthread_join(hilo_Readers[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }

    return 0;
}
