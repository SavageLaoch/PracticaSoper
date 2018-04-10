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
#define MAXBUF 37


int sem_id;

char producir_item(int num_item){
  if(num_item < 26){
    return num_item + 65;
  }else{
    return num_item % 27 + 48;
  }
}

void consumir_item(char item){
  printf("Consumiendo item: %c\n",item);
  fflush(stdout);
}


int main(){
  char *buffer; /* shared buffer */
	int key, id_zone;
  int pid;
  int i;
  unsigned short array[3];/*mutex1 0,vacioMaxbuf 1,lleno 0 2*/

  key = ftok(FILEKEY, KEY);
 	if (key == -1) {
 		fprintf (stderr, "Error with key \n");
 		exit(EXIT_FAILURE);
 	}
 	id_zone = shmget (key, sizeof(char)*MAXBUF, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
 	if (id_zone == -1) {
 		fprintf (stderr, "Error with id_zone \n");
 		exit(EXIT_FAILURE);
 	}

  array[0] = 1;
	array[1] = MAXBUF;
  array[2] = 0;
	if(Crear_Semaforo(SEMKEY,3,&sem_id) == ERROR){
		shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
		printf("Error al crear los semaforos\n");
		exit(EXIT_FAILURE);
	}
	if(Inicializar_Semaforo(sem_id,array) == ERROR){
		shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
		printf("Error al inicializar los semaforos\n");
		exit(EXIT_FAILURE);
	}

  pid = fork();
  if(pid < 0){
    printf("No se ha podido crear el proceso\n");
    exit(EXIT_SUCCESS);
  }
  if(pid == 0){
    /* Productor*/
    char c;
   	buffer = shmat (id_zone, (char *)0, 0);
   	if (buffer == NULL) {
   		fprintf (stderr, "Error reserve shared memory \n");
   		exit(EXIT_FAILURE);
    }
    for(i = 0;i < MAXBUF;i++){
      c = producir_item(i);
      if (Down_Semaforo(sem_id,1,SEM_UNDO) == ERROR){
        printf("Fallo al bajar el semaforo vacio\n");
        exit(EXIT_FAILURE);
      }
      if (Down_Semaforo(sem_id,0,SEM_UNDO) == ERROR){
        printf("Fallo al bajar el semaforo mutex\n");
        exit(EXIT_FAILURE);
      }
      buffer[i] = c;
      if (Up_Semaforo(sem_id,0,SEM_UNDO) == ERROR){
        printf("Fallo al subir el semaforo mutex\n");
        exit(EXIT_FAILURE);
      }
      if (Up_Semaforo(sem_id,2,SEM_UNDO) == ERROR){
        printf("Fallo al subir el semaforo lleno\n");
        exit(EXIT_FAILURE);
      }
      printf("Generado item: %c\n",c);
      fflush(stdout);
    }
  }else{
    /*CPnsumidor*/
    char c;
    buffer = shmat (id_zone, (char *)0, 0);
   	if (buffer == NULL) {
   		fprintf (stderr, "Error reserve shared memory \n");
   		exit(EXIT_FAILURE);
    }
    for(i = 0;i < MAXBUF;i++){
      if (Down_Semaforo(sem_id,2,SEM_UNDO) == ERROR){
        printf("Fallo al bajar el semaforo lleno\n");
        exit(EXIT_FAILURE);
      }
      if (Down_Semaforo(sem_id,0,SEM_UNDO) == ERROR){
        printf("Fallo al bajar el semaforo mutex\n");
        exit(EXIT_FAILURE);
      }
      c = buffer[i];
      if (Up_Semaforo(sem_id,0,SEM_UNDO) == ERROR){
        printf("Fallo al subir el semaforo mutex\n");
        exit(EXIT_FAILURE);
      }
      if (Up_Semaforo(sem_id,1,SEM_UNDO) == ERROR){
        printf("Fallo al subir el semaforo vacio\n");
        exit(EXIT_FAILURE);
      }
      consumir_item(c);
    }
  }
  wait(NULL);
  if (pid != 0) {
    shmdt ((char *)buffer);
	  shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
    Borrar_Semaforo(sem_id);
  }
  exit(EXIT_SUCCESS);
}
