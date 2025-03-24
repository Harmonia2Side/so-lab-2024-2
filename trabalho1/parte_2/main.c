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
#include <string.h>
#include <sys/stat.h>
// #include <unistd.h>

#define FILE_IS_NOT_OVER 0
#define FILE_IS_OVER 1

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

int readReceita(Receita *r, FILE *arquivo) {

  fscanf(arquivo, "%s", r->nomePaciente);

  fscanf(arquivo, "%d", &r->idMedicamento);

  if (fscanf(arquivo, "%d", &r->qtd) == EOF) {
    return 1;
  }

  return 0;
}

void printReceita(Receita *r) {
  printf("\n");
  printf("Nome do paciente: %s\n", r->nomePaciente);
  printf("ID do medicamento: %d\n", r->idMedicamento);
  printf("Quantidade: %d\n", r->qtd);
}

Receita **tabela;

int writePos = 0;
int readPos = 0;

int finished = 0;

void *produtor(void *ptr) {
  FILE *arquivo = (FILE *)ptr;
  while (true) {
    Receita *r;
    r = (Receita *)malloc(sizeof(Receita));
    // Se arquivo terminou, excerra thread
    if (readReceita(r, arquivo) == FILE_IS_OVER) {
      sem_wait(&LOCK);
      finished++;
      sem_post(&LOCK);
      break;
    }

    sem_wait(&EMPTY);
    sem_wait(&LOCK);

    tabela[writePos] = r;
    writePos = (writePos + 1) % N;

    sem_post(&LOCK);
    sem_post(&FULL);
  }

  return NULL;
}

void *consumidor() {
  while (true) {
    if (finished == N) {

      break;
    }

    sem_wait(&FULL);
    // processamento: Imprime a receita em tela
    Receita *r = tabela[readPos];
    printReceita(r);
    // Desaloca a estrutura de receita
    free(r);
    // incrementa buffer circular
    readPos = (readPos + 1) % N;

    sem_post(&EMPTY);
  }
  return NULL;
}

int main(int argc, char *argv[]) {

  pthread_t produtor1, produtor2, produtor3, produtor4;
  pthread_t consumidor1;

  // Aloca tabela partilhada
  tabela = (Receita **)malloc(N * sizeof(Receita));

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

  // aloca vetor de descritores de arquivo
  files = (FILE **)malloc(N * sizeof(FILE *));

  for (int i = 0; i < N; i++) {
    files[i] = (FILE *)malloc(sizeof(FILE));
  }

  // Leitura de arquivo um por um
  struct dirent *entry;
  int file_counter = 0;
  while ((entry = readdir(dir)) != NULL && (file_counter < N)) {
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

  // cria threads
  pthread_create(&produtor1, NULL, produtor, (void *)files[0]);
  pthread_create(&produtor2, NULL, produtor, (void *)files[1]);
  pthread_create(&produtor3, NULL, produtor, (void *)files[2]);
  pthread_create(&produtor4, NULL, produtor, (void *)files[3]);
  pthread_create(&consumidor1, NULL, consumidor, NULL);

  pthread_join(produtor1, NULL);
  pthread_join(produtor2, NULL);
  pthread_join(produtor3, NULL);
  pthread_join(produtor4, NULL);
  pthread_join(consumidor1, NULL);

  // while (finished < N) {
  //   printf("finished = %d\n", finished);
  //   // sleep(1);
  // }

  sem_destroy(&EMPTY);
  sem_destroy(&FULL);
  sem_destroy(&LOCK);

  for (int i = 0; i < N; i++) {
    fclose(files[i]);
  }

  free(tabela);

  return 0;
}