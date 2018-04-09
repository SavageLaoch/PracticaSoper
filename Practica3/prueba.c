#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/shm.h> /* shm* */
#include "semaforos.h"
#define FILEKEY "/bin/cat"
#define SEMKEY 75788
#define KEY 1300
#define MAXBUF 30

void main(){
  char c = 48;
  printf("%c\n",c );
}
