#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

typedef struct _Estructura{
  char cadena[100];
  int numero;
}Estructura;

int es_Primo(int num){
  int i;
  for (i = 2; i<num; i++){
    if (!(num%i)) {
      return -1;
    }
  }
  return 0;
}

void* comprueba_Primos(void* estructura){
  int i;
  Estructura * num;
  num = (Estructura*)estructura;
  for (i = 1;i <= num->numero;i++){
    es_Primo(i);
  }
  pthread_exit(NULL);
}

int main(int argc,char *argv[]){
    Estructura *e;
    e = (Estructura*)malloc(sizeof(Estructura));
    int i,j;
    int status;
    double time_ini, time_fin;
    struct timeval ti, tf;
    double tiempo;
    gettimeofday(&ti, NULL);
    pthread_t hilos[100];
    e->numero = atoi(argv[1]);
    time_ini = clock();
    for (i = 0; i < 100; i++){
      pthread_create(&hilos[i], NULL, comprueba_Primos,(void *) e);
    }
    for (i = 0; i < 100; i++){
      pthread_join(hilos[i],NULL);
    }
    free(e);
    time_fin = clock();
    gettimeofday(&tf, NULL);
    tiempo= (tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000.0;
    printf("Has tardado: %g milisegundos\n", tiempo);
    exit(EXIT_SUCCESS);
}
