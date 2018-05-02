#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>

#include "carrera.h"
#include "caballos.h"
#include "monitor.h"


int main(int argc, char *argv[]) {
	int num_caballos, max_distancia, num_apostadores, num_ventanillas, max_dinero;

	/*Semilla para los numeros aleatorios*/
	srand(time(NULL));

	if (argc<6){
		printf("Error en los parametros\n");
		printf("Escribe estos 5 parametros:\n<num_caballos>\n<max_distancia>\n<num_apostadores>\n<num_ventanillas>\n<max_dinero>\n");
		exit(EXIT_SUCCESS);
	}

	num_caballos = atoi(argv[1]);
	max_distancia = atoi(argv[2]);
	num_apostadores = atoi(argv[3]);
	num_ventanillas = atoi(argv[4]);
	max_dinero = atoi(argv[5]);
	/* ,num_apostadores,num_ventanillas,max_dinero*/
	carrera(num_caballos,max_distancia);
	exit(EXIT_SUCCESS);
}
