#ifndef CABALLOS_H
#define CABALLOS_H
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


#define NORMAL 0
#define REMONTADORA 1
#define GANADORA 2
#define PRIMERO 3
#define ULTIMO 4
#define MAXBUFFER 100
#define KEY 1300
#define FILEKEY "/bin/cat"

int dado(int modo);

int avanzar_caballo(int posicion);

void caballo(int pipe[2],int pid);

#endif
