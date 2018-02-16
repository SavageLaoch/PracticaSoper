#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

void ejecutar(char* exe, char* mode){
  char *prog[] = {exe,"NULL"};
  if (exe == NULL || mode == NULL){
    return;
  }
  if (strcmp(mode,"-l")){
    execl("/usr/bin",exe,exe,NULL);
  }else if (strcmp(mode,"-lp")){
    
  }
}

int main(int argc, char *argv[]){
  int i;
  int pid;
  if (argc < 3){
    printf("No se han introducido los parámetros suficientes\n");
    exit(EXIT_FAILURE);
  }
  for (i = 1; i <= argc; i++){
    if ((pid = fork()) == 0){

    }
  }
}
