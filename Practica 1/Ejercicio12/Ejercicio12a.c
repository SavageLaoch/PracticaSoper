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

int comprueba_Primos(void* estructura){
  int i;
  Estructura * num;
  num = (Estructura*)estructura;
  for (i = 1;i <= num->numero;i++){
    es_Primo(i);
  }
  return 1;
}

int main(int argc,char *argv[]){
    Estructura *e;
    e = (Estructura*)malloc(sizeof(Estructura));
    int i,j;
    int status;
    pid_t pid;
    struct timeval ti, tf;
    double tiempo;
    e->numero = atoi(argv[1]);
    gettimeofday(&ti, NULL);
    for (i = 0; i < 100; i++){
      pid = fork();
      if (!pid){
          comprueba_Primos((void*)e);
          free(e);
          exit(EXIT_SUCCESS);
      }
      /*waitpid(pid,&status,WUNTRACED | WCONTINUED);*/
    }
    for (i = 0; i < 100; i++){
      wait(&status);
    }
    free(e);
    gettimeofday(&tf, NULL);
    tiempo= (tf.tv_sec - ti.tv_sec)*1000 + (tf.tv_usec - ti.tv_usec)/1000.0;
    printf("Has tardado: %g milisegundos\n", tiempo);
    exit(EXIT_SUCCESS);
}
