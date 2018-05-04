#include "utils.h"

int main(int argc, char *argv[]) {
	int num_caballos, max_distancia, num_apostadores, num_ventanillas, max_dinero;
	int monitor_id, apostador_id, gestor_id;
	int **pipes;
	int pipe_status;
	int clave, msqid, clave2, msqid2;
	int key, id_zone, key2, id_zone2;
	int sem_id,i;
	unsigned short *array;
	char *posicion, *cotizacion;

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

 	/* Creamos la memoria compartida de la cotizacion de cada caballo*/
	key2 = ftok(FILEKEY, KEY4);
 	if (key == -1) {
 		fprintf (stderr, "Error with key \n");
 		exit(EXIT_FAILURE);
 	}
 	id_zone2 = shmget (key2, sizeof(char)*MAXBUFFER, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
 	if (id_zone == -1) {
 		fprintf (stderr, "Error with id_zone \n");
 		exit(EXIT_FAILURE);
 	}
 	cotizacion = shmat (id_zone2, (char *)0, 0);

 	/*Creamos e inicializamos los semaforos*/
 	array = (unsigned short *)malloc(sizeof(unsigned short));
 	array[0]=1;
 	array[1]=1;
 	if (Crear_Semaforo(SEMKEY, 2, &sem_id)==ERROR){
		printf("Error al crear el semaforo\n");
		exit(EXIT_FAILURE);
	}	
	if (Inicializar_Semaforo(sem_id, array)==ERROR){
		printf("Error al inicializar el semaforo\n");
		exit(EXIT_FAILURE);
	}

	 /* Inicializamos la posicion de los caballos */
	if (Down_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
		printf("Error al bajar el semaforo al principio");
	}
	for(i = 0; i < 2*num_caballos;i++){
		posicion[i] = 0;
	}
	if (Up_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
		printf("Error al subir el semaforo");
	}

  	/*Creamos al proceso monitor*/
	monitor_id=fork();
	if(monitor_id<0){
		printf("Error al crear el fork\n");
		exit(EXIT_SUCCESS);
	}else if(monitor_id == 0){
		monitor(num_caballos,max_distancia,sem_id,id_zone,id_zone2);
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

	/*Creamos al proceso gestor de apuestas*/
	gestor_id=fork();
	if(gestor_id<0){
		printf("Error al crear el fork\n");
		exit(EXIT_SUCCESS);
	}else if(gestor_id == 0){
		gestor_apuestas(num_ventanillas,num_caballos,num_apostadores,msqid2,id_zone2,sem_id);
		exit(EXIT_SUCCESS);
	}

	/*Esperamos a que empiece la carrera (la cuenta la lleva el monitor)*/
	pause();

	/*Matamos al proceso apostador y al gestor porque ya no puede haber mas apuestas*/
	kill(apostador_id,SIGUSR1);
	kill(gestor_id,SIGUSR1);

	/*Hacemos la carrera*/
	carrera(num_caballos,max_distancia,num_apostadores,num_ventanillas,max_dinero,pipes,msqid,sem_id,id_zone);

	/*Liberamos todo*/
	

	/*Liberamos memoria*/
	for (i=0;i<num_caballos;i++){
		free(pipes[i]);
	}
	free(pipes);
	free(array);

	/* Eliminamos la cola de mensajes entre carrera y caballo*/
	msgctl (msqid, IPC_RMID, (struct msqid_ds *)NULL);

	/* Eliminamos la cola de mensajes entre apostador y gestor de apuestas*/
	msgctl (msqid2, IPC_RMID, (struct msqid_ds *)NULL);

	/*Eliminamos la memoria compartida de la posicion de los caballos*/
	shmdt ((char *)posicion);
	shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);

	/*Eliminamos la memoria compartida de la cotizacion de los caballos*/
	shmdt ((char *)cotizacion);
	shmctl (id_zone2, IPC_RMID, (struct shmid_ds *)NULL);

	/*Borramos los semaforos*/
	if (Borrar_Semaforo(sem_id)==ERROR) {
		printf("Error al borrar los semaforos\n");
	}
	
	printf("Proceso carrera termina y borra semaforo\n");

	exit(EXIT_SUCCESS);
}
