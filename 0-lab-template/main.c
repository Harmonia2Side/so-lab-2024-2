#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

void catcher(int signum) {
	switch (signum) {
		case SIGUSR1: puts("catcher caught SIGUSR1");
		break;
		case SIGUSR2: puts("catcher caught SIGUSR2");
		break;
		default:
		printf("catcher caught unexpected signal %d\n",signum);
	}
}
int main() {
	sigset_t sigset;
	struct sigaction sact;
	time_t t;
	if (fork() == 0) {
		sleep(10);
		puts("child is sending SIGUSR2 signal - which should be blocked");
		kill(getppid(), SIGUSR2);
		sleep(5);
		puts("child is sending SIGUSR1 signal - which should be caught");
		kill(getppid(), SIGUSR1);
		exit(0);
	}
	// Inicializa a máscara sem nenhum sinal (nenhum sinal bloqueado)
	sigemptyset(&sact.sa_mask);
	// Não configura nenhuma flag especial
	sact.sa_flags = 0;
	// Define função de tratamento do sinal
	sact.sa_handler = catcher;
	/* 
		As linhas a seguir, que executam a função sigaction, associam os sinais 
		SIGUSR1 e SIGUSR2 à estrutura sact definida anteriormente.
		Na estrutura estão contidas:
		- sa_mask: a máscara de sinais bloqueados,
		- sa_flags: flag especial (não relevante no caso)
		- sa_handler: função de tratamento do sinal
		Sem a utilização do sigaction, o Sistema não associa os sinais especificados com a
		estrutura de "configuração", e então a função de tratamento não seria chamada e os sinais
		não seriam ignorados. 
		Por ser um ponteiro, modificações posteriores a 'sact' resultarão em mudanças efetivas
		de "configuração".
	*/
	// "configura" SIGUSR1
	if (sigaction(SIGUSR1, &sact, NULL) != 0)
		perror("1st sigaction() error");
	// "configura" SIGUSR2
	else if (sigaction(SIGUSR2, &sact, NULL) != 0)
		perror("2nd sigaction() error");
	else {
		// preenche a mascará com todos os sinais suportados no sistema
		sigfillset(&sigset);
		// Remove bloqueio de SIGUSR1
		sigdelset(&sigset, SIGUSR1);
		// habilitar a linha a seguir faz o programa tornar-se um processo zumbi??
		//sigdelset(&sigset, SIGUSR2);
		time(&t);
		printf("parent is waiting for child to send SIGUSR1 at %s",
		ctime(&t));
		// aguarda envio de algum sinal
		if (sigsuspend(&sigset) == -1) {
			perror("sigsuspend() returned -1 as expected");
		}
		time(&t);
		printf("sigsuspend is over at %s", ctime(&t));
	}
}