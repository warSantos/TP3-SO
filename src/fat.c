#include "fat.h"

#define stage(N) printf("%d\n", N);

char *__strtok(char *str, char delim){

	int i = 0;
	while(str[i] != delim && str[i] != '\0'){
		
		i++;
	}
	char *temp = malloc(i + 1);
	temp[i] = '\0';
	strncpy(temp, str, i);
	return temp;
}
/// 
int findCluster(char **path, int bloco_atual, int *ptr_enter){
		
	if(*path == NULL){ // se for NULL então retorne o bloco do diretório corrente.
		
		return bloco_atual;
	}else if((*path)[0] == '/' && (*path)[1] == '\0'){ // se for o diretório raiz.
		(*path)++; // move o ponteiro para \0.
		return -1;
	}
	int i = 0, j;
	while((*path)[i] != '/' && (*path)[i] != '\0'){
		
		++i;
	}
	char delim = '/';
	char *temp;
	j = 2;
	if(!i){ // se for um caminho absoluto.
		
		(*path)++;  // move o ponteiro de path para depois da barra				
		temp = __strtok((*path), delim);		
		while(1){ // movendo o ponteiro para o próximo arquivo.
			
			if((*path)[0] == '/'){
				(*path)++;
				stage(1)
				break;
			}else if((*path)[0] == '\0'){
				stage(2)
				break;
			}
			(*path)++;
		}
		while(j < 32){
			// se achou o arquivo ou diretório retorne o bloco dele.
			if(!strcmp(temp, (char *)root_dir[j].filename) &&
				((*path)[0] == '\0' || root_dir[j].attributes == 1)){
				// se existir arquivo com o mesmo nome e ele for um diretório ou um arquivo no final do caminho,
				// retorne o bloco dele.				
				*ptr_enter = j; // economizar a pesquisa.
				return root_dir[j].first_block;
			}
			//printf("name: %s attr: %d fb: %d\n", root_dir[j].filename, root_dir[j].attributes, root_dir[j].first_block);
			j++;
		}
	}else{ // se for um caminho relativo.
		
		temp = __strtok((*path), delim);		
		while(1){ // movendo o ponteiro para o próximo arquivo.
			
			if((*path)[0] == '/'){
				(*path)++;	
				break;
			}else if((*path)[0] == '\0'){
				break;
			}
			(*path)++;
		}
		while(j < 32){
			// se achou o arquivo ou diretório retorne o bloco dele.			
			dir_entry_t *dir = (dir_entry_t *) read_cluster(bloco_atual);
			if(!strcmp(temp, (char *) dir[j].filename) &&
				((*path)[0] == '\0' || dir[j].attributes == 1)){
				// se existir arquivo com o mesmo nome e ele for um diretório ou um arquivo no final do caminho,
				// retorne o bloco dele.
				*ptr_enter = j; // economizar a pesquisa.
				return dir[j].first_block;
			}
			j++;
		}
	}
	// retorna bloco inexistente.
	if((*path)[0] == '\0'){ // se chegou ao final da string e o caminho e válido.
		return -2;			// retorne que o arquivo não existe.
	}else{ // se não chegou ao fim da string porque um diretório intermediário não é válido
		return -3; // retorne que o caminho até o arquivo não é válido.
	}
}

data_cluster *read_cluster(int block){
	
	data_cluster *d;
	if(block == -1){ // alterando o ponteiro para o diretório root.

		d = (data_cluster *)&root_dir;
	}else{ // alterando o ponteiro de diretório para o arquivo fat.part logo.

		FILE *ptr_file = fopen(fat_name, "r");
		d = malloc(CLUSTER_SIZE);
		fseek(ptr_file, CLUSTER_DATA + (block * CLUSTER_SIZE), SEEK_SET);
		fwrite(d, sizeof(CLUSTER_SIZE), 1, ptr_file);
		fclose(ptr_file);
	}
	return d;
}

// escrever no arquivo.
/*rb+*/
void init(){

	FILE* ptr_file;
	int i;
	ptr_file = fopen(fat_name,"wb");
	for (i = 0; i < CLUSTER_SIZE; ++i)
		boot_block[i] = 0xbb;

	fwrite(&boot_block, sizeof(boot_block), 1, ptr_file);

	for (i = 0; i < NUM_CLUSTER; ++i)
		fat[i] = 0xfffe;

	fwrite(&fat, sizeof(fat), 1, ptr_file);
	
    memset(root_dir, 0x00, sizeof(root_dir));
	root_dir[2].filename[0] = 't';
	root_dir[2].filename[1] = 'm';
	root_dir[2].filename[2] = 'p';
	root_dir[2].attributes = 1;
	root_dir[2].first_block = 12000;
	root_dir[2].size = 1024;
	root_dir[3].filename[0] = 'u';
	root_dir[3].filename[1] = 's';
	root_dir[3].filename[2] = 'r';
	root_dir[3].attributes = 1;
	root_dir[3].first_block = 13000;
	root_dir[3].size = 1024;
	root_dir[4].filename[0] = 'b';
	root_dir[4].filename[1] = 'i';
	root_dir[4].filename[2] = 'n';
	root_dir[4].attributes = 1;
	root_dir[4].first_block = 14000;
	root_dir[4].size = 1024;
	root_dir[5].filename[0] = 'a';
	root_dir[5].filename[1] = 'r';
	root_dir[5].filename[2] = 'q';
	root_dir[5].attributes = 0;
	root_dir[5].first_block = 14000;
	root_dir[5].size = 1024;

	printf("sr: %d\n", sizeof(root_dir));
	fwrite(&root_dir, sizeof(root_dir), 1, ptr_file);

    memset(clusters, 0x00, sizeof(clusters));
    fwrite(&clusters, sizeof(clusters), 1, ptr_file);
    
	fclose(ptr_file);
}

void load(){

	FILE* ptr_file;	
	ptr_file = fopen(fat_name, "r+b");
	fseek(ptr_file, sizeof(boot_block), SEEK_SET);
	fread(fat, sizeof(fat), 1, ptr_file);
	fread(root_dir, sizeof(root_dir), 1, ptr_file);
	fclose(ptr_file);
}

void listaDir(dir_entry_t *dir){
	int i;
	for(i = 0; i < 32; ++i){
			
		if(dir[i].filename[0]){ // se existir arquivo ou diretório na respectiva entrada.

			if(dir[i].filename[0] /*== '.' && oculto == 'o'*/){ // se for um diretório oculto.
				
				printf("%s\t", dir[i].filename);
			}else {
				// imprime diretório.
				printf("%s\t", dir[i].filename);
			}
		}
	}
	printf("\n");
}

void ls(char *arg, char oculto){

	int block = cluster_atual; // recebe o cluster atual.
	int bkp; // salva bloco do diretório anterior ao arquivo ou diretório procurado.
	int ptr_enter = 0; // ponterio para entrada (otimização). 
	int i;
	char *str = arg;
	printf("arg: %s\n", arg);
	while(str[0] != '\0' && block > -3){ // enquanto não chegar no ultimo arquivo do caminho && não achar nada inválido.
						
		bkp = block; // realizando bkp do bloco do diretório anterior.
		block = findCluster(&str, block, &ptr_enter);
		printf("str: %s, bloco: %d, bkp: %d\n", str, block, bkp);
		//getchar();
	}
	dir_entry_t *dir;
	if(block < -1){ // se o caminho até o arquivo for inválido.
		
		printf("ls: não foi possível acessar '%s': Arquivo ou diretório não encontrado\n", arg);
	}else if(block == -1){ // se for para listar a raiz

		listaDir(root_dir); // lista a raiz.
	}else if(block > 0){

		if(bkp == -1){ // se o diretório antecessor a ele for a raiz.
			dir = root_dir; 
		}else{ // se não aponte para o diretório corrente.
			dir = (dir_entry_t *) read_cluster(bkp);
		}
		if(dir[ptr_enter].attributes){	// se a entrada corresponder a um diretório.	
			dir = (dir_entry_t *) read_cluster(block); // aponte para o diretório da respectiva entrada.
			listaDir(dir); // imprima o diretório.
		}else{
			//TO-DO, falta formatar a saída do ls no arquivo.
			char *newDir = strtok(arg, "/");
			while(newDir != NULL){
				printf("-->: %s\n", newDir);	
				newDir = strtok(NULL, "/");
			}		
		}
	}
}

int free_enter(dir_entry_t *t){

	int i;
	for(i = 2; i < 32; ++i){
		
		if(!t[i].filename){
			
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

	
}
/*
printf("mkdir: não foi possível criar o diretório \"%s\": 
			Limite máximo de diretórios atingidos.\n", str);
printf("mkdir: não foi possível criar o diretório \“%s\”: Arquivo existe\n", str);
*/

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