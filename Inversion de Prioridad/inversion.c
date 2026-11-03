#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// 1. Declarar el mutex y su objeto de atributos
pthread_mutex_t mutex;
pthread_mutexattr_t attr;

void* hilo_baja_prioridad(void* arg) {
    pthread_mutex_lock(&mutex);
    printf("[Baja] Tengo el recurso compartido. Trabajando...\n");
    sleep(3); // Simula que tarda en liberar el recurso
    printf("[Baja] Liberando recurso.\n");
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* hilo_alta_prioridad(void* arg) {
    sleep(1); // Espera a que el hilo bajo tome el mutex primero
    printf("[Alta] Intentando entrar a la sección crítica...\n");
    
    // Aquí ocurre la magia: al intentar bloquear, el OS detecta que 'Baja' tiene el mutex
    // y eleva automáticamente la prioridad de 'Baja' para que termine rápido.
    pthread_mutex_lock(&mutex); 
    
    printf("[Alta] ¡Éxito! Recurso obtenido por Alta.\n");
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t alta, baja;

    // 2. Configurar el mutex para que use HERENCIA DE PRIORIDAD
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setprotocol(&attr, PTHREAD_PRIO_INHERIT); // <-- Línea clave
    pthread_mutex_init(&mutex, &attr);

    // 3. Crear los hilos
    pthread_create(&baja, NULL, hilo_baja_prioridad, NULL);
    pthread_create(&alta, NULL, hilo_alta_prioridad, NULL);

    // Esperar a que terminen
    pthread_join(baja, NULL);
    pthread_join(alta, NULL);

    // 4. Limpieza
    pthread_mutex_destroy(&mutex);
    pthread_mutexattr_destroy(&attr);
    return 0;
}
