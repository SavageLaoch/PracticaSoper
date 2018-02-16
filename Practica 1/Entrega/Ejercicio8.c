#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

void ejecutar(char* exe, char* mode){
  char *prog[] = {exe,NULL};
  char path[100] = "/bin/";
  char path2[100] = "/usr/bin/";
  if (exe == NULL || mode == NULL){
    return;
  }
  if (!strcmp(mode,"-l")){
    execl(strcat(path,exe),exe,NULL);
    execl(strcat(path2,exe),exe,NULL);
  }else if (!strcmp(mode,"-lp")){
    execlp(exe,exe,NULL);
  }else if (!strcmp(mode,"-v")){
    execv(strcat(path,exe),prog);
    execv(strcat(path2,exe),prog);
  }else if (!strcmp(mode,"-vp")){
    execvp(exe,prog);
  }else{
    printf("No se ha pasado un paramtreo correcto\n");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]){
  int i;
  int pid;
  int status;
  if (argc < 3){
    printf("No se han introducido los parámetros suficientes\n");
    exit(EXIT_FAILURE);
  }
  for (i = 1; i < argc; i++){
    if ((pid = fork()) == 0){
        ejecutar(argv[i],argv[argc - 1]);
        exit(EXIT_SUCCESS);
    }else{
      wait(&status);
    }
  }
  exit(EXIT_SUCCESS);
}
