/**
 * @brief Ejercicio 2
 *
 * @file Ejercicio2.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/**
 * @brief Main
 *
 * El main crea los 4 procesos hijos y los elimina mandandoles la senal SIGTERM
 *
 * @return 0
 */
int main(){
	int pid,i;

	for (i=0;i<4;i++){
		pid=fork();
		if (pid<0){
			printf("Error al hacer el fork");
		}else if (pid==0){
			printf("Soy el proceso hijo %d \n",getpid());
			usleep(30000000);
			printf("Soy el proceso hijo %d y ya me toca terminar \n",getpid());
			break;
		}else{
			usleep(5000000);
			kill(pid,SIGTERM);
		}
	}
	exit(EXIT_SUCCESS);
	return 0;
}