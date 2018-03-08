/**
 * @brief Ejercicio 4 Apartado b
 *
 * @file Ejercicio4modb.c
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
 * Realiza lo mismo que el apartado b pero en este caso mostramos el id del hijo
 * y su padre o solo el id del padre
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
              printf("HIJO  %d DEL PADRE %d\n",getpid(),getppid());
            }else{
              printf("PADRE %d \n",getpid());
            }
          }
        }
        wait(&status);
    exit(EXIT_SUCCESS);
}
