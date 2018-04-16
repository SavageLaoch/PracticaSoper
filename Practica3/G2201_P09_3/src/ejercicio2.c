/**
 * @brief Ejercicio 2
 *
 * @file ejercicio2.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

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
#define FILEKEY "/bin/cat"
#define KEY 1300



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

/**
 * @brief manejador
 *
 * Este es el manejador de la senal SIGUSR1, cuando un proceso recibe
 * esta senal, lee de la zona de memoria compartida e imprime su contenido
 * 
 * @return 
 */

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

/**
 * @brief Main
 *
 * En este main se crean tantos procesos hijos como se pasen por parametro 
 * y entre esos procesos y el padre van accediendo a una zona de memoria compartida
 * 
 * @return EXIT_FAILURE o EXIT_SUCCESS segun si ha salido todo bien o no.
 */

int main(int argc, char const *argv[]) {
  int i,pid;
  int key,id_zone;
  Info *buffer;

  /* Manejador de la señal*/
  if(signal(SIGUSR1,manejador) == SIG_ERR){
    printf("Error al capturar la señal\n");
    exit(EXIT_FAILURE);
  }

  /* Comprobacion de argumentos*/
  if(argc < 2){
    printf("Introduzca un numero de argumentos correcto\n");
    exit(EXIT_FAILURE);
  }

  /* Creacion de la memoria dinamica*/
  key = ftok(FILEKEY, KEY);
  if (key == -1) {
    fprintf (stderr, "Error with key \n");
    return -1;
  }
  id_zone = shmget (key, sizeof(Info), IPC_CREAT | IPC_EXCL |SHM_R | SHM_W);
  if (id_zone == -1) {
    fprintf (stderr, "Error with id_zone \n");
    return -1;
  }
  buffer = shmat (id_zone, (char *)0, 0);
  if (buffer == NULL) {
    fprintf (stderr, "Error reserve shared memory \n");
    return -1;
  }

  buffer->id = 0;

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

      printf("Introduzca un nombre: \n");
      fscanf(stdin,"%s",buffer->nombre);
      buffer->id ++;
      kill(getppid(),SIGUSR1);
      exit(EXIT_SUCCESS);
    }
  }
  while(wait(NULL) > 0);
  shmdt ((char *)buffer);
  shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
  exit(EXIT_SUCCESS);
}
