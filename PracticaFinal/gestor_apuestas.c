#include "utils.h"

typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
 	char nombre_apuesta[MAXCHAR]; /*Nombre del apuesta*/
	int num_caballo; /*Numero del caballo*/
  	double cuantia; /*Cuantia de la apuesta*/
}Mensaje;

typedef struct _Estructura{
	int *cotizacion;
	int *apuesta;
	int *eleccion;
	int *caballo;
	int *ganancias;
	int msqid;
}Estructura;

void terminaryliberar(int sig){
	exit(EXIT_SUCCESS);
}

void* ventanilla(void* estructura){
	Estructura * e;
	Mensaje mensaje;
	e = (Estructura*)estructura;

	while(1){
  		msgrcv(e->msqid,(struct msgbuf *) &mensaje,sizeof(Mensaje) - sizeof(long),1,0);
  		printf("Recibo mensaje con cuantia %f\n",mensaje.cuantia);
  	}

	return 0;
}

void gestor_apuestas(int num_ventanillas,int num_caballos,int num_apostadores,int msqid){
	int *cotizacion, *apuesta, *eleccion, *caballo, *ganancias;
	int i;
	Estructura *e;
	pthread_t *hilos;

	/* Establecemos el manejador de la alarma */
	if (signal(SIGUSR1, retorno) == SIG_ERR){
		puts("Error en la captura");
		exit (EXIT_FAILURE);
	}

	/*Algunas de estas cosas hay que hacerlas luego con memoria compartida para que las use
	el monitor, pero de momento vamos a ver que funcione asi*/

	e = (Estructura*)malloc(sizeof(Estructura));
	hilos = (pthread_t*)malloc(sizeof(pthread_t) * num_ventanillas);

	/* Inicializamos la cotizacion de los caballos */
	cotizacion = (int*)malloc(sizeof(int) * num_caballos);
	if (cotizacion == NULL){
		printf("Error al inicializar la posicion de los caballos\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < num_caballos;i++){
		cotizacion[i] = num_caballos;
	}

	/* Inicializamos el dinero apostado en cada caballo */
	caballo = (int*)malloc(sizeof(int) * num_caballos);
	if (caballo == NULL){
		printf("Error al inicializar la posicion de los caballos\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < num_caballos;i++){
		caballo[i] = 1;
	}

	/* Inicializamos el dinero apostado por cada apostador */
	apuesta = (int*)malloc(sizeof(int) * num_apostadores);
	if (apuesta == NULL){
		printf("Error al inicializar la posicion de los caballos\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < num_caballos;i++){
		apuesta[i] = 0;
	}

	/* Inicializamos a que caballo ha apostado cada apostador */
	eleccion = (int*)malloc(sizeof(int) * num_apostadores);
	if (eleccion == NULL){
		printf("Error al inicializar la posicion de los caballos\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < num_caballos;i++){
		eleccion[i] = -1;
	}	

	/* Inicializamos las posibles ganancias de cada apostador */
	ganancias = (int*)malloc(sizeof(int) * num_apostadores);
	if (ganancias == NULL){
		printf("Error al inicializar la posicion de los caballos\n");
		exit(EXIT_FAILURE);
	}
	for(i = 0; i < num_caballos;i++){
		ganancias[i] = 1;
	}


	/*Inicializamos los threads ventanillas*/
	e->cotizacion=cotizacion;
	e->ganancias=ganancias;
	e->apuesta=apuesta;
	e->eleccion=eleccion;
	e->caballo=caballo;
	e->msqid=msqid;
	
	/*for (i=0;i<num_ventanillas;i++){
		pthread_create(&hilos[i], NULL, ventanilla,(void *) e);
	}*/

	printf("He creado %d ventanillas\n",num_ventanillas);
	/*printf("Gestor esperando a ser finalizado\n");*/

	/*for (i = 0; i < num_ventanillas; i++){
      pthread_join(hilos[i],NULL);
    }*/

	return;
}
