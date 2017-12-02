#include "shell.h"

int main(int argc, char **argv){

	pwd[0] = '/'; // colocando o shell inicial na raiz do sistema.
	cluster_atual = -1; // mirando o cluster para o a / "raiz".
	// ### DEBUG ###
	init();
	load();
	while(1){

		shell();
	}
	return 0;
}
