//
// Created by gera on 21/05/23.
//
//OBTENER TAMANO DE LA MEMORIA
#include "funcionesMenu.h"
#include "EstadoWriters.h"
#include "EstadoReaders.h"
#include "EstadoEgoistas.h"
sem_t * semaforoBitacora;

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

//IMPRIMIR MEMORIA COMPARTIDA
int imprimirMemoria() {
    // Obtener la clave de la memoria compartida
    key_t claveMemoria = ftok("..//..//generadorKey", 123);
    if (claveMemoria == -1) {
        perror("ftok");
        return 1;
    }

    // Obtener el identificador de la memoria compartida
    int idMemoria = shmget(claveMemoria, 0, 0);
    if (idMemoria == -1) {
        perror("shmget");
        return 1;
    }

    // Adjuntar la memoria compartida a nuestro espacio de direcciones
    void* memoriaCompartida = shmat(idMemoria, NULL, 0);
    if (memoriaCompartida == (void*)-1) {
        perror("shmat");
        return 1;
    }

    // Obtener el número de líneas de la memoria compartida
    int cantidadLineas = obtenerCantLineas (idMemoria);

    // Recorrer las líneas de la memoria compartida e imprimir sus valores
    struct LineaMemoria* lineas = (struct LineaMemoria*)memoriaCompartida;
    for (int i = 0; i < cantidadLineas; i++) {
        printf("Linea %d:\n", lineas[i].numLinea);
        if ( lineas[i].pid == 0){
            printf("    ---------------------\n");
        }
        else{
            printf("  PID: %d\n", lineas[i].pid);
            printf("  Hora y fecha: %s\n", lineas[i].horaFecha);
        }

    }

    // Desvincular la memoria compartida
    if (shmdt(memoriaCompartida) == -1) {
        perror("shmdt");
        return 1;
    }

    return 0;
}
void imprimirBitacora(){
    printf("%s", "-> Bitacora <-\n");
    char * contenido = leerBitacora(semaforoBitacora);
    printf("%s",contenido);
    free(contenido); //importante porque reserva espacio con malloc
}
void solicitarOpcion(int *opcion) {
    char entrada[100];
    long input;
    int valido = 0;
    char *endptr;

    printf("1) Estado de la memoria compartida\n");
    printf("2) Estado de los writers\n");
    printf("3) Estado de los readers\n");
    printf("4) Estado de la Readers egoistas\n");
    printf("5) Mostrar bitacora\n");
    printf("6) Limpiar bitacora\n");
    printf("7) Salir\n");

    do {
        printf("Solicite una opción: ");
        fgets(entrada, sizeof(entrada), stdin);
        entrada[strcspn(entrada, "\n")] = '\0';  // Eliminar el salto de línea del final

        input = strtol(entrada, &endptr, 10);

        if (endptr == entrada || *endptr != '\0' || input < 1 || input > 7) {
            printf("Opción inválida. Por favor, ingrese un número del 1 al 7.\n");
        } else {
            valido = 1;
        }
    } while (!valido);

    *opcion = (int)input;
}

int menuPrincipal() {
    int opcion = 0;
    solicitarOpcion(&opcion);
    switch (opcion) {
        case 1:
            printf("Estado de la memoria compartida\n");
            // Agrega aquí la lógica para la opción 1
            imprimirMemoria();
            break;
        case 2:
            printf("Estado de los writers\n");
            // Agrega aquí la lógica para la opción 2
            imprimirEstadoWriters();
            break;
        case 3:
            printf("Estado de los readers\n");
            // Agrega aquí la lógica para la opción 3
            imprimirEstadoReaders();
            break;
        case 4:
            printf("Estado de la Readers egoistas\n");
            // Agrega aquí la lógica para la opción 4
            imprimirEstadoEgoistas();
            break;
        case 5:
            printf("Mostrar bitacora\n");
            imprimirBitacora();
            // Agrega aquí la lógica para la opción 5
            break;
        case 6:
            printf("Limpiar bitacora\n");
            limpiarBitacora(semaforoBitacora);
            // Agrega aquí la lógica para la opción 6
            break;
        case 7:
            return 1;
        default:
            printf("Opción inválida\n");
            break;
    }
    return 0;
}
void cargarSemaforoBitacora(sem_t * semaforo){
    semaforoBitacora = semaforo;
}
