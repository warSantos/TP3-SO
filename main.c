#include "shell.h"

int main(int argc, char **argv){

	not_used = 65535;
	final_file = 65533;
	k_bytes = 1024;
	pwd[0] = '/'; // colocando o shell inicial na raiz do sistema.
	current_block = 65534; // mirando o cluster para o a / "raiz".
	// ### DEBUG ###
	
	if(fopen(fat_name, "r") == NULL){
		init();
		mkdir("/");
	}
	load();
	shell();
	return 0;
}
