#include "sync_car.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static struct station estacao;
static pthread_mutex_t controle = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t liberar = PTHREAD_COND_INITIALIZER;
static int pode_sentar;
static int carro_retornou;
static int vagas;

static void *passageiro(void *arg)
{
    (void)arg;
    station_wait_for_car(&estacao);
    pthread_mutex_lock(&controle);
    while (!pode_sentar)
        pthread_cond_wait(&liberar, &controle);
    pthread_mutex_unlock(&controle);
    station_on_board(&estacao);
    return NULL;
}

static void *carro(void *arg)
{
    (void)arg;
    station_load_car(&estacao, vagas);
    pthread_mutex_lock(&controle);
    carro_retornou = 1;
    pthread_mutex_unlock(&controle);
    return NULL;
}

/* Observacao limitada ao teste; a implementacao nao usa polling ou sleeps. */
static void aguardar_estado(int esperando, int embarcando)
{
    const struct timespec pausa = {0, 1000000};
    for (int i = 0; i < 3000; i++) {
        pthread_mutex_lock(&estacao.mutex);
        int atingiu = estacao.passageiros_esperando == esperando &&
                      estacao.passageiros_embarcando == embarcando;
        pthread_mutex_unlock(&estacao.mutex);
        if (atingiu)
            return;
        nanosleep(&pausa, NULL);
    }
    assert(0 && "estado esperado nao foi atingido em tres segundos");
}

static void cenario(int total, int assentos)
{
    pthread_t passageiros[8], trem;
    station_init(&estacao);
    pode_sentar = 0;
    carro_retornou = 0;
    vagas = assentos;
    station_load_car(&estacao, 0);
    station_load_car(&estacao, 10);
    for (int i = 0; i < total; i++)
        assert(pthread_create(&passageiros[i], NULL, passageiro, NULL) == 0);
    aguardar_estado(total, 0);
    station_load_car(&estacao, 0);
    aguardar_estado(total, 0);
    assert(pthread_create(&trem, NULL, carro, NULL) == 0);
    int autorizados = total < assentos ? total : assentos;
    aguardar_estado(total - autorizados, autorizados);
    pthread_mutex_lock(&controle);
    assert(!carro_retornou);
    pode_sentar = 1;
    pthread_cond_broadcast(&liberar);
    pthread_mutex_unlock(&controle);
    assert(pthread_join(trem, NULL) == 0);
    /* Passageiros excedentes precisam aguardar o proximo carro. */
    if (total > assentos)
        station_load_car(&estacao, total - assentos);
    for (int i = 0; i < total; i++)
        assert(pthread_join(passageiros[i], NULL) == 0);
    aguardar_estado(0, 0);
    assert(estacao.assentos_disponiveis == 0);
    assert(pthread_cond_destroy(&estacao.carro_disponivel) == 0);
    assert(pthread_cond_destroy(&estacao.embarque_concluido) == 0);
    assert(pthread_mutex_destroy(&estacao.mutex) == 0);
    printf("OK: %d passageiros, %d vagas; confirmacoes retidas ate reservar todas as vagas\n",
           total, assentos);
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "negativo") == 0) {
        station_init(&estacao);
        station_load_car(&estacao, -1);
        return 0;
    }
    cenario(1, 1);
    cenario(4, 4);
    cenario(3, 8);
    cenario(8, 3);
    puts("Todos os testes adicionais passaram.");
    return 0;
}
