#include "fat.h"

/*

	se achar uma '/'

		se a barra for o primeiro caractere	
			retorne o root.
		se nao for		
			se a string anterior a barra existe na entrada de diretório	
				retorna o bloco da barra e sua string.
	

*/

int findCluster(char **path, int bloco_atual){

	int i = 0, j, block = -1;	
	while((*path)[i] != '/' && (*path)[i] != '\0'){
		
		printf("char %c\n", (*path)[i]);	
		++i;
	}	
	if((*path)[i] == '\0'){

		*path = &((*path)[i]);
	}else{
	
		*path = &((*path)[i + 1]);
	}
	if(!i){ // se for um caminho absoluto
		
		// sinaliza o bloco de root.
		return -2;
	}else{
		j = 2;		
		/*
		char delim = '/';
		char *temp = strtok(*path, &delim);
		while(j < 32){
			
			/*
			if(!strcmp(temp, (dir_entry_t *) clusters[bloco_atual].filename)){
				
				return (dir_entry_t *) clusters[bloco_atual].first_block;
			}
			j++;
		} 
		// retorna bloco inexistente.*/
		return -1;
	}	
	return block;
}

void init(){

	FILE* ptr_file;
	int i;
	ptr_file = fopen(fat_name,"wb");
	for (i = 0; i < 1024; ++i)
		boot_block[i] = 0xbb;

	fwrite(&boot_block, sizeof(boot_block), 1, ptr_file);

	for (i = 0; i < NUM_CLUSTER; ++i)
		fat[i] = 0xfffe;

	fwrite(&fat, sizeof(fat), 1, ptr_file);
	
    memset(root_dir, 0x00, sizeof(root_dir));
	fwrite(&root_dir, sizeof(root_dir), 1, ptr_file);

    memset(clusters, 0x00, sizeof(clusters));
    fwrite(&clusters, sizeof(clusters), 1, ptr_file);
    
	fclose(ptr_file);
}

void load(){

	FILE* ptr_file;
	int i;
	ptr_file = fopen(fat_name, "rb");
	fseek(ptr_file, sizeof(boot_block), SEEK_SET);
	fread(fat, sizeof(fat), 1, ptr_file);
	fread(root_dir, sizeof(root_dir), 1, ptr_file);
	fclose(ptr_file);
}