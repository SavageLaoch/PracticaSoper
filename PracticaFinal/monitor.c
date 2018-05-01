#include "carrera.h"
#include "caballos.h"
#include "monitor.h"

typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
	int tirada; /*Informacion a transmitir en el mensaje*/
}Mensaje;


void monitor_antes(int num_caballos,int max_distancia){
	/*Falta implementar*/
	printf("1111111111111111111111111111\n");
}

void monitor_durante(int num_caballos,int max_distancia){
	/*Falta implementar*/
	printf("2222222222222222222222222222\n");
}

void monitor_despues(int num_caballos,int max_distancia){
	/*Falta implementar*/
	printf("33333333333333333333333333333\n");
}

void manejador2(int sig){
	return;
}


void monitor(int num_caballos,int max_distancia){

	/* Creamos los manejadores de funciones */
	if(signal(SIGUSR1,manejador2) == SIG_ERR){
		printf("Error en el manejador\n");
	}

	monitor_antes(num_caballos,max_distancia);
	pause();
	monitor_durante(num_caballos,max_distancia);
	pause();
	monitor_despues(num_caballos,max_distancia);

}