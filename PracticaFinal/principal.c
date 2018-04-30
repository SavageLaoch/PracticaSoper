#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <stdlib.h>
#include "carrera.h"
#include "caballos.h"
int main(int argc, char *argv[]) {
  carrera(1,10);
  exit(EXIT_SUCCESS);
}
