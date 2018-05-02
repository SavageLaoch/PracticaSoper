#include "carrera.h"
#include "caballos.h"
#include "monitor.h"
#include "semaforos.h"

#include <time.h>
#include <sys/wait.h>

#define KEY2 1400
#define FILEKEY2 "/bin/cat"
#define SEMKEY 75798


typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
	int tirada; /*Informacion a transmitir en el mensaje*/
}Mensaje;

void manejador3(int sig){
	return;
}

void carrera(int num_caballos,int max_distancia){
	int **pipes;
	int pipe_status;

	int flag = 0;
	int i,f;

	int clave,msqid;
	int pid[100];

	int *siguiente_tirada;
	int max,min;

	int monitor_id;
	int key, id_zone;
	char *buffer;

	unsigned short *array;
	int sem_id;

	Mensaje mensaje;

	/* Establecemos la semilla */
	srand(time(NULL));

	/* Creamos los manejadores de senales */
	if(signal(SIGUSR1,manejador3) == SIG_ERR){
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

  	/* Creamos la cola de mensajes */
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

	/* Creamos la memoria compartida */
	key = ftok(FILEKEY2, KEY2);
 	if (key == -1) {
 		fprintf (stderr, "Error with key \n");
 		exit(EXIT_FAILURE);
 	}
 	id_zone = shmget (key, sizeof(char)*MAXBUFFER, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
 	if (id_zone == -1) {
 		fprintf (stderr, "Error with id_zone \n");
 		exit(EXIT_FAILURE);
 	}
 	buffer = shmat (id_zone, (char *)0, 0);

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
		buffer[i] = 0;
	}
	if (Up_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
		printf("Error al subir el semaforo");
	}

  	/* Inicializamos la siguiente tirada de los caballos */
	siguiente_tirada = (int*)malloc(sizeof(int) * num_caballos);
	if (siguiente_tirada == NULL){
		printf("Error al inicializar la posicion de los caballos\n");
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

	/*Esperamos a que empiece la carrera (la cuenta la lleva el monitor)*/
	pause();

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
			buffer[i] += mensaje.tirada;
			buffer[num_caballos+i] = mensaje.tirada;
			if(i == 0){
				max = i;
				min = i;
			}else{
				if(buffer[max] < buffer[i]){
					max = i;
				}
				if(buffer[i] < buffer[min]){
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
			
			if(buffer[i] >= max_distancia){
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

	/* Eliminamos la cola de mensajes */
	msgctl (msqid, IPC_RMID, (struct msqid_ds *)NULL);

	/*Eliminamos la memoria compartida*/
	shmdt ((char *)buffer);
	shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);

	/*Borramos los semaforos*/
	if (Borrar_Semaforo(sem_id)==ERROR) {
		printf("Error al borrar los semaforos\n");
	}
	
	exit(EXIT_SUCCESS);
}
