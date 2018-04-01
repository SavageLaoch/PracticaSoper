#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "semaforos.h"

#define SEMKEY 75798
#define NUMCAJAS 3

void MasDe1000(int sig){
	int tot,caj;
	FILE *fp;
	char s[50];
	int sem_id, t; 

	printf("ESTOY HACIENDO LA SENAL DE MASDE1000\n");

	/*Leo que semaforo manda la senal*/	
	fp=fopen("turnos.txt","w+");
	rewind(fp);
	fscanf(fp, "%d",&t);
	printf("El turno que me han pasado es %d\n",t);
	fclose(fp);

	/*Quitamos 1000 de la caja que manda la senal*/
	if (Down_Semaforo(sem_id, t, SEM_UNDO)==ERROR){
		printf("Error al bajar el semaforo %d en masde1000\n",t);
	}
	sprintf(s,"dinerocaja%d.txt",t);
	fp=fopen(s,"r+");
	fscanf(fp,"%d",&caj);
	rewind(fp);
	fprintf(fp, "%d",caj-1000);
	fclose(fp);
	if (Up_Semaforo(sem_id, t, SEM_UNDO)==ERROR){
		printf("Error al subir el semaforo %d en masde1000\n",t);
	}

	/*Aumentamos 1000 en la cuenta global*/
	fp=fopen("cuentaglobal.txt","r+");
	fscanf(fp,"%d",&tot);
	rewind(fp);
	fprintf(fp, "%d",tot+1000);
	fclose(fp);	

	/*Subo el semaforo de turnos*/
	if (Up_Semaforo(sem_id, NUMCAJAS, SEM_UNDO)==ERROR){
		printf("Error al subir el semaforo de turnos \n");
	}
	return;
}
void ClientesAcabados(int sig){
    int tot,caj,c=0;
	FILE *fp;
	char s[50];
	int sem_id,t;

	printf("ESTOY HACIENDO LA SENAL DE CLIENTESACABADOS\n");

	/*Leo que semaforo manda la senal*/	
	fp=fopen("turnos.txt","w+");
	rewind(fp);
	fscanf(fp, "%d",&t);
	printf("El turno que me han pasado es %d\n",t);
	fclose(fp);

	/*Quitamos lo que queda en la caja que manda la senal*/
	if (Down_Semaforo(sem_id, t, SEM_UNDO)==ERROR){
		printf("Error al bajar el semaforo %d en clientesacabdaos\n",t);
	}
	sprintf(s,"dinerocaja%d.txt",t);
	fp=fopen(s,"r+");
	fscanf(fp,"%d",&caj);
	rewind(fp);
	fprintf(fp, "%d",c);
	fclose(fp);
	if (Up_Semaforo(sem_id, t, SEM_UNDO)==ERROR){
		printf("Error al subir el semaforo %d en clientesacabados\n",t);
	}

	/*Aumentamos lo que quedaba en la caja en la cuenta global*/
	fp=fopen("cuentaglobal.txt","r+");
	fscanf(fp,"%d",&tot);
	rewind(fp);
	fprintf(fp, "%d",tot+caj);
	fclose(fp);	

	/*Subo el semaforo de turnos*/
	if (Up_Semaforo(sem_id, NUMCAJAS, SEM_UNDO)==ERROR){
		printf("Error al subir el semaforo de turnos \n");
	}
	return;
}

int main (int argc, char *argv[]){
	int n,i,j,al,al2,pid,cl,caj,sum,c=0,m=-1;
	int sem_id; 
	unsigned short *array;
	FILE *fp1, *fp2;
	char s[50];

	printf("---------------------\n");

	/*Definimos las senales*/
	if (signal(SIGUSR1,MasDe1000) == SIG_ERR){
		perror("signal");
		exit (EXIT_FAILURE);
	}
	if (signal(SIGUSR2,ClientesAcabados) == SIG_ERR){
		perror("signal");
		exit (EXIT_FAILURE);
	}

	/*Asignamos a n el numero de cajas*/
	n=NUMCAJAS;

	/*Creamos e inicializamos los semaforos*/
	if (Crear_Semaforo(SEMKEY, n+1, &sem_id)==ERROR){
		printf("Error al crear el semaforo\n");
	}
	array = (unsigned short *)malloc(sizeof(short)*n);
	for (i=0; i<=n; i++){
		array [i] = 1;
	}	
	if (Inicializar_Semaforo(sem_id, array)==ERROR){
		printf("Error al inicializar el semaforo\n");
	}

	/*Creamos los ficheros de los clientes*/
	for (i=0;i<n;i++){
		sprintf(s,"clientescaja%d.txt",i);
		fp1=fopen(s,"w+");
		for (j=0;j<10;j++){
			al=rand()%300;
			fprintf(fp1, "%d\n",al);
		}
		fclose(fp1);
	}

	/*Creamos el fichero de la cuenta global*/
	fp1=fopen("cuentaglobal.txt","w+");
	fprintf(fp1, "%d",c);
	fclose(fp1);

	/*Creamos el fichero de turnos*/
	fp1=fopen("turnos.txt","w+");
	fprintf(fp1, "%d",m);
	fclose(fp1);

	/*Creamos los procesos hijos (cajeros) y los ficheros con el dinero
	recaudado por cada caja*/
	for (i=0;i<n;i++){
		pid=fork();
		if (pid<0){
			printf("Error al hacer el fork\n");
		}else if (pid==0){
		/*Aqui va lo que hace cada hijo*/
			
			/*Creamos los ficheros de las cajas con 0*/
			sprintf(s,"dinerocaja%d.txt",i);
			fp2=fopen(s,"w+");
			fprintf(fp2, "%d",c);
			fclose(fp2);

			/*Abro el archivo de los clientes*/
			sprintf(s,"clientescaja%d.txt",i);
			fp1=fopen(s,"r");

			while(!feof(fp1)){

				/*Leo el importe que paga el cliente y espero para la gestion*/
				fscanf(fp1,"%d\n",&cl);
				printf("El importe que paga el cliente %d es %d\n",i,cl);
				al2=rand()%5;
				sleep(al2);		

				/*Leo lo que habia de dinero en la caja*/
				if (Down_Semaforo(sem_id, i, SEM_UNDO)==ERROR){
					printf("Error al bajar el semaforo %d\n",i);
				}

				sprintf(s,"dinerocaja%d.txt",i);
				fp2=fopen(s,"r+");
				fscanf(fp2,"%d",&caj);
				printf("En la %d caja hay %d\n",i,caj);

				/*Sumo lo de la caja mas lo del cliente y lo imprimo en la caja*/
				sum = cl + caj;
				printf("Sum en caja %d vale %d\n",i,sum);
				rewind(fp2);
				fprintf(fp2, "%d",sum);
				fclose(fp2);	

				/*Compruebo si en la caja hay mas de 1000 y aviso al padre*/
				if (sum>1000){
					printf("NOS HEMOS PASADO DE 1000\n");
					/*Imprimo la i en el archivo de turnos*/
					if (Down_Semaforo(sem_id, n, SEM_UNDO)==ERROR){
						printf("Error al bajar el semaforo de turnos \n");
					}
					printf("Imprimo el turno\n");
					fp2=fopen("turnos.txt","w+");					
					fprintf(fp2, "%d",i);
					rewind(fp2);
					fclose(fp2);
					

					/*Mando senal de MasDe1000*/
					if (kill(getppid(),SIGUSR1)==-1){
						printf("Error al mandar la senal de masde1000 en la caja %d\n",i);
					}
				}
				if (Up_Semaforo(sem_id, i, SEM_UNDO)==ERROR){
					printf("Error al subir el semaforo %d\n",i);
				}

			}
			printf("CLIENTES ACABADOS\n");
			/*Imprimo la i en el archivo de turnos*/
			if (Down_Semaforo(sem_id, n, SEM_UNDO)==ERROR){
				printf("Error al bajar el semaforo de turnos \n");
			}
			printf("Imprimo el turno\n");
			fp2=fopen("turnos.txt","w+");
			rewind(fp2);
			fprintf(fp2, "%d",i);
			fclose(fp2);

			/*Mando senal de HeAcabado*/
			if (kill(getppid(),SIGUSR2)==-1){
				printf("Error al mandar la senal de clientesacabados en la caja %d\n",i);
			}
			/*Cierro el archivo de los clientes*/
			fclose(fp1);
			return 0;
		}
	}
	/*Aqui va lo que hace el padre una vez ha creado los hijos*/
	for (i=0;i<n;i++){
		wait(0);
	}

	/*Borramos los semaforos*/
	if (Borrar_Semaforo(sem_id)==ERROR) {
		printf("Error al borrar los semaforos\n");
	}

	return 0;
}