/**
 * @brief Ejercicio 4 Apartado a
 *
 * @file Ejercicio4a.c
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
 * Este main crea procesos hijo cada vez que i es congruente con dos
 *
 * @retrun EXIT_SUCCESS o EXIT_FAILURE
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
              printf("HIJO  %d\n",i);
            }else{
              printf("PADRE %d \n",i);
            }
          }
        }
    exit(EXIT_SUCCESS);
}
