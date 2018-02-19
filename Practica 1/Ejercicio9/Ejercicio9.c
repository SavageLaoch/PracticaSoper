#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_PROC 3
int main (void)
{
  int pid;
  int i,a,b;
  int status;
  int fd[2],pipe_status;

  pipe_status=pipe(fd);
  if(pipe_status==-1){
    printf("Error creando la tuberia");
    exit(EXIT_FAILURE);
  }

  for (i=0 ; i <= NUM_PROC; i++){
      if ((pid=fork()) <0 ){
          printf("Error al emplear fork\n");
          exit(EXIT_FAILURE);
      }else if (pid == 0){
          printf("HIJO  %d DEL PADRE %d\n",getpid(),getppid());
          switch (i){
            case(0):
              close(fd[1]);
              read(fd[0],b,sizeof(int));
            case(1):

            case(2):

            case(3):

          }
          break;
      }else{
          printf("Introduzca el operando \n");
          scanf("%d",&a);
          close(fd[0]);
          write(fd[1],a,sizeof(int));
          

          waitpid(pid,&status,0);
      }
  }
	printf("Terminado: %d\n",getpid());
  exit(EXIT_SUCCESS);
}
