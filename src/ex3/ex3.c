#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <pthread.h>
#include <string.h>

#define QUEUE_NAME		"/cola"
#define QUEUE_MAX_MSG 	10
#define QUEUE_MSG_SIZE	8

mqd_t cola;

/* Función de hilo Productor */
void * funcion_hilo_prod ( void * arg ) {
	char mensaje[QUEUE_MSG_SIZE+1];
	unsigned int prioridad, i;
	int ret;
	
	/* Bucle de Recepcion */
    for ( i = 1; i <= QUEUE_MAX_MSG; i++ ) {
		sprintf(mensaje, "MSG %d", i);
		prioridad = i % 2;
		ret = mq_send(cola, mensaje, strlen(mensaje)+1, prioridad);
		
		if ( ret != -1 ) {
			printf("[PROD] -> (%8s,%2d)\n", mensaje, prioridad);
		} else {
			perror("mq_send error");
		}	
	}
	
	printf("\n");
	
	pthread_exit(NULL);
}

/* Función de cada Consumidor */
void * funcion_hilo_cons ( void * arg ) {
    char mensaje[QUEUE_MSG_SIZE+1];
    unsigned int prioridad, i;
    ssize_t msg_len;

	/* Bucle de Envio */
    for ( i = 1; i <= QUEUE_MAX_MSG; i++ ) {
        msg_len = mq_receive(cola, mensaje, QUEUE_MSG_SIZE+1, &prioridad);

        if ( msg_len >= 0 ) {
            mensaje[msg_len] = '\0'; // Poner centinela \0
            printf("[CONS] <- (%8s,%2d)\n", mensaje, prioridad);
        } else {
            perror("mq_receive error");
        }
    }

	printf("\n");

    pthread_exit(NULL);
}

int main ( void ) {
    pthread_t hilo_prod, hilo_cons;
	struct mq_attr attr;
	
	/* Configurar Atributos de Cola */
	attr.mq_flags   = 0;
	attr.mq_maxmsg  = QUEUE_MAX_MSG;
	attr.mq_msgsize = QUEUE_MSG_SIZE+1;
	attr.mq_curmsgs = 0;
	
	/* Abrir cola */
	mq_unlink(QUEUE_NAME);
	cola = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0660, &attr);

	/* Manejar Error Abrir Cola*/
	if ( cola == (mqd_t)-1 ) {
		perror("ERROR ABRIR COLA\n");
		exit(1);
	}

	/* Crear Hilo Productor, y que acabe */
    pthread_create(&hilo_prod, NULL, funcion_hilo_prod, NULL);
    pthread_join(hilo_prod, NULL);
    
    /* Crear Hilo Consumidor, y que acabe */
    pthread_create(&hilo_cons, NULL, funcion_hilo_cons, NULL);
    pthread_join(hilo_cons, NULL);
    
    /* Cerrar y Desvincular cola */
    mq_close(cola);
    mq_unlink(QUEUE_NAME);

    return 0;
}
