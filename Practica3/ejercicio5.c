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
	int ultimo;
}mensaje;

int main(int argc, char *argv[]){
	key_t clave;
	int msqid,pid,i;
	mensaje msg;
	FILE *f;
	char *buffer;

	printf("Vamo a calmarno\n");

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
	printf("Clave obtenida\n");

	/*Creamos la cola de mensajes*/
	msqid = msgget (clave, 0600 | IPC_CREAT);
	if (msqid == -1){
		perror("Error al obtener identificador para cola mensajes");
		return(0);
	}
	printf("Cola creada\n");

	/*Con 2 forks creamos dos hijos*/
	pid = fork();
	if(pid < 0){
    	printf("No se ha podido crear el proceso hijo 1\n");
    	exit(EXIT_SUCCESS);
	}
	if(pid == 0){
	printf("Soy el proceso A");
	/*Aqui va lo que hace el proceso A*/
		/*Leemos del fichero*/
		f=fopen(argv[1],"r");
		buffer = (char*)malloc(16*sizeof(char));
		while (fgets(buffer,16,f)!=NULL){ /*Esto esta fatal*/
			printf("%s",buffer);			
			/*Mandamos el mensaje*/
			msg.id=1; /*1 significa que aun no esta traducido, se lo manda al proceso B*/
			strcpy(msg.contenido,buffer);
			msg.ultimo=0;
			msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long),IPC_NOWAIT);
		}
		fclose(f);
		/*Aviso al proceso B de que he acabado mandando un mensaje con el valor de ultimo a 1*/
		msg.id=1; 
		msg.ultimo=1;
		msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long),IPC_NOWAIT);
		exit(EXIT_SUCCESS);
	}
	pid = fork();
	if(pid < 0){
    	printf("No se ha podido crear el proceso hijo 2\n");
    	exit(EXIT_SUCCESS);
	}
	if (pid==0){
	printf("Soy el proceso B");
	/*Aqui va lo que hace el proceso B*/
		while (1){ /* Mientras que el proceso A no ha acabado */
			/* Recibimos un mensaje */
			msgrcv (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long), 1, 0); /* Leemos solo los mensajes tipo 1 */
			if (msg.ultimo==1){
				printf("Este es el ultimo mensaje");
				/*Aviso al proceso C de que he acabado*/
				msg.id=2; 
				msg.ultimo=1;
				msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long),IPC_NOWAIT);
				exit(EXIT_SUCCESS);
			}
			printf("Recibido mensaje tipo %ld \n", msg.id);
			printf("Mensaje = %s \n", msg.contenido);
			/* Modificamos el mensaje */
			for(i=0;i<sizeof(msg.contenido);i++){
				msg.contenido[i]=msg.contenido[i]+1;
			}
			/* Mandamos el mensaje */
			msg.id=2; /*2 significa que ya esta traducido, se lo manda al proceso C*/
			msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long),IPC_NOWAIT);
		}
		exit(EXIT_SUCCESS);
	}
	printf("Soy el proceso C");
	/*Aqui va lo que hace el proceso C*/
	f=fopen(argv[2],"w");
	while(1){/* Mientras que el procesoBA no ha acabado */
		/* Recibimos un mensaje */
		msgrcv (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long), 2, 0); /* Leemos solo los mensajes tipo 2 */
		if (msg.ultimo==1){
			printf("YA SE HA TERMINADO TODO");
			exit(EXIT_SUCCESS);
		}
		printf("Recibido mensaje tipo %ld \n", msg.id);
		printf("Mensaje = %s \n", msg.contenido);
		/*Imprimimos el mensaje en el fichero*/
		fprintf(f,"%s",msg.contenido);
	}
	fclose(f);
	exit(EXIT_SUCCESS);












	return 0;
}