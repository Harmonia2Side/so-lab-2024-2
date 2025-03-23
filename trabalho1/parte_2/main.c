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

Receita tabela[4];

int writePos = 0;
int readPos = 0;

void produtor(FILE *arquivo) {
  while (true) {
    // read data from file
    Receita r; // TODO - read

    sem_wait(&EMPTY);
    sem_wait(&LOCK);

    tabela[writePos] = r;
    writePos = (writePos + 1) % N;

    sem_post(&LOCK);
    sem_post(&FULL);
  }
}

void consumidor() {
  while (true) {
    sem_wait(&FULL);

    Receita r = tabela[readPos];
    readPos = (readPos + 1) % N;

    sem_post(&EMPTY);

    // process data
  }
}

int main() {
  pthread_t consumidor1, consumidor2, consumidor3, consumidor4, produtor;

  FILE *arquivo1;
  FILE *arquivo2;
  FILE *arquivo3;
  FILE *arquivo4;

  arquivo1 = fopen("arquivos_teste/arquivo1.txt", "r");
  arquivo2 = fopen("arquivos_teste/arquivo2.txt", "r");
  arquivo3 = fopen("arquivos_teste/arquivo3.txt", "r");
  arquivo4 = fopen("arquivos_teste/arquivo4.txt", "r");

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

  fclose(arquivo1);
  fclose(arquivo2);
  fclose(arquivo3);
  fclose(arquivo4);

  return 0;
}