#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void * funcion_hilo ( void * arg ) {
    int id = *(int *)arg;

    for ( int i = 0; i < 5; i++ ) {
        printf("Hilo %d\n", id);
    }
}

int main ( int argc, char * argv[] ) {
    pthread_t * hilos;
    int * ids;
    pthread_attr_t attr_fifo, attr_rr;
    struct sched_param sp_fifo, sp_rr;
    int num_hilos;

    /* Comprobar argumentos */
    if ( argc != 2 ) {
        return 1;
    }

    num_hilos 	= atoi(argv[1]);
    hilos 		= calloc(num_hilos, sizeof(pthread_t));
	ids 		= calloc(num_hilos, sizeof(int));
	
    pthread_attr_init(&attr_fifo);
    pthread_attr_setschedpolicy(&attr_fifo, SCHED_FIFO);
    sp_fifo.sched_priority = 10;
    pthread_attr_setschedparam(&attr_fifo, &sp_fifo);
    
    pthread_attr_init(&attr_rr);
    pthread_attr_setschedpolicy(&attr_rr, SCHED_FIFO);
	sp_rr.sched_priority = 20;
    pthread_attr_setschedparam(&attr_rr, &sp_rr);
    
    printf("Num Hilos: %d\n\n", num_hilos);

    for ( int i = 0; i < num_hilos; i++ ) {
		ids[i] = i;
        pthread_create(&hilos[i], ( i & 1 == 0 ) ? &attr_fifo : &attr_rr, funcion_hilo, &ids[i]);
    }

    for ( int i = 0; i < num_hilos; i++ ) {
        pthread_join(hilos[i], NULL);
    }

    return 0;
}
