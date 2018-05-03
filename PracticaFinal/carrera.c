#include "utils.h"

typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
	int tirada; /*Informacion a transmitir en el mensaje*/
}Mensaje;

void carrera(int num_caballos,int max_distancia,int num_apostadores,int num_ventanillas,int max_dinero){
	int **pipes;
	int pipe_status;

	int flag = 0;
	int i,f;

	int clave, msqid, clave2, msqid2;
	int pid[100];

	int *siguiente_tirada;
	int max,min;

	int monitor_id, apostador_id, gestor_id;
	int key, id_zone;
	char *posicion;

	unsigned short *array;
	int sem_id;

	Mensaje mensaje;

	/* Establecemos la semilla */
	srand(time(NULL));

	/* Creamos los manejadores de senales */
	if(signal(SIGUSR1,retorno) == SIG_ERR){
		printf("Error en el manejador\n");
	}

  	/* Creamos las pipes */
	pipes = (int**) malloc(sizeof(int*)*num_caballos);
	if(pipes==NULL){
		printf("Error al reservar memoria");
		exit(EXIT_FAILURE);
	}
	for (i=0;i<num_caballos;i++){
		pipes[i]=(int*)malloc(sizeof(int)*2);
		if (pipes[i]==NULL){
			printf("Error al reservar memoria");
			exit(EXIT_FAILURE);
		}
		pipe_status=pipe(pipes[i]);
		if (pipe_status==-1){
			printf("Error al crear la tuberia %d",i);
			exit(EXIT_FAILURE);
		}
	}

  	/* Creamos la cola de mensajes entre carrera y caballo*/
	clave = ftok (FILEKEY, KEY);
	if (clave==(key_t) -1){
		printf("Error al coger el key de la cola de mensajes");
		exit(EXIT_FAILURE);
	}
	msqid = msgget(clave,0666 | IPC_CREAT);
	if (msqid == -1){
		printf("Error al coger el key de la cola de mensajes");
		exit(EXIT_FAILURE);
	}

	/* Creamos la cola de mensajes entre apostador y gestor de apuestas*/
	clave2 = ftok (FILEKEY, KEY2);
	if (clave2==(key_t) -1){
		printf("Error al coger el key de la cola de mensajes");
		exit(EXIT_FAILURE);
	}
	msqid2 = msgget(clave2,0666 | IPC_CREAT);
	if (msqid == -1){
		printf("Error al coger el key de la cola de mensajes");
		exit(EXIT_FAILURE);
	}

	/* Creamos la memoria compartida de la posicion de los caballos*/
	key = ftok(FILEKEY, KEY3);
 	if (key == -1) {
 		fprintf (stderr, "Error with key \n");
 		exit(EXIT_FAILURE);
 	}
 	id_zone = shmget (key, sizeof(char)*MAXBUFFER, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
 	if (id_zone == -1) {
 		fprintf (stderr, "Error with id_zone \n");
 		exit(EXIT_FAILURE);
 	}
 	posicion = shmat (id_zone, (char *)0, 0);

 	/*Creamos e inicializamos los semaforos*/
 	array = (unsigned short *)malloc(sizeof(unsigned short));
 	array[0]=1;
 	if (Crear_Semaforo(SEMKEY, 1, &sem_id)==ERROR){
		printf("Error al crear el semaforo\n");
		exit(EXIT_FAILURE);
	}	
	if (Inicializar_Semaforo(sem_id, array)==ERROR){
		printf("Error al inicializar el semaforo\n");
		exit(EXIT_FAILURE);
	}

	 /* Inicializamos la posicion de los caballos */
	if (Down_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
		printf("Error al bajar el semaforo");
	}
	for(i = 0; i < 2*num_caballos;i++){
		posicion[i] = 0;
	}
	if (Up_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
		printf("Error al subir el semaforo");
	}

  	/* Inicializamos la siguiente tirada de los caballos */
	siguiente_tirada = (int*)malloc(sizeof(int) * num_caballos);
	if (siguiente_tirada == NULL){
		printf("Error al inicializar la posicion de los posicion\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < num_caballos;i++){
		siguiente_tirada[i] = NORMAL;
	}

	/*Creamos al proceso monitor*/
	monitor_id=fork();
	if(monitor_id<0){
		printf("Error al crear el fork\n");
		exit(EXIT_SUCCESS);
	}else if(monitor_id == 0){
		monitor(num_caballos,max_distancia,sem_id,id_zone);
		exit(EXIT_SUCCESS);
	}

	/*Creamos al proceso gestor de apuestas*/
	gestor_id=fork();
	if(gestor_id<0){
		printf("Error al crear el fork\n");
		exit(EXIT_SUCCESS);
	}else if(gestor_id == 0){
		gestor_apuestas(num_ventanillas,num_caballos,num_apostadores,msqid2);
		exit(EXIT_SUCCESS);
	}

	/*Creamos al proceso apostador*/
	apostador_id=fork();
	if(apostador_id<0){
		printf("Error al crear el fork\n");
		exit(EXIT_SUCCESS);
	}else if(apostador_id == 0){
		apostador(num_apostadores,num_caballos,max_dinero,msqid2);
		exit(EXIT_SUCCESS);
	}

	/*Esperamos a que empiece la carrera (la cuenta la lleva el monitor)*/
	pause();

	/*Matamos al proceso apostador y al gestor porque ya no puede haber mas apuestas*/
	kill(apostador_id,SIGUSR1);
	kill(gestor_id,SIGUSR1);

	/* Hacemos la carrera */
	for(i = 0;i < num_caballos; i++){
		f = fork();
		if(f < 0){
			printf("Error al crear el fork\n");
			exit(EXIT_SUCCESS);
		}else if(f == 0){   /* Creamos los procesos caballo */
			caballo(pipes[i]);
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
			kill(pid[i],SIGUSR1);
		}

   		/* Leemos los mensajes */
		min = 0;
		max = 0;
		if (Down_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo");
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
			printf("Error al bajar el semaforo");
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

 	/* Mandamos la señal de acabado a los caballos */
	for (i = 0; i < num_caballos;i ++){
		kill(pid[i],SIGUSR2);
	}

	/*Esta es la espera al proceso monitor*/
	wait(NULL);	

	/* Eliminamos la cola de mensajes entre carrera y caballo*/
	msgctl (msqid, IPC_RMID, (struct msqid_ds *)NULL);

	/* Eliminamos la cola de mensajes entre apostador y gestor de apuestas*/
	msgctl (msqid2, IPC_RMID, (struct msqid_ds *)NULL);

	/*Eliminamos la memoria compartida de la posicion de los caballos*/
	shmdt ((char *)posicion);
	shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);

	/*Borramos los semaforos*/
	if (Borrar_Semaforo(sem_id)==ERROR) {
		printf("Error al borrar los semaforos\n");
	}
	
	exit(EXIT_SUCCESS);
}
