/**
 * @brief Ejercicio 4 Apartado a modificado
 *
 * @file Ejercicio4moda.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

#include <stdio.h>
#include <stdlib.h>
#include	<sys/types.h>
#include	<sys/wait.h>
#include <unistd.h>

/**
 * @brief Main
 *
 * Realizamos lo mismo que el apartado a pero enseñamos de quien es cada hijo y su id
 * o el id del padre
 * @return EXIT_SUCCES
 */

#define NUM_PROC 6
int main (void)
{
  int pid;
  int i;
  for (i=0; i <= NUM_PROC; i++){
      if (i % 2 == 0) {
          if ((pid=fork()) <0 ){
              printf("Error al emplear fork\n");
              exit(EXIT_FAILURE);
            }else if (pid == 0){
              printf("HIJO  %d DEL PADRE %d\n",getpid(),getppid());
            }else{
              printf("PADRE %d \n",getpid());
            }
          }
        }
    exit(EXIT_SUCCESS);
}
