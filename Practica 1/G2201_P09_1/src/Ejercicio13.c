/**
 * @brief Ejercicio 13
 *
 * @file Ejercicio13.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define MAX_CHAR 500
#define MAX_MATRIZ 25

/**
 * @brief Variable global tam
 * Usaremos esta variable para que podamos compartir el tamaño
 * entre los hilos.
 */

int tam;

/**
 * @brief Estructura Matriz
 * Guardaremos en esta estructura todos los datos referentes
 * a la matriz.
 * Nota: hemos decidido guardar la matriz en un array en vez de en una matriz
 * como tal. Esto cambiara un poco la forma de proceder en la funcion de multiplica_matriz
 * @param matriz Array de enteros que contienen la matriz
 * @param multiplicador Numero por el que se multiplicara cada numero de la matriz
 * @param num Numero de la matriz (1 o 2)
 */

typedef struct _Matriz{
  int matriz[MAX_MATRIZ];
  int multiplicador;
  int num;
}Matriz;

/**
 * @brief multiplica_matriz
 * Esta funcion multiplica la matriz que se le pasa
 * por un numero entero
 * @param matriz Puntero a void que contiene un puntero de tipo Matriz
 * @return void*
 */

void * multiplica_matriz(void* matriz){
    Matriz *m;
    int i,fila,j;
    char imprimir[MAX_CHAR];
    /**
     * Cambiamos el puntero al tipo original
     */
    m = (Matriz*) matriz;
    fila = 0;
    /**
     * Dado que es una matriz de tamaño m, tenemos
     * que iterar el array hasta m*m.
     * Lo guardamos en la misma matriz ya que no nos dicen nada de que haya que guardarla.
     */
    for (i = 0;i < tam*tam;i++){
      m->matriz[i] = m->matriz[i] * m->multiplicador;
      /**
       * Cuando alcanzamos el tamaño de fila deseado (que es la iteracion + 1),
       * pasamos a imprimir la fila correspondiente iterando los nuevos valores
       * de la matriz.
       * Tenemos que poner fflush para que pueda imprimirse todo el buffer junto
       * Por ultimo, aumentamos en uno la fila en la que estamos
       */
      if ((i+1)%tam == 0){
        sprintf(imprimir,"Hilo %d multiplicando fila %d resultado", m->num,fila);
        for (j = tam - 1; j > -1; j--){
          sprintf(imprimir,"%s %d",imprimir,m->matriz[i-j]);
        }
        printf("%s\n",imprimir);
        fflush(stdout);
        fila++;
        usleep(100000);
      }
    }
    return NULL;
}

/**
 * @brief Main
 *
 * El main sera el encargado de crear las dos estructuras Matrices,
 * pidiendo por pantalla los datos deseados: tamaño, multiplicador y la matriz
 * Despues, genera los hilos donde se realizaran las operaciones.
 *
 * @return EXIT_SUCCESS o EXIT_FAILURE
 */

int main(void) {
  Matriz *m1;
  Matriz *m2;
  int i;
  pthread_t p1,p2;
  /**
   * Creamos las dos estructuras matriz
   */
  m1 = (Matriz*)malloc(sizeof(Matriz));
  if (m1 == NULL){
    printf("Fallo al crear la estructura 1\n");
    return EXIT_FAILURE;
  }
  m2 = (Matriz*)malloc(sizeof(Matriz));
  if (m2 == NULL){
    printf("Fallo al crear la estructura 2\n");
    free(m1);
    return EXIT_FAILURE;
  }
  /**
   * Introducimos los datos del tamaño y vemos si concuerdan con los que
   * nos pide el enunciado.
   * Hemos admitido tambien una matriz de 1 x 1
   */
  printf("Introduzca dimension de la matriz cuadrada:\n");
  fscanf(stdin,"%d", &tam);
  if (tam > 5 || tam < 1){
    free(m1);
    free(m2);
    printf("No se ha introducida un tamaño de matriz adecuado\n");
    return EXIT_FAILURE;
  }
  /**
   * Introducimos ambos multiplicadores
   */
  printf("Introduzca multiplicador 1:\n");
  fscanf(stdin,"%d",&m1->multiplicador);
  printf("Introduzca multiplicador 2:\n");
  fscanf(stdin,"%d",&m2->multiplicador);
  /**
   * Introducimos ambas matrices
   * Aclarar que se pueden meter como aparecen en el enunciado de la practica,
   * pero si se ponen numeros de mas, no seran multiplicados y pueden dar errores
   * de memoria.
   * Si se introducen presionando intro despues de cada numero solo se dejara poner
   * los numeros correctos.
   * Tenemos que añadir otra vez m*m numeros
   */
  printf("Introduzca matriz 1:\n");
  for (i = 0;i < tam*tam;i++){
    fscanf(stdin,"%d",&(m1->matriz[i]));
  }
  m1->num = 1;
  printf("Introduzca matriz 2:\n");
  for (i = 0;i < tam*tam;i++){
    fscanf(stdin,"%d",&(m2->matriz[i]));
  }
  m2->num = 2;
  /**
   * Por ultimo, creamos los threads pasandole sus matrices correspondientes
   */
  printf("Realizando producto:\n");
  pthread_create(&p1,NULL,multiplica_matriz,(void*) m1);
  pthread_create(&p2,NULL,multiplica_matriz,(void*) m2);
  pthread_join(p1,NULL);
  pthread_join(p2,NULL);
  free(m1);
  free(m2);
  return EXIT_SUCCESS;
}
