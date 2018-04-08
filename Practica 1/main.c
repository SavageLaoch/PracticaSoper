#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void main(){
  int fichero, lectura, escritura;
  char buffer[256]="Hola Mundo";
  char buffer2[256];
  fichero = open("/home/alumnos/e362984/fichero.txt", O_RDWR);
  lectura = read(fichero,(void*)buffer2,20*sizeof(char));
  escritura = write(fichero,(void*)buffer,10);


  printf("Entero de lectura %d\n", lectura);
  printf("Entero de escritura %d\n", escritura);

  printf("%s\n",buffer2);
  return;
}
