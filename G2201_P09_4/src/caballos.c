#include "utils.h"

/**
 * @brief Caballos
 *
 * @file caballos.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
	int tirada; /*Informacion a transmitir en el mensaje*/
}Mensaje;

/**
 * @brief dado
 *
 * Genera un numero aleatorio teniendo en cuenta el modo.
 *
 * @param modo Modo de tirada
 * @return int numero aleatorio
 */
int dado(int modo){
	if (modo == GANADORA){
		return aleat_num(1,7);
	}else if (modo == REMONTADORA){
		return aleat_num(1,6) + aleat_num(1,6);
	}
	return aleat_num(1,6);
}

/**
 * @brief avanzar_caballo
 *
 * Devuelve una tirada segun la posicion actual del caballo
 *
 * @param posicion Posicion actual del caballo
 * @return int Tirada del caballo
 */
int avanzar_caballo(int posicion){
	if(posicion == PRIMERO){
		return dado(GANADORA);
	}else if(posicion == ULTIMO){
		return dado(REMONTADORA);
	}
	return dado(NORMAL);
}

/**
 * @brief caballo
 *
 * Proceso caballo.
 *
 */
void caballo(int pipe[2],int msqid){
	char buffer[MAXBUFFER];
	int posicion, tirada;
	Mensaje mensaje;
	/*printf("Funcion de caballo para el caballo %d\n",getpid());*/

	/* Creamos los manejadores de funciones */
	if(signal(SIGUSR1,terminar) == SIG_ERR){
		printf("Error en la señal\n");
	}

	/* Establecemos la semilla */
	srand(time(NULL));

	/* Hacemos el bucle del proceso caballo*/
	while(1){
		/*pause();*/

		/* Leemos la tirada del proceso principal */
		close(pipe[1]);
		read(pipe[0],buffer,sizeof(buffer));
		posicion = atoi(buffer);

		/* Hacemos la siguiente tirada */
		tirada = avanzar_caballo(posicion);

		/* Enviamos el mensaje */
		mensaje.id=getpid();
		mensaje.tirada=tirada;
		msgsnd(msqid,(struct msgbuf *) &mensaje,sizeof(Mensaje) - sizeof(long),IPC_NOWAIT);
	}
}
