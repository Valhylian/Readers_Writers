#include "../Bitacora/Bitacora.h"
#include "EstadoEgoista.h"
#include <stdbool.h>
#include <string.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "../Bitacora/Bitacora.h"

sem_t  * semaforoBitacora;
sem_t *semaforo;
sem_t *egoista;
sem_t *semaforoEstadoEgoista;

int contEgoistaRestriccion = 0;

int generarAleatorio(int min, int max) {
    // Semilla para la generación de números aleatorios
    srand(time(NULL));

    // Generar el número aleatorio entre min y max
    int num = (rand() % (max - min + 1)) + min;
    return num;
}


//Estructura para pasar paramtros al hilo Writer
struct ParametrosHilo {
    int idMemoria;
    int idMemoriaEstadoEgoista;
    int cantidadLineas;
    int pid;
    sem_t *semaforo;
    sem_t *semaforoEstadoEgoista;
    int segSleep;
    int segEscritura;
    struct Egoista *estadoEgoista;
};


void* procesoEgoista(void* argumento) {
    struct ParametrosHilo* parametros = (struct ParametrosHilo*)argumento;
    int idMemoria = parametros->idMemoria;
    int idMemoriaEstadoEgoistas = parametros->idMemoriaEstadoEgoista;
    int cantidadLineas = parametros->cantidadLineas;
    int pid = parametros->pid;
    int segSleep = parametros->segSleep;
    int segEscritura = parametros->segEscritura;
    struct Egoista *estadoEgoista = parametros->estadoEgoista;
    printf("Proceso Egoista: %d\n", pid);

    //SOLICITAR MEMORIA COMPARTIDA FINALIZADORA----------------------------------------------
    key_t claveFinalizador= obtener_key_t("..//..//finalizadorKey", 123);
    // Crear la memoria compartida
    int idFinalizador = shmget(claveFinalizador, sizeof(bool), IPC_CREAT | 0666);
    // Adjuntar la memoria  a nuestro espacio de direcciones
    bool* terminar = (bool*)shmat(idFinalizador, NULL, 0);

    //---------------------------
    //pedir semaforo para contEgoistaRestriccion
    sem_t *semaforoCnt;
    semaforoCnt = sem_open("/semaforo_egoistaCnt", O_CREAT, 0644, 1);

    while(!*terminar){
        //esperar semaforo de restriccion <3
        sem_wait(egoista);
        //si pudo entrar entonces no hay restriccion, hace su proceso normal
        sem_wait(semaforo); //semaforo de la memoria compartida principal

        //actualizar contadores
        sem_wait(semaforoCnt);
        contEgoistaRestriccion++;
        sem_post(semaforoCnt);

        /*proceso del egoista*/
        printf("Proceso Egoista: %d PROCESANDO\n", pid);
        sleep(segEscritura);

        int aleatorio = generarAleatorio(0, cantidadLineas-1);

        //BORRAR LINEA
        // Adjuntar la memoria compartida a nuestro espacio de direcciones
        void* memoriaCompartida = shmat(idMemoria, NULL, 0);
        if (memoriaCompartida == (void*)-1) {
            perror("Error al adjuntar la memoria compartida");
            break;
        }
        // Castear la memoria compartida a un array de struct LineaMemoria
        struct LineaMemoria* lineas = (struct LineaMemoria*)memoriaCompartida;

        printf("Proceso Egoista: %d intenta borraar la linea: %d\n", pid,aleatorio);
        char buffer[1000]= "";
        char bufferMensaje[1000]= "";
        char convertido[10]= "";

        if (lineas[aleatorio].pid == 0) {
            strcat(bufferMensaje, "La línea esta vacía");
        }else{
            lineas[aleatorio].pid = 0;
            strcat(bufferMensaje, "La línea borrada es:  ");
            sprintf(convertido, "%d", pid);
            strcat(bufferMensaje, " -PID: ");
            strcat(bufferMensaje, convertido);

            sprintf(convertido, "%d", aleatorio);
            strcat(bufferMensaje, " -Linea: ");
            strcat(bufferMensaje, convertido);

            printf("Linea borrada con exito");
        }
        char * fechaHora = obtenerFechaHoraActual();
        parsearInfoBitacora(buffer,pid, "Reader egoísta",fechaHora,"Borrando",bufferMensaje);
        escribirBitacora(semaforoBitacora, buffer);
        /* fin del proceso egoista*/

        if (contEgoistaRestriccion >= 3){
            //reset contador
            sem_wait(semaforoCnt);
            contEgoistaRestriccion=0;
            sem_post(semaforoCnt);
            //liberar semaforo principal
            sem_post(semaforo);
        }
        else{
            //reset contador
            //liberar semaforo principal
            sem_post(semaforo);
            //liberar semaforo egoista
            sem_post(egoista);
        }
        printf("Proceso Egoista: %d Durmiedno\n", pid);
        sleep(segSleep);
    }


    printf("Proceso: %d sale\n", pid);
    // Desvincular la memoria compartida
    shmdt(terminar);
    pthread_exit(NULL);
}


//Proceso de ls hilos:
int main() {
    //SEMAFOROS:
    //1- bitaciora
    semaforoBitacora = obtenerSemaforoBitacora();
    //2- principal
    semaforo = sem_open("/semaforo_writer", O_CREAT, 0644, 1);
    //3-egoista
    egoista = sem_open("/semaforo_egoista", O_CREAT, 0644, 1);


    //SOLICITAR CANT DE READERS AL USUARIO
    int cantEgoistas;
    int segEscritura;
    int segSleep;

    //Solicitar al usuario la informacion necesaria
    printf("Ingrese la cantidad de procesos egoistas: ");
    scanf("%d", &cantEgoistas);

    printf("Ingrese los segundos de escritura: ");
    scanf("%d", &segEscritura);

    printf("Ingrese los segundos dormidos: ");
    scanf("%d", &segSleep);

    //arreglo de hilos lectores
    pthread_t hilo_Readers [cantEgoistas];
    struct ParametrosHilo parametros[cantEgoistas];
    struct Egoista arregloEstadosEgoista[cantEgoistas];

    //Solicita memoria principal
    int idMemoriaPrincipal = solicitarIDMemoriaPrincipal();
    int cantLineas = obtenerCantLineas (idMemoriaPrincipal);

    //Solicita memoria finalizadora
    key_t claveFinalizador= obtener_key_t("..//..//finalizadorKey", 123);
    int idFinalizador = shmget(claveFinalizador, sizeof(bool), IPC_CREAT | 0666);

    //Solicita memoria estados
    int idMemoriaEgoistas = memoriaEstadoEgoistas(cantEgoistas);

    //Pedir semaforo para estados
    semaforoEstadoEgoista = sem_open("/semaforo_estadoEgoista", O_CREAT, 0644, 1);
    if (semaforoEstadoEgoista == SEM_FAILED) {
        printf("Error al pedir el semaforo de estado de egoistas");
    }

    for (int i=0; i<cantEgoistas; i++){
        // Crear estado Egoista
        arregloEstadosEgoista[i].pid = i+1;
        arregloEstadosEgoista[i].estado = 0;
        agregarEgoistaEnPosicion(i, &arregloEstadosEgoista[i], idMemoriaEgoistas);

        // Crear una instancia de la estructura de parámetros
        parametros[i].idMemoria = idMemoriaPrincipal;
        parametros[i].cantidadLineas = cantLineas;
        parametros[i].pid = i+1;
        parametros[i].semaforo = semaforo;
        parametros[i].segSleep = segSleep;
        parametros[i].segEscritura = segEscritura;
        parametros[i].estadoEgoista = &arregloEstadosEgoista[i];
        parametros[i].idMemoriaEstadoEgoista = idMemoriaEgoistas;
        parametros[i].semaforoEstadoEgoista = semaforoEstadoEgoista;

        if (pthread_create(&hilo_Readers[i], NULL, procesoEgoista, (void*)&parametros[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }
    // Esperar a que todos los hilos terminen su ejecución
    for (int i = 0; i < cantEgoistas; i++) {
        if (pthread_join(hilo_Readers[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }

    printf("Hello, World!\n");
    return 0;
}
