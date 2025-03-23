#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int N = 4;

sem_t EMPTY;
sem_t FULL;
sem_t LOCK;

typedef struct Receita {
    char nomePaciente[100];
    unsigned int idMedicamento;
    unsigned int qtd;
} Receita;

void produtor(FILE *arquivo) {
    while (true) {
        // read data from file

        // sem_wait(EMPTY)
        // sem_wait(LOCK)
        // put data writepos
        // sem_post(LOCK)
        // sem_post(FULl)
    }
}

void consumidor() {
    while (true) {
        // sem_wait(FULL)
        // read data readpos
        // sem_post(EMPTY)

        // process data
    }
}

int main() {
    pthread_t consumidor1, consumidor2, consumidor3, consumidor4, produtor;

    FILE *arquivo1; // todo fopen
    FILE *arquivo2; // todo fopen
    FILE *arquivo3; // todo fopen
    FILE *arquivo4; // todo fopen

    sem_init(&EMPTY, 0, N);
    sem_init(&FULL, 0, 0);
    sem_init(&LOCK, 0, 1);

    pthread_create(&consumidor1, NULL, consumidor, arquivo1);
    pthread_create(&consumidor2, NULL, consumidor, arquivo2);
    pthread_create(&consumidor3, NULL, consumidor, arquivo3);
    pthread_create(&consumidor4, NULL, consumidor, arquivo4);

    pthread_join(consumidor1, NULL);
    pthread_join(consumidor2, NULL);
    pthread_join(consumidor3, NULL);
    pthread_join(consumidor4, NULL);

    sem_destroy(&EMPTY);
    sem_destroy(&FULL);
    sem_destroy(&LOCK);

    return 0;
}