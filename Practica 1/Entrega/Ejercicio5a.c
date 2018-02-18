/**
 * @brief Ejercicio 5 Apartado a
 *
 * @file Ejercicio5a.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_PROC 6

/**
 * @brief Main
 * En este apartado, haremos que cada proceso genere a su vez solo un proceso
 * cuando i no sea congruente con 2
 * @return EXIT_SUCCESS
 */

int main (void)
{
  int pid;
  int i;
  int status;
  for (i=0; i <= NUM_PROC; i++){
      if (i % 2 != 0) {
          if ((pid=fork()) <0 ){
              printf("Error al emplear fork\n");
              exit(EXIT_FAILURE);
            }else if (pid == 0){
              printf("HIJO  %d DEL PADRE %d\n",getpid(),getppid());
            }else{
              printf("PADRE %d \n",getpid());
              break;
            }
          }
        }
        wait(&status);
    exit(EXIT_SUCCESS);
}
