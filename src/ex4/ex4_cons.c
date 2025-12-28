#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>

#define SHMEM_NAME		"/shared_mem"
#define SEM_PROD_NAME	"/sem_prod"
#define SEM_CONS_NAME	"/sem_cons"
#define MSG_SIZE 		100

int main ( void ) {
	int fd, i, endstr;
	char * shmem;
	char msg[MSG_SIZE];
	sem_t * sem_prod, * sem_cons;
	
	/* Abrir shmem */
	fd = shm_open(SHMEM_NAME, O_RDWR, 0666);

	if ( fd == -1 ) {
        perror("shm_open falló");
        exit(1);
    }

	/* Mapear shmem a espacio de direcciones */
	shmem = mmap(NULL, sizeof(char), PROT_READ, MAP_SHARED, fd, 0);

	if ( shmem == MAP_FAILED ) {
        perror("mmap falló");
        exit(1);
    }

	/* Abrir Semaforos */
	sem_prod = sem_open(SEM_PROD_NAME, 0);
    sem_cons = sem_open(SEM_CONS_NAME, 0);

	printf("Reciviendo de memoria compartida...\n\n");

	/* Bucle de REcepcion */
	endstr = 0;
	for ( i = 0; ! endstr ; i++) {
		sem_wait(sem_cons);
		msg[i] = *shmem;
		if ( *shmem == '\0' ) {
			endstr = 1;
		}
		sem_post(sem_prod);
	}
	
	printf("Mensaje Recibido:\n\n\033[34m%s\033[0m\n\n", msg);
	
	/* Cerrar y Desmapear */
	close(fd);
	munmap(shmem, sizeof(char));

    return 0;
}
