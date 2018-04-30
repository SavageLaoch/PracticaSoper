#ifndef CARRERA_H
#define CARRERA_H
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
#include "caballos.h"
#include <string.h>

#define NORMAL 0
#define REMONTADORA 1
#define GANADORA 2
#define PRIMERO 3
#define ULTIMO 4
#define MAXBUFFER 100
#define KEY 1300
#define FILEKEY "/bin/cat"

void carrera(int num_caballos,int max_distancia);

#endif
