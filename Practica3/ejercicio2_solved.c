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
#include "semaforos.h"
#define FILEKEY "/bin/cat"
#define SEMKEY 75788
#define KEY 1300


int sem_id;


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
	if(Down_Semaforo(sem_id,0,SEM_UNDO) == ERROR){
				printf("Error al bajar el semaforo 0\n");
				exit(EXIT_FAILURE);
			}
	printf("Hijo terminado\n");
	if(Up_Semaforo(sem_id,0,SEM_UNDO) == ERROR){
				printf("Error al subir el semaforo 0\n");
				exit(EXIT_FAILURE);
			}
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
	unsigned short array[2];

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

	array[0] = 1;
	array[1] = 1;
	if(Crear_Semaforo(SEMKEY,2,&sem_id) == ERROR){
		shmdt ((char *)buffer);
		shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
		printf("Error al crear los semaforos\n");
		exit(EXIT_FAILURE);
	}
	if(Inicializar_Semaforo(sem_id,array) == ERROR){
		shmdt ((char *)buffer);
		shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
		printf("Error al inicializar los semaforos\n");
		exit(EXIT_FAILURE);
	}
	/* 0 para entrada salida 1 para memoria compartida*/
	for (i = 0;i < atoi(argv[1]);i++){
		pid = fork();
		if(pid < 0){
			printf("Error al crear el hijo %d\n",i);
			exit(EXIT_FAILURE);
		}
		if(pid == 0){
			sleep(aleat_num(1,5));
			if(Down_Semaforo(sem_id,0,SEM_UNDO) == ERROR){
				printf("Error al bajar el semaforo 0\n");
				exit(EXIT_FAILURE);
			}
			printf("Hijo %d: Introduzca el nombre del cliente:\n",i);
			fscanf(stdin,"%s",info.nombre);
			if(Up_Semaforo(sem_id,0,SEM_UNDO) == ERROR){
				printf("Error al subir el semaforo 0\n");
				exit(EXIT_FAILURE);
			}
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

	 		if(Down_Semaforo(sem_id,1,SEM_UNDO) == ERROR){
				printf("Error al bajar el semaforo 0\n");
				exit(EXIT_FAILURE);
			}
	 		buffer[0] ++;
	 		if(Up_Semaforo(sem_id,1,SEM_UNDO) == ERROR){
				printf("Error al subir el semaforo 0\n");
				exit(EXIT_FAILURE);
			}

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
  Borrar_Semaforo(sem_id);

	exit(EXIT_SUCCESS);
}
