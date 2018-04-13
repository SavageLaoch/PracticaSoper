#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/shm.h> /* shm* */
#include <sys/msg.h>
#include "semaforos.h"
#define FILEKEY "/bin/cat"
#define SEMKEY 75788
#define KEY 1300
#define MAXBUF 37


int sem_id;

typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
	char *contenido; /*Informacion a transmitir en el mensaje*/
}mensaje;

int main(int argc, char *argv[]){
	key_t clave;
	int msqid;
	mensaje msg;
	FILE *f;
	char *buffer;

	/*Comprobamos los parametros*/
	if (argc < 3){
		printf("No se han introducido suficientes parámetros\n");
		exit(EXIT_FAILURE);
	}

	/*Obtenemos la clave de la cola*/
	clave = ftok (FILEKEY, KEY);
	if (clave == (key_t) -1){
		perror("Error al obtener clave para cola mensajes\n");
		exit(EXIT_FAILURE);
	}

	/*Creamos la cola de mensajes*/
	msqid = msgget (clave, 0600 | IPC_CREAT);
	if (msqid == -1){
		perror("Error al obtener identificador para cola mensajes");
		return(0);
	}

	/*Con 2 forks creamos dos hijos*/
	pid = fork();
	if(pid < 0){
    	printf("No se ha podido crear el proceso hijo 1\n");
    	exit(EXIT_SUCCESS);
	}
	if(pid == 0){
	/*Aqui va lo que hace el proceso A*/
		/*Leemos del fichero*/
		f=fopen("Entrada.txt","r");
		while (fgets(buffer,16000/sizeof(char),f)!=NULL){ /*Esto esta fatal*/			
			/*Mandamos el mensaje*/
			msg.id=1; /*1 significa que aun no esta traducido*, se lo manda al proceso B*/
			strcpy(msg.contenido,buffer);
			msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long));
		}
		fclose(f);
		exit(EXIT_SUCCESS);
	}
	pid = fork();
	if(pid < 0){
    	printf("No se ha podido crear el proceso hijo 2\n");
    	exit(EXIT_SUCCESS);
	}
	if (pid==0){
	/*Aqui va lo que hace el proceos B*/
	


		exit(EXIT_SUCCESS);
	}
	/*Aqui va lo que hace el proceso C*/












	return 0;
}