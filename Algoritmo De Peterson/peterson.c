/*
 * Algoritmo de Peterson — exclusión mutua para dos procesos.
 * Solo usa variables compartidas (sin instrucciones atómicas especiales):
 *   quiere[i]  -> el proceso i avisa de que quiere entrar
 *   turno      -> a quién le toca cuando ambos quieren entrar
 */
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>

#define ITERACIONES 200000

static atomic_int quiere[2];
static atomic_int turno;
static int contador = 0;   /* recurso compartido */

static void entrar(int i)
{
    int j = 1 - i;
    quiere[i] = 1;      /* quiero entrar */
    turno = j;          /* cedo el turno al otro */
    while (quiere[j] && turno == j)
        ;               /* espera activa */
}

static void salir(int i)
{
    quiere[i] = 0;      /* ya salí */
}

static void *proceso(void *arg)
{
    int i = *(int *)arg;
    for (int k = 0; k < ITERACIONES; k++) {
        entrar(i);
        contador++;     /* sección crítica */
        salir(i);
    }
    return NULL;
}

int main(void)
{
    pthread_t hilo[2];
    int id[2] = {0, 1};

    for (int i = 0; i < 2; i++)
        pthread_create(&hilo[i], NULL, proceso, &id[i]);
    for (int i = 0; i < 2; i++)
        pthread_join(hilo[i], NULL);

    const int esperado = 2 * ITERACIONES;
    printf("contador = %d (esperado %d)\n", contador, esperado);
    puts(contador == esperado ? "OK: exclusión mutua respetada"
                              : "FALLO: hubo condición de carrera");
    return contador == esperado ? 0 : 1;
}
