/**
 *
 * Descripcion: Implementacion de funciones de generacion de permutaciones
 *
 * Fichero: permutaciones.c
 * Autor: Miguel Angel Sanchez y Juan Velasco
 * Version: 1.0
 * Fecha: 11-10-2017
 *
 */


#include "permutaciones.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef ERR
  #define ERR -1
  #define OK (!(ERR))
#endif
/***************************************************/
/* Funcion: aleat_num Fecha: 11-10-2017            */
/* Autores: Miguel Angel Sanchez y Juan Velasco    */
/*                                                 */
/* Rutina que genera un numero aleatorio           */
/* entre dos numeros dados                         */
/*                                                 */
/* Entrada:                                        */
/* int inf: limite inferior                        */
/* int sup: limite superior                        */
/* Salida:                                         */
/* int: numero aleatorio                           */
/***************************************************/
int aleat_num(int inf, int sup)
{
  if (inf>sup) return ERR;
  int randm,rango,grupo,limite;
  rango=sup-inf+1;
  grupo=RAND_MAX/rango;
  limite=grupo*rango;
  do{
    randm=rand();
  }while(randm>=limite);
  return (randm/grupo)+inf;
}

/***************************************************/
/* Funcion: genera_perm Fecha: 11-10-2017          */
/* Autores: Miguel Angel Sanchez y Juan Velasco    */
/*                                                 */
/* Rutina que genera una permutacion               */
/* aleatoria                                       */
/*                                                 */
/* Entrada:                                        */
/* int n: Numero de elementos de la                */
/* permutacion                                     */
/* Salida:                                         */
/* int *: puntero a un array de enteros            */
/* que contiene a la permutacion                   */
/* o NULL en caso de error                         */
/***************************************************/
int* genera_perm(int N){
  int* perm;
  int i,temp,ran;
  if (N<=0) return NULL;
  perm=(int*)malloc(sizeof(int)*N);
  if (perm==NULL) return NULL;
  for (i=0;i<N;i++) {
    perm[i]=i;
  }
  for (i=0;i<N;i++){
    ran=aleat_num(i,N-1);
    temp=perm[i];
    perm[i]=perm[ran];
    perm[ran]=temp;
  }
  return perm;
}

/***************************************************/
/* Funcion: genera_permutaciones Fecha: 11-10-2017 */
/* Autores: Miguel Angel Sanchez y Juan Velasco    */
/*                                                 */
/* Funcion que genera n_perms permutaciones        */
/* aleatorias de tamanio elementos                 */
/*                                                 */
/* Entrada:                                        */
/* int n_perms: Numero de permutaciones            */
/* int N: Numero de elementos de cada              */
/* permutacion                                     */
/* Salida:                                         */
/* int**: Array de punteros a enteros              */
/* que apuntan a cada una de las                   */
/* permutaciones                                   */
/* NULL en caso de error                           */
/***************************************************/
int** genera_permutaciones(int n_perms, int N){
  int **perms;
  int i,j;
  if (n_perms<=0||N<=0) return NULL;
  perms=(int**)malloc(sizeof(int*)*n_perms);
  if (perms==NULL) return NULL;
  for (i=0;i<n_perms;i++){
    perms[i]=genera_perm(N);
    if (perms[i]==NULL){
      for(j=1;j<i;j++){
        free(perms[j]);
      }
      free(perms);
      return NULL;
    }
  }
  return perms;
}
