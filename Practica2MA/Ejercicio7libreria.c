/**
 * @brief Ejercicio 7 con nuestra libreria
 *
 * @file Ejercicio7libreria.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <stdlib.h>
#include "semaforos.h"
#define SEMKEY 75798
#define N_SEMAFOROS 2

/**
 * @brief Main
 *
 * Este main crea semaforos y opera con ellos con el objetivo de comprobar el correcto
 * funcionamiento de nuestra libreria semaforos.c
 * 
 * @return 0 o -1
 */

int main ( ){
/*
* Declaración de variables
*/
	int sem_id; /* ID de la lista de semáforos */
	unsigned short *array;
	int *active;


/*
* Creamos una lista o conjunto con dos semáforos
*/
	if (Crear_Semaforo(SEMKEY, N_SEMAFOROS, &sem_id)==ERROR){
		printf("Error al crear el semaforo");
		return -1;
	}

/*
* Inicializamos los semáforos
*/
	array = (unsigned short *)malloc(sizeof(short)*N_SEMAFOROS);
	array [0] = array [1] = 1;
	if (Inicializar_Semaforo(sem_id, array)==ERROR){
		printf("Error al inicializar el semaforo");
		return -1;
	}
/*
* Operamos sobre los semáforos
*/


	if (Down_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
		printf("Error al bajar el semaforo 0");
	}

	if (Up_Semaforo(sem_id, 1, SEM_UNDO)==ERROR){
		printf("Error al subir el semaforo 1");
	}

/*
* Veamos los valores de los semáforos
*/
	semctl (sem_id, N_SEMAFOROS, GETALL, array);
	printf ("Los valores de los semáforos son %d y %d\n", array [0], array [1]);
/*
* Operamos sobre los semáforos
*/

	active[0]=0;
	active[1]=1;
	if (UpMultiple_Semaforo(sem_id,N_SEMAFOROS, SEM_UNDO, active)==ERROR){
		printf("Error en el up multiple\n");
	}

/*
* Veamos los valores de los semáforos
*/
	semctl (sem_id, N_SEMAFOROS, GETALL, array);
	printf ("Los valores de los semáforos despues del upmultiple son %d y %d\n", array [0], array [1]);

/*
* Operamos sobre los semáforos
*/
	if (DownMultiple_Semaforo(sem_id,N_SEMAFOROS, SEM_UNDO, active)==ERROR){
		printf("Error en el down multiple\n");
	}
	
/*
* Veamos los valores de los semáforos
*/
	semctl (sem_id, N_SEMAFOROS, GETALL, array);
	printf ("Los valores de los semáforos despues del downmultiple son %d y %d\n", array [0], array [1]);



/* Eliminar la lista de semáforos */

	if (Borrar_Semaforo(sem_id)==ERROR) {
		printf("Error al borrar los semaforos");
	}

	return 0;
}/* fin de la función main */
