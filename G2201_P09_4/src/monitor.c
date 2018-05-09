#include "utils.h"

/**
 * @brief Monitor
 *
 * @file monitor.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
	int tirada; /*Informacion a transmitir en el mensaje*/
}Mensaje;

typedef struct _MemComp{
	double cotizacion[MAX];
	double apuesta[MAX];
	int eleccion[MAX];
	int caballo[MAX];
	double ganancias[MAX];
}MemComp;

/**
 * @brief BubbleSort
 *
 * Funcion que ordena una tabla de mayor a menor
 *
 * @param tabla Tabla a ordenar
 * @param ip Primer elemento de la tabla
 * @param iu Ultimo elemento de la tabla
 * @return double* Tabla ordenada
 */
double* BubbleSort(double* tabla, int ip, int iu){
    if (tabla==NULL || ip<0 || iu<ip) return NULL;
    int i=iu;
    double swap;
    int cont,j;
    cont=0;
    while (i>=ip+1){
      for (j=ip;j<i;j++){
        if (tabla[j]<tabla[j+1]){
          swap=tabla[j];
          tabla[j]=tabla[j+1];
          tabla[j+1]=swap;
        }
        cont++;
      }
      i--;
    }
    return tabla;
}

/**
 * @brief monitor_antes
 *
 * Funcion del monitor antes de la carrera
 *
 */
void monitor_antes(int num_caballos,int max_distancia,int sem_id,int id_zone){
	int i,j;
	MemComp *mem;

	mem = shmat (id_zone, (struct _MemComp *)0, 0);

	syslog (LOG_NOTICE, "Vamos a hacer la cuenta atras inicial");
	printf("-----CUENTA ATRAS-----\n");
	for (i=TIEMPO;i>0;i--){
		printf("La carrera empieza en %d\n",i);
		sleep(1);

		if (Down_Semaforo(sem_id, 1, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo");
		}
		for(j=0;j<num_caballos;j++){
			printf("Cotizacion del caballo %d = %f\n",j,mem->cotizacion[j]);
		}
		if (Up_Semaforo(sem_id, 1, SEM_UNDO)==ERROR){
			printf("Error al subir el semaforo");
		}

	}
	printf("\n----------GO----------\n");
	syslog (LOG_NOTICE, "Que comience la carrera");
	kill(getppid(),SIGUSR1);
	shmdt ((MemComp *)mem);
}

/**
 * @brief monitor_durante
 *
 * Funcion del monitor durante la carrera
 *
 */
void monitor_durante(int num_caballos,int max_distancia,int sem_id,int id_zone){
	int i,flag=0;
	long *posicion;
	char *antes;

	/*Inicializamos el array antes*/
	antes = (char *)malloc(sizeof(char) * num_caballos);
	for(i=0;i<num_caballos;i++){
		antes[i]=0;
	}

	/* Obtenemos la memoria compartida */
	posicion = shmat (id_zone, (char *)0, 0);

	while (1){
		if (Down_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo monitor durante");
		}
		for (i=0; i<num_caballos; i++){
			if (posicion[i]!=antes[i]){
				antes[i]=posicion[i];
				fflush(stdout);
				printf("El caballo %d avanza a la posicion %ld con una tirada de %ld\n\n",i,posicion[i],posicion[num_caballos+i]);
				fflush(stdout);
				if (posicion[i]>=max_distancia) flag=1;
			}
		}
		if (flag==1){
			if (Up_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
				printf("Error al subir el semaforo");
			}
			syslog (LOG_NOTICE, "Tenemos un ganador");
			shmdt ((char *)posicion);
			free(antes);
			return;
		}
		if (Up_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al subir el semaforo");
		}
	}
}

/**
 * @brief monitor_despues
 *
 * Funcion del monitor despues de la carrera
 *
 */
void monitor_despues(int max_dinero,int num_apostadores,int num_caballos,int max_distancia,int sem_id, int id_zone,int id_zone2){
	long *posicion;
	int i,j=0, ganador=0, k;
	MemComp *mem;
	int ganadores[MAX],ganadores_ord[MAX];
	double ganancias[MAX], ganancias_ordcopy[MAX], *ganancias_ord;
	FILE *f;
	double ben, rest;

	/* Obtenemos la memoria compartida */
	posicion = shmat (id_zone, (char *)0, 0);
	mem = shmat (id_zone2, (struct _MemComp *)0, 0);

	syslog (LOG_NOTICE, "Cuenta atras para la entrega de premios");
	printf("\n-----FINAL DE LA CARRERA-----\n");
	for(i=15;i>0;i--){
		printf("Quedan %d segundos para los resultados\n",i);
		sleep(1);
	}

	syslog (LOG_NOTICE, "Entrega de premios");
	/*Calculamos las posiciones finales y las imprimimos en el report*/
	printf("\n--Posiciones finales:\n");
	if (Down_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo");
	}
	if (Down_Semaforo(sem_id, 2, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo");
	}
	f=fopen("Report.txt","a");
	fprintf(f,"\nResultado de la carrera:\n");
	for (i=0; i<num_caballos; i++){
		if (posicion[i]>posicion[ganador]) ganador=i;
		printf("Posicion final del caballo %d = %ld\n",i,posicion[i]);
		fprintf(f,"Posicion final del caballo %d = %ld\n",i,posicion[i]);			
	}
	printf("Ha ganado el caballo %d!\n",ganador);
	fprintf(f,"Ha ganado el caballo %d!\n",ganador);
	fclose(f);	
	if (Up_Semaforo(sem_id, 2, SEM_UNDO)==ERROR){
			printf("Error al subir el semaforo");
	}
	if (Up_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al subir el semaforo");
	}


	/*Calculamos los 10 apostadores con mas beneficios*/
	printf("\n--Lista de los apostadores con mas beneficios:\n");

	/*Creamos una lista con todos los ganadores*/
	for (i=0;i<num_apostadores;i++){
		if (mem->eleccion[i]==ganador){
			ganadores[j]=i;
			j++;
		}
	}

	/*Creamos una lista con lo que han ganado esos ganadores*/
	for (i=0;i<j;i++){
		ganancias[i]=mem->ganancias[ganadores[i]];
	}

	/*Ordenamos la lista las ganancias de mayor a menor*/
	for (i=0;i<j;i++){
		ganancias_ordcopy[i]=ganancias[i];
	}
	ganancias_ord = BubbleSort(ganancias_ordcopy,0,j-1);
	for (i=0;i<j;i++){
	}

	/*Ordenamos la lista de los ganadores*/
	for (i=0;i<j;i++){
		for (k=0;k<j;k++){
			if (ganancias_ord[i]==ganancias[k]){
				ganadores_ord[i]=ganadores[k];
				break;
			}
		}
	}

	/*Imprimimos cuales son las 10 mayores ganancias*/
	for (i=0;i<j && i<10;i++){
		printf("Puesto numero %d = Apostador-%d con %f euros\n",i+1,ganadores_ord[i],ganancias_ord[i]);
	}
	if(i==0){
		printf("No hay ganadores\n");
	}else if(i<10){
		printf("No hay mas ganadores\n");
	}else {
		printf("Estos han sido los 10 primeros ganadores\n");
	}

	/*Imprimimos en el report el resultado de las apuestas*/
	if (Down_Semaforo(sem_id, 2, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo");
	}
	f=fopen("Report.txt","a");
	fprintf(f,"\nResultado de las apuestas:\n/ Nombre / Apuesta / Beneficios / Dinero Restante /\n");
	for(i=0;i<num_apostadores;i++){
		if(mem->apuesta[i]==0) break;
		ben = 0 - mem->apuesta[i];
		rest = max_dinero - mem->apuesta[i];
		if(mem->eleccion[i]==ganador){
			ben = mem->ganancias[i];
			rest = max_dinero + mem->ganancias[i];
		}
		fprintf(f,"Apostador %d / %f / %f / %f /\n",i,mem->apuesta[i],ben,rest);
	}

	if (Up_Semaforo(sem_id, 2, SEM_UNDO)==ERROR){
		printf("Error al bajar el semaforo");
	}
	printf("Report generado en el archivo 'Report.txt'\n");
	fclose(f);
	shmdt ((char *)posicion);
	shmdt ((MemComp *)mem);


}

/**
 * @brief monitor
 *
 * Proceso monitor.
 *
 */
void monitor(int max_dinero,int num_apostadores,int num_caballos, int max_distancia, int sem_id,int id_zone, int id_zone2){

	syslog (LOG_NOTICE, "Soy el monitor");
	monitor_antes(num_caballos,max_distancia,sem_id,id_zone2);
	monitor_durante(num_caballos,max_distancia,sem_id,id_zone);
	monitor_despues(max_dinero,num_apostadores,num_caballos,max_distancia,sem_id,id_zone,id_zone2);
	kill(getppid(),SIGUSR1);
	syslog (LOG_NOTICE, "Monitor termina");
	closelog();
	return;
}
