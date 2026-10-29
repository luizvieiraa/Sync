// SPDX-License-Identifier: GPL-3.0
#include "sync_car.h"

/* Inicializa uma estacao sem passageiros e sem assentos disponiveis. */
void station_init(struct station *station)
{
    pthread_mutex_init(&station->mutex, NULL);
    pthread_cond_init(&station->carro_disponivel, NULL);
    pthread_cond_init(&station->embarque_concluido, NULL);

    station->passageiros_esperando = 0;
    station->assentos_disponiveis = 0;
    station->passageiros_embarcando = 0;
}
