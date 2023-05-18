#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

// Estructura para almacenar la información de cada línea en la memoria compartida
struct LineaMemoria {
    int pid;
    char horaFecha[50];
    int numLinea;
};

//Estructura para pasar paramtros al hilo Writer
struct ParametrosHilo {
    int idMemoria;
    int cantidadLineas;
    int pid;
    sem_t *semaforo;
    int segSleep;
    int segEscritura;
};

//OBTENER KEY UNICO DE MEMORIA-----------------------------
key_t obtener_key_t(const char* ruta, int id_proyecto) {
    key_t clave;

    clave = ftok(ruta, id_proyecto);
    if (clave == -1) {
        perror("ftok");
        // Manejo del error, si es necesario
    }
    return clave;
}

//PROCESOS ESCRITORES--------------------------------------
void* procesoWriter(void* argumento) {
    struct ParametrosHilo* parametros = (struct ParametrosHilo*)argumento;
    int idMemoria = parametros->idMemoria;
    int cantidadLineas = parametros->cantidadLineas;
    int pid = parametros->pid;
    sem_t *semaforo = parametros->semaforo;
    int segSleep = parametros->segSleep;
    int segEscritura = parametros->segEscritura;

    printf("Proceso: %d\n", pid);

    while(1){
        //ESPERAR SEMAFORO
        printf("Proceso: %d Esperando semaforo\n", pid);
        sem_wait(semaforo);

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
            strcpy(lineas[lineaVacia].horaFecha, "horaFecha"); //aqui tiene que ir la hora y la fecha real!
            printf("Escritura exitosa en la línea %d\n", lineaVacia + 1);
        } else {
            printf("No hay líneas vacías disponibles\n");
        }

        // Desvincular la memoria compartida
        if (shmdt(memoriaCompartida) == -1) {
            perror("Error al desvincular la memoria compartida");
            break;
        }

        //LIBERAR SEMAFORO
        printf("Proceso: %d Libera el semaforo\n", pid);
        sem_post(semaforo);

        //TIEMPO DORMIDO
        printf("Proceso: %d durmiendo\n", pid);
        usleep( segSleep*1000000);
    }
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

    struct ParametrosHilo parametros[cantidadWriters];

    for (int i=0; i<cantidadWriters; i++){
        // Crear una instancia de la estructura de parámetros
        parametros[i].idMemoria = idMemoria;
        parametros[i].cantidadLineas = 20; //cambiar estooooooooooooooooooooo
        parametros[i].pid = i+1;
        parametros[i].semaforo = semaforo;
        parametros[i].segSleep = segSleep;
        parametros[i].segEscritura = segEscritura;

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

