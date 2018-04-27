#include "carrera.h"


void carrera(int numCaballos){
  int **pipes;

  /* Creamos las pipes */
  pipes = (int**) malloc(sizeof(int*)*numCaballos);
  if(pipes==NULL){
    printf("Error al reservar memoria");
    exit(EXIT_FAILURE);
  }
  for (i=0;i<numCaballos;i++){
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

  

}
