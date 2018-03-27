#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "semaforos.h"
#include "permutaciones.h"

#define SEMKEY 75798
#define NUM_OP 50
#define NUM_CAJA 10
#define MAX_CADENA 100

int crear_clientes(){
  FILE *cliente;
  int i,j,random;
  char fichero[MAX_CADENA];
  for (i = 1; i <= NUM_CAJA; i++){
    sprintf(fichero,"clientesCaja%d.txt",i);
    cliente = fopen(fichero,"w");
    if (cliente == NULL){
      return -1;
    }
    for (j = 0; j < NUM_OP;j++){
      random = aleat_num(0,300);
      fprintf(cliente,"%d\n",random);
    }
    fclose(cliente);
  }
  return 0;
}

int cobrar(int caja,int cliente){
  int cobrar,dinero;
  int i;
  char fichero[MAX_CADENA],fichero2[MAX_CADENA];
  FILE *fcliente,*fcaja;
  if (caja < 1 || caja > 10){
    return -1;
  }
  if (cliente < 1 || cliente > NUM_OP){
    return -1;
  }

  sprintf(fichero,"clientesCaja%d.txt",caja);
  fcliente = fopen(fichero,"r");
  if (fcliente == NULL){
    return -1;
  }

  sprintf(fichero2,"caja%d.txt",caja);
  fcaja = fopen(fichero2,"w+");
  if (fcaja == NULL){
    return -1;
  }
  for (i = 1; i <= cliente; i++){
    fscanf(fcliente,"%d\n",&dinero);
  }
  fscanf(fcaja,"%d",&cobrar);
  cobrar = cobrar + dinero;
  fclose(fcaja);

  fcaja = fopen(fichero,"w");
  if (fcaja == NULL){
    return -1;
  }
  fprintf(fcaja,"%d",cobrar);
  fclose(fcaja);
  fclose(fcliente);
  return cobrar;
}

void manejador(int sig){
  int sem_id;
  int total,antiguo_caja,nuevo_caja;
  char fichero[MAX_CADENA],fichero2[MAX_CADENA];
  FILE *cuentas, *caja;
  if (sig > 8){
    sig = sig - 1;
  }
  if (Crear_Semaforo(SEMKEY, NUM_CAJA, &sem_id) == ERROR){
		printf("Error al crear el semaforo\n");
    exit(EXIT_FAILURE);
	}

  sprintf(fichero2,"cuentas.txt");
  cuentas = fopen(fichero2,"r");
  if (cuentas == NULL){
    exit(EXIT_FAILURE);
  }
  fscanf(cuentas,"%d",&total);
  fclose(cuentas);

  cuentas = fopen(fichero2,"w");
  if (cuentas == NULL){
    exit(EXIT_FAILURE);
  }

  sprintf(fichero,"caja%d.txt",sig);
  Down_Semaforo(sem_id,sig - 1,SEM_UNDO);

  caja = fopen(fichero,"r");
  if (caja == NULL){
    fclose(cuentas);
    exit(EXIT_FAILURE);
  }
  fscanf(caja,"%d",&antiguo_caja);
  fclose(caja);
  total = total + 1000;
  nuevo_caja = antiguo_caja - 1000;

  caja = fopen(fichero,"w");
  if (caja == NULL){
    fclose(cuentas);
    exit(EXIT_FAILURE);
  }
  fprintf(caja,"%d",nuevo_caja);
  fprintf(cuentas,"%d",total);
  fclose(caja);
  Up_Semaforo(sem_id,sig - 1,SEM_UNDO);
  fclose(cuentas);
  return;
}


int main() {
  int i,j,pid,res;
  int sig;
  int status;
  FILE *caja,*cuentas;
  char fichero[MAX_CADENA],fichero2[MAX_CADENA];
  int sem_id;
	unsigned short array[NUM_CAJA];

  for(i = 1; i <= NUM_CAJA;i++){
    sig = i;
    if (i > 8){
      sig = i + 1;
    }
    if(signal(sig,manejador) == SIG_ERR){
      perror("signal");
      exit(EXIT_FAILURE);
    }
  }
  sprintf(fichero2,"cuentas.txt");
  cuentas = fopen(fichero2,"w");
  if (cuentas == NULL){
    exit(EXIT_FAILURE);
  }
  fprintf(cuentas, "0");
  fclose(cuentas);

  if (crear_clientes() == -1){
    exit(EXIT_FAILURE);
  }

  if (Crear_Semaforo(SEMKEY, NUM_CAJA, &sem_id) == ERROR){
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
      fclose(caja);
      for(j = 1; j <= NUM_OP;j++){
        Down_Semaforo(sem_id,i-1,SEM_UNDO);
        res = cobrar(i,j);
        Up_Semaforo(sem_id,i-1,SEM_UNDO);
        if (res == -1){
          exit(EXIT_FAILURE);
        }else if(res >= 1000 || j == NUM_OP){
          if(i > 8){
            kill(getppid(),i+1);
          }else{
            kill(getppid(),i);
          }
        }
      }
      exit(EXIT_SUCCESS);
    }
  }
  for(i = 1; i <= NUM_CAJA;i++){
    wait(&status);
  }
  exit(EXIT_SUCCESS);
}
