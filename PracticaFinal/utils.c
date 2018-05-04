#include "utils.h"

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

void terminar(int sig){
	printf("Termino proceso\n");
	exit(EXIT_SUCCESS);
}

void retorno(int sig){
	return;
}