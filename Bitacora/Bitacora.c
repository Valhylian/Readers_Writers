//
// Created by gera on 20/05/23.
//
//
// Created by gera on 20/05/23.
//

#include "Bitacora.h"
const char* obtenerRutaBitacora() {
    const char* rutaArchivo = __FILE__;  // Ruta del archivo fuente actual

    // Obtener la ruta del directorio padre
    char* rutaDirectorioPadre = strdup(rutaArchivo);
    char* ultimoSeparador = strrchr(rutaDirectorioPadre, '/');
    if (ultimoSeparador != NULL) {
        *(ultimoSeparador + 1) = '\0';  // Agregar el carácter nulo después del último separador
    }

    // Concatenar el nombre del archivo de bitácora a la ruta del directorio padre
    const char* nombreArchivoBitacora = "bitacora.txt";
    char* rutaBitacora = malloc(strlen(rutaDirectorioPadre) + strlen(nombreArchivoBitacora) + 1);
    strcpy(rutaBitacora, rutaDirectorioPadre);
    strcat(rutaBitacora, nombreArchivoBitacora);

    free(rutaDirectorioPadre);
    return rutaBitacora;
}

sem_t* obtenerSemaforoBitacora() {
    sem_t* semaforo;
    semaforo = sem_open("/semaforo_Bitacora", O_CREAT, 0644, 1);
    if (semaforo == SEM_FAILED) {
        perror("sem_open");
        return NULL;
    }
    return semaforo;
}

void escribirBitacora(sem_t * semaforo, char* contenido) {
    sem_wait(semaforo);
    const char* rutaArchivo = obtenerRutaBitacora();
    FILE* archivo = fopen(rutaArchivo, "a");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        sem_post(semaforo);
        return;
    }

    fputs(contenido, archivo);
    fputs("\n", archivo);
    fclose(archivo);
    sem_post(semaforo);
}

char* leerBitacora(sem_t* semaforo) {
    sem_wait(semaforo);
    const char* rutaArchivo = obtenerRutaBitacora();

    FILE* archivo = fopen(rutaArchivo, "r");
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

void finalizarSemaforoBitacora(sem_t * semaforo) {
    if (semaforo != NULL) {
        sem_destroy(semaforo);
    }
}
void estadoToString(char * estado ,int i){
    switch (i) {
        case 1:
            strcpy(estado, "Creacion");
            break;
        case 2:
            strcpy(estado, "Escribiendo");
            break;
        case 3:
            strcpy(estado, "Durmiendo");
            break;
        case 4:
            strcpy(estado, "Leyendo");
            break;
        default:
            strcpy(estado, "estado no definido");

    }

}


