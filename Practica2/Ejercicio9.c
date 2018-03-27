#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include "semaforos.h"
#include "permutaciones.h"

#define SEMKEY 75798
#define NUM_OP 50
#define NUM_CAJA 10
#define MAX_CADENA 100

int crear_clientes(){
  FILE *cliente[NUM_CAJA];
  int i,j,random;
  char fichero[MAX_CADENA];
  for (i = 0; i < NUM_CAJA; i++){
    sprintf(fichero,"clientesCaja%d.txt",i);
    cliente[i+1] = fopen(fichero,"w");
    if (cliente[i+1] == NULL){
      return -1;
    }
    for (j = 0; j < NUM_OP;j++){
      random = aleat_num(0,300);
      fprintf(cliente[i],"%d\n",random);
    }
    fclose(cliente[i]);
  }
  return 0;
}

int cobrar(FILE *f,int caja,int cliente){
  int sem_id,cobrar;
  char fichero[MAX_CADENA];
  FILE *cliente;
  if (f == NULL){
    return -1;
  }
  if (caja < 1 || caja > 10){
    return -1;
  }
  if (Crear_Semaforo(SEMKEY, NUM_CAJA, &sem_id)==ERROR){
		printf("Error al crear el semaforo\n");
    return -1;
	}
  sprintf(fichero,"clientesCaja%d.txt",caja);
  cliente = fopen(fichero,"r");
}

int main() {
  int i,j,pid,res;
  FILE *caja;
  char fichero[MAX_CADENA];
  int sem_id;
	unsigned short array[MAX_CAJA];

  if (crear_clientes() == -1){
    exit(EXIT_FAILURE);
  }

  if (Crear_Semaforo(SEMKEY, NUM_CAJA, &sem_id)==ERROR){
		printf("Error al crear el semaforo\n");
    exit(EXIT_FAILURE);
	}
  for(i = 0;i < NUM_CAJA;i++){
    array[i] = 1;
  }
	if(Inicializar_Semaforo(sem_id, array) == ERROR){
    printf("Error al inicializar el semaforo\n");
    exit(EXIT_FAILURE);
  }

  for(i = 1; i <= NUM_CAJA;i++){
    pid = fork();
    if (pid < -1){
      exit(EXIT_FAILURE);
    }
    if (pid == 0){
      sprintf(fichero,"caja%d.txt",i);
      caja = fopen(fichero,"w");
      if (caja == NULL){
        exit(EXIT_FAILURE);
      }
      fprintf(caja,"0");
      for(j = 1; j <= NUM_OP;j++){
        res = cobrar(caja,i,j);
        if (res == -1){
          exit(EXIT_FAILURE);
        }else if(res >= 1000){
          if(i >8){
            kill(getppid(),i+1);
          }else{
            kill(getppid(),i);
          }
        }

      }
    }
  }
  exit(EXIT_SUCCESS);
}
