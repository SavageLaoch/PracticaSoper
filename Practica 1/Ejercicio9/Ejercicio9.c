#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_PROC 5
int main (void)
{
  int pid;
  int i,a,b;
  int status;
  for (i=0; i <= NUM_PROC; i++){
    if ((pid=fork()) <0 ){
      printf("Error al emplear fork\n");
      exit(EXIT_FAILURE);
    }else if (pid == 0){
      printf("ESTAMOS EN EL HIJO");
      siwtch(i){
        case(0):

          break;
        case(1):

          break;
        case(2):

          break;
        case(3):

          break;
      }
    }else{
      printf("ESTAMOS EN EL PADRE");
      printf("Introduce los 2 numeros:\n");
      scanf("%d %d",&a,&b);
      waitpid(pid,&status,0);
    }
  }
  wait(&status);
  exit(EXIT_SUCCESS);
}
