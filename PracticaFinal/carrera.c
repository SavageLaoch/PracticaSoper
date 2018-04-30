#include "carrera.h"
#include "caballos.h"
typedef struct _Mensaje{
	long id; /*Campo obligatorio a long que identifica el tipo de mensaje*/
	int tirada; /*Informacion a transmitir en el mensaje*/
}Mensaje;

void carrera(int num_caballos,int max_distancia){
  int **pipes;
  int pipe_status;

  int *caballos;

  int flag = 1;
  int i;

  int clave,msqid;
  int pid[100];

  int *siguiente_tirada;
  int max,min;

  Mensaje mensaje;
  /* Creamos las pipes */
  pipes = (int**) malloc(sizeof(int*)*num_caballos);
  if(pipes==NULL){
    printf("Error al reservar memoria");
    exit(EXIT_FAILURE);
  }
  for (i=0;i<num_caballos;i++){
    pipes[i]=(int*)malloc(sizeof(int)*2);
    if (pipes[i]==NULL){
      printf("Error al reservar memoria");
      exit(EXIT_FAILURE);
    }
    pipe_status=pipe(pipes[i]);
    if (pipe_status==-1){
      printf("Error al crear la tuberia %d",i);
      exit(EXIT_FAILURE);
    }
  }
  /* Inicializamos la posicion de los caballos */
  caballos = (int*)malloc(sizeof(int) * num_caballos);
  if (caballos == NULL){
    printf("Error al inicializar la posicion de los caballos\n");
    exit(EXIT_FAILURE);
  }
  for(i = 0; i < num_caballos;i++){
    caballos[i] = 0;
  }
  /* Inicializamos la siguiente tirada de los caballos */
  siguiente_tirada = (int*)malloc(sizeof(int) * num_caballos);
  if (siguiente_tirada == NULL){
    printf("Error al inicializar la posicion de los caballos\n");
    exit(EXIT_FAILURE);
  }
  for(i = 0; i < num_caballos;i++){
    siguiente_tirada[i] = NORMAL;
  }
  
  /* Creamos la cola de mensajes */
  clave = ftok (FILEKEY, KEY);
  if (clave==(key_t) -1){
    printf("Error al coger el key de la cola de mensajes");
    exit(EXIT_FAILURE);
  }
  msqid = msgget(clave,0666 | IPC_CREAT);
  if (msqid == -1){
    printf("Error al coger el key de la cola de mensajes");
    exit(EXIT_FAILURE);
  }
  printf("Punto 1\n");
  /* Hacemos la carrera */
  for(i = 0;i < num_caballos; i++){
    pid[i] = fork();
    printf("Pid p%d\n",pid[i]);
    if(pid[i] == -1){
      printf("Error al crear el fork\n");
      exit(EXIT_SUCCESS);
    }
    printf("Punto 2\n");
    /* Creamos los procesos caballo */
    if(pid[i] == 0){
      printf("Pid p%d\n",pid[i]);
      caballo(pipes[i],pid[i]);
      exit(EXIT_SUCCESS);
    }
  }

  /* Creamos el proceso principal */
  printf("Punto 3\n");

  while(flag){
    /* Enviamos la siguiente tirada */
    for(i = 0; i < num_caballos; i ++){
      char tirada[100];
      sprintf(tirada,"%d\0",siguiente_tirada[i]);
      close(pipes[i][0]);
      write(pipes[i][1],tirada,strlen(tirada));
      kill(pid[i],SIGUSR1);
    }
    /* Leemos los mensajes */
    min = 0;
    max = 0;
    printf("Punto 4\n");
    for(i = 0; i < num_caballos; i ++){
      msgrcv(msqid,(struct msgbuf *) &mensaje,sizeof(Mensaje) - sizeof(long),pid[i],IPC_NOWAIT);
      caballos[i] = caballos[i] + mensaje.tirada;
      if(i == 0){
        max = i;
        min = i;
      }else{
        if(caballos[max] < caballos[i]){
          max = i;
        }
        if(caballos[i] < caballos[min]){
          min = i;
        }
      }
    }
    /* Calculamos como tiene que ser la siguiente tirada */
    for(i = 0; i < num_caballos;i++){
      if(caballos[i] >= max_distancia){
        flag = 1;
      }
      if(i == max){
        siguiente_tirada[i] = GANADORA;
      }else if(i == min){
        siguiente_tirada[i] = REMONTADORA;
      }else{
        siguiente_tirada[i] = NORMAL;
      }
    }
  }
  /* Mandamos la señal de acabado a los caballos */
  for (i = 0; i < num_caballos;i ++){
    kill(pid[i],SIGUSR2);
  }
  /* Eliminamos la cola de mensajes */
  msgctl (msqid, IPC_RMID, (struct msqid_ds *)NULL);
  for(i = 0;i < num_caballos;i++){
    printf("Caballo %d: %d\n",i,caballos[i]);
  }
  exit(EXIT_SUCCESS);
}
