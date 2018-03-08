/**
 * @brief Ejercicio 12 Apartado a
 *
 * @file Ejercicio12a.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

/**
 * @brief Estructura estructura
 *
 * Usaremos esta estructura para pasarsela a las funciones
 * de los procesos
 *
 */

typedef struct _Estructura{
  char cadena[100];
  int numero;
}Estructura;

/**
 * @brief es_primo
 *
 * Esta funcion comprueba si el numero pasado es un primo o no
 * @param num Numero a comprobar que es primo
 * @return -1 (No es) o 0 (Es)
 */

int es_primo(int num){
  int i;
  for (i = 2; i<num; i++){
    if (!(num%i)) {
      return -1;
    }
  }
  return 0;
}

/**
 * @brief comprueba_primos
 *
 * Esta funcion calcula los numeros primos hasta un numero de primos
 * concreto
 * @param estructua Puntero a una estructura Estructura (pasado como void)
 * @return 1
 */

int comprueba_primos(void* estructura){
  int i;
  int res;
  Estructura * num;
  num = (Estructura*)estructura;
  i = 0;
  res = 0;
  while (res < num->numero){
    if (es_primo(i) == 0){
      res ++;
    }
    i ++;
  }
  return 1;
}

/**
 * @brief Main
 *
 * Este main comprueba el tiempo que tarda en ejecutar con 100 procesos
 * la funcion comprueba_primos hasta un numero n que se pasa como parametro.
 * @param argc Numero de parametros que se pasan
 * @param argv Parametros que se pasan
 * @return EXIT_SUCCESS o EXIT_FAILURE
 */

int main(int argc,char *argv[]){
    Estructura *e;
    e = (Estructura*)malloc(sizeof(Estructura));
    int i;
    int status;
    pid_t pid;
    struct timeval ti, tf;
    double tiempo;
    if (argc < 2){
      printf("No hay suficientes parametros de entrada\n");
      exit(EXIT_FAILURE);
    }
    e->numero = atoi(argv[1]);
    gettimeofday(&ti, NULL);
    for (i = 0; i < 100; i++){
      pid = fork();
      if (pid == -1){
        printf("Error en la creacion de procesos\n");
        free(e);
        exit(EXIT_FAILURE);
      }
      if (!pid){
          comprueba_primos((void*)e);
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
