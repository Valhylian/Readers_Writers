//
// Created by gera on 20/05/23.
//
//
// Created by gera on 20/05/23.
//

#include "Bitacora.h"
const char* nombreArchivo = "bitacora.txt";

sem_t * obtenerSemaforoBitacora(){

    //SEMAFORO
    sem_t *semaforo;
    semaforo = sem_open("/semaforo_Bitacora", O_CREAT, 0644, 1);
    if (semaforo == SEM_FAILED) {
        perror("sem_open");
        return NULL;
    }
    return semaforo;
}
char * estadoToString(char * buffer ,int i){
    if (buffer == NULL) {
        // Manejar error de asignación de memoria
        return NULL;
    }

    switch (i) {
        case 1:
            strcpy(buffer, "Creacion");
            break;
        case 2:
            strcpy(buffer, "Escribiendo");
            break;
        case 3:
            strcpy(buffer, "Durmiendo");
            break;
        case 4:
            strcpy(buffer, "Leyendo");
            break;
        default:
            strcpy(buffer, "Desconocido");
    }
    return buffer;
}
void escribirBitacora(char * contenido){
    sem_t * semaforo = obtenerSemaforoBitacora();
    sem_wait(semaforo);
    FILE* archivo = fopen(nombreArchivo, "a");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        sem_post(semaforo);
        return;
    }
    fputs(contenido, archivo);
    fclose(archivo);
    sem_post(semaforo);
}
char* leerBitacora( ) {
    sem_t* semaforo = obtenerSemaforoBitacora();
    sem_wait(semaforo);

    FILE* archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        sem_post(semaforo);
        return NULL;
    }

    fseek(archivo, 0, SEEK_END);
    long size = ftell(archivo);
    rewind(archivo);

    char* contenido = (char*)malloc((size + 1) * sizeof(char));
    if (contenido == NULL) {
        printf("Error de asignación de memoria.\n");
        fclose(archivo);
        sem_post(semaforo);
        return NULL;
    }

    size_t bytesRead = fread(contenido, sizeof(char), size, archivo);
    if (bytesRead != size) {
        printf("Error al leer el archivo.\n");
        fclose(archivo);
        free(contenido);
        sem_post(semaforo);
        return NULL;
    }

    contenido[size] = '\0';  // Agregar el carácter nulo al final del contenido
    fclose(archivo);
    sem_post(semaforo);

    return contenido;
}
