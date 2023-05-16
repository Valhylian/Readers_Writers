#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>

#define MAX_MESSAGE_LENGTH 100

typedef struct {
    pid_t pid;
    char datetime[MAX_MESSAGE_LENGTH];
    int line;
} Message;

int main() {
    int shmid;
    Message *shared_memory;
    int num_lines;
    int i;

    // Solicitar al usuario la cantidad de líneas
    printf("Ingrese la cantidad de líneas: ");
    scanf("%d", &num_lines);

    // Crear la memoria compartida
    shmid = shmget(IPC_PRIVATE, num_lines * sizeof(Message), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Error al crear la memoria compartida");
        exit(1);
    }

    // Adjuntar la memoria compartida
    shared_memory = (Message *)shmat(shmid, NULL, 0);
    if (shared_memory == (Message *)-1) {
        perror("Error al adjuntar la memoria compartida");
        exit(1);
    }

    // Mostrar el ID de la memoria compartida
    printf("ID de la memoria compartida: %d\n", shmid);

    /*
    // Escribir en cada línea de la memoria compartida
    for (i = 0; i < num_lines; i++) {
        // Obtener el PID del proceso actual
        pid_t pid = getpid();

        // Obtener la hora y la fecha actual
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(shared_memory[i].datetime, sizeof(shared_memory[i].datetime), "%Y-%m-%d %H:%M:%S", timeinfo);

        // Guardar la información en la estructura del mensaje
        shared_memory[i].pid = pid;
        shared_memory[i].line = i;
    }*/

    // Desasociar la memoria compartida
    if (shmdt(shared_memory) == -1) {
        perror("Error al desasociar la memoria compartida");
        exit(1);
    }

    return 0;
}