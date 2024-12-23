#include <stdio.h>
#include <time.h>
#include <unistd.h>
//#include <stdlib.h>

void quickSort(int valor[], int esquerda, int direita);

int main() {
  int vetor[10] = {47, 3, 0, 7, 947, 9, 8, 66, 64, 16};
//   printf("%d", vetor[0]);

  int is_parent = fork();
  if (is_parent) {
    return 1;
    // do normal order
    clock_t c1, c2; /* variaveis que contam ciclos de processador */
    double tmp;
    c1 = clock();
    // ... codigo a ser executado
    c2 = clock();
    tmp = (c2 - c1) * 1000 / CLOCKS_PER_SEC; // tempo de execucao em milisec .
  } else {
    // do quicksort
    clock_t c1, c2; /* variaveis que contam ciclos de processador */
    double tmp;
    c1 = clock();
    quickSort(vetor, vetor[0], vetor[1]);
    c2 = clock();
    tmp = (c2 - c1) * 1000 / CLOCKS_PER_SEC; // tempo de execucao em milisec .
  }
  int n;
  printf("\nvetor:\n");
  for(n=0;n<10;n++){
    printf("%d, ", vetor[n]);
  }
  printf("\n");

  return 0;
}

void quickSort(int valor[], int esquerda, int direita) {
  int i, j, x, y;
  i = esquerda;
  j = direita;
  x = valor[(esquerda + direita) / 2];
  while (i <= j) {
    while (valor[i] < x && i < direita) {
      i++;
    }
    while (valor[j] > x && j > esquerda) {
      j--;
    }
    if (i <= j) {
      y = valor[i];
      valor[i] = valor[j];
      valor[j] = y;
      i++;
      j--;
    }
  }
  if (j > esquerda) {
    quickSort(valor, esquerda, j);
  }
  if (i < direita) {
    quickSort(valor, i, direita);
  }
}