#include "fat.h"

int findCluster(char **path, int bloco_atual, int *ptr_enter){

	int i = 0, j;	
	while((*path)[i] != '/' && (*path)[i] != '\0'){
		
		++i;
	}	
	if((*path)[i] == '\0'){

		*path = &((*path)[i]);
	}else{
	
		*path = &((*path)[i + 1]);
	}
	char delim = '/';
	char *temp = strtok(*path, &delim);
	j = 2;
	if(!i){ // se for um caminho absoluto
		
		if((*path)[i] == '\0'){ // isso pode ter sido um ls /
				 
			// sinaliza o bloco de root.
			return -1;
		}
		while(j < 32){
			// se achou o arquivo ou diretório retorne o bloco dele.			
			if(!strcmp(temp, root_dir[j].filename)){
				// se o nome do arquivo for igual retorne o bloco dele.
				*ptr_enter = j; // economizar a pesquisa.
				return root_dir[j].first_block;
			}
			j++;
		} 		
	}else{ // se for um caminho relativo.

		while(j < 32){
			// se achou o arquivo ou diretório retorne o bloco dele.			
			dir_entry_t *dir = (dir_entry_t *) &clusters[bloco_atual];
			if(!strcmp(temp,  dir[j].filename)){
				// se o nome do arquivo for igual retorne o bloco dele.
				*ptr_enter = j; // economizar a pesquisa.	
				return dir[j].first_block;				
			}
			j++;
		}		
	}	
	// retorna bloco inexistente.
	if((*str) == '\0'){ // se chegou ao final da string o caminho e válido.
		return -2;
	}else{ // se não chegou ao fim da string poruqe um diretório intermediário não é válido
		return -3;
	}
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

int free_enter(dir_entry_t *t){

	int i;
	for(i = 2; i < 32; ++i){
		
		if(!t[i].size){
			
			return i;
		}
	}
	return -1;
}
// TO-DO: falta terminar a criação de diretório e a verificação se é na raiz ou não.
void mkdir(char *arg){

	int block = cluster_atual; // recebe o cluster atual.
	int bkp; // salva bloco do diretório anterior ao arquivo ou diretório procurado.
	int ptr_enter = -1; // ponterio para entrada (otimização). 
	int i;
	char *str = arg;
	char *newDir;

	while((*str) != '\0' && block != -1 && block != -2){

		bkp = block; // realizando bkp do bloco do diretório anterior.
		block = findCluster(&str, block, 1, &ptr_enter);
	}	
	newDir = strtok(arg, "/");
	for(newDir != NULL){
		
		newDir = strtok(NULL, "/");
	}
	if(block == -1 && (*str) == '\0'){ /* se ele chegou ao final da string então o caminho é válido e não existe
									   arquivo com este mesmo nome, então pode ser criado o diretório se houver espaço*/		
	}else if(block > -1 && (*str) == '\0'){ // se ele chegou ao final da string então o caminho e existe ou arquivo.
		
		dir_entry_t *dir = (dir_entry_t *) clusters[bkp];		
		if(dir[ptr_enter].attributes == 0){ // se não for um diretório, então crie o diretório
										
			if(free_enter(dir) > -1){ // se existir entrada estiver vazia.
				
				data_cluster *t = malloc(sizeof(data_cluster));
				// criando entrada para o diretório '.'.
				t[0].filename[0] = '.';
				
				// criando entrada para o diretório '..'.
				t[1].filename[0] = '.';
				t[1].filename[2] = '.';
				return;
			}							
			printf("mkdir: não foi possível criar o diretório \"%s\": 
			Limite máximo de diretórios atingidos.\n", str);
		}else{ // se for um diretório, então informe erro.
			
			printf("mkdir: não foi possível criar o diretório \“%s\”: Arquivo existe\n", str);
		}
	}
}
/*
typedef struct _dir_entry_t
{
	unsigned char filename[18];
	unsigned char attributes;
	unsigned char reserved[7];
	unsigned short first_block;
	unsigned int size;
}dir_entry_t;
*/