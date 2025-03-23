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

FILE **files;

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

int main(int argc, char *argv[]) {

  //   pthread_t consumidor1, consumidor2, consumidor3, consumidor4, produtor;

  // Leitura de parâmetros de entrada
  if (argc != 2) {
    printf("Uso: %s <diretório contendo arquivos de teste>\n", argv[0]);
    printf("Favor consultar README.md para mais informações\n");
    return EXIT_FAILURE;
  }

  // Entra no diretório
  const char *dirPath = argv[1];
  printf("Abrindo diretório \"%s\"\n", dirPath);
  DIR *dir = opendir(dirPath);
  // Tratamento de erro
  if (!dir) {
    perror("opendir");
    return EXIT_FAILURE;
  }

  // Lê todos os arquivos dentro do diretório e coloca os descritores de cada um
  // num elemento do vetor

  // aloca vetor de descritores de arquivo
  files = (FILE **)malloc(N * sizeof(FILE *));

  for (int i = 0; i < N; i++) {
    files[i] = (FILE *)malloc(sizeof(FILE));
  }

  // Leitura de arquivo um por um
  struct dirent *entry;
  int file_counter = 0;
  while ((entry = readdir(dir)) != NULL && (file_counter < N)) {
    // printf("Abrindo arquivo \"%s\"\n", entry->d_name);
    // printf("file_counter: %d\n", file_counter);
    struct stat stats;
    char filePath[512];

    // monta o caminho do arquivo baseado no diretório
    snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, entry->d_name);
    if (stat(filePath, &stats) == -1) {
      perror("stat");
      continue;
    }

    // Abre o arquivo e o coloca no vetor de descritores de arquivo
    if (S_ISREG(stats.st_mode)) {
      files[file_counter] = fopen(filePath, "r");
      if (!files[file_counter]) {
        perror("fopen");
        continue;
      }

      // log
      printf("Arquivo aberto: %s\n", filePath);
      file_counter++;
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

  for (int i = 0; i < N; i++) {
    fclose(files[i]);
  }

  return 0;
}