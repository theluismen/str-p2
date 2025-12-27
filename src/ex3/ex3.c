#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <pthread.h>
#include <string.h>

#define QUEUE_NAME		"/cola"
#define QUEUE_MAX_MSG 	10
#define QUEUE_MSG_SIZE	8

mqd_t cola;

void * funcion_hilo_prod ( void * arg ) {
	char mensaje[QUEUE_MSG_SIZE+1];
	unsigned int prioridad, i;
	
    for ( i = 1; i <= QUEUE_MAX_MSG; i++ ) {
		sprintf(mensaje, "MSG %d", i);
		prioridad = i % 2;
		if ( mq_send(cola, mensaje, strlen(mensaje)+1, prioridad) != -1 ) {
			printf("[PROD] -> (%8s,%2d)\n", mensaje, prioridad);
		} else {
			perror("mq_send error");
		}	
	}
	
	printf("\n");
	
	return NULL;
}

void * funcion_hilo_cons ( void * arg ) {
    char mensaje[QUEUE_MSG_SIZE+1];
    unsigned int prioridad, i;
    ssize_t msg_len;

    for ( i = 1; i <= QUEUE_MAX_MSG; i++ ) {
        // Bloqueante: espera hasta que haya un mensaje
        msg_len = mq_receive(cola, mensaje, QUEUE_MSG_SIZE+1, &prioridad);

        if ( msg_len >= 0 ) {
            // Asegurarse de que el mensaje está null-terminated
            mensaje[msg_len] = '\0';
            printf("[CONS] <- (%8s,%2d)\n", mensaje, prioridad);
        } else {
            perror("mq_receive error");
        }
    }

	printf("\n");

    return NULL;
}


int main ( void ) {
    pthread_t hilo_prod, hilo_cons;
	struct mq_attr attr;
	
	attr.mq_flags   = 0;
	attr.mq_maxmsg  = QUEUE_MAX_MSG;
	attr.mq_msgsize = QUEUE_MSG_SIZE+1;
	attr.mq_curmsgs = 0;
	
	mq_unlink(QUEUE_NAME);
	cola = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0660, &attr);

	if ( cola == (mqd_t)-1 ) {
		perror("ERROR ABRIR COLA\n");
		exit(1);
	}

    pthread_create(&hilo_prod, NULL, funcion_hilo_prod, NULL);
    pthread_join(hilo_prod, NULL);
    
    pthread_create(&hilo_cons, NULL, funcion_hilo_cons, NULL);
    pthread_join(hilo_cons, NULL);
    
    mq_close(cola);
    mq_unlink(QUEUE_NAME);

    return 0;
}
