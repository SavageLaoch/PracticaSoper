#include "apostador.h"

typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
  char nombre_apostador[MAXCHAR]; /*Nombre del apostador*/
	int num_caballo; /*Numero del caballo*/
  double cuantia; /*Cuantia de la apuesta*/
}Mensaje;

int aleat_num(int inf, int sup){
	if (inf>sup) return -1;
	int randm,rango,grupo,limite;
	rango=sup-inf+1;
	grupo=RAND_MAX/rango;
	limite=grupo*rango;
	do{
		randm=rand();
	}while(randm>=limite);
	return (randm/grupo)+inf;
}

void captura(int sig){
  exit(EXIT_SUCCESS);
}

void apostador(int num_apostadores,int num_caballos){
  int clave,msqid;
  Mensaje mensaje;
  char nombre_apostador[MAXCHAR];
	int num_caballo;
  double cuantia;
  int i;

  /* Establecemos la semilla */
  srand(time(NULL));

  /* Establecemos el manejador de la alarma */
  if (signal(SIGALRM, captura) == SIG_ERR){
    puts("Error en la captura");
    exit (EXIT_FAILURE);
  }

  /* Creamos la cola de mensajes */
  clave = ftok (FILEKEY, KEY);
	if (clave==(key_t) -1){
		printf("Error al coger el key de la cola de mensajes");
	}
	msqid = msgget(clave,0600 | IPC_CREAT);
	if (msqid == -1){
		printf("Error al coger el key de la cola de mensajes");
	}

  /* Establecemos la alarma de 30 segundos antes de la carrera */
  alarm(30);

  /* Creamos el bucle de los apostadores */
  for(i = 0; i < num_apostadores; i++){
    /* Preparamos el mensaje */
    sprintf(nombre_apostador,"Apostador-%d",i);
    num_caballo = aleat_num(0,num_caballo - 1);
    cuantia = aleat_num(10,100)/10.0;
    /* Enviamos el mensaje */
    mensaje.id = 1;
    strcpy(mensaje.nombre_apostador,nombre_apostador);
    mensaje.num_caballo = num_caballo;
    mensaje.cuantia = cuantia;
    msgsnd(msqid,(struct msgbuf *) &mensaje,sizeof(Mensaje) - sizeof(long) - strlen(nombre_apostador) - sizeof(int) - sizeof(double),IPC_NOWAIT);
    sleep(1);
  }
  exit(EXIT_SUCCESS);
}
