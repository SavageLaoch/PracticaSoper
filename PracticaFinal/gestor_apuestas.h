#ifndef GESTOR_APUESTAS_H
#define GESTOR_APUESTAS_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/msg.h>
#include <string.h>
#include <time.h>

#define KEY 1400
#define FILEKEY "/bin/cat"
#define MAXCHAR 20

void gestor_apuestas(int num_ventanillas,int num_caballos);

#endif
