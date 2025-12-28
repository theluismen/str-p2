#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

#define NUM_THREADS 4

/* Estructura para los hilos */
struct Hilo_Info {
    int hilo;
    int sched;
    int prior;
};

/* Función de cada hilo */
void * funcion_hilo ( void * arg ) {
    struct Hilo_Info * data = ( struct Hilo_Info * ) arg;
    struct sched_param param;
    volatile int temp;
    int i, j;
    
    /* Setear prioridad de hilo */
    param.sched_priority = data->prior;
    /* Configurar politica y prioridad de hilo*/
    pthread_setschedparam(pthread_self(), data->sched, &param);
    
    /* Mensaje de Creación de hilo */
    printf("Hilo %d: SCHED = %d, PRIOR = %d\n", data->hilo, data->sched, data->prior);
    
    /* Bucle de mostrar mensajes */
    for ( i = 0; i < 8; i++ ) {
		/* Bucle de retardo */
		for ( j = 0; j < 100000000; j++ ) {
			// Realizar un cálculo sencillo para consumir tiempo de CPU.
			temp = j * j;
		}
        printf("Hilo %d : %d\n", data->hilo, i);
    }
    
    pthread_exit(NULL);
}

int main() {
    pthread_t hilos[NUM_THREADS];
    struct Hilo_Info hilos_info[NUM_THREADS];
    int scheds[] = {SCHED_RR, SCHED_RR, SCHED_FIFO, SCHED_FIFO };  // Políticas de planificación
    int priors[] = {10, 10, 20, 20};  // Prioridades
    int i;
    
    /* Bucle de Creación de Hilos */
    for ( i = 0; i < NUM_THREADS; i++ ) {
        hilos_info[i].hilo = i + 1;
        hilos_info[i].sched = scheds[i];
        hilos_info[i].prior = priors[i];
        
        pthread_create(&hilos[i], NULL, funcion_hilo, (void*) &hilos_info[i]);
    }
    
    /* Bucle de Espera de Hilos */
    for ( i = 0; i < NUM_THREADS; i++ ) {
        pthread_join(hilos[i], NULL);
    }
    
    return 0;
}
