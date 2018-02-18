/**
 * @brief Ejercicio 8 de la practica
 * 
 * @file Ejercicio8.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

/**
 * @brief Funcion ejecutar
 *
 * Esta funcion ejecuta el comando con la función que se han pasado por los parametros
 * @param exe Comando a ejecutar
 * @param mode Modo de ejecucion
 * @return EXIT_SUCCES o EXIT_FAILURE
 */

void ejecutar(char* exe, char* mode){
  char *prog[] = {exe,NULL};
  char path[100] = "/bin/"; 
  char path2[100] = "/usr/bin/";
  if (exe == NULL || mode == NULL){
    printf("Algun parametro es nulo\n");
    exit(EXIT_FAILURE);
  }
  if (!strcmp(mode,"-l")){
    /**
     * Dado que no sabemos que path vamos a necesitar hemos procedido de la siguiente manera:
     * Tras leer la documentacion descubrimos que estas funciones, si no hacen nada, devuelven
     * un parametro y si funciona correctamente acaba con la ejecucion del proceso que lo lleve a cabo.
     * Por tanto, si falla el primero el segundo sera el correcto mientras que si el primero es el
     * correcto acabara la funcion. Sucede lo mismo con execv
     */ 
    execl(strcat(path,exe),exe,NULL);
    execl(strcat(path2,exe),exe,NULL);
  }else if (!strcmp(mode,"-lp")){
    /**
     * Sin embargo, estas funciones buscan el path automaticamente y con una sola ejecucion
     * es suficiente. Sucede lo mismo con execvp
     */
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
  exit(EXIT_SUCCESS);
}

/**
 * @brief Main del programa
 * 
 * Generamos un proceso padre que a la vez invoca tantos procesos hijos como comandos se pasen.
 * Estos procesos hijos ejecutan cada uno de los comandos con la funcion indicada con el parametro
 * de entrada.
 * @param argc Numero de parametros de entrada incluyendo el nombre del programa
 * @param argv Paramtreos de entrada
 * @return EXIT_SUCCESS o EXIT_FAILURE
 */

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
