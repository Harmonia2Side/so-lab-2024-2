#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t r_wait(int *status) {
	int retval;
	while (((retval = wait(status)) == -1) && (errno == EINTR)) ;
	return retval;
}
int main(int argc, char *argv[]) {
   // printf("argv = %s\n", argv[0]);
	pid_t childpid;
	if ( argc < 2 ) {
		fprintf (stderr, "Uso: %s comando arg1 arg2 ... \n", argv[0]);
		return 1;
	}
	childpid = fork();
	if ( childpid == -1 ) {
		perror ("Falha em fork");
		return 1;
	}
	if (childpid == 0 ) {	/* Código filho */
		execvp(argv[1], &argv[1]);
		perror ("Filho falhou em execvp o comando");
		return 1;
	}
	if (childpid != r_wait(NULL)) {	/* Código Pai */
		perror ("Parent failed to wait");
		return 1;
	}
	printf("Filho completo -- pai saindo imediatamente.\n");
	return 0;
}

