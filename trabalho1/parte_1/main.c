#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_CLIENTES 20
#define SOFA_LUGARES 4
#define CADEIRAS_BARBEIROS 3
#define NUM_BARBEIROS 1
#define NUM_CLIENTES 10

pthread_mutex_t mutexLoja = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexFila = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexPagamento = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condSofa = PTHREAD_COND_INITIALIZER;
pthread_cond_t condCadeiras = PTHREAD_COND_INITIALIZER;
pthread_cond_t condBarbeiro = PTHREAD_COND_INITIALIZER;
pthread_cond_t condPagamento = PTHREAD_COND_INITIALIZER;
pthread_cond_t condCorteConcluido = PTHREAD_COND_INITIALIZER;
pthread_cond_t condClienteSaiu = PTHREAD_COND_INITIALIZER;

int clientesNaLoja = 0;
int clientesNoSofa = 0;
int clientesNasCadeiras = 0;
int clientePagando = 0;
int clientesAguardandoCorte = 0;
bool lojaAberta = true;
bool clienteSaiu = false;

int clientesAguardandoPagamento = 0;

void EntrarNaLoja(int id) {
    sleep(rand() % 3);
    pthread_mutex_lock(&mutexLoja);
    clientesNaLoja++;
    printf("Cliente %d: Entrou na loja. Clientes na loja: %d\n", id, clientesNaLoja);
    pthread_mutex_unlock(&mutexLoja);
}

void SentarNoSofa(int id) {
    sleep(rand() % 3);
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
    sleep(rand() % 3);
    pthread_mutex_lock(&mutexFila);
    clientesNoSofa--;
    pthread_cond_signal(&condSofa);

    while (clientesNasCadeiras >= CADEIRAS_BARBEIROS) {
        printf("Cliente %d: Esperando cadeira de barbeiro.\n", id);
        pthread_cond_wait(&condCadeiras, &mutexFila);
    }
    clientesNasCadeiras++;
    clientesAguardandoCorte++;
    printf("Cliente %d: Sentou na cadeira do barbeiro\n", id);
    pthread_cond_signal(&condBarbeiro);
    pthread_mutex_unlock(&mutexFila);
}

void Pagar(int id) {
    pthread_mutex_lock(&mutexFila);
    printf("Cliente %d: Aguardando pela chegada do barbeiro\n", id);
    pthread_cond_wait(&condCorteConcluido, &mutexFila);

    sleep(rand() % 3);

    clientesNasCadeiras--;
    pthread_cond_signal(&condCadeiras);

    printf("Cliente %d: Se dirigindo ao caixa para pagar pelo serviço\n", id);
    
    clientesAguardandoPagamento++;
    clientePagando = id;
    
    pthread_cond_broadcast(&condBarbeiro);

    while (clientePagando == id) {
        pthread_cond_wait(&condPagamento, &mutexFila);
    }

    printf("Cliente %d: Pagou pelo serviço.\n", id);
    pthread_mutex_unlock(&mutexFila);
}

void SairDaLoja(int id) {
    sleep(rand() % 3);
    pthread_mutex_lock(&mutexLoja);
    pthread_cond_signal(&condClienteSaiu);
    clientesNaLoja--;
    clienteSaiu = true;
    printf("Cliente %d: Saiu da loja.\n", id);
    pthread_mutex_unlock(&mutexLoja);
}

void CortarCabelo(int id){
    sleep(rand() % 3);
    pthread_mutex_lock(&mutexFila);
    if (clientesAguardandoCorte <= 0) {
        pthread_mutex_unlock(&mutexFila);
        return;
    }
    clientesAguardandoCorte--;
    pthread_mutex_unlock(&mutexFila);

    printf("Barbeiro %d: Cortando cabelo\n", id);
    sleep(rand() % 3);
    printf("Barbeiro %d: Terminou de cortar cabelo.\n", id);

    pthread_mutex_lock(&mutexFila);
    pthread_cond_signal(&condCorteConcluido);
    pthread_mutex_unlock(&mutexFila);
}

void AceitarPagamento(int id) {
    sleep(rand() % 3);
    pthread_mutex_lock(&mutexFila);

    if (clientePagando == 0 || clientesAguardandoPagamento == 0) {
        pthread_mutex_unlock(&mutexFila);
        return;
    }

    int cliente_atual = clientePagando;
    printf("Barbeiro %d: Aceitando pagamento do cliente %d.\n", id, cliente_atual);

    clientePagando = 0;
    clientesAguardandoPagamento--;
    
    pthread_cond_broadcast(&condPagamento);

    pthread_mutex_unlock(&mutexFila);
}

void *cliente(void *arg) {
    int id = *(int *)arg;
    free(arg);

    pthread_mutex_lock(&mutexLoja);
    if (clientesNaLoja >= MAX_CLIENTES) {
        printf("Cliente %d: Loja cheia, voltarei mais tarde.\n", id);
        pthread_mutex_unlock(&mutexLoja);
        pthread_exit(NULL);
    }
    pthread_mutex_unlock(&mutexLoja);

    EntrarNaLoja(id);
    SentarNoSofa(id);
    SentarNaCadeira(id);
    Pagar(id);
    SairDaLoja(id);

    return NULL;
}

void *barbeiro(void *arg) {
    int id = *(int *)arg;
    free(arg);

    while (1) {
        pthread_mutex_lock(&mutexFila);
        
        if (!lojaAberta && clientesAguardandoCorte == 0 && clientePagando == 0) {
            pthread_mutex_unlock(&mutexFila);
            break;
        }

        while (clientesAguardandoCorte == 0 && clientePagando == 0 && lojaAberta) {
            printf("Barbeiro %d: Aguardando cliente para corte ou pagamento\n", id);
            pthread_cond_wait(&condBarbeiro, &mutexFila);
        }

        if (!lojaAberta && clientesAguardandoCorte == 0 && clientePagando == 0) {
            pthread_mutex_unlock(&mutexFila);
            break;
        }

        if (clientePagando > 0) {
            pthread_mutex_unlock(&mutexFila);
            AceitarPagamento(id);
        } else if (clientesAguardandoCorte > 0) {
            pthread_mutex_unlock(&mutexFila);
            CortarCabelo(id);
        } else {
            pthread_mutex_unlock(&mutexFila);
        }
    }

    printf("Barbeiro %d: Saindo da loja.\n", id);
    return NULL;
}

int main() {
    pthread_t clientes[NUM_CLIENTES], barbeiros[NUM_BARBEIROS];

    for (int i = 0; i < NUM_BARBEIROS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&barbeiros[i], NULL, barbeiro, id);
    }

    for (int i = 0; i < NUM_CLIENTES; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&clientes[i], NULL, cliente, id);
        usleep(rand() % 500000);
    }

    for (int i = 0; i < NUM_CLIENTES; i++) {
        pthread_join(clientes[i], NULL);
    }

    pthread_mutex_lock(&mutexFila);
    lojaAberta = false;
    pthread_mutex_unlock(&mutexFila);
    
    pthread_cond_broadcast(&condBarbeiro);

    for (int i = 0; i < NUM_BARBEIROS; i++) {
        pthread_join(barbeiros[i], NULL);
    }

    return 0;
}
