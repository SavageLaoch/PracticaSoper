/**
 * @brief Ejercicio 6
 *
 * @file Ejercicio6.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @brief Estructura
 *
 * @param cadena Una cadena de 80 caracteres
 * @param numero Un enterp
 */

typedef struct _Estructura{
  char cadena[80];
  int numero;
}Estructura;

/**
 * @brief Main
 *
 * En este ejercicio crearemos en el proceso padre memoria para un puntero a nuestra Estructura
 * y veremos que se crea una copia para el hijo donde pediremos los parametros por teclado.
 * Tambien veremos que no se peude acceder desde el apdre a esta informacion
 * @return EXIT_SUCCESS
 */

int main(void){
    Estructura *e;
    int pid;
    int status;
    e = (Estructura*)malloc(sizeof(Estructura));
    if (e == NULL){
      exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid == -1){
      exit(EXIT_FAILURE);
    }
    if (pid <= 0){
      printf("Introduzca una cadena de caracteres\n");
      fscanf(stdin,"%s",e->cadena);
      printf("Introduzca un numero\n");
      fscanf(stdin,"%d",&e->numero);
      printf("Imprimir en el proceso hijo: %s,%d\n",e->cadena,e->numero );
    }else{
      wait(&status);
    }
    free(e);
    exit(EXIT_SUCCESS);
}
