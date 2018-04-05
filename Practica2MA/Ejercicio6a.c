/**
 * @brief Ejercicio 6a
 *
 * @file Ejercicio6a.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#define NUM_PROC 5
#define SECS 40

/**
 * @brief Main
 *
 * En este main se crea un proceso hijo y este hijo establece una alarma. Cuando
 * se recibe la senal de esa alarma el proceso hijo termina y con ello el padre,
 * que le estaba esperando.
 *
 * @return 
 */

int main (void)
{
	int pid, counter, error;
	sigset_t set1, set2, oset;

	sigemptyset(&set1);
	sigemptyset(&set2);

	pid = fork();
	if (pid == 0){
		/*Alarma en 40 segundos*/
		if (alarm(SECS)){
			printf("Existe una alarma previa establecida\n");
		}		
		sigaddset(&set1, SIGUSR1);
		sigaddset(&set1, SIGUSR2);
		sigaddset(&set1, SIGALRM);

		sigaddset(&set2, SIGUSR1);
		sigaddset(&set2, SIGALRM);
		while(1){
			/*Bloquear sigusr1, sigusr2 y sigalarm*/			
			error = sigprocmask(SIG_BLOCK, &set1,&oset);
			if (error){
				printf("Error al bloquear las senales");
			}

			for (counter = 0; counter < NUM_PROC; counter++){
				printf("%d\n", counter);
				sleep(1);
			}
			/*Desbloquear sigalarm y sigusr1*/			
			error = sigprocmask(SIG_UNBLOCK, &set2,&oset);			
			if (error){
				printf("Error al desbloquear las senales");
			}
			sleep(3);
		}
	}
	while(wait(NULL)>0);
}




