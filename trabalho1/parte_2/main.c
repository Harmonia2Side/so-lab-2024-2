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

void readReceita(Receita *r, FILE *arquivo) {

  // Buffer to hold the characters read from the file
  // char buffer[100];
  // Read the line until a newline is found

  // does not work
  // if (fgets(buffer, sizeof(buffer), arquivo) != NULL) {
  //   printf("Read line: %s", buffer);
  // } else {
  //   printf("EOF\n");
  // }

  // does not work
  // fgets(buffer, 99, arquivo);
  // buffer[strcspn(buffer, "\n")] = 0;

  // testing
  fscanf(arquivo, "%s", r->nomePaciente);

  // strcpy(r->nomePaciente, buffer);
  fscanf(arquivo, "%d", &r->idMedicamento);
  fscanf(arquivo, "%d", &r->qtd);
}

void printReceita(Receita *r) {
  // TODO: Imprimir a receita
  printf("\n");
  printf("Nome do paciente: %s\n", r->nomePaciente);
  printf("ID do medicamento: %d\n", r->idMedicamento);
  printf("Quantidade: %d\n", r->qtd);
}

Receita **tabela;

int writePos = 0;
int readPos = 0;

void *produtor(void *ptr) {
  FILE *arquivo = (FILE *)ptr;
  while (true) {
    // read data from file
    Receita *r;
    r = (Receita *)malloc(sizeof(Receita));
    readReceita(r, arquivo);
    sem_wait(&EMPTY);
    sem_wait(&LOCK);

    tabela[writePos] = r;
    writePos = (writePos + 1) % N;

    sem_post(&LOCK);
    sem_post(&FULL);
  }

  // TODO: Finalizar thread quando arquivo terminar?
  return NULL;
}

void *consumidor() {
  while (true) {
    sem_wait(&FULL);
    // processamento: Imprime a receita em tela
    Receita *r = tabela[readPos];
    printReceita(r);
    // Desaloca a estrutura de receita
    free(r);
    // incrementa buffer circular
    readPos = (readPos + 1) % N;

    sem_post(&EMPTY);

    // process data
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

  // // teste de leitura de arquivo
  // char* ;
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

  sem_destroy(&EMPTY);
  sem_destroy(&FULL);
  sem_destroy(&LOCK);

  for (int i = 0; i < N; i++) {
    fclose(files[i]);
  }

  free(tabela);

  return 0;
}