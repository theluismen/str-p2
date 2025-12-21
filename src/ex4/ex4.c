#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SHMEM_NAME		"/cola"

void * funcion_hilo_prod ( void * arg ) {
	for ( int i = 0; i < 100; i++ ) {
		printf("%d\n", i);
	}
	return NULL;
}

void * funcion_hilo_cons ( void * arg ) {
	for ( int i = 200; i < 300; i++ ) {
		printf("%d\n", i);
	}
    return NULL;
}

int main ( void ) {
    pthread_t hilo_prod, hilo_cons;
	struct mq_attr attr;

	pthread_create(&hilo_prod, NULL, funcion_hilo_prod, NULL );
	pthread_create(&hilo_cons, NULL, funcion_hilo_cons, NULL );

	pthread_join(hilo_prod, NULL);
	pthread_join(hilo_cons, NULL);

    return 0;
}
