#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define SHMEM_NAME		"/shared_mem"
#define SEM_PROD_NAME	"/sem_prod"
#define SEM_CONS_NAME	"/sem_cons"
#define MSG_SIZE 		100

int main ( void ) {
	int fd, i;
	char * shmem;
	char msg[MSG_SIZE] = "Un dia de partit\n"
						"Al Gol Nord vaig anar\n"
						"Només entrar a la grada\n"
						"Em vaig enamorar";
	sem_t * sem_prod, * sem_cons;

	/* Abrir shmem */
	fd = shm_open(SHMEM_NAME, O_CREAT | O_RDWR, 0666);

	if ( fd == -1 ) {
        perror("shm_open falló");
        exit(1);
    }

	/* Dimensionar la shmem */
	ftruncate(fd, sizeof(char));

	/* Mapear shmem a espacio de direcciones */
	shmem = mmap(NULL, sizeof(char), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if ( shmem == MAP_FAILED ) {
        perror("mmap falló");
        exit(1);
    }

	/* Inicializar Semaforos */
	sem_prod = sem_open(SEM_PROD_NAME, O_CREAT, 0666, 1);
    sem_cons = sem_open(SEM_CONS_NAME, O_CREAT, 0666, 0);

	printf("Mensaje:\n\n\033[34m%s\033[0m\n\n", msg); // msg en VERDE
	printf("Escribiendo en memoria compartida...\n");
	
	/* Bucle de Envio */
	for ( i = 0; i < strlen(msg)+1; i++ ) {
		sem_wait(sem_prod);
		*shmem = msg[i];
		sem_post(sem_cons);
	}
	
	printf("Mensaje Enviado\n\n");
	
	/* Cerrar, Desmapear y Desvincular */
	close(fd);
	munmap(shmem, sizeof(char));
	shm_unlink(SHMEM_NAME);
	sem_unlink(SEM_PROD_NAME);
	sem_unlink(SEM_CONS_NAME);

    return 0;
}
