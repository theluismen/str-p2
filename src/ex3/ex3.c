#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <pthread.h>
#include <string.h>

#define QUEUE_NAME		"/cola"
#define QUEUE_MAX_MSG 	10
#define QUEUE_MSG_SIZE	32

mqd_t cola;

void * funcion_hilo_prod ( void * arg ) {
	char mensaje[QUEUE_MSG_SIZE];
	unsigned int prioridad = 1;
	
	
    for ( int i = 0; i < 5; i++ ) {
		sprintf(mensaje, "Hola como tas %d", i);
		if ( mq_send(cola, mensaje, strlen(mensaje)+1, prioridad) != -1 ) {
			printf("[PROD] - Enviado Mensaje: %d %s", i, mensaje);
		} else {
			perror("mq_send");
		}	
	}
}

void * funcion_hilo_cons ( void * arg ) {
    char mensaje[QUEUE_MSG_SIZE];
    unsigned int prioridad;
    ssize_t bytes_recibidos;

    while (1) {
        // Bloqueante: espera hasta que haya un mensaje
        bytes_recibidos = mq_receive(cola, mensaje, QUEUE_MSG_SIZE, &prioridad);

        if (bytes_recibidos >= 0) {
            // Asegurarse de que el mensaje está null-terminated
            mensaje[bytes_recibidos] = '\0';
            printf("[CONS] - Recibido Mensaje (prio %u): %s\n", prioridad, mensaje);
        } else {
            perror("mq_receive");
        }

        // Simular tiempo de procesamiento
        //usleep(150000); // 150 ms
    }

    return NULL;
}


int main ( void ) {
    pthread_t hilo_prod, hilo_cons;
	struct mq_attr attr;
	
	attr.mq_flags   = 0;
	attr.mq_maxmsg  = QUEUE_MAX_MSG;
	attr.mq_msgsize = QUEUE_MSG_SIZE;
	attr.mq_curmsgs = 0;
	
	cola = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0660, &attr);

	if ( cola == (mqd_t)-1 ) {
		printf("ERROR ABRIR COLA\n");
		exit(1);
	}

    pthread_create(&hilo_prod, NULL, funcion_hilo_prod, NULL);
    pthread_create(&hilo_cons, NULL, funcion_hilo_cons, NULL);

    pthread_join(hilo_prod, NULL);
    pthread_join(hilo_cons, NULL);
    
    mq_close(cola);
    mq_unlink(QUEUE_NAME);

    return 0;
}
