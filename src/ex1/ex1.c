#include <stdio.h>
#include <stdlib.h>

void * funcion_hilo ( void * arg ) {
    int n = *(int *)arg;

    for ( int i = 0; i < 3; i++ ) {
        printf("Hilo %d", n);
    }
}

int main ( int argc, char * argv[] ) {
    pthread_t * hilos;
    pthread_attr_t attr;
    int num_hilos;

    /* Comprobar argumentos */
    if ( argc != 2 ) {
        return 1;
    }

    num_hilos = atoi(argv[1]);
    hilos = calloc(num_hilos, sizeof(pthread_t));

    pthread_attr_init(&attr);
    pthread_setschedpolicy(&attr, SCHED_FIFO);

    printf("Num Hilos: %d\n\n", num_hilos);

    for ( int i = 0; i < num_hilos; i++ ) {
        pthread_create(&hilos[i], &attr, funcion_hilo, i);
    }

    for ( int i = 0; i < num_hilos; i++ ) {
        pthread_join(&hilos[i], NULL);
    }

    return 0;
}
