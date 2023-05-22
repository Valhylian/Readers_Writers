




#include "funcionesMenu.h"


// Estructura para almacenar la información de cada línea en la memoria compartida




int main() {
    sem_t * semaforoBitacora;
    semaforoBitacora = obtenerSemaforoBitacora();
   cargarSemaforoBitacora(semaforoBitacora);

    int salida = 0;
    do {
        printf("\n------- Menú principal -------\n\n");
        salida = menuPrincipal();
    }while(salida == 0);
    return 0;
}
