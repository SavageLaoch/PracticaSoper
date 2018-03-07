#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

int tam;

void * multiplica_matriz(void* matriz){
    int* m;
    int i,fila,j;
    m = (int*) matriz;
    fila = 0;
    for (i = 1;i <= tam*tam;i++){
      m[i] = m[i] * m[0];
      if (i%tam == 0){
        printf("\nHilo %d multiplicando fila %d resultado", m[26],fila);
        for (j = tam - 1; j > -1; j--){
          printf(" %d",m[i-j]);
        }
        fila++;
        usleep(100000);
      }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  int matriz1[27];
  int matriz2[27];
  int i;
  pthread_t p1,p2;
  printf("Introduzca dimension de la matriz cuadrada:\n");
  fscanf(stdin,"%d", &tam);
  if (tam > 5 || tam < 1){
    printf("No se ha introducida un tamaño de matriz adecuado\n");
    return EXIT_FAILURE;
  }
  printf("Introduzca multiplicador 1:\n");
  fscanf(stdin,"%d",&matriz1[0]);
  printf("Introduzca multiplicador 2:\n");
  fscanf(stdin,"%d",&matriz2[0]);
  printf("Introduzca matriz 1:\n");
  for (i = 1;i <= tam*tam;i++){
    fscanf(stdin,"%d",&matriz1[i]);
  }
  matriz1[26] = 1;
  printf("Introduzca matriz 2:\n");
  for (i = 1;i <= tam*tam;i++){
    fscanf(stdin,"%d",&matriz2[i]);
  }
  matriz2[26] = 2;
  printf("Realizando producto:\n");
  pthread_create(&p1,NULL,multiplica_matriz,(void*) matriz1);
  pthread_create(&p2,NULL,multiplica_matriz,(void*) matriz2);
  pthread_join(p1,NULL);
  pthread_join(p2,NULL);
  return EXIT_SUCCESS;
}
