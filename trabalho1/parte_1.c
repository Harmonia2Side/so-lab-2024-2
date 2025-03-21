#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_CLIENTES 20
#define SOFA_LUGARES 4
#define CADEIRAS_BARBEIROS 3
#define NUM_BARBEIROS 3

pthread_mutex_t mutexLoja = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCaixa = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexFila = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condSofa = PTHREAD_COND_INITIALIZER;
pthread_cond_t condCadeiras = PTHREAD_COND_INITIALIZER;
pthread_cond_t condPagamento = PTHREAD_COND_INITIALIZER;
pthread_cond_t condBarbeiros = PTHREAD_COND_INITIALIZER;

int clientesNaLoja = 0;
int clientesNoSofa = 0;
int clientesNasCadeiras = 0;
int clienteProntoParaPagar = 0;
int barbeiroDisponivelParaPagamento = 0;
int barbeirosLivres = NUM_BARBEIROS;

void EntrarNaLoja(int id) {
    pthread_mutex_lock(&mutexLoja);
    if (clientesNaLoja >= MAX_CLIENTES) {
        printf("Cliente %d: Loja cheia, indo embora.\n", id);
        pthread_mutex_unlock(&mutexLoja);
        pthread_exit(NULL);
    }
    clientesNaLoja++;
    printf("Cliente %d: Entrou na loja. Clientes na loja: %d\n", id, clientesNaLoja);
    pthread_mutex_unlock(&mutexLoja);
}

void SentarNoSofa(int id) {
    pthread_mutex_lock(&mutexFila);
    while (clientesNoSofa >= SOFA_LUGARES) {
        printf("Cliente %d: Esperando vaga no sofá.\n", id);
        pthread_cond_wait(&condSofa, &mutexFila);
    }
    clientesNoSofa++;
    printf("Cliente %d: Sentou no sofá. No sofá: %d\n", id, clientesNoSofa);
    pthread_mutex_unlock(&mutexFila);
}

void SentarNaCadeira(int id) {
    pthread_mutex_lock(&mutexFila);
    while (clientesNasCadeiras >= CADEIRAS_BARBEIROS) {
        printf("Cliente %d: Esperando cadeira de barbeiro.\n", id);
        pthread_cond_wait(&condCadeiras, &mutexFila);
    }
    clientesNoSofa--;
    clientesNasCadeiras++;
    barbeirosLivres--;
    printf("Cliente %d: Sentou na cadeira do barbeiro.\n", id);
    pthread_cond_signal(&condSofa);
    pthread_mutex_unlock(&mutexFila);
}

void Pagar(int id) {
    pthread_mutex_lock(&mutexCaixa);
    clienteProntoParaPagar = id;
    printf("Cliente %d: Esperando para pagar.\n", id);
    
    while (!barbeiroDisponivelParaPagamento) {
        pthread_cond_wait(&condPagamento, &mutexCaixa);
    }
    
    printf("Cliente %d: Pagamento realizado.\n", id);
    clienteProntoParaPagar = 0;
    barbeiroDisponivelParaPagamento = 0;

    pthread_mutex_unlock(&mutexCaixa);
}

void SairDaLoja(int id) {
    pthread_mutex_lock(&mutexLoja);
    clientesNasCadeiras--;
    clientesNaLoja--;
    barbeirosLivres++;
    printf("Cliente %d: Saiu da loja.\n", id);
    
    pthread_cond_signal(&condCadeiras);
    pthread_cond_signal(&condBarbeiros);
    pthread_mutex_unlock(&mutexLoja);
}

void CortarCabelo(int id) {
    printf("Barbeiro %d: Cortando cabelo...\n", id);
    sleep(2);
    printf("Barbeiro %d: Terminou de cortar cabelo.\n", id);
}

void AceitarPagamento(int id) {
    pthread_mutex_lock(&mutexCaixa);
    while (!clienteProntoParaPagar) {
        pthread_cond_wait(&condPagamento, &mutexCaixa);
    }
    printf("Barbeiro %d: Aceitando pagamento do Cliente %d.\n", id, clienteProntoParaPagar);
    barbeiroDisponivelParaPagamento = 1;
    pthread_cond_signal(&condPagamento);
    pthread_mutex_unlock(&mutexCaixa);
}

void* cliente(void* arg) {
    int id = *(int*)arg;
    free(arg);

    EntrarNaLoja(id);
    SentarNoSofa(id);
    SentarNaCadeira(id);
    Pagar(id);
    SairDaLoja(id);

    return NULL;
}

void* barbeiro(void* arg) {
    int id = *(int*)arg;
    free(arg);

    while (1) {
        pthread_mutex_lock(&mutexFila);
        while (clientesNasCadeiras == 0) {
            printf("Barbeiro %d: Aguardando cliente...\n", id);
            pthread_cond_wait(&condBarbeiros, &mutexFila);
        }
        pthread_mutex_unlock(&mutexFila);
        
        CortarCabelo(id);
        AceitarPagamento(id);
    }
    return NULL;
}

int main() {
    pthread_t clientes[10], barbeiros[NUM_BARBEIROS];

    for (int i = 0; i < NUM_BARBEIROS; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&barbeiros[i], NULL, barbeiro, id);
    }

    for (int i = 0; i < 10; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&clientes[i], NULL, cliente, id);
        usleep(rand() % 100000);
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(clientes[i], NULL);
    }

    return 0;
}