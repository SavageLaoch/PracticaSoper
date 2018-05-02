#include "gestor_apuestas.h"

typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
  char nombre_apostador[MAXCHAR]; /*Nombre del apostador*/
	int num_caballo; /*Numero del caballo*/
  double cuantia; /*Cuantia de la apuesta*/
}Mensaje;



void gestor_apuestas(int num_ventanillas,int num_caballos){
	int *cotizacion;
	int *dinero;

  /* Inicializamos la cotizacion de los caballos */

}
