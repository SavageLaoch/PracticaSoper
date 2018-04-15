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
	char contenido[16]; /*Informacion a transmitir en el mensaje*/
	int ultimo;
}mensaje;

int main(int argc, char *argv[]){
	key_t clave;
	int msqid,pid1,pid2,i;
	mensaje msg;
	FILE *f;
	char buffer[16];

	/*Comprobamos los parametros*/
	if (argc < 3){
		printf("No se han introducido suficientes parámetros:\nEl primer archivo tiene que ser entrada.txt \n");
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
		perror("Error al obtener identificador para cola mensajes\n");
		return(0);
	}

	/*Con 2 forks creamos dos hijos*/
	pid1 = fork();
	if(pid1 < 0){
    	printf("No se ha podido crear el proceso hijo 1\n");
    	exit(EXIT_SUCCESS);
	}
	if(pid1 == 0){
	printf("Soy el proceso A\n");
	/*Aqui va lo que hace el proceso A*/
		/*Leemos del fichero*/
		f=fopen(argv[1],"r");	
		while (fgets(buffer,17,f)!=NULL){ /*Mientras que hay fichero*/
			/*Mandamos el mensaje*/
			msg.id=1; /*1 significa que aun no esta traducido, se lo manda al proceso B*/
			strcpy(msg.contenido,buffer);
			msg.ultimo=0;			
			msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long) - sizeof(int),IPC_NOWAIT);
			printf("Enviado mensaje tipo %ld = %s \n", msg.id,msg.contenido);
		}
		fclose(f);
		/*Aviso al proceso B de que he acabado mandando un mensaje con el valor de ultimo a 1*/
		msg.id=1; 
		msg.ultimo=1;
		msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long) - sizeof(int),IPC_NOWAIT);
		printf("Enviado mensaje de fin A\n");
		exit(EXIT_SUCCESS);
	}
	pid2 = fork();
	if(pid2 < 0){
    	printf("No se ha podido crear el proceso hijo 2\n");
    	exit(EXIT_SUCCESS);
	}
	if (pid2==0){
	printf("Soy el proceso B\n");
	/*Aqui va lo que hace el proceso B*/
		while (1){ /* Mientras que el proceso A no ha acabado */
			/* Recibimos un mensaje */
			msgrcv (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long) - sizeof(int), 1, 0); /* Leemos solo los mensajes tipo 1 */
			if (msg.ultimo==1){
				/*Aviso al proceso C de que he acabado*/
				msg.id=2; 
				msg.ultimo=1;
				msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long) - sizeof(int),IPC_NOWAIT);
				printf("Enviado mensaje de fin B\n");
				exit(EXIT_SUCCESS);
			}
			printf("Recibido mensaje tipo %ld = %s \n", msg.id,msg.contenido);
			/* Modificamos el mensaje */
			for(i=0;i<sizeof(msg.contenido);i++){
				if (msg.contenido[i]==' '){
					continue;
				}else if (msg.contenido[i]=='Z'){
					msg.contenido[i]='A';
				}else if (msg.contenido[i]=='z'){
					msg.contenido[i]='a';
				}else {
					msg.contenido[i]=msg.contenido[i]+1;
				}
			}
			/* Mandamos el mensaje */
			msg.id=2; /*2 significa que ya esta traducido, se lo manda al proceso C*/
			msgsnd (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long) - sizeof(int),IPC_NOWAIT);
			printf("Enviado mensaje tipo %ld = %s \n", msg.id,msg.contenido);
		}
		exit(EXIT_SUCCESS);
	}
	printf("Soy el proceso C\n");
	/*Aqui va lo que hace el proceso C*/
	f=fopen(argv[2],"w");
	while(1){/* Mientras que el procesoBA no ha acabado */
		/* Recibimos un mensaje */
		msgrcv (msqid, (struct msgbuf *) &msg, sizeof(mensaje) - sizeof(long) - sizeof(int), 2, 0); /* Leemos solo los mensajes tipo 2 */
		if (msg.ultimo==1){
			printf("YA HA TERMINADO TODO\n");
			fclose(f);
			exit(EXIT_SUCCESS);
		}
		printf("Recibido mensaje tipo %ld = %s\n", msg.id, msg.contenido);
		/*Imprimimos el mensaje en el fichero*/
		fprintf(f,"%s",msg.contenido);
	}
	fclose(f);
	exit(EXIT_SUCCESS);
}