#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "semaforos.h"

#define SEMKEY 75798
#define NUMCAJAS 3
int sem_id;

int main (int argc, char *argv[]){
	FILE *fp2, *fp;
	int i,t;
	for (i=0; i<5; i++){
		fp2=fopen("turnos.txt","w+");
		rewind(fp2);
		fprintf(fp2, "%d",i);
		fclose(fp2);

		fp=fopen("turnos.txt","r");		
		fscanf(fp, "%d",&t);
		printf("El turno que me han pasado es %d\n",t);
		rewind(fp);
		fclose(fp);		
	}

	return 0;
}