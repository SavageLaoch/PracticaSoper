#include "utils.h"

typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
	int tirada; /*Informacion a transmitir en el mensaje*/
}Mensaje;

int dado(int modo){
	if (modo == GANADORA){
		return aleat_num(1,7);
	}else if (modo == REMONTADORA){
		return aleat_num(1,6) + aleat_num(1,6);
	}
	return aleat_num(1,6);
}

int avanzar_caballo(int posicion){
	if(posicion == PRIMERO){
		return dado(GANADORA);
	}else if(posicion == ULTIMO){
		return dado(REMONTADORA);
	}
	return dado(NORMAL);
}

void caballo(int pipe[2],int msqid){
	char buffer[MAXBUFFER];
	int posicion, tirada;
	Mensaje mensaje;
	/*printf("Funcion de caballo para el caballo %d\n",getpid());*/

	/* Creamos los manejadores de funciones */
	if(signal(SIGUSR1,retorno) == SIG_ERR){
		printf("Error en la señal\n");
	}
	if (signal(SIGUSR2,terminar) == SIG_ERR){
		printf("Error segundo manejador\n");
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
