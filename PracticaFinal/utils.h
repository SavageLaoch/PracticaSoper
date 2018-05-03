#ifndef UTILS_H
#define UTILS_H

#include "semaforos.h"

#include "carrera.h"
#include "caballos.h"
#include "monitor.h"
#include "apostador.h"
#include "gestor_apuestas.h"

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
#include <sys/wait.h>
#include <pthread.h>

#define NORMAL 0
#define REMONTADORA 1
#define GANADORA 2
#define PRIMERO 3
#define ULTIMO 4
#define MAXBUFFER 100
#define MAXCHAR 20

#define KEY 1300
#define KEY2 1400
#define KEY3 1500
#define SEMKEY 75798
#define FILEKEY "/bin/cat"

int aleat_num(int inf, int sup);

void terminar(int sig);

void retorno(int sig);

#endif