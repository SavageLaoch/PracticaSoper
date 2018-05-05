#include "utils.h"

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

/*Funcion de ordenacion*/
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

void monitor_antes(int num_caballos,int max_distancia,int sem_id,int id_zone){
	int i,j;
	MemComp *mem;

	mem = shmat (id_zone, (MemComp *)0, 0);

	printf("-----CUENTA ATRAS-----\n");
	for (i=20;i>0;i--){
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
	kill(getppid(),SIGUSR1);
}

void monitor_durante(int num_caballos,int max_distancia,int sem_id,int id_zone){
	int i,flag=0;
	char *posicion;
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
				printf("El caballo %d avanza a la posicion %d con una tirada de %d\n\n",i,posicion[i],posicion[num_caballos+i]);
				fflush(stdout);
				if (posicion[i]>=max_distancia) flag=1;
			}
		}
		if (flag==1){
			if (Up_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
				printf("Error al subir el semaforo");
			}
			return;
		}
		if (Up_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al subir el semaforo");
		}
	}
}

void monitor_despues(int num_apostadores,int num_caballos,int max_distancia,int sem_id, int id_zone,int id_zone2){
	/*Falta implementar el resultado de las apuestas*/
	char *posicion;
	int i,j=0, ganador=0, k;
	MemComp *mem;
	int ganadores[MAX],ganadores_ord[MAX];
	double ganancias[MAX], ganancias_ordcopy[MAX], *ganancias_ord;
	

	/* Obtenemos la memoria compartida */
	posicion = shmat (id_zone, (char *)0, 0);
	mem = shmat (id_zone2, (MemComp *)0, 0);


	printf("\n-----FINAL DE LA CARRERA-----\n");

	/*Calculamos las posiciones finales*/
	printf("\n--Posiciones finales:\n");
	if (Down_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al bajar el semaforo");
	}
	for (i=0; i<num_caballos; i++){
		if (posicion[i]>posicion[ganador]) ganador=i;
		printf("Posicion final del caballo %d = %d\n",i,posicion[i]);
	}
	if (Up_Semaforo(sem_id, 0, SEM_UNDO)==ERROR){
			printf("Error al subir el semaforo");
	}
	printf("Ha ganado el caballo %d!\n",ganador);

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
		printf("Puesto numero %d = Apostador %d con %f euros\n",i+1,ganadores_ord[i],ganancias_ord[i]);
	}
	if(i<10){
		printf("No hay mas ganadores\n");
	}else {
		printf("Estos han sido los 10 primeros ganadores\n");
	}
}

void monitor(int num_apostadores,int num_caballos, int max_distancia, int sem_id,int id_zone, int id_zone2){

	/* Establecemos la semilla */
	srand(time(NULL));

	monitor_antes(num_caballos,max_distancia,sem_id,id_zone2);
	monitor_durante(num_caballos,max_distancia,sem_id,id_zone);
	monitor_despues(num_apostadores,num_caballos,max_distancia,sem_id,id_zone,id_zone2);
	printf("Monitor termina\n");
	return;
}
