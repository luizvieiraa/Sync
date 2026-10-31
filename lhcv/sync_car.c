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

/* Aguarda uma vaga e a reserva antes de permitir o embarque. */
void station_wait_for_car(struct station *station)
{
    pthread_mutex_lock(&station->mutex);
    station->passageiros_esperando++;

    while (station->assentos_disponiveis == 0) {
        pthread_cond_wait(&station->carro_disponivel, &station->mutex);
    }

    station->assentos_disponiveis--;
    station->passageiros_esperando--;
    station->passageiros_embarcando++;
    pthread_mutex_unlock(&station->mutex);
}

/* Confirma o embarque e avisa quando o carro pode encerrar a espera. */
void station_on_board(struct station *station)
{
    pthread_mutex_lock(&station->mutex);
    station->passageiros_embarcando--;

    if (station->passageiros_embarcando == 0 &&
        (station->assentos_disponiveis == 0 ||
         station->passageiros_esperando == 0)) {
        pthread_cond_signal(&station->embarque_concluido);
    }

    pthread_mutex_unlock(&station->mutex);
}

/* Disponibiliza vagas e aguarda a conclusao dos embarques autorizados. */
void station_load_car(struct station *station, int count)
{
    pthread_mutex_lock(&station->mutex);
    station->assentos_disponiveis = count;
    pthread_cond_broadcast(&station->carro_disponivel);

    while (station->passageiros_embarcando > 0 ||
           (station->assentos_disponiveis > 0 &&
            station->passageiros_esperando > 0)) {
        pthread_cond_wait(&station->embarque_concluido, &station->mutex);
    }

    station->assentos_disponiveis = 0;
    pthread_mutex_unlock(&station->mutex);
}
