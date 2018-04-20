/**
 * @brief Ejercicio 5
 *
 * @file ejercicio5.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

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

/**
 * @brief Main
 *
 * En este main se crean 3 procesos que se van mandando mensajes a traves de una cola de mensajes
 * El proceso A le va mandando al proceso B mensajes de 16 en 16 bytes con lo que lee de un fichero
 * El proceso B lee el mensaje, lo traduce y lo manda al C
 * El proceso C lee el mensaje traducido y lo imprime en un fichero
 *
 * @return EXIT_FAILURE o EXIT_SUCCESS segun haya terminado mal o bien el programa
 */

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
    	exit(EXIT_FAILURE);
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
    	exit(EXIT_FAILURE);
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
			memset(buffer,32,16);
			strcpy(buffer,msg.contenido);
			for(i=0;i<sizeof(msg.contenido);i++){
				if (buffer[i]==' '){
					continue;
				}else if (buffer[i]==0){
					buffer[i]=0;
				}else if (buffer[i]==10){
					buffer[i]=10;
				}else if (buffer[i]=='Z'){
					buffer[i]='A';
				}else if (buffer[i]=='z'){
					buffer[i]='a';
				}else {
					buffer[i]=buffer[i]+1;
				}
				strcpy(msg.contenido,buffer);
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
			printf("PROGRAMA TERMINADO CON EXITO\n");
			fclose(f);
			exit(EXIT_SUCCESS);
		}
		printf("Recibido mensaje tipo %ld = %s\n", msg.id, msg.contenido);
		/*Imprimimos el mensaje en el fichero*/
		fprintf(f,"%s",msg.contenido);
	}
	msgctl(msqid,IPC_RMID,(struct msqid_ds*)NULL);
	fclose(f);
	exit(EXIT_SUCCESS);
}
