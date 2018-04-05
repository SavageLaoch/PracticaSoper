/**
 * @brief semaforos.c nuestra propia libreria de semaforos
 *
 * @file semaforos.c
 * @author Miguel Angel Sanchez y Juan Velasco
 */
#include "semaforos.h"

union semun {
		int val;
		struct semid_ds *semstat;
		unsigned short *array;
	} arg;

int n_sems;

/**
 * Inicializa el semaforo.
 * @brief Funcion Inicializar_Semaforo.
 * @param semid: Id del semaforo.
 * @param *array: Valores iniciales.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */

int Inicializar_Semaforo(int semid, unsigned short *array){
	arg.array = array;
	
	if (semctl (semid, 0, SETALL, arg)==-1) return ERROR;
	return OK;
}

/**
 * Borra un semaforo.
 * @brief Borrar_Semaforo.
 * @param semid: Id del semaforo.
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */

int Borrar_Semaforo(int semid){
	if (semctl (semid, 0, IPC_RMID, 0)==-1) return ERROR;
	return OK;
}

/**
 * Crea un semaforo con la clave y el tamaño especificado. 
 * @brief Crear_Semaforo.
 * @param key: Clave del semaforo.
 * @param size: Tamaño del semaforo.
 * @return *semid: Id del semaforo.
 * @return int: ERROR en caso de error, 0 si ha creado el semaforo,  1 si ya estaba creado.       
 */

int Crear_Semaforo(key_t key, int size, int *semid){  
    if( !semid || size < 1){
        return ERROR;
    }
    *semid=semget(key, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);

    /* Si el semaforo ya esta creado */
    if(*semid == ERROR && errno == EEXIST){
        *semid=semget(key, size,  SHM_R | SHM_W);
        return 1;
    }
    /* Si ocurre un error */
    if(*semid == ERROR){
        return ERROR;
    }

    return 0;
}

/**	
 * Baja el semaforo 
 * @brief Down_Semaforo
 * @param semid: Id del semaforo.
 * @param num_sem: Numero de semaforo dentro del array.
 * @param undo: Flag necesaria
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */

int Down_Semaforo(int id, int num_sem, int undo){
	struct sembuf sem_oper;
	sem_oper.sem_num = num_sem; /* Actuamos sobre el semáforo num_sem de la lista */
	sem_oper.sem_op =-1; /* Decrementar en 1 el valor del semáforo */
	sem_oper.sem_flg = undo; /* Para evitar interbloqueos si un
	proceso acaba inesperadamente */
	if (semop (id, &sem_oper, 1)==-1) return ERROR;
	return OK;

}

/**
 * Baja todos los semaforos que indica el array active.
 * @brief DownMultiple_Semaforo
 * @param semid: Id del semaforo.
 * @param size: Numero de semaforos del array.
 * @param undo: Flag necesaria
 * @param *active: Semaforos que se bajan
 *  @return int: OK si todo fue correcto, ERROR en caso de error.
 */

int DownMultiple_Semaforo(int id,int size,int undo,int *active){
	int N_SEMAFOROS;
	struct sembuf sem_oper;
	int i;
	N_SEMAFOROS=sizeof(active)/sizeof(int);
	for (i=0;i<N_SEMAFOROS;i++){
		sem_oper.sem_num = active[i]; /* Actuamos sobre el semáforo num_sem de la lista */
		sem_oper.sem_op =-1;  /* Decrementar en 1 el valor del semáforo */
		sem_oper.sem_flg = undo; /* Para evitar interbloqueos si un
		proceso acaba inesperadamente */
		if (semop (id, &sem_oper, 1)==-1) return ERROR;
		}
	
	return OK;
}

/**
 * Sube el semaforo indicado
 * @brief Up_Semaforo
 * @param semid: Id del semaforo.
 * @param num_sem: Numero de semaforo dentro del array.
 * @param undo: Flag necesaria
 * @return int: OK si todo fue correcto, ERROR en caso de error.
 */

int Up_Semaforo(int id, int num_sem, int undo){
	struct sembuf sem_oper;
	sem_oper.sem_num = num_sem; /* Actuamos sobre el semáforo num_sem de la lista */
	sem_oper.sem_op =1; /* Incrementar en 1 el valor del semáforo */
	sem_oper.sem_flg = undo; /* Para evitar interbloqueos si un
	proceso acaba inesperadamente */
	if (semop (id, &sem_oper, 1)==-1) return ERROR;
	return OK;

}

/**
 * Sube todos los semaforos que indica el array active.
 * @brief UpMultiple_Semaforo
 * @param semid: Id del semaforo.
 * @param size: Numero de semaforos del array.
 * @param undo: Flag necesaria
 * @param *active: Semaforos que se bajan
 *  @return int: OK si todo fue correcto, ERROR en caso de error.
 */

int UpMultiple_Semaforo(int id,int size, int undo, int *active){
	int N_SEMAFOROS;
	struct sembuf sem_oper;
	int i;
	N_SEMAFOROS=sizeof(active)/sizeof(int);
	for (i=0;i<N_SEMAFOROS;i++){
		sem_oper.sem_num = active[i]; /* Actuamos sobre el semáforo num_sem de la lista */
		sem_oper.sem_op =1;  /* Decrementar en 1 el valor del semáforo */
		sem_oper.sem_flg = undo; /* Para evitar interbloqueos si un
		proceso acaba inesperadamente */
		if (semop (id, &sem_oper, 1)==-1) return ERROR;
		}
	
	return OK;
}
