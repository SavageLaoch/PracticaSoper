/**
 * @brief Ejercicio 9
 *
 * @file Ejercicio9.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

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
#define NUMCAJAS 5
#define NUMCLIENTES 30
int sem_id, acabados=0;

/**
 * @brief ImprimirTurno
 *
 * Imprime el turno que se le pasa en el archivo de turnos
<<<<<<< HEAD
 *
=======
 * 
>>>>>>> 06c66660c3eea852500336ce5b66cdb22bae7a26
 * @param uturno a imprimir
 * @return void
 */

void ImprimirTurno(int i){
	FILE *fp;
	printf("Me he quedado en imprimir turno\n");
	if (Down_Semaforo(sem_id, NUMCAJAS, SEM_UNDO)==ERROR){
		printf("Error al bajar el semaforo de turnos \n");
	}
	fp=fopen("turnos.txt","w");
	fp=fopen("turnos.txt","w");					
	fprintf(fp, "%d",i);
	printf("He impreso el turno %d\n",i);
	fclose(fp);
	return;
}

/**
 * @brief LeerTurno
 *
 * Lee el turno del archivo de turnos y lo devuelve
<<<<<<< HEAD
 *
=======
 * 
>>>>>>> 06c66660c3eea852500336ce5b66cdb22bae7a26
 * @return t Turno
 */

int LeerTurno(){
	FILE *fp;
	int t;

	fp=fopen("turnos.txt","r");
	fscanf(fp, "%d",&t);
	printf("He leido el turno %d\n",t);
	fclose(fp);
	if (Up_Semaforo(sem_id, NUMCAJAS, SEM_UNDO)==ERROR){
		printf("Error al subir el semaforo de turnos \n");
	}
	return t;
}

/**
 * @brief ModificarCaja
 *
 * Modifica la caja i segun el parametro m. Si m=0 significa que tiene
 * que sumar simplemente cl a lo que hay en la caja e imprimirlo. Si m=1 significa
 * que lo ha llamado la funcion masde1000 y tiene que retirar 900 euros de la caja.
 * (Hemos puesto una comprobacion aqui dentro de que lo que hay en la caja sea mayor
 * que 1000 para evitar que se quite 900 dos veces seguidas). Por ultimo si m=2
 * significa que se han acabado los clientes y hay que sacar todo el dinero de la caja.
<<<<<<< HEAD
 *
=======
 *  
>>>>>>> 06c66660c3eea852500336ce5b66cdb22bae7a26
 * @param i Caja a modificar
 * @param cl Lo que hay que sumar a la caja (cuando hay que quitar todo simplemente
 * se pasa 0)
 * @param m Modo en el que se llama a esta funcion.
 * @return caj (lo que hemos leido de la caja) o sum (lo que vamos a meter en la
 * caja) segun quien lo llame.
 */

int ModificarCaja(int i, int cl, int m){
	FILE *fp;
	char s[50];
	int sum,caj;

	printf("Voy a bajar el semaforo de la caja %d\n",i);
	printf("Me he quedado en modificar caja\n");
	if (Down_Semaforo(sem_id, i, SEM_UNDO)==ERROR){
		printf("Error al bajar el semaforo %d\n",i);
	}

	sprintf(s,"dinerocaja%d.txt",i);
	fp=fopen(s,"r");
	fscanf(fp,"%d",&caj);
	fclose(fp);

	if (m==1 && caj<=1000){
		printf("Ya tengo menos de 1000 en la caja %d\n",i);
		if (Up_Semaforo(sem_id, i, SEM_UNDO)==ERROR){
		printf("Error al subir el semaforo %d\n",i);
		}
		return 0;
	}
	if (m==2) cl=-caj;

	if (m==1 && caj<1000){
		printf("Ya tengo menos de 1000 en la caja %d\n",i);
		if (Up_Semaforo(sem_id, i, SEM_UNDO)==ERROR){
		printf("Error al subir el semaforo %d\n",i);
		}	
		return 0;
	}
	if (m==2) cl=-caj; 


	sum = cl + caj;
	fp=fopen(s,"w");
	fprintf(fp, "%d",sum);
	fclose(fp);
	printf("En la %d caja hay %d\n",i,sum);

	if (Up_Semaforo(sem_id, i, SEM_UNDO)==ERROR){
		printf("Error al subir el semaforo %d\n",i);

	}
	if (m==0) return sum;
	return caj;
}

/**
 * @brief MasDe1000
 *
 * Manejador de la senal SIGUSR1. Saca 900 de la caja correspondiente y los
 * suma a la cuenta global
<<<<<<< HEAD
 *
=======
 * 
>>>>>>> 06c66660c3eea852500336ce5b66cdb22bae7a26
 * @return void
 */

void MasDe1000(int sig){
	int tot;
	int t;
	FILE *fp;

	t=LeerTurno();
	printf("ESTOY HACIENDO LA SENAL DE MASDE1000 en %d\n",t);
	fflush(stdout);	

	ModificarCaja(t, -900, 1);
	printf("YA HEMOS SACADO 900 EN %d\n",t);

	/*Aumentamos 900 en la cuenta global*/
	fp=fopen("cuentaglobal.txt","r");
	fscanf(fp,"%d",&tot);
	fclose(fp);
	fp=fopen("cuentaglobal.txt","w");
	fprintf(fp, "%d",tot+900);
	fclose(fp);

	printf("HE TERMINADO DE HACER LA SENAL DE MASDE1000 en %d\n",t);
	return;
}

/**
 * @brief ClientesAcabados
 *
<<<<<<< HEAD
 * Manejador de la senal SIGUSR2. Saca todo el dinero de la caja correspondiente
 * y lo suma a la cuenta global.
 *
=======
 * Manejador de la senal SIGUSR2. Saca todo el dinero de la caja correspondiente 
 * y lo suma a la cuenta global.
 * 
>>>>>>> 06c66660c3eea852500336ce5b66cdb22bae7a26
 * @return t Turno
 */

void ClientesAcabados(int sig){
    int tot,caj;
	int t;
	FILE *fp;
	FILE *fp;	
	printf("Entrada clientes acabados\n");
	t=LeerTurno();
	printf("ESTOY HACIENDO LA SENAL DE CLIENTESACABADOS en %d\n",t);

	caj = ModificarCaja(t, 0, 2);
	printf("YA HEMOS SACADO TODO EL DINERO EN %d\n",t);

	/*Aumentamos lo que quedaba en la caja en la cuenta global*/
	fp=fopen("cuentaglobal.txt","r");
	fscanf(fp,"%d",&tot);
	fclose(fp);
	fp=fopen("cuentaglobal.txt","w");
	fprintf(fp, "%d",tot+caj);
	fclose(fp);
	fclose(fp);	

	printf("HE TERMINADO DE HACER LA SENAL DE CLIENTESACABADOS en %d ---------------------\n",t);
	acabados++;
	return;
}

/**
 * @brief Main
 *
 * En el main se crean semaforos, los ficheros de los clientes, el fichero de cuenta
 * global, y se crean los procesos hijos o cajeros. Estos cajeros crean los ficheros
 * de sus respectivas cajas y van leyendo de los ficheros de los clientes y sumando
 * dinero a las cajas. Mandaran senales al proceso padre bien si sus cajas superan
 * los 1000 euros o bien si se quedan sin clientes (terminan de leer el archivo
 * de clientes)
<<<<<<< HEAD
 *
=======
 * 
>>>>>>> 06c66660c3eea852500336ce5b66cdb22bae7a26
 * @return EXIT_SUCCES o EXIT_FAILURE
 */

int main (int argc, char *argv[]){
	int i,j,al,al2,pid,cl,sum,c=0,tot,caj;
	unsigned short *array;
	FILE *fp1, *fp2, *fp;
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


	/*Creamos e inicializamos los semaforos*/
	if (Crear_Semaforo(SEMKEY, NUMCAJAS+1, &sem_id)==ERROR){
		printf("Error al crear el semaforo\n");
	}
	array = (unsigned short *)malloc(sizeof(unsigned short)*(NUMCAJAS+1));
	for (i=0; i<=NUMCAJAS; i++){
		array [i] = 1;
	}
	if (Inicializar_Semaforo(sem_id, array)==ERROR){
		printf("Error al inicializar el semaforo\n");
	}

	/*Creamos los ficheros de los clientes*/
	for (i=0;i<NUMCAJAS;i++){
		sprintf(s,"clientescaja%d.txt",i);
		fp1=fopen(s,"w");
		for (j=0;j<NUMCLIENTES;j++){
			al=rand()%300;
			fprintf(fp1, "%d\n",al);
		}
		fclose(fp1);
	}

	/*Creamos el fichero de la cuenta global*/
	fp1=fopen("cuentaglobal.txt","w");
	fprintf(fp1, "%d",c);
	fclose(fp1);

	/*Creamos los procesos hijos (cajeros) y los ficheros con el dinero
	recaudado por cada caja*/
	for (i=0;i<NUMCAJAS;i++){
		pid=fork();
		if (pid<0){
			printf("Error al hacer el fork\n");
		}else if (pid==0){
		/*Aqui va lo que hace cada hijo*/

			/*Creamos los ficheros de las cajas con 0*/
			sprintf(s,"dinerocaja%d.txt",i);
			fp2=fopen(s,"w");
			fprintf(fp2, "%d",c);
			fclose(fp2);

			/*Abro el archivo de los clientes*/
			sprintf(s,"clientescaja%d.txt",i);
			fp1=fopen(s,"r");

			while(fscanf(fp1,"%d\n",&cl) == 1){

				/*Leo el importe que paga el cliente y espero para la gestion*/


				/*printf("El importe que paga el cliente %d es %d\n",i,cl);
				*/
				al2=rand()%2;
				sleep(al2);

				sum = ModificarCaja(i, cl, 0);

				
				/*printf("El importe que paga el cliente %d es %d\n",i,cl);
				*/
				al2=rand()%2;
				sleep(al2);		

				sum = ModificarCaja(i, cl, 0);	


				/*Compruebo si en la caja hay mas de 1000 y aviso al padre*/
				if (sum>1000){
					printf("NOS HEMOS PASADO DE 1000 en la caja %d\n",i);


					ImprimirTurno(i);

					printf("MANDO SENAL DE NOS HEMOS PASADO DE 1000 en la caja %d\n",i);


					
					ImprimirTurno(i);

					printf("MANDO SENAL DE NOS HEMOS PASADO DE 1000 en la caja %d\n",i);
					

					if (kill(getppid(),SIGUSR1)==-1){
						printf("Error al mandar la senal de masde1000 en la caja %d\n",i);
					}
				}
			}
			/*Cierro el archivo de los clientes*/
			fclose(fp1);

			printf("CLIENTES ACABADOS en la caja %d\n",i);

			ImprimirTurno(i);

			printf("MANDO SENAL DE CLIENTES ACABADOS en la caja %d\n",i);
			printf("Numacabdos: %d\n",acabados );
			if (kill(getppid(),SIGUSR2)==-1){
				printf("Error al mandar la senal de clientesacabados en la caja %d\n",i);

			}

			exit(EXIT_SUCCESS);
		}
	}

			}			
			
			return EXIT_SUCCESS;
		}
	/*Esperamos a los procesos hijos*/
	while (wait(NULL) > 0);

	/*Esto lo hacemos porque nos ha ocurrido que algunas veces veces los procesos hijos
<<<<<<< HEAD
	 han acabado pero al padre aun le ha quedado hacerse cargo de alguna senal de
=======
	 han acabado pero al padre aun le ha quedado hacerse cargo de alguna senal de 
>>>>>>> 06c66660c3eea852500336ce5b66cdb22bae7a26
	 ClientesAcabados*/
	/*if (acabados!=NUMCAJAS){
		printf("\nVAMOS A HACER UNA ULTIMA RONDA\n");
		for (i=0;i<NUMCAJAS;i++){
			caj = ModificarCaja(i, 0, 2);
			fp=fopen("cuentaglobal.txt","r");
			fscanf(fp,"%d",&tot);
			fclose(fp);
			fp=fopen("cuentaglobal.txt","w");
			fprintf(fp, "%d",tot+caj);
			fclose(fp);
		}
	}*/

	/*Borramos los semaforos*/
	if (Borrar_Semaforo(sem_id)==ERROR) {
		printf("Error al borrar los semaforos\n");
	}

	printf("\nTodo ha ido correctamente\n\n");



	return EXIT_SUCCESS;
}
