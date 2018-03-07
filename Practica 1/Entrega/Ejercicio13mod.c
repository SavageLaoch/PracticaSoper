#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

typedef struct _Matriz{
  int matriz[25];
  int multiplicador;
  int num;
}Matriz;

int tam;
int fila1 = 0;
int fila2 = 0;

void * multiplica_matriz(void* matriz){
    Matriz* m;
    int i,fila,j;
    m = (Matriz*) matriz;
    fila = 0;
    for (i = 0;i < tam*tam;i++){
      m->matriz[i] = m->matriz[i] * m->multiplicador;
      if ((i+1)%tam == 0){
        printf("Hilo %d multiplicando fila %d resultado", m->num,fila);
        for (j = tam - 1; j > -1; j--){
          printf(" %d",m->matriz[i-j]);
        }
        if (m->num == 1){
          printf(" - el Hilo 2 va por la fila %d\n",fila2);
          fflush(stdout);
          fila1++;
          fila = fila1;
        }else{
          printf(" - el Hilo 1 va por la fila %d\n",fila1);
          fflush(stdout);
          fila2++;
          fila = fila2;
        }
      }
      usleep(100000);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  Matriz *m1;
  Matriz *m2;
  int i;
  pthread_t p1,p2;
  m1 = (Matriz*)malloc(sizeof(Matriz));
  if (m1 == NULL){
    printf("Fallo al crear la estructura 1\n");
    return EXIT_FAILURE;
  }
  m2 = (Matriz*)malloc(sizeof(Matriz));
  if (m2 == NULL){
    printf("Fallo al crear la estructura 2\n");
    free(m1);
    return EXIT_FAILURE;
  }
  printf("Introduzca dimension de la matriz cuadrada:\n");
  fscanf(stdin,"%d", &tam);
  if (tam > 5 || tam < 1){
    printf("No se ha introducida un tamaño de matriz adecuado\n");
    return EXIT_FAILURE;
  }
  printf("Introduzca multiplicador 1:\n");
  fscanf(stdin,"%d",&m1->multiplicador);
  printf("Introduzca multiplicador 2:\n");
  fscanf(stdin,"%d",&m2->multiplicador);
  printf("Introduzca matriz 1:\n");
  for (i = 0;i < tam*tam;i++){
    fscanf(stdin,"%d",&(m1->matriz[i]));
  }
  m1->num = 1;
  printf("Introduzca matriz 2:\n");
  for (i = 0;i < tam*tam;i++){
    fscanf(stdin,"%d",&(m2->matriz[i]));
  }
  m2->num = 2;
  printf("Realizando producto:\n");
  pthread_create(&p1,NULL,multiplica_matriz,(void*) m1);
  pthread_create(&p2,NULL,multiplica_matriz,(void*) m2);
  pthread_join(p1,NULL);
  pthread_join(p2,NULL);
  free(m1);
  free(m2);
  return EXIT_SUCCESS;
}
