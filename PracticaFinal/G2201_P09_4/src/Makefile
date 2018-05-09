all: Proyecto


Proyecto: principal.c
	gcc -Wall -g semaforos.h semaforos.c caballos.h caballos.c carrera.h carrera.c monitor.c monitor.h apostador.c apostador.h gestor_apuestas.c gestor_apuestas.h utils.c utils.h principal.c -o Proyecto -lpthread

clean:
	rm Proyecto Report.txt
