/**
 * @brief Ejercicio 4 Apartado b
 *
 * @file Ejercicio4b.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @brief Main
 *
 * Se generan procesos hijos cuando i es congruente con 2 pero esta vez el padre esperara
 * a que termine un hijo (cuando llegue al wait) para terminar
 * @return EXIT_SUCCESS
 */

#define NUM_PROC 6
int main (void)
{
  int pid;
  int i;
  int status;
  for (i=0; i <= NUM_PROC; i++){
      if (i % 2 == 0) {
          if ((pid=fork()) <0 ){
              printf("Error al emplear fork\n");
              exit(EXIT_FAILURE);
            }else if (pid == 0){
              printf("HIJO  %d\n",i);
            }else{
              printf("PADRE %d \n",i);
            }
          }
        }
        wait(&status);
    exit(EXIT_SUCCESS);
}
