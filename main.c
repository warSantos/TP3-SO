#include "shell.h"

int main(int argc, char **argv){

	pwd[0] = '/'; // colocando o shell inicial na raiz do sistema.
	current_block = -1; // mirando o cluster para o a / "raiz".
	// ### DEBUG ###
	init();
	load();
	shell();
	return 0;
}
