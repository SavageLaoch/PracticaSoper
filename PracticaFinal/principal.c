#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <stdlib.h>
#include "carrera.h"
#include "caballos.h"
#include <time.h>

int main(int argc, char *argv[]) {
	int num_caballos, max_distancia;

	/*Semilla para los numeros aleatorios*/
	srand(time(NULL));

	if (argc<3){
		printf("Error en los parametros\n");
		printf("Escribe ./Proyecto <num_caballos> <max_distancia> \n");
		exit(EXIT_SUCCESS);
	}
	
	num_caballos = atoi(argv[1]);
	max_distancia = atoi(argv[2]);
	carrera(num_caballos,max_distancia);
	exit(EXIT_SUCCESS);
}
