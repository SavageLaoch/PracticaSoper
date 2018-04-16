/**
 * @brief Ejercicio 4
 *
 * @file ejercicio4.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

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
 * @brief crear_fichero
 *
 * Esta funcion crea el fichero requerido
 * 
 * @param item Indica el item a consumir
 * @return void*
 */

void * crear_fichero(void* fichero){
	FILE *f;
	int MAX,i;
	int imprimir;
	f = fopen((char*)fichero,"w");
	if (f == NULL){
		printf("Fallo al abrir el fichero al crearlo\n");
		return NULL;
	}
	MAX = aleat_num(1000,2000);
	for (i = 1; i <= MAX; i++){
		imprimir = aleat_num(100,1000);
		if (i == MAX){
			fprintf(f,"%d",imprimir);
		}else{
		fprintf(f,"%d,",imprimir);
		}
	}
	fclose(f);
	return NULL;
}

/**
 * @brief leer_fichero
 *
 * Esta funcion lee del fichero pasado por parametro
 * 
 * @param fichero Fichero desde el que hay que leer
 * @return void*
 */

void * leer_fichero(void* fichero){
	char *buffer;
	struct stat tam;
	int f;
	int i;
	f = open((char*)fichero,O_RDWR);
	if(fstat(f,&tam) == -1){
		close(f);
		printf("Error a la hora de abrir el fichero\n");
		return NULL;
	}
	buffer = (char*)mmap(0,tam.st_size,PROT_READ | PROT_WRITE,MAP_SHARED,f,0);
	if(buffer == MAP_FAILED){
		close(f);
		printf("ERROR en el mmap\n");
		return NULL;
	}
	for(i = 0; i < tam.st_size;i++){
		if(buffer[i] == ','){
			buffer[i] = ' ';
		}
		printf("%c",buffer[i]);
	}
	munmap(0,tam.st_size);
	close(f);
	return NULL;
}

/**
 * @brief Main
 *
 * En este main se crean 2 hilos que crean y leen de un fichero.
 * 
 * @return 0
 */

int main(int argc, char *argv[]){
	pthread_t p1,p2;
	pthread_create(&p1,NULL,crear_fichero,(void*)"fichero.txt");
	pthread_join(p1,NULL);
	pthread_create(&p2,NULL,leer_fichero,(void*)"fichero.txt");
	pthread_join(p2,NULL);
	return 0;
}
