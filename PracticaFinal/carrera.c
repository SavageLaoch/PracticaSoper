#include "utils.h"

typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
	int tirada; /*Informacion a transmitir en el mensaje*/
}Mensaje;

void carrera(int num_caballos,int max_distancia,int num_apostadores,int num_ventanillas,int max_dinero,int **pipes,int msqid, int sem_id,int id_zone){
	

	int flag = 0;
	int i,f;

	int pid[100];

	int *siguiente_tirada;
	int max,min;

	char *posicion;

	Mensaje mensaje;

	posicion = shmat (id_zone, (char *)0, 0);

	/* Inicializamos la siguiente tirada de los caballos */
	siguiente_tirada = (int*)malloc(sizeof(int) * num_caballos);
	if (siguiente_tirada == NULL){
		printf("Error al inicializar la posicion de los posicion\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < num_caballos;i++){
		siguiente_tirada[i] = NORMAL;
	}

	/* Hacemos la carrera */
	for(i = 0;i < num_caballos; i++){
		f = fork();
		if(f < 0){
			printf("Error al crear el fork\n");
			exit(EXIT_FAILURE);
		}else if(f == 0){   /* Creamos los procesos caballo */
			caballo(pipes[i],msqid);
			exit(EXIT_SUCCESS);
		}else{
			pid[i]=f;
		}
	}
	sleep(1);

  	/* Creamos el proceso principal */
	while(!flag){		

    	/* Enviamos la siguiente tirada */
		for(i = 0; i < num_caballos; i++){
			char tirada[100];
			sprintf(tirada,"%d",siguiente_tirada[i]);
			close(pipes[i][0]);
			write(pipes[i][1],tirada,strlen(tirada)+1);
			/*kill(pid[i],SIGUSR1);*/
		}

   		/* Leemos los mensajes */
		min = 0;
		max = 0;
		if (Down_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo dentro bucle");
		}
		for(i = 0; i < num_caballos; i++){
			msgrcv(msqid,(struct msgbuf *) &mensaje,sizeof(Mensaje) - sizeof(long),pid[i],0);
			posicion[i] += mensaje.tirada;
			posicion[num_caballos+i] = mensaje.tirada;
			if(i == 0){
				max = i;
				min = i;
			}else{
				if(posicion[max] < posicion[i]){
					max = i;
				}
				if(posicion[i] < posicion[min]){
					min = i;
				}
			}			
		}
		if (Up_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
				printf("Error al subir el semaforo");
		}

   		/* Calculamos como tiene que ser la siguiente tirada */
   		
   		if (Down_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo dentro bucle 2");
		}
		
		for(i = 0; i < num_caballos;i++){
			
			if(posicion[i] >= max_distancia){
				flag = 1;
			}
			
			if(i == max){
				siguiente_tirada[i] = PRIMERO;
			}else if(i == min){
				siguiente_tirada[i] = ULTIMO;
			}else{
				siguiente_tirada[i] = NORMAL;
			}
		}
		if (Up_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al subir el semaforo");
		}
	}

	/*Esta es la espera al proceso monitor*/
	wait(NULL);

	/* Mandamos la señal de acabado a los caballos */
	for (i = 0; i < num_caballos;i ++){
		kill(pid[i],SIGUSR1);
		wait(NULL);
	}

	/*Liberamos memoria*/
	free(siguiente_tirada);
	shmdt ((char *)posicion);
	return;

}
