#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <stdbool.h>
#include "../Bitacora/Bitacora.h"
//MEMORIA Y SEMAFOROS A FINALIZAR:
//1- MEMORIA PRINCIPAL
//2- SEMAFORO PRINCIPAL (WRITER)
//3- MEMORIA FINALIZADORA
//4- MEMORIA ESTADO WRITERS

const char* ruta = "..//..//generadorKey";
int id_proyecto = 123; // Identificador de proyecto arbitrario

//obtener key
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
    finalizarSemaforoBitacora(semaforoBitacora);
    printf("Semáforo de la bitacora liberado\n");
    // Obtener la clave de la memoria compartida----------------------------------
    key_t claveMemoria = obtener_key_t(ruta, id_proyecto);
    if (claveMemoria == -1) {
        perror("Error al obtener la clave de la memoria compartida");
    }
    printf("La clave obtenida es %d\n", claveMemoria);

    // Obtener el ID de la memoria compartida
    int idMemoria = shmget(claveMemoria, 0, 0);
    if (idMemoria == -1) {
        perror("Error al obtener el ID de la memoria compartida");
    }

    // Desvincular la memoria compartida
    if (shmctl(idMemoria, IPC_RMID, NULL) == -1) {
        perror("Error al eliminar la memoria compartida");
    }

    //CERRAR MEMORIA COMPARTIDA DEL ESTADO DE LOS WRITERS
    //--------------------------------------------------------------------------
    // Obtener la clave de la memoria compartida----------------------------------
    key_t claveWriters = obtener_key_t("..//..//generadorWriters", 123);
    if (claveWriters == -1) {
        perror("Error al obtener la clave de la memoria writers");
    }
    printf("La clave writers obtenida es %d\n", claveMemoria);

    // Obtener el ID de la memoria compartida
    int idMemoriaWriters = shmget(claveWriters, 0, 0);
    if (idMemoriaWriters == -1) {
        perror("Error al obtener el ID de la memoria writers");
    }

    // Desvincular la memoria compartida
    if (shmctl(idMemoriaWriters, IPC_RMID, NULL) == -1) {
        perror("Error al eliminar la memoria compartida writera");
    }

    //CERRAR MEMORIA COMPARTIDA DEL ESTADO DE LOS READERS
    //--------------------------------------------------------------------------
    // Obtener la clave de la memoria compartida----------------------------------
    key_t claveReaders = obtener_key_t("..//..//generadorReaders1", 123);
    if (claveReaders == -1) {
        perror("Error al obtener la clave de la memoria writers");
    }
    printf("La clave readers obtenida es %d\n", claveMemoria);

    // Obtener el ID de la memoria compartida
    int idMemoriaReaders= shmget(claveReaders, 0, 0);
    if (idMemoriaReaders == -1) {
        perror("Error al obtener el ID de la memoria readers");
    }

    // Desvincular la memoria compartida
    if (shmctl(idMemoriaReaders, IPC_RMID, NULL) == -1) {
        perror("Error al eliminar la memoria compartida readers");
    }

    //CERRAR MEMORIA COMPARTIDA DEL ESTADO DE LOS EGOISTAS
    //--------------------------------------------------------------------------
    // Obtener la clave de la memoria compartida----------------------------------
    key_t claveEgoistas = obtener_key_t("..//..//generadorEgoista", 123);
    if (claveEgoistas == -1) {
        perror("Error al obtener la clave de la memoria egoistas");
    }
    printf("La clave egoistas obtenida es %d\n", claveMemoria);

    // Obtener el ID de la memoria compartida
    int idMemoriaEgoistas= shmget(claveEgoistas, 0, 0);
    if (idMemoriaEgoistas == -1) {
        perror("Error al obtener el ID de la memoria egoistas");
    }

    // Desvincular la memoria compartida
    if (shmctl(idMemoriaEgoistas, IPC_RMID, NULL) == -1) {
        perror("Error al eliminar la memoria compartida egoistass");
    }
    //--------------------------------------------------------------------------
    //CERRAR MEMORIA DE FINALIZACION
    //SOLICITAR MEMORIA COMPARTIDA FINALIZADORA
    key_t claveFinalizador= obtener_key_t("..//..//finalizadorKey", 123);
    if (claveFinalizador == -1) {
        perror("Error al obtener la clave de la memoria compartida");
    }
    // Crear la memoria compartida
    int idFinalizador = shmget(claveFinalizador, sizeof(bool), IPC_CREAT | 0666);
    if (idFinalizador == -1) {
        perror("Error al crear la memoria compartida");
    }
    // Adjuntar la memoria  a nuestro espacio de direcciones
    bool* terminar = (bool*)shmat(idFinalizador, NULL, 0);
    if (terminar == (bool*)-1) {
        perror("shmat");
    }

    // Establecer la variable compartida
    *terminar = true;

    // Desvincular la memoria compartida
    if (shmdt(terminar) == -1) {
        perror("shmdt");
    }
    // Desvincular la memoria compartida
    if (shmctl(idFinalizador, IPC_RMID, NULL) == -1) {
        perror("Error al eliminar la memoria compartida");
    }

    //---------------------------------------------------------------------------
    // Cerrar semaforos
    if (sem_unlink("/semaforo_writer") == -1) {
        perror("Error al cerrar el semáforo");
    }
    if (sem_unlink("/semaforo_estadoWriter") == -1) {
        perror("Error al cerrar el semáforo");
    }
    if(sem_unlink("/semaforo_readCnt") == -1){
        perror("Error al cerrar el semáforo readCnt");
    }
    if(sem_unlink("/semaforo_estadoReader") == -1){
        perror("Error al cerrar el semaforo_estadoReader");
    }
    if(sem_unlink("/semaforo_egoista") == -1){
        perror("Error al cerrar el semaforo_egoista");
    }
    if(sem_unlink("/semaforo_estadoEgoista") == -1){
        perror("Error al cerrar el semaforo_estadoEgoista");
    }
    printf("Recursos liberados correctamente.\n");

    return 0;
}
