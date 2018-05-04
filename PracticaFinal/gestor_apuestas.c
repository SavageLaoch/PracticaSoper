#include "utils.h"

typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
 	char nombre_apuesta[MAXCHAR]; /*Nombre del apostador*/
	int num_caballo; /*Numero del caballo*/
  	double cuantia; /*Cuantia de la apuesta*/
}Mensaje;

typedef struct _Estructura{
	double *cotizacion;
	int *apuesta;
	int *eleccion;
	int *caballo;
	double *ganancias;
	int msqid;
	int num_apostadores;
}Estructura;

void terminaryliberar(int sig){
	exit(EXIT_SUCCESS);
}

void* ventanilla(void* estructura){
	Estructura * e;
	Mensaje mensaje;
	e = (Estructura*)estructura;
	int i=0,j,tot;

	while(1){
		/*Asumimos un mensaje*/
  		msgrcv(e->msqid,(struct msgbuf *) &mensaje,sizeof(Mensaje) - sizeof(long),1,0);
  		
  		/*Comrobamos el caballo*/
  		printf("Recibo apuesta %f de %s por el caballo %d \n",mensaje.cuantia, mensaje.nombre_apuesta,  mensaje.num_caballo);
  		
  		/*Asignamos ganancias*/
  		e->ganancias[i]=mensaje.cuantia * e->cotizacion[mensaje.num_caballo];
  		printf("GANANCIAS %f\n",e->ganancias[i]);
  		e->apuesta[i] += mensaje.cuantia;

  		/*Actualizamos cotizacion*/
  		tot=0;
  		for (j=0;j<e->num_apostadores;j++){
  			tot += e->apuesta[j];
  		}
  		e->cotizacion[mensaje.num_caballo] = tot / e->caballo[mensaje.num_caballo];
  		printf("COTIZACION %f\n", e->cotizacion[mensaje.num_caballo]);

  		i++;
  	}

	return 0;
}

void gestor_apuestas(int num_ventanillas,int num_caballos,int num_apostadores,int msqid){
	int *apuesta, *eleccion, *caballo;
	double *ganancias, *cotizacion;
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
	cotizacion = (double*)malloc(sizeof(double) * num_caballos);
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
	ganancias = (double*)malloc(sizeof(double) * num_apostadores);
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
	e->num_apostadores=num_apostadores;
	
	for (i=0;i<num_ventanillas;i++){
		pthread_create(&hilos[i], NULL, ventanilla,(void *) e);
	}

	
	/*printf("Gestor esperando a ser finalizado\n");*/

	/*for (i = 0; i < num_ventanillas; i++){
      pthread_join(hilos[i],NULL);
    }*/
    pause();
    printf("Gestor termina\n");

	return;
}
