#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#define NUM_PROC 5

void manejador_SIGTERM(int sig){
	printf("Soy %d y he recibido la senal SIGTERM\n",getpid());
	exit(0);
}

int main (void)
{
	int pid, counter;

	if (signal(SIGTERM,manejador_SIGTERM) == SIG_ERR){
		perror("signal");
		exit (EXIT_FAILURE);
	}

	pid = fork();
	if (pid == 0){
		while(1){
			for (counter = 0; counter < NUM_PROC; counter++){
				printf("%d\n", counter);
				sleep(1);
			}
			sleep(3);
		}
	}
	else{
		/*El padre envia la senal SIGTERM al hijo 40 segs despues de crearlo*/
		sleep(40);
		kill(pid,SIGTERM);
	}
	while(wait(NULL)>0);
}




