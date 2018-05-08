#include "utils.h"

/**
 * @brief Apostador
 *
 * @file apostador.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

typedef struct _Mensaje{
  long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
  char nombre_apostador[MAXCHAR]; /*Nombre del apostador*/
	int num_caballo; /*Numero del caballo*/
  double cuantia; /*Cuantia de la apuesta*/
}Mensaje;

/**
 * @brief apostador
 *
 * Proceso apostador. Genera una apuesta cada segundo.
 *
 */
void apostador(int num_apostadores,int num_caballos,int max_dinero,int msqid){
  Mensaje mensaje;
  char nombre_apostador[MAXCHAR];
  int num_caballo;
  double cuantia;
  int i;

  /* Establecemos la semilla */
  srand(time(NULL));

  /* Establecemos el manejador de la senal */
  if (signal(SIGUSR1, terminar) == SIG_ERR){
    puts("Error en la captura");
    exit (EXIT_FAILURE);
  }

  syslog (LOG_NOTICE, "Soy el proceso apostador");
  closelog();

  /* Creamos el bucle de los apostadores */
  for(i = 1; i <= num_apostadores; i++){

    /* Preparamos el mensaje */
    sprintf(nombre_apostador,"Apostador %d",i);
    num_caballo = aleat_num(0,num_caballos - 1);
    cuantia = aleat_num(10,10*max_dinero)/10.0;

    /* Enviamos el mensaje */
    mensaje.id = i;
    strcpy(mensaje.nombre_apostador,nombre_apostador);
    mensaje.num_caballo = num_caballo;
    mensaje.cuantia = cuantia;
    msgsnd(msqid,(struct msgbuf *) &mensaje,sizeof(Mensaje) - sizeof(long), IPC_NOWAIT);
    sleep(1);

  }
  exit(EXIT_SUCCESS);
}
