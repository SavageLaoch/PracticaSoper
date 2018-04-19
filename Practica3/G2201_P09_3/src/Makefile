all: Ejercicio2 Ejercicio2_solved Ejercicio3 Ejercicio4 Ejercicio5


Ejercicio2: ejercicio2.c
	gcc -Wall -g ejercicio2.c -o Ejercicio2
Ejercicio2_solved: ejercicio2_solved.c
	gcc -Wall -g semaforos.h semaforos.c ejercicio2_solved.c -o Ejercicio2_solved
Ejercicio3: ejercicio3.c
	gcc -Wall -g semaforos.h semaforos.c ejercicio3.c -o Ejercicio3
Ejercicio4: ejercicio4.c
	gcc -Wall -g -pthread ejercicio4.c -lpthread -o Ejercicio4
Ejercicio5: ejercicio5.c
	gcc -Wall -g ejercicio5.c -o Ejercicio5


clean:
	rm Ejercicio2 Ejercicio2_solved Ejercicio3 Ejercicio4 Ejercicio5 fichero.txt
