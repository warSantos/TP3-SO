#include "fat.h"

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

char *last_token(char *str, char delim){
	
	int len = strlen(str), bkp, i = 0;
	char *temp = malloc(len + 1);
	len--;
	while(str[len] == delim){ // removendo excesso barras no final do arquivo.
		len--;		
	}	
	bkp = len;
	while(len > -1 && str[len] != delim){ // encontrando a primeira posição pós barra
										  // ou o início do arquivo.
		len--;
	}
	len++;
	while(len <= bkp){ // copiando o token pós barra.
		temp[i] = str[len];
		len++;
		i++;
	}
	temp[i] = '\0';
	return temp;
}

int findCluster(char **path, int block, int *ptr_enter){
		
	if(*path == NULL){ // se for NULL então retorne o bloco do diretório corrente.
		
		return block;
	}else if((*path)[0] == '/' && (*path)[1] == '\0'){ // se for o diretório raiz.
		(*path)++; // move o ponteiro para \0.
		return 65534; // 65534 representa o diretório root.
	}
	int i = 0, j;
	while((*path)[i] != '/' && (*path)[i] != '\0'){
		
		++i;
	}
	char delim = '/';
	char *temp;
	j = 0;
	if(!i){ // se for um caminho absoluto.
		
		(*path)++;  // move o ponteiro de path para depois da barra				
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
		// se achou o arquivo ou diretório retorne o bloco dele.
		dir_entry_t *dir = (dir_entry_t *) read_cluster(block);
		while(j < 32){
			
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
		
	if(block == 65534){ // alterando o ponteiro para o diretório root.
		
		return (data_cluster *) root_dir;
	}
	// alterando o ponteiro de diretório para o arquivo fat.part logo.	
	FILE *ptr_file = fopen(fat_name, "rb");
	data_cluster *d;
	d = malloc(CLUSTER_SIZE);
	printf("\tread: %d/\n", CLUSTER_DATA + (block * CLUSTER_SIZE), (CLUSTER_DATA + (block * CLUSTER_SIZE)) / 1024);
	fseek(ptr_file, CLUSTER_DATA + (block * CLUSTER_SIZE), SEEK_SET);
	fread(d, CLUSTER_SIZE, 1, ptr_file);
	fclose(ptr_file);
	//listaDir((dir_entry_t *)d);
	return d;
}

dir_entry_t *is_root(int block){

	return (dir_entry_t *) read_cluster(block);
}

void init(){

	FILE* ptr_file;
	int i;
	ptr_file = fopen(fat_name,"wb");
	for (i = 0; i < CLUSTER_SIZE; ++i)
		boot_block[i] = 0xbb;

	fwrite(&boot_block, sizeof(boot_block), 1, ptr_file);

	for (i = 0; i < NUM_CLUSTER; ++i){
		fat[i] = 0xfffe;
		fat_sucessors[i]=i+1;
	}
	next_available_block = 0;
		
	fwrite(&fat, sizeof(fat), 1, ptr_file);

    memset(root_dir, 0x00, sizeof(root_dir));	
	fwrite(&root_dir, sizeof(root_dir), 1, ptr_file);

    memset(clusters, 0x00, sizeof(clusters));
    fwrite(&clusters, sizeof(clusters), 1, ptr_file);
    
	fclose(ptr_file);
}

void load(){

	FILE* ptr_file;	
	ptr_file = fopen(fat_name, "rb+");
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
				printf("%s : %d\t", dir[i].filename, dir[i].first_block);
				//printf("%s\t", dir[i].filename);
			}else {				
				printf("%s : %d\t", dir[i].filename, dir[i].first_block);
			}
		}
	}
	printf("\n");
}

void ls(char *arg, char oculto){

	int block = current_block; // recebe o bloco atual.
	int bkp; // salva bloco do diretório anterior ao arquivo ou diretório procurado.
	int ptr_enter = 0; // ponterio para entrada (otimização). 
	char *str = arg;
	while(str[0] != '\0' && block > -3){ // enquanto não chegar no ultimo arquivo do caminho e não achar nada inválido.
						
		bkp = block; // realizando bkp do bloco do diretório anterior.
		block = findCluster(&str, block, &ptr_enter);
		printf("\tstr: %s, bloco: %d, bkp: %d\n", str, block, bkp);
		//getchar();
	}
	dir_entry_t *dir;
	if(block < -1){ // se o caminho até o arquivo for inválido.
		
		printf("ls: não foi possível acessar '%s': Arquivo ou diretório não encontrado\n", arg);
	}else if(block == 65534){ // se for para listar a raiz

		listaDir(root_dir); // lista a raiz.
	}else if(block > -1){// se for válido e não for a raiz.
		dir = is_root(bkp); // retorne o diretório.
		if(dir[ptr_enter].attributes == 1){	// se a entrada corresponder a um diretório.	
			dir = (dir_entry_t *) read_cluster(block); // aponte para o diretório da respectiva entrada.
			listaDir(dir); // imprima o diretório.
		}else{
			//TO-DO, falta formatar a saída do ls no arquivo.
			char *newDir = last_token(arg, '/');
			printf("%s\n", newDir);
		}
	}
}

int free_entry(dir_entry_t *dir){
	int i;
	for(i = 2; i < 32; ++i){
		if(dir[i].filename[0] == 0){
			return i;
		}
	}
	return 0;
}
/*
int free_blocks(void){
	int i = 0;
	while(i < 4086){
		if(fat[i] == 65534){ // se este id estiver livre.
			return i;
		}
		++i;
	}
	return -1;
}*/

int available_block(){
	if(next_available_block != NUM_CLUSTER){
		return fat_sucessors[next_available_block];
	}else{
		return -1;
	}
}

void clear_block(int index){
	//Se o bloco já está livre, não é necessário executar essa função.
	if(fat_sucessors[index] > -1){
		//Insira o código para liberar a posição fat[index] aqui
		if(next_available_block != NUM_CLUSTER){
			//Existe pelo menos um bloco livre.
			int previous_sucessor = fat_sucessors[index];
			fat_sucessors[index]=previous_sucessor;
			//fat[index] = null;
			fat_sucessors[next_available_block]=index;
		}else{
			//No momento, não existe nenhum bloco disponível.
			fat_sucessors[index] = NUM_CLUSTER;
			fat[index] = 65535;
			next_available_block = index;
		}
	}
}

int allocate_block(){
	if(next_available_block != NUM_CLUSTER){
		int previous_block = next_available_block;
		int previous_value = fat_sucessors[next_available_block];
		fat_sucessors[next_available_block] = -1;
		fat[next_available_block] = previous_block; //Escreve os dados de fato na FAT.
		next_available_block = previous_value;
		return previous_block;
	}else{
		return -1;
	}
}



void set_dir_entry(dir_entry_t *parent_dir, int block_parent_dir, char *str, int new_entry,	
	int block, int size, int attributes){
	
	char *temp = last_token(str, '/');
	strncpy((char *)parent_dir[new_entry].filename, temp, strlen(temp)); // adicionando nome a entrada.
	parent_dir[new_entry].first_block = block; // adicionando bloco.
	parent_dir[new_entry].attributes = attributes; // definindo tipo do arquivo.
	parent_dir[new_entry].size = size; // definindo o tamanho do arquivo.
	if(parent_dir == root_dir){ // se o diretório que for receber a nova entrada for a raiz.
		persist_on_disk(&parent_dir[new_entry], ENTRY_BY_CLUSTER, ROOT_ENTRY(new_entry));
	}else{ // se não for a raiz aponte para o trecho de dados.
		persist_on_disk(&parent_dir[new_entry], ENTRY_BY_CLUSTER, 
		CLUSTER_DATA + (CLUSTER_SIZE * block_parent_dir) + (new_entry * ENTRY_BY_CLUSTER));
	}
}

dir_entry_t *new_dir(int block, int block_parent_dir){
	
	dir_entry_t *new = malloc(CLUSTER_SIZE);
	memset(new, 0x00, CLUSTER_SIZE);
	// configurando o diretório '.'.
	new[0].filename[0] = '.'; // definindo o nome
	new[0].filename[1] = '\0';
	new[0].first_block =  block_parent_dir; // apontando para o ele mesmo
	new[0].size = ENTRY_BY_CLUSTER; // tamanho de 32 logo que ele só é a entrada.
	new[0].attributes = 1; // definindo como diretório.
	// configurando o diretório '..'.
	new[1] = new[0];
	new[1].filename[1] = '.';
	new[0].filename[2] = '\0';
	new[0].first_block = block;
	return new;
}

void create_dir(dir_entry_t *parent_dir, int block_parent_dir, char *str){
	
	int new_entry, block;
	new_entry = free_entry(parent_dir);
	
	if(new_entry > 0){ // se existir entrada disponível.
		//block = free_blocks();
		block = available_block();
		if(block > -1){ // se existir espaço no disco.
			
			// Adicionando a entrada no diretório pai
			set_dir_entry(parent_dir, block_parent_dir, str, new_entry, block, CLUSTER_SIZE, 1);			
			// Criando diretório com configurações iniciais.
			dir_entry_t *new_d = new_dir(block, block_parent_dir);	
			// atualizando a fat.
			fat[block] = 65535; // 65535 sinaliza que este é o último bloco.			
			// persistindo a atualização da fat no arquivo.
			persist_on_disk(&fat[block], 2, FAT_ENTRY(block));
			// persistindo o novo diretório no disco.
			//printf("\twrite: %d/%d\n", (block * 1024) + CLUSTER_DATA, ((block * 1024) + CLUSTER_DATA)/1024);
			persist_on_disk(new_d, CLUSTER_SIZE, (block * 1024) + CLUSTER_DATA);
			free(new_d);
		}else{
			printf("mkdir: não foi possível criar o diretório \"%s\""
			": Limite máximo do disco atingido.\n", str);
		}			
	}else{
		printf("mkdir: não foi possível criar o diretório \"%s\""
		": Não há entradas disponíveis neste diretório.\n", str);
	}
}

void persist_on_disk(void *data, int size_data, int block){

	FILE *ptr_file =fopen(fat_name, "rb+");
	fseek(ptr_file, block, SEEK_SET);	
	if(!fwrite(data, size_data, 1, ptr_file)){
		perror("persist_on_disk");
	}
	fclose(ptr_file);
}

void mkdir(char *arg){

	int block = current_block; // recebe o bloco atual.
	int bkp; // salva bloco do diretório anterior ao arquivo ou diretório procurado.
	int ptr_enter = 0; // ponterio para entrada (otimização). 
	char *str = arg;
	if(strcmp("/", str) && root_dir[0].filename[0] == 0){
		printf("mkdir: não foi possível criar o diretório \“%s\”: Diretório raiz inexistente.\n", str);
		return;
	}
	while(str[0] != '\0' && block > -3){ // enquanto não chegar no ultimo arquivo do caminho && não achar nada inválido.
						
		bkp = block; // realizando bkp do bloco do diretório anterior.
		block = findCluster(&str, block, &ptr_enter);
		printf("\tstr: %s, bloco: %d, bkp: %d\n", str, block, bkp);	
	}
	dir_entry_t *dir;
	if(block == -3){ // se o caminho ate o arquivo for inválido
		printf("mkdir: não foi possível criar o diretório “%s”: Arquivo ou diretório não encontrado.\n", arg);
	}else if(block == -2){ // se o caminho até o arquivo for válido porém o arquivo não existir
		
		dir = is_root(bkp); // retorne o diretório.	
		create_dir(dir, bkp, arg);
	}else if(block == 65534){ // se o diretório a ser solicitada criação for a raiz.
		stage(1)
		if(root_dir[0].filename[0] == '.'){ // se a raiz já estiver sido criada.
			printf("mkdir: não foi possível criar o diretório \“/\”: Arquivo existe\n");
		}else{ // se não estiver sido.
			// configurando o diretório root.
			dir_entry_t *t = new_dir( 65534, 65534);
			memcpy(root_dir, t, CLUSTER_SIZE);
			free(t);
		}		
	}else{ // se o arquivo existir.		
		printf("mkdir: não foi possível criar o diretório \“%s\”: Arquivo existe\n", arg);		
	}
}

int create_file(char *arg){

	int block = current_block; // recebe o bloco atual.
	int bkp; // salva bloco do diretório anterior ao arquivo ou diretório procurado.
	int ptr_enter = 0; // ponterio para entrada (otimização). 
	int new_block; // recebe o novo bloco para o arquivo a ser criado.
	int new_entry; // recebe o a nova entrada para o arquivo a ser criado.
	char *str = arg;
	if(strcmp("/", str) && root_dir[0].filename[0] == 0){
		printf("mkdir: não foi possível criar o diretório \“%s\”: Diretório raiz inexistente.\n", str);
		return;
	}
	while(str[0] != '\0' && block > -3){ // enquanto não chegar no ultimo arquivo do caminho && não achar nada inválido.
						
		bkp = block; // realizando bkp do bloco do diretório anterior.
		block = findCluster(&str, block, &ptr_enter);
		printf("\tstr: %s, bloco: %d, bkp: %d\n", str, block, bkp);	
	}
	dir_entry_t *dir;
	if(block == -3){ // se o caminho for inválido.
		
		printf("create: não foi possível criar '%s': Arquivo ou diretório não encontrado\n", arg);
	}else{ // se o caminho não for válido.
		
		if(block == -2){ // se nao existir arquivo com mesmo nome.
			dir = is_root(bkp);			
			new_entry = free_entry(dir);
			if(free_entry > 0){ // se existirem entradas disponíveis no diretório.
				//new_block = free_blocks();
				new_block = available_block();
				if(new_block > -1){ // se existir bloco livre
					set_dir_entry(dir, bkp, arg, new_entry, new_block, 0, 0);
					// atualizando a fat.
					fat[block] = 65535; // 65535 sinaliza que este é o último bloco.			
					// persistindo a atualização da fat no arquivo.
					persist_on_disk(&fat[block], 2, FAT_ENTRY(block));
					return new_block; // retorna o bloco do novo arquivo criado.
				}else{					
					printf("create: não foi possível criar o arquivo \"%s\""
					": Limite máximo do disco atingido.\n", str);
				}
			}else{				
				printf("create: não foi possível criar o arquivo \"%s\""
				": Não há entradas disponíveis neste diretório.\n", str);
			}
		}else{
			printf("mkdir: não foi possível criar o arquivo “%s”: Arquivo existe\n", arg);
		}
	}
	return -1;
	/*
	void write(char *buffer, char *path){
		
	}


	void append(char *buffer, char *path){
		
	}

	void read(char *path){
		
	}
	 */
}