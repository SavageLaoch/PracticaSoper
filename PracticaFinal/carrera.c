#include "carrera.h"
#include "caballos.h"
#include "monitor.h"

#define KEY2 1400
#define FILEKEY2 "/bin/cat"

typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
	int tirada; /*Informacion a transmitir en el mensaje*/
}Mensaje;

void carrera(int num_caballos,int max_distancia){
	int **pipes;
	int pipe_status;

	int *caballos;

	int flag = 0;
	int i,f;

	int clave,msqid;
	int pid[100];

	int *siguiente_tirada;
	int max,min;

	int cont=0,monitor_id;
	int key, id_zone;
	char *buffer;

	Mensaje mensaje;

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

  	/* Inicializamos la posicion de los caballos */
	caballos = (int*)malloc(sizeof(int) * num_caballos);
	if (caballos == NULL){
		printf("Error al inicializar la posicion de los caballos\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < num_caballos;i++){
		caballos[i] = 0;
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

	/*Creamos al proceso monitor*/
	monitor_id=fork();
	if(monitor_id<0){
		printf("Error al crear el fork\n");
		exit(EXIT_SUCCESS);
	}else if(monitor_id == 0){
		monitor(num_caballos,max_distancia);
		exit(EXIT_SUCCESS);
	}

	/*Cuenta atras para que empiece*/
	for (i=3;i>0;i--){
		printf("%d\n",i);
		sleep(1);
	}
	printf("\n");

	/*Avisamos al monitor de que empieza la carrera*/
	kill(monitor_id,SIGUSR1);

	/* Hacemos la carrera */
	for(i = 0;i < num_caballos; i++){
		f = fork();
		if(f < 0){
			printf("Error al crear el fork\n");
			exit(EXIT_SUCCESS);
		}else if(f == 0){   /* Creamos los procesos caballo */
			printf("Soy un hijo con pid %d\n",getpid());
			caballo(pipes[i]);
			exit(EXIT_SUCCESS);
		}else{
			pid[i]=f;
			printf("Soy el padre y guardo el id %d\n",f);
		}
	}
	sleep(1);
	for(i=0;i<num_caballos;i++){
		printf("%d ",pid[i]);
	}
	printf("\n");

  	/* Creamos el proceso principal */

	while(!flag){
		printf("/////////// RONDA NUMERO %d //////////\n",cont);
		cont++;

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
		for(i = 0; i < num_caballos; i++){
			msgrcv(msqid,(struct msgbuf *) &mensaje,sizeof(Mensaje) - sizeof(long),pid[i],0);
			printf("---------Leo mensaje del caballo con id %ld\n",mensaje.id);
			printf("El caballo esta en la posicion %d\n",caballos[i]);
			printf("Me manda una tirada de %d\n", mensaje.tirada);
			caballos[i] += mensaje.tirada;
			if(i == 0){
				max = i;
				min = i;
			}else{
				if(caballos[max] < caballos[i]){
					max = i;
				}
				if(caballos[i] < caballos[min]){
					min = i;
				}
			}
		}
   		/* Calculamos como tiene que ser la siguiente tirada */
		for(i = 0; i < num_caballos;i++){
			if(caballos[i] >= max_distancia){
				printf("EL CABALLO %d HA GANADO!\n",i);
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
	}
 	
 	/* Mandamos la señal de acabado a los caballos */
	for (i = 0; i < num_caballos;i ++){
		kill(pid[i],SIGUSR2);
	}

	/*Avisamos al monitor de que ha acabado la carrera*/
	kill(monitor_id,SIGUSR1);
 	
 	/* Eliminamos la cola de mensajes */
	msgctl (msqid, IPC_RMID, (struct msqid_ds *)NULL);

	/*Eliminamos la memoria compartida*/
	shmdt ((char *)buffer);
	 shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);

	printf("Estos son los resultados:\n");
	for(i = 0;i < num_caballos;i++){
		printf("Caballo %d: %d\n",i,caballos[i]);
	}
	exit(EXIT_SUCCESS);
}
