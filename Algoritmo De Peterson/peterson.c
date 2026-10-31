#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <stdint.h>
#include <unistd.h>

// Variables de control atómicas (solucionan visibilidad y reordenamiento)
atomic_bool interesado[2] = {false, false};
atomic_int turno = 0;

void* hilo(void* arg) {
    int id = (int)(intptr_t)arg;
    int otro = 1 - id;

    // 1. PROTOCOLO DE ENTRADA (Demuestra interés y cede el turno)
    interesado[id] = true;
    turno = otro;
    
    // 2. ESPERA ACTIVA (Busy waiting)
    // Se queda esperando si el otro hilo está interesado y es su turno
    while (interesado[otro] && turno == otro) {
        // No hace nada, solo espera
    }

    // --- SECCIÓN CRÍTICA ---
    printf("--> Hilo %d ENTRO a la sección crítica\n", id);
    sleep(1); // Simula trabajo
    printf("<-- Hilo %d SALIÓ de la sección crítica\n", id);
    // -----------------------

    // 3. PROTOCOLO DE SALIDA (Indica que ya no está interesado)
    interesado[id] = false;

    return NULL;
}

int main(void) {
    pthread_t t0, t1;

    // Creamos los hilos pasando el ID de forma segura mediante casting
    pthread_create(&t0, NULL, hilo, (void*)(intptr_t)0);
    pthread_create(&t1, NULL, hilo, (void*)(intptr_t)1);

    pthread_join(t0, NULL);
    pthread_join(t1, NULL);

    printf("Fin del programa. Exclusión mutua exitosa.\n");
    return 0;
}
