#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

const char* ruta = "..//..//generadorKey";
int id_proyecto = 123; // Identificador de proyecto arbitrario

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
    // Obtener la clave de la memoria compartida
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

    // Desvincular la memoria compartida
    if (shmctl(idMemoria, IPC_RMID, NULL) == -1) {
        perror("Error al eliminar la memoria compartida");
        return 1;
    }

    // Cerrar el semáforo
    if (sem_unlink("/semaforo_writer") == -1) {
        perror("Error al cerrar el semáforo");
        return 1;
    }

    printf("Recursos liberados correctamente.\n");

    return 0;
}
