/*
 * main.exe
 * uso:
 * main.exe <diretório contendo arquivos de teste>
 * Cada execução do programa entrará apenas em um
 * diretório e lerá apenas arquivos dentro do diretório
 * adentrado.
 * Para execução de múltiplos testes em múltiplos diretórios,
 * por favor execute o programa múltiplas vezes.
 */

#include <dirent.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

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

int main(int argc, char *argv[]) {

  //   pthread_t consumidor1, consumidor2, consumidor3, consumidor4, produtor;

  //   FILE *arquivo1; // TODO: fopen
  //   FILE *arquivo2; // TODO: fopen
  //   FILE *arquivo3; // TODO: fopen
  //   FILE *arquivo4; // TODO: fopen

  // arquivo1 = fopen("arquivos_teste/arquivo1.txt", "r");
  // arquivo2 = fopen("arquivos_teste/arquivo2.txt", "r");
  // arquivo3 = fopen("arquivos_teste/arquivo3.txt", "r");
  // arquivo4 = fopen("arquivos_teste/arquivo4.txt", "r");

  // Leitura de parâmetros de entrada
  if (argc != 2) {
    printf("Uso: %s <diretório contendo arquivos de teste>\n", argv[0]);
    printf("Favor consultar README.md para mais informações");
    return EXIT_FAILURE;
  }

  // Entra no diretório
  const char *dirPath = argv[1];
  DIR *dir = opendir(dirPath);
  // Tratamento de erro
  if (!dir) {
    perror("opendir");
    return EXIT_FAILURE;
  }

  // Lê todos os arquivos dentro do diretório e coloca os descritores de cada um
  // num elemento do vetor
  int file_counter = 0;
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    struct stat stats;
    char filePath[512];

    snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, entry->d_name);
    if (stat(filePath, &stats) == -1) {
      perror("stat");
      continue;
    }

    if (S_ISREG(stats.st_mode)) {
      FILE *file = fopen(filePath, "r");
      if (!file) {
        perror("fopen");
        continue;
      }

      // Process the file here
      printf("Processing file: %s\n", filePath);

      fclose(file);
    }
  }

  closedir(dir);

  // Inicializa semáforos
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

  // fclose(arquivo1);
  // fclose(arquivo2);
  // fclose(arquivo3);
  // fclose(arquivo4);

  return 0;
}