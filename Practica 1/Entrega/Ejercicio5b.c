/**
 * @brief Ejercicio 5 Apartado b
 *
 * @file Ejercicio5b.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_PROC 6

/**
 * @breif Main
 * En este apartado haremos que el proceso padre genere tres hijos y
 * espere a que terminen los tres para terminar
 * @return EXIT_SUCCESS
 */

int main (void)
{
  int pid;
  int i;
  int status;
  for (i=0 ; i <= NUM_PROC; i++){
      if (i % 2 != 0) {
          if ((pid=fork()) <0 ){
              printf("Error al emplear fork\n");
              exit(EXIT_FAILURE);
            }else if (pid == 0){
              printf("HIJO  %d DEL PADRE %d\n",getpid(),getppid());
	            break;
            }else{
              printf("PADRE %d \n",getpid());
	            waitpid(pid,&status,0);
            }
          }
        }
	 printf("Terminado: %d\n",getpid());
   exit(EXIT_SUCCESS);
}
