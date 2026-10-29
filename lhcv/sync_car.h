#ifndef SYNC_CAR_H
#define SYNC_CAR_H

#include <pthread.h>

struct station {
    pthread_mutex_t mutex;
    pthread_cond_t carro_disponivel;
    pthread_cond_t embarque_concluido;
    int passageiros_esperando;
    int assentos_disponiveis;
    int passageiros_embarcando;
};

void station_init(struct station *station);
void station_load_car(struct station *station, int count);
void station_wait_for_car(struct station *station);
void station_on_board(struct station *station);

#endif
