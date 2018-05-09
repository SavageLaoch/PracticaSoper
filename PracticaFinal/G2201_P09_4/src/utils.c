#include "utils.h"
/**
 * @brief Utils
 *
 * @file utils.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

/**
 * @brief aleat_num
 *
 * Genera un numero aleatorio entre inf y sup
 *
 * @param inf Cota inferior
 * @param sup Cota superior
 * @return int Entero generado
 */
int aleat_num(int inf, int sup){
	if (inf>sup) return -1;
	int randm,rango,grupo,limite;
	rango=sup-inf+1;
	grupo=RAND_MAX/rango;
	limite=grupo*rango;
	do{
		randm=rand();
	}while(randm>=limite);
	return (randm/grupo)+inf;
}

/**
 * @brief terminar
 *
 * Manejador que termina el proceso
 *
 */
void terminar(int sig){
	exit(EXIT_SUCCESS);
}

/**
 * @brief retorno
 *
 * Manejador que saca al proceso de un pause()
 *
 */
void retorno(int sig){
	return;
}