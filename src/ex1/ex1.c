//#include <stdio.h>
//#include <stdlib.h>
//#include <pthread.h>

//void * funcion_hilo ( void * arg ) {
    //int id = *(int *)arg;

    //for ( int i = 0; i < 5; i++ ) {
        //printf("Hilo %d\n", id);
    //}
//}

//int main ( int argc, char * argv[] ) {
    //pthread_t * hilos;
    //int * ids;
    //pthread_attr_t attr_fifo, attr_rr;
    //struct sched_param sp_fifo, sp_rr;
    //int num_hilos;

    ///* Comprobar argumentos */
    //if ( argc != 2 ) {
        //return 1;
    //}

    //num_hilos 	= atoi(argv[1]);
    //hilos 		= calloc(num_hilos, sizeof(pthread_t));
	//ids 		= calloc(num_hilos, sizeof(int));
	
    //pthread_attr_init(&attr_fifo);
    //pthread_attr_setschedpolicy(&attr_fifo, SCHED_FIFO);
    //sp_fifo.sched_priority = 10;
    //pthread_attr_setschedparam(&attr_fifo, &sp_fifo);
    
    //pthread_attr_init(&attr_rr);
    //pthread_attr_setschedpolicy(&attr_rr, SCHED_FIFO);
	//sp_rr.sched_priority = 20;
    //pthread_attr_setschedparam(&attr_rr, &sp_rr);
    
    //printf("Num Hilos: %d\n\n", num_hilos);

    //for ( int i = 0; i < num_hilos; i++ ) {
		//ids[i] = i;
        //pthread_create(&hilos[i], ( i & 1 == 0 ) ? &attr_fifo : &attr_rr, funcion_hilo, &ids[i]);
    //}

    //for ( int i = 0; i < num_hilos; i++ ) {
        //pthread_join(hilos[i], NULL);
    //}

    //return 0;
//}
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>

// Número de hilos
#define NUM_THREADS 3

// Estructura para los hilos
typedef struct {
    int thread_id;
    int policy;
    int priority;
} thread_data_t;

// Función que será ejecutada por cada hilo
void* thread_function(void* arg) {
    thread_data_t* data = (thread_data_t*) arg;
    struct sched_param param;
    
    // Establecer la prioridad para cada hilo
    param.sched_priority = data->priority;
    
    if (pthread_setschedparam(pthread_self(), data->policy, &param) != 0) {
        perror("Error al establecer la política de programación");
        pthread_exit(NULL);
    }
    
    printf("Hilo %d: Política = %d, Prioridad = %d\n", data->thread_id, data->policy, data->priority);
    
    // Hacer que el hilo realice alguna operación para observar el comportamiento
    for (int i = 0; i < 5; i++) {
        printf("Hilo %d en ejecución... Iteración %d\n", data->thread_id, i);
        //usleep(500000); // 0.5 segundos
		for (int i = 0; i < 100000000; i++) {
			// Realizar un cálculo sencillo para consumir tiempo de CPU.
			volatile int temp = i * i;
		}
    }
    
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    
    // Establecer políticas y prioridades para los hilos
    int policies[] = {SCHED_FIFO, SCHED_FIFO, SCHED_FIFO };  // Tres políticas de planificación
    int priorities[] = {30, 20, 10};  // Diferentes prioridades
    
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i + 1;
        thread_data[i].policy = policies[i];
        thread_data[i].priority = priorities[i];
        
        // Crear los hilos
        if (pthread_create(&threads[i], NULL, thread_function, (void*) &thread_data[i]) != 0) {
            perror("Error al crear el hilo");
            exit(EXIT_FAILURE);
        }
    }
    
    // Esperar a que todos los hilos terminen
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}
