#include <stdio.h>
#include <stdlib.h>
/*#include	<sys/types.h>
#include	<sys/wait.h>*/
#include <unistd.h>

typedef struct _Estructura{
  char cadena[80];
  int numero;
}Estructura;

int main(void){
    Estructura *e;
    int pid;
    e = (Estructura*)malloc(sizeof(Estructura));
    if (e == NULL) return;
    pid = fork();
    if (pid == 0){
      printf("Introduzca una cadena de caracteres\n");
      fscanf(stdin,"%s",e->cadena);
      printf("Introduzca un numero\n");
      fscanf(stdin,"%d",&e->numero);
      printf("Imprimir en el proceso hijo: %s,%d\n",e->cadena,e->numero );
    }else{
      wait();
    }
    free(e);
}
