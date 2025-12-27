#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

#define NUM_THREADS 3

// Estructura para los hilos
struct Hilo_Info {
    int hilo;
    int sched;
    int prior;
};

// Función que será ejecutada por cada hilo
void * funcion_hilo ( void * arg ) {
    struct Hilo_Info * data = ( struct Hilo_Info * ) arg;
    struct sched_param param;
    int i;
    
    param.sched_priority = data->prior;
    
    pthread_setschedparam(pthread_self(), data->sched, &param);
    
    printf("Hilo %d: SCHED = %d, PRIOR = %d\n", data->hilo, data->sched, data->prior);
    
    // Hacer que el hilo realice alguna operación para observar el comportamiento
    for ( i = 0; i < 10; i++ ) {
        printf("Hilo %d : %d\n", data->hilo, i);
        //usleep(500000);
		for (int i = 0; i < 100000000; i++) {
			// Realizar un cálculo sencillo para consumir tiempo de CPU.
			volatile int temp = i * i;
		}
    }
    
    pthread_exit(NULL);
}

int main() {
    pthread_t hilos[NUM_THREADS];
    struct Hilo_Info hilos_info[NUM_THREADS];
    int i;
    
    // Establecer políticas y prioridades para los hilos
    int scheds[] = {SCHED_FIFO, SCHED_FIFO, SCHED_FIFO };  // Tres políticas de planificación
    int priors[] = {30, 30, 30};  // Diferentes prioridades
    
    for ( i = 0; i < NUM_THREADS; i++ ) {
        hilos_info[i].hilo = i + 1;
        hilos_info[i].sched = scheds[i];
        hilos_info[i].prior = priors[i];
        
        pthread_create(&hilos[i], NULL, funcion_hilo, (void*) &hilos_info[i]);
    }
    
    // Esperar a que todos los hilos terminen
    for ( i = 0; i < NUM_THREADS; i++ ) {
        pthread_join(hilos[i], NULL);
    }
    
    return 0;
}
