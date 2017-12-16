#include "shell.h"

int main(int argc, char **argv){

	k_bytes = 1024;
	pwd[0] = '/'; // colocando o shell inicial na raiz do sistema.
	current_block = 65534; // direcionando o cluster para o a / "raiz".
	not_used = 65535; // valor de bloco livre.
	end_file = 65533; // valor de último bloco.
	// ### DEBUG ###
	
	if(fopen(fat_name, "r") == NULL){
		init();
		mkdir("/");
	}
	load();
	shell();
	return 0;
}
