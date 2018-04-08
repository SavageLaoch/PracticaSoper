#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/shm.h> /* shm* */
#define FILEKEY "/bin/cat"
#define KEY 1300


/**
 * @brief aleat_num
 *
 * Genera un numero aleatorio entre inf y sup
 *
 * @param inf Cota inferior
 * @param sup Cota superior
 * @return int Entero generado
 */

int aleat_num(int inf, int sup){
  if (inf>sup) return -1;
  int randm,rango,grupo,limite;
  rango=sup-inf+1;
  grupo=RAND_MAX/rango;
  limite=grupo*rango;
  do{
    randm=rand();
  }while(randm>=limite);
  return (randm/grupo)+inf;
}

void manejador(int sig){
	printf("Hijo terminado\n");
	return;
}

struct info{
 char nombre[80];
 int id;
}info;

int main(int argc, char *argv[]){
	int i,pid;
	int *buffer; /* shared buffer */
	int key, id_zone;

	if (signal(SIGUSR1,manejador) == SIG_ERR){
		perror("signal error");
		exit (EXIT_FAILURE);
	}

	srand(time(NULL));
	if (argc < 2){
		printf("No se han introducido suficientes parámetros\n");
		exit(EXIT_FAILURE);
	}
	key = ftok(FILEKEY, KEY);
 	if (key == -1) {
 		fprintf (stderr, "Error with key \n");
 		exit(EXIT_FAILURE);
 	}
 	id_zone = shmget (key, sizeof(int), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
 	if (id_zone == -1) {
 		fprintf (stderr, "Error with id_zone \n");
 		exit(EXIT_FAILURE);
 	}
 	buffer = shmat (id_zone, (char *)0, 0);
 	if (buffer == NULL) {
 		fprintf (stderr, "Error reserve shared memory \n");
 		exit(EXIT_FAILURE);
	 }
	buffer[0] = 0;
	for (i = 0;i < argc;i++){
		pid = fork();
		if(pid < 0){
			printf("Error al crear el hijo %d\n",i);
			exit(EXIT_FAILURE);
		}
		if(pid == 0){
			sleep(aleat_num(1,5));
			printf("Hijo %d: Introduzca el nombre del cliente:\n",i);
			fscanf(stdin,"%s",info.nombre);
			id_zone = shmget (key, sizeof(int), SHM_R | SHM_W);
 			if (id_zone == -1) {
 				fprintf (stderr, "Error with id_zone \n");
 				exit(EXIT_FAILURE);
 			}
 			buffer = shmat (id_zone, (char *)0, 0);
 			if (buffer == NULL) {
 				fprintf (stderr, "Error reserve shared memory \n");
 				exit(EXIT_FAILURE);
	 		}
	 		buffer[0] ++;
	 		shmdt ((char *)buffer);
 			shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
 			kill(getppid(),SIGUSR1);
			exit(EXIT_SUCCESS);
		}
	}
	while(wait(NULL) > 0);
	printf("Valor de la memoria compartida: %d\n",buffer[0]);
	shmdt ((char *)buffer);
	shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);

	exit(EXIT_SUCCESS);
}