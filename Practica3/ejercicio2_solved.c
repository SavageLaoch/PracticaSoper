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
#include <signal.h>
#include "semaforos.h"
#define FILEKEY "/bin/cat"
#define SEMKEY 75788
#define KEY 1300

int sem_id;

typedef struct info{
 char nombre[80];
 int id;
}Info;

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
  int key,id_zone;
  Info *buffer;
  key = ftok(FILEKEY, KEY);
  if (key == -1) {
    fprintf (stderr, "Error with key \n");
    return;
  }
  id_zone = shmget (key, sizeof(Info),  IPC_CREAT | SHM_R | SHM_W);
  if (id_zone == -1) {
    fprintf (stderr, "Error with id_zone manejador\n");
    return;
  }
  buffer = shmat (id_zone, (char *)0, 0);
  if (buffer == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return;
  }
  printf("Usuario: %s. Id: %d\n",buffer->nombre,buffer->id);
  return;
}

int main(int argc, char const *argv[]) {
  int i,pid;
  int key,id_zone;
  Info *buffer;
  int sem_id;
  unsigned short array[1];

  /* Manejador de la señal*/
  if(signal(SIGUSR1,manejador) == SIG_ERR){
    printf("Error al capturar la señal\n");
    exit(EXIT_FAILURE);
  }

  srand(time(NULL));
  /* Comprobacion de argumentos*/
  if(argc < 2){
    printf("Introduzca un numero de argumentos correcto\n");
    exit(EXIT_FAILURE);
  }

  array[0] = 1;
  if(Crear_Semaforo(SEMKEY,1,&sem_id) == ERROR){
		printf("Error al crear los semaforos\n");
		exit(EXIT_FAILURE);
	}
	if(Inicializar_Semaforo(sem_id,array) == ERROR){
		printf("Error al inicializar los semaforos\n");
		exit(EXIT_FAILURE);
	}
  /* Creacion de la memoria dinamica*/
  key = ftok(FILEKEY, KEY);
  if (key == -1) {
    fprintf (stderr, "Error with key \n");
    Borrar_Semaforo(sem_id);
    return -1;
  }
  id_zone = shmget (key, sizeof(Info), IPC_CREAT | IPC_EXCL |SHM_R | SHM_W);
  if (id_zone == -1) {
    fprintf (stderr, "Error with id_zone \n");
    Borrar_Semaforo(sem_id);
    return -1;
  }
  buffer = shmat (id_zone, (char *)0, 0);
  if (buffer == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    Borrar_Semaforo(sem_id);
    return -1;
  }
  if(Down_Semaforo(sem_id,0,SEM_UNDO) == ERROR){
				printf("Error al bajar el semaforo 0\n");
				exit(EXIT_FAILURE);
	}
  buffer->id = 0;
  if(Up_Semaforo(sem_id,0,SEM_UNDO) == ERROR){
				printf("Error al bajar el semaforo 0\n");
				exit(EXIT_FAILURE);
			}
  for (i = 0;i < atoi(argv[1]);i++){
    pid = fork();
    if (pid == -1){
      printf("Error al crear el hijo %d\n",i);
      exit(EXIT_FAILURE);
    }
    if (pid == 0){
      sleep(aleat_num(1,1));
      id_zone = shmget (key, sizeof(Info),   IPC_CREAT | SHM_R | SHM_W);
      if (id_zone == -1) {
        fprintf (stderr, "Error with id_zone hijo\n");
        exit(EXIT_FAILURE);
      }
      buffer = shmat (id_zone, (char *)0, 0);
      if (buffer == NULL) {
        fprintf (stderr, "Error reserve shared memory \n");
        exit(EXIT_FAILURE);
      }
      if(Down_Semaforo(sem_id,0,SEM_UNDO) == ERROR){
				printf("Error al bajar el semaforo 0\n");
				exit(EXIT_FAILURE);
			}
      printf("Introduzca un nombre: \n");
      fscanf(stdin,"%s",buffer->nombre);
      buffer->id ++;
      kill(getppid(),SIGUSR1);
      if(Up_Semaforo(sem_id,0,SEM_UNDO) == ERROR){
				printf("Error al bajar el semaforo 0\n");
				exit(EXIT_FAILURE);
			}
      exit(EXIT_SUCCESS);
    }
  }
  while(wait(NULL) > 0);
  shmdt ((char *)buffer);
  shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
  Borrar_Semaforo(sem_id);
  exit(EXIT_SUCCESS);
}
