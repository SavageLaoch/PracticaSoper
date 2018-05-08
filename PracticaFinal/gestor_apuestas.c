#include "utils.h"

typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
 	char nombre_apuesta[MAXCHAR]; /*Nombre del apostador*/
	int num_caballo; /*Numero del caballo*/
  	double cuantia; /*Cuantia de la apuesta*/
}Mensaje;

typedef struct _MemComp{
	double cotizacion[MAX];
	double apuesta[MAX];
	int eleccion[MAX];
	int caballo[MAX];
	double ganancias[MAX];
}MemComp;

typedef struct _Estructura{
	int msqid;
	int num_apostadores;
	int num_caballos;
	int id_zone;
	int sem_id;
	int num_ventanilla;
}Estructura;

void terminaryliberar(int sig){
	exit(EXIT_SUCCESS);
}

void* ventanilla(void* estructura){
	Estructura *e;
	MemComp *mem;
	Mensaje mensaje;
	int i,tot;
	FILE *f;
	int apostador;
	int ventanilla;

	
	e = (Estructura*)estructura;
	mem = shmat (e->id_zone, (struct _MemComp *)0, 0);
	ventanilla = e->num_ventanilla;
	if (Up_Semaforo(e->sem_id, 3, SEM_UNDO)==ERROR){
		printf("Error al subir el semaforo ventanilla\n");
	}

	while(1){
		/*Asumimos un mensaje*/
  		msgrcv(e->msqid,(struct msgbuf *) &mensaje,sizeof(Mensaje) - sizeof(long),0,0);
  		/*Comrobamos el caballo*/
  		apostador = mensaje.id - 1;
  		if (Down_Semaforo(e->sem_id, 1, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo");
		}

		if (Down_Semaforo(e->sem_id, 2, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo");
		}
		f=fopen("Report.txt","a");
		fprintf(f,"/ %d         / %d          / %d       / %f / %f /\n",apostador,ventanilla,mensaje.num_caballo,mem->cotizacion[mensaje.num_caballo],mensaje.cuantia);
		fclose(f);
		if (Up_Semaforo(e->sem_id, 2, SEM_UNDO)==ERROR){
			printf("Error al subir el semaforo");
		}

  		/*Asignamos ganancias*/
  		mem->ganancias[apostador]=mensaje.cuantia * mem->cotizacion[mensaje.num_caballo];

  		/*Actualizamos  el dinero apostado por cada apostador y su eleccion*/
  		mem->apuesta[apostador] = mensaje.cuantia;
  		mem->eleccion[apostador] = mensaje.num_caballo;

  		/*Actualizamos el dinero apostado por nuestro caballo*/
  		mem->caballo[mensaje.num_caballo] += mensaje.cuantia;

  		/*Actualizamos la cotizacion de todos los caballos*/  		
  		tot=0;
  		for (i=0;i<e->num_caballos;i++){
  			tot += mem->caballo[i];
  		}
  		for (i=0;i<e->num_caballos;i++){
  			mem->cotizacion[i] = tot / mem->caballo[i];
  		}  		

  		if (Up_Semaforo(e->sem_id, 1, SEM_UNDO)==ERROR){
			printf("Error al subir el semaforo");
		}
  	}

	return NULL;
}

void gestor_apuestas(int num_ventanillas,int num_caballos,int num_apostadores,int msqid,int id_zone,int sem_id){
	int i;
	Estructura *e;
	MemComp *mem;
	pthread_t *hilos;
	FILE *f;

	f=fopen("Report.txt","w");
	fprintf(f,"Listado de apuestas realizadas:\n/ Apostador / Ventanilla / Caballo / Cotizacion / Cantidad /\n");
	fclose(f);

	mem = shmat (id_zone, (struct _MemComp *)0, 0);
	e = (Estructura*)malloc(sizeof(Estructura));
	hilos = (pthread_t*)malloc(sizeof(pthread_t) * num_ventanillas);

	/* Establecemos el manejador de la alarma */
	if (signal(SIGUSR1, retorno) == SIG_ERR){
		puts("Error en la captura");
		exit (EXIT_FAILURE);
	}

	
	if (Down_Semaforo(sem_id, 1, SEM_UNDO)==ERROR){
		printf("Error al bajar el semaforo");
	}
	/*Inicializamos la cotizacion de los caballos*/
	for(i = 0; i < num_caballos;i++){
		mem->cotizacion[i] = num_caballos;
	}
	/* Inicializamos el dinero total apostado en cada caballo */
	for(i = 0; i < num_caballos;i++){
		mem->caballo[i] = 1;
	}
	/* Inicializamos las posibles ganancias de cada apostador */
	for(i = 0; i < num_apostadores;i++){
		mem->ganancias[i] = 0;
	}
	/* Inicializamos a que caballo ha apostado cada apostador */
	for(i = 0; i < num_apostadores;i++){
		mem->eleccion[i] = -1;
	}
	

	if (Up_Semaforo(sem_id, 1, SEM_UNDO)==ERROR){
		printf("Error al subir el semaforo");
	}


	/*Inicializamos los threads ventanillas*/
	e->msqid = msqid;
	e->num_apostadores = num_apostadores;
	e->num_caballos = num_caballos;
	e->id_zone = id_zone;
	e->sem_id = sem_id;
	e->num_ventanilla = 0;
	
	for (i=0;i<num_ventanillas;i++){
		pthread_create(&hilos[i], NULL, ventanilla,(void *) e);
		if (Down_Semaforo(sem_id, 3, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo ventanilla\n");
		}
		e->num_ventanilla++;
	}
	/*Esperamos a que el proceso principal nos mate*/
    pause();

    free(e);
    free(hilos);
    shmdt ((MemComp *)mem);
	exit(EXIT_SUCCESS);
}
