#ifndef MONITOR_H
#define MONITOR_H

void monitor_antes(int num_caballos,int max_distancia,int sem_id,int id_zone);

void monitor_durante(int num_caballos,int max_distancia,int sem_id,int id_zone);

void monitor_despues(int max_dinero,int num_apostadores,int num_caballos,int max_distancia,int sem_id,int id_zone,int id_zone2);

void monitor(int max_dinero,int num_apostadores,int num_caballos,int max_distancia,int sem_id,int id_zone,int id_zone2);

#endif