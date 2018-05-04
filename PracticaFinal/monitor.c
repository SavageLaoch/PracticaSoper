#include "utils.h"

typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
	int tirada; /*Informacion a transmitir en el mensaje*/
}Mensaje;


void monitor_antes(int num_caballos,int max_distancia,int sem_id){
	/*Falta implementar el estado de las apuestas*/
	int i;

	printf("-----CUENTA ATRAS-----\n");
	for (i=5;i>0;i--){
		printf("La carrera empieza en %d\n",i);
		sleep(1);
	}
	printf("\n----------GO----------\n");
	kill(getppid(),SIGUSR1);
}

void monitor_durante(int num_caballos,int max_distancia,int sem_id,int id_zone){
	int i,flag=0;
	char *posicion;
	char *antes;

	/*Inicializamos el array antes*/
	antes = (char *)malloc(sizeof(char) * num_caballos);
	for(i=0;i<num_caballos;i++){
		antes[i]=0;
	}

	/* Obtenemos la memoria compartida */
	posicion = shmat (id_zone, (char *)0, 0);

	while (1){
		if (Down_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo monitor durante");
		}
		for (i=0; i<num_caballos; i++){
			if (posicion[i]!=antes[i]){
				antes[i]=posicion[i];
				fflush(stdout);
				printf("El caballo %d avanza a la posicion %d con una tirada de %d\n\n",i,posicion[i],posicion[num_caballos+i]);
				fflush(stdout);
				if (posicion[i]>=max_distancia) flag=1;
			}
		}
		if (flag==1){
			if (Up_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
				printf("Error al subir el semaforo");
			}
			return;
		}
		if (Up_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al subir el semaforo");
		}
	}
}

void monitor_despues(int num_caballos,int max_distancia,int sem_id, int id_zone){
	/*Falta implementar el resultado de las apuestas*/
	char *posicion;
	int i, max=0;

	/* Obtenemos la memoria compartida */
	posicion = shmat (id_zone, (char *)0, 0);


	printf("\n-----FINAL DE LA CARRERA-----\n");
	if (Down_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo monitor despues");
	}
	for (i=0; i<num_caballos; i++){
		if (posicion[i]>posicion[max]) max=i;
		printf("Posicion final del caballo %d = %d\n",i,posicion[i]);
	}
	if (Up_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al subir el semaforo");
	}
	printf("\nEL CABALLO %d HA GANADO!\n\n",max);

}

void monitor(int num_caballos, int max_distancia, int sem_id,int id_zone){

	monitor_antes(num_caballos,max_distancia,sem_id);
	monitor_durante(num_caballos,max_distancia,sem_id,id_zone);
	monitor_despues(num_caballos,max_distancia,sem_id,id_zone);
	printf("Monitor termina\n");
	kill(getppid(),SIGUSR1);

}
