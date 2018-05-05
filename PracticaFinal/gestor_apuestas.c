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
	FILE *f;
}Estructura;

void terminaryliberar(int sig){
	exit(EXIT_SUCCESS);
}

void* ventanilla(void* estructura){
	Estructura *e;
	MemComp *mem;
	Mensaje mensaje;
	int i,tot;

	e = (Estructura*)estructura;
	mem = shmat (e->id_zone, (MemComp *)0, 0);

	while(1){
		/*Asumimos un mensaje*/
  		msgrcv(e->msqid,(struct msgbuf *) &mensaje,sizeof(Mensaje) - sizeof(long),0,0);
  		printf("Soy la ventanilla %d\n",e->num_ventanilla);
  		
  		/*Comrobamos el caballo*/
  		/*printf("Recibo apuesta %f de Apostador %ld por el caballo %d \n",mensaje.cuantia, mensaje.id,  mensaje.num_caballo);
  		*/
  		if (Down_Semaforo(e->sem_id, 1, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo");
		}

		if (Down_Semaforo(e->sem_id, 2, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo");
		}
		fprintf(e->f,"/ %ld         / %d          / %d       / %f / %f /\n",mensaje.id,e->num_ventanilla,mensaje.num_caballo,mem->cotizacion[mensaje.num_caballo],mensaje.cuantia);
		if (Up_Semaforo(e->sem_id, 2, SEM_UNDO)==ERROR){
			printf("Error al subir el semaforo");
		}

  		/*Asignamos ganancias*/
  		mem->ganancias[mensaje.id]=mensaje.cuantia * mem->cotizacion[mensaje.num_caballo];

  		/*Actualizamos  el dinero apostado por cada apostador y su eleccion*/
  		mem->apuesta[mensaje.id] = mensaje.cuantia;
  		mem->eleccion[mensaje.id] = mensaje.num_caballo;

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

	return 0;
}

void gestor_apuestas(int num_ventanillas,int num_caballos,int num_apostadores,int msqid,int id_zone,int sem_id){
	int i;
	Estructura *e;
	MemComp *mem;
	pthread_t *hilos;
	FILE *f;

	f=fopen("Report.txt","w");
	fprintf(f,"/ Apostador / Ventanilla / Caballo / Cotizacion / Cantidad /\n");


	mem = shmat (id_zone, (char *)0, 0);
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
	
	/* Inicializamos el dinero apostado por cada apostador */
	/*for(i = 0; i < num_apostadores;i++){
		mem->apuesta[i] = 0;
	}*/
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
	e->f=f;
	
	for (i=0;i<num_ventanillas;i++){
		pthread_create(&hilos[i], NULL, ventanilla,(void *) e);
		e->num_ventanilla++;
	}

	e->num_ventanilla=44;
	
	/*printf("Gestor esperando a ser finalizado\n");*/

	/*for (i = 0; i < num_ventanillas; i++){
      pthread_join(hilos[i],NULL);
    }*/
    pause();
    printf("Gestor terminado\n");

	return;
}
