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

int findCluster(char **path, int block, int *ptr_entry){
		
	if(*path == NULL){ // se for NULL então retorne o bloco do diretório corrente.
		
		return block;
	}else if((*path)[0] == '/' && (*path)[1] == '\0'){ // se for o diretório raiz.
		(*path)++; // move o ponteiro para \0.
		return ROOT_BLOCK; // ROOT_BLOCK representa o diretório root.
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
		while(j < ENTRY_BY_CLUSTER){
			// se achou o arquivo ou diretório retorne o bloco dele.
			if(!strcmp(temp, (char *)root_dir[j].filename) &&
				((*path)[0] == '\0' || root_dir[j].attributes == 1)){
				// se existir arquivo com o mesmo nome e ele for um diretório ou um arquivo no final do caminho,
				// retorne o bloco dele.				
				*ptr_entry = j; // economizar a pesquisa.
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
		while(j < ENTRY_BY_CLUSTER){
			
			if(!strcmp(temp, (char *) dir[j].filename) &&
				((*path)[0] == '\0' || dir[j].attributes == 1)){
				// se existir arquivo com o mesmo nome e ele for um diretório ou um arquivo no final do caminho,
				// retorne o bloco dele.
				*ptr_entry = j; // economizar a pesquisa.
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
		
	if(block == ROOT_BLOCK){ // alterando o ponteiro para o diretório root.
		
		return (data_cluster *) root_dir;
	}
	// alterando o ponteiro de diretório para o arquivo fat.part logo.	
	FILE *ptr_file = fopen(fat_name, "rb");
	data_cluster *d;
	d = malloc(k_bytes);	
	fseek(ptr_file, CLUSTER_DATA + (block * k_bytes), SEEK_SET);
	fread(d, k_bytes, 1, ptr_file);
	fclose(ptr_file);
	return d;
}

dir_entry_t *is_root(int block){

	return (dir_entry_t *) read_cluster(block);
}

void init(void){

	FILE* ptr_file;
	int i;
	ptr_file = fopen(fat_name,"wb");
	for (i = 0; i < k_bytes; ++i)
		boot_block[i] = 0xbb;

	fwrite(&boot_block, sizeof(boot_block), 1, ptr_file);
	
	for (i = 0; i < NUM_CLUSTER; ++i){
		fat[i] = 0xffff;
		fat_sucessors[i] = i + 1;
	}
	next_available_block = 0;

	fwrite(&fat, sizeof(fat), 1, ptr_file);

    memset(root_dir, 0x00, sizeof(root_dir));	
	fwrite(&root_dir, sizeof(root_dir), 1, ptr_file);

    memset(clusters, 0x00, sizeof(clusters));
    fwrite(&clusters, sizeof(clusters), 1, ptr_file);
    
	fclose(ptr_file);
}

void load(void){

	FILE* ptr_file;	
	ptr_file = fopen(fat_name, "rb+");
	fseek(ptr_file, sizeof(boot_block), SEEK_SET);
	fread(fat, sizeof(fat), 1, ptr_file);
	fread(root_dir, sizeof(root_dir), 1, ptr_file);
	fclose(ptr_file);
}

void lista_dir(dir_entry_t *dir, char oculto, char info){
	int i;
	for(i = 0; i < ENTRY_BY_CLUSTER; ++i){			
		if(dir[i].filename[0]){ // se existir arquivo ou diretório na respectiva entrada.

			if(dir[i].filename[0] != '.' || // se for um diretório oculto e a opção de listar todos
					(dir[i].filename[0] == '.' && oculto == 'o')){ // diretórios ou se não for um diretório oculto
				
				if(info == 'i'){ // se info estiver ativo então imprima todas as informações.
					if(dir[i].attributes){ // se for um diretório.
						printf("\033[1;34m%s\t\033[0mbloco\t%d\ttamanho\t%d\n", dir[i].filename, dir[i].first_block, dir[i].size);	
					}else{
						printf("%s\tbloco\t%d\ttamanho\t%d\n", dir[i].filename, dir[i].first_block, dir[i].size);
					}
				}else{
					if(dir[i].attributes){ // se for um diretório.
						printf("\033[1;34m%s\033[0m\t", dir[i].filename);
					}else{
						printf("%s\t", dir[i].filename);
					}						
				}
			}
		}
	}
	printf("\n");
}

void ls(char *arg, char oculto, char info){

	int block = current_block; // recebe o bloco atual.
	int bkp; // salva bloco do diretório anterior ao arquivo ou diretório procurado.
	int ptr_entry = 0; // ponterio para entrada (otimização). 
	char *str = arg;
	while(str[0] != '\0' && block > -3){ // enquanto não chegar no ultimo arquivo do caminho e não achar nada inválido.
						
		bkp = block; // realizando bkp do bloco do diretório anterior.
		block = findCluster(&str, block, &ptr_entry);		
	}
	dir_entry_t *dir;
	if(block < -1){ // se o caminho até o arquivo for inválido.

		printf("ls: não foi possível acessar '%s': Arquivo ou diretório não encontrado\n", arg);
	}else if(block == ROOT_BLOCK){ // se for para listar a raiz

		lista_dir(root_dir, oculto, info); // lista a raiz.
	}else if(block > -1){// se for válido e não for a raiz.
		dir = is_root(bkp); // verificando se o diretório que o contém é a raiz.
		if(dir[ptr_entry].attributes == 1){	// se a entrada corresponder a um diretório.
			dir = (dir_entry_t *) read_cluster(block); // aponte para o diretório da respectiva entrada.
			lista_dir(dir, oculto, info); // imprima o diretório.
		}else{
			//TO-DO, falta formatar a saída do ls no arquivo.
			char *newDir = last_token(arg, '/');
			printf("%s\n", newDir);
		}
	}
}

int free_entry(dir_entry_t *dir){
	int i;
	for(i = 2; i < ENTRY_BY_CLUSTER; ++i){
		if(dir[i].filename[0] == 0){
			return i;
		}
	}
	return 0;
}

int free_blocks(int init){
	int i = init;
	while(i < 4086){
		if(fat[i] == not_used){ // se este id estiver livre.
			return i;
		}
		++i;
	}
	return -1;
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
		CLUSTER_DATA + (k_bytes * block_parent_dir) + (new_entry * ENTRY_BY_CLUSTER));
	}
}

dir_entry_t *new_dir(int block, int block_parent_dir){
	
	dir_entry_t *new = malloc(k_bytes);
	memset(new, 0x00, k_bytes);
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
		block = free_blocks(0);
		if(block > -1){ // se existir espaço no disco.
			
			// Adicionando a entrada no diretório pai
			set_dir_entry(parent_dir, block_parent_dir, str, new_entry, block, k_bytes, 1);			
			// Criando diretório com configurações iniciais.
			dir_entry_t *new_d = new_dir(block, block_parent_dir);	
			// atualizando a fat.
			fat[block] = end_file; // end_file sinaliza que este é o último bloco.			
			// persistindo a atualização da fat no arquivo.
			persist_on_disk(&fat[block], 2, FAT_ENTRY(block));
			// persistindo o novo diretório no disco.			
			persist_on_disk(new_d, k_bytes, (block * k_bytes) + CLUSTER_DATA);
			free(new_d);
		}else{
			printf("mkdir: não foi possível criar o diretório \"%s\""
			": Limite máximo do disco atingido.\n", str);
		}			
	}else{
		printf("create: não foi possível criar o diretório \"%s\""
		": Não há entradas disponíveis neste diretório.\n", str);
	}
}

void persist_on_disk(void *data, int size_data, int block){
	//printf("block_persist: %d\n", block);
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
	int ptr_entry = 0; // ponterio para entrada (otimização). 
	char *str = arg;
	if(strcmp("/", str) && root_dir[0].filename[0] == 0){
		printf("mkdir: não foi possível criar o diretório \“%s\”: diretório raiz inexistente.\n", str);
		return;
	}
	while(str[0] != '\0' && block > -3){ // enquanto não chegar no ultimo arquivo do caminho && não achar nada inválido.
						
		bkp = block; // realizando bkp do bloco do diretório anterior.
		block = findCluster(&str, block, &ptr_entry);
	}
	dir_entry_t *dir;
	if(block == -3){ // se o caminho ate o arquivo for inválido
		printf("mkdir: não foi possível criar o diretório “%s”: arquivo ou diretório não encontrado.\n", arg);
	}else if(block == -2){ // se o caminho até o arquivo for válido porém o arquivo não existir
		
		dir = is_root(bkp); // verificando se o diretório que o conterá é a raiz.
		create_dir(dir, bkp, arg);
	}else if(block == ROOT_BLOCK){ // se o diretório a ser solicitada criação for a raiz.
		
		if(root_dir[0].filename[0] == '.'){ // se a raiz já estiver sido criada.
			printf("mkdir: não foi possível criar o diretório \“/\”: arquivo existe\n");
		}else{ // se não estiver sido.
			// configurando o diretório root.
			dir_entry_t *t = new_dir( ROOT_BLOCK, ROOT_BLOCK);
			memcpy(root_dir, t, k_bytes);
			persist_on_disk(root_dir, k_bytes, ROOT_ENTRY(0));
			free(t);
		}		
	}else{ // se o arquivo existir.		
		printf("mkdir: não foi possível criar o diretório \“%s\”: arquivo existe\n", arg);		
	}
}

int create_file(char *arg, int size_file, int ignore){

	int block = current_block; // recebe o bloco atual.
	int bkp; // salva bloco do diretório anterior ao arquivo ou diretório procurado.
	int ptr_entry = 0; // ponterio para entrada (otimização). 
	int new_block; // recebe o novo bloco para o arquivo a ser criado.
	int new_entry; // recebe o a nova entrada para o arquivo a ser criado.
	char *str = arg;
	if(strcmp("/", str) && root_dir[0].filename[0] == 0){
		printf("diretório: %s\n", root_dir[0].filename);
		printf("mkdir: não foi possível criar o arquivo \“%s\”: diretório raiz inexistente.\n", str);
		return -1;
	}
	while(str[0] != '\0' && block > -3){ // enquanto não chegar no ultimo arquivo do caminho && não achar nada inválido.
						
		bkp = block; // realizando bkp do bloco do diretório anterior.
		block = findCluster(&str, block, &ptr_entry);		
	}
	dir_entry_t *dir;
	if(block == -3){ // se o caminho não for válido.
		
		printf("create: não foi possível criar '%s': arquivo ou diretório não encontrado\n", arg);
	}else{ // se o caminho for inválido.

		dir = is_root(bkp); // verificando se o diretório que o conterá é a raiz.
		if(block == -2 || (ignore && !dir[ptr_entry].attributes)){ // se nao existir arquivo com mesmo nome ou existir 
																   // e foi ordenado que ele seja sobrescrito.
			if(block == -2){ // se a entrada de diretório não existe.
				new_entry = free_entry(dir);
			}else{ // se sim.
				new_entry = ptr_entry;
			}
			if(free_entry > 0){ // se existirem entradas disponíveis no diretório.
				
				if(block == -2){ // se a entrada de diretório não existe.
					new_block = free_blocks(0);
				}else{ // se sim.
					new_block = block;
				}				
				if(new_block > -1){ // se existir bloco livre.
					set_dir_entry(dir, bkp, arg, new_entry, new_block, size_file, 0);
					// atualizando a fat.
					// se o arquivo não existe
					if(block == -2){
						fat[new_block] = end_file; // end_file sinaliza que este é o último bloco.			
						// persistindo a atualização da fat no arquivo.
						persist_on_disk(&fat[new_block], 2, FAT_ENTRY(block));
					}else{ //se o arquivo existe então remova os blocos dele da fat.
						int removed_blocks = new_block, aux;
						while(fat[removed_blocks] != end_file){
							aux = removed_blocks;
							removed_blocks = fat[removed_blocks];
							fat[aux] = not_used;
							// persistindo a atualização da fat no arquivo.
							persist_on_disk(&fat[aux], 2, FAT_ENTRY(aux));
						}
						fat[new_block] = end_file; //marcando novamnte o 1º bloco do arquivo como último.
						persist_on_disk(&fat[new_block], 2, FAT_ENTRY(new_block));
					}
					printf("new_block: %d\n", new_block);
					return new_block; // retorna o bloco do novo arquivo criado.
				}else{					
					printf("create: não foi possível criar o arquivo \"%s\""
					": limite máximo do disco atingido.\n", str);
				}
			}else{				
				printf("create: não foi possível criar o arquivo \"%s\""
				": não há entradas disponíveis neste diretório.\n", str);
			}
		}else{
			printf("create: não foi possível criar o arquivo “%s”: arquivo existe\n", arg);
		}
	}
	return -1;
}

int size_in_block(int n_bytes){

	if(n_bytes % k_bytes){ // se resto > 0
		return (n_bytes / k_bytes) + 1;
	}// se não.
	return (n_bytes / k_bytes);
}

int limit_disk(char *arg, char *path, short int *buff){
	
	int len = strlen(arg) + 1;
	// verificando a quantidade de blocos necessários para o arquivo.
	int n_blocks = size_in_block(len);
	int i, next;
	// verificando se existe disponível a quantidade de blocos requerida.
	// pelo arquivo.		
	i = 0;
	next = -1;
	while(i < n_blocks){ // enquanto i for menor que o numero de blocos.

		buff[i] = free_blocks(next + 1);
		if(buff[i] == -1){ // se nao existir blocos disponíveis.
			printf("write: não foi possível criar o arquivo \"%s\""
					": limite máximo do disco atingido.\n", path);
			return -1;
		}
		next = buff[i];
		++i;
	}
	return n_blocks;
}

void __write(char *arg, char *path){

	short int buff[100];
	memset(buff, 0xffff, sizeof(buff));	
	int n_blocks = limit_disk(arg, path, buff), len = strlen(arg) + 1;
	buff[0] = create_file(path, len, 1);
	if(buff[0] == -1){ // se algum erro ocorreu nao função create.
		return;
	}
	// se o tamanho do arquivo em disco for maior que o novo dado.
	int i = 0;
	while(i < n_blocks){ // persistindo dados no disco.
		// 2 
		if(i == n_blocks - 1){ // se for o restante da da string.
			persist_on_disk(arg + (i * k_bytes), strlen(arg + (i * k_bytes)) + 1, (buff[i] * k_bytes) + CLUSTER_DATA);
			fat[buff[i]] = end_file; // marcando o bloco como último.
		}else{ // se for o início ou o começo.
			persist_on_disk(arg + (i * k_bytes), k_bytes, (buff[i] * k_bytes) + CLUSTER_DATA);
			fat[buff[i]] = buff[i + 1];
		}
		persist_on_disk(&fat[buff[i]], 2, FAT_ENTRY(buff[i]));
		++i;
	}
}

void append(char *arg, char *path){

	int block = current_block; // recebe o bloco atual.
	int bkp; // salva bloco do diretório anterior ao arquivo ou diretório procurado.
	int ptr_entry = 0; // ponterio para entrada (otimização). 
	char *str = path;
	while(str[0] != '\0' && block > -3){ // enquanto não chegar no ultimo arquivo do caminho && não achar nada inválido.
						
		bkp = block; // realizando bkp do bloco do diretório anterior.
		block = findCluster(&str, block, &ptr_entry);		
	}
	if(block == -3){ // verificando se o caminho ate o arquivo e válido.

		printf("append: não foi possível criar '%s': arquivo ou diretório não encontrado\n", arg);
		return;
	}else if(block == -2){ // se o arquivo não exite.
		__write(arg, path);
	}else{ // se o arquivo existir.

		dir_entry_t *dir = is_root(bkp); // verificando se o arquivo esta na raiz.
		if(dir[ptr_entry].attributes){ // se o arquivo for um diretório.
			printf("append: não foi possível criar o arquivo “%s”: arquivo é um diretório\n", arg);
			return;
		}
				
		int last_block = block;
		int len = strlen(arg) + 1;
		while(fat[last_block] != end_file){ // enquanto não chegar no último bloco do arquivo.

			last_block = fat[last_block];			
		}
		int str_rest = (dir[ptr_entry].size % k_bytes) - 1, i = 0; // recebendo o len do dado já escrito.
		int free_space = (k_bytes - str_rest); // calculando o espaço livre no último bloco.
		// recebendo o tamanho do arquivo.
		// TO-DO Mudar o valor len do buff para 4086.
		short int buff[100];
		memset(buff, 0xffff, sizeof(buff));
		int n_blocks = limit_disk(arg + free_space, path, buff);
		if(n_blocks == -1){
			printf("write: não foi possível criar o arquivo \"%s\""
					": limite máximo do disco atingido.\n", path);
			return;
		}
		printf("len: %d\n", len);
		printf("str_rest: %d\n", str_rest);
		if(str_rest){ // se o tamanho do arquivo mod k_bytes for > 0.
			// realizando a primeira escrita (preenchendo o restante do bloco).			
			if(free_space <= len){ // se o espaço livre no bloco for menor que a string a ser escrita.
				// então preencha o restante do bloco até o máximo permitido.
				persist_on_disk(arg, free_space, (str_rest  + (last_block * k_bytes) + CLUSTER_DATA));
				fat[last_block] = buff[0]; // removendo marcação de último bloco do arquivo.
				persist_on_disk(&fat[last_block], 2, FAT_ENTRY(last_block));
			}else{ // se nao use o len do arg e grave o resto da string no bloco.
				set_dir_entry(dir, bkp, (char *) dir[ptr_entry].filename, ptr_entry, block, dir[ptr_entry].size + len - 1, 0);
				persist_on_disk(arg, len, (str_rest + (last_block * k_bytes) + CLUSTER_DATA));
				return;
			}
			while(i < n_blocks){ // persistindo dados no disco.

				if(i == n_blocks - 1){ // se for o restante da da string.
					persist_on_disk(&arg[((i * k_bytes) + free_space)],
						strlen(&arg[((i * k_bytes) + free_space)]),
							(buff[i] * k_bytes) + CLUSTER_DATA);
					fat[buff[i]] = end_file; // marcando o bloco como último.
				}else{ // se for o início ou o meio.
					persist_on_disk(&arg[((i * k_bytes) + free_space)], k_bytes, (buff[i] * k_bytes) + CLUSTER_DATA);
					fat[buff[i]] = buff[i + 1];
				}
				persist_on_disk(&fat[buff[i]], 2, FAT_ENTRY(buff[i]));
				++i;
			}
		}else{ // se o tamanho do arquivo mod k_bytes for 0.
			fat[last_block] = buff[0]; // atualizando o último bloco do arquivo na fat para os novos blocos.
			persist_on_disk(&fat[last_block], 2, FAT_ENTRY(last_block));
			persist_on_disk(arg, 1, last_block);
			while(i < n_blocks){ // persistindo dados no disco.

				if(i == n_blocks - 1){ // se for o restante da da string.
					
					persist_on_disk(arg + (i * k_bytes) + 1, strlen(arg + (i * k_bytes) + 1) + 1, (buff[i] * k_bytes) + CLUSTER_DATA);
					fat[buff[i]] = end_file; // marcando o bloco como último.
				}else{ // se for o início ou o começo.

					persist_on_disk(arg + (i * k_bytes) + 1, k_bytes, (buff[i] * k_bytes) + CLUSTER_DATA);
					fat[buff[i]] = buff[i + 1];
				}
				persist_on_disk(&fat[buff[i]], 2, FAT_ENTRY(buff[i]));
				++i;
			}
		}// atualizando a entrada de diretório.
		set_dir_entry(dir, bkp, (char *) dir[ptr_entry].filename, ptr_entry, block, dir[ptr_entry].size + len - 1, 0);
	}
}

void __read(char *arg){

	int block = current_block; // recebe o bloco atual.
	int bkp; // salva bloco do diretório anterior ao arquivo ou diretório procurado.
	int ptr_entry = 0; // ponterio para entrada (otimização). 
	char *str = arg;
	while(str[0] != '\0' && block > -3){ // enquanto não chegar no ultimo arquivo do caminho && não achar nada inválido.
						
		bkp = block; // realizando bkp do bloco do diretório anterior.
		block = findCluster(&str, block, &ptr_entry);
	}
	if(block < -1){ // verificando se o caminho ate o arquivo e válido
					// ou se o arquivo não existir.
		printf("read: não foi possível acessar '%s': arquivo ou diretório não encontrado\n", arg);
		return;
	}else{ // se ele existir.
		dir_entry_t *dir = is_root(bkp); // verificando se o arquivo esta na raiz.
		if(block == ROOT_BLOCK || dir[ptr_entry].attributes){ // se ele for um diretório.
			printf("read: não foi possível ler '%s': arquivo é um diretório.\n", arg);
			return;
		}
		int temp_block, i = 0, n_bytes = k_bytes;
		temp_block = block;
		char text[dir[ptr_entry].size];
		while(1){ // enquanto não percorrer todos os blocos do arquivo.
						
			if((dir[ptr_entry].size / (k_bytes * (i + 1))) == 0){ // se estiver no começo então escreva um bloco inteiro.				
				n_bytes = dir[ptr_entry].size % k_bytes;
			}
			memcpy(&text[(i * k_bytes)], read_cluster(temp_block), n_bytes);
			if(fat[temp_block] == end_file){ // se este for o último bloco do arquivo.	
				
				break;
			}
			temp_block = fat[temp_block];
			++i;
		}
		printf("%s\n", text);
	}
}

void __unlink(char *arg){

	int block = current_block; // recebe o bloco atual.
	int bkp; // salva bloco do diretório anterior ao arquivo ou diretório procurado.
	int ptr_entry = 0; // ponterio para entrada (otimização).
	int i;
	char *str = arg;
	while(str[0] != '\0' && block > -3){ // enquanto não chegar no ultimo arquivo do caminho && não achar nada inválido.
	
		bkp = block; // realizando bkp do bloco do diretório anterior.
		block = findCluster(&str, block, &ptr_entry);		
	}
	if(block < -1){ // verificando se o caminho ate o arquivo e válido
					// ou se o arquivo não existir.
		printf("unlink: não foi possível remover '%s': arquivo ou diretório não encontrado.\n", arg);
		return;
	}else if(block == ROOT_BLOCK){ // se estiver tenando apagar a raiz.
		for(i = 2; i < ENTRY_BY_CLUSTER; ++i){
			if(root_dir[i].filename[0] != 0){
				printf("unlink: não foi possível remover '/': diretório não vazio.\n");
				return;
			}
		}
		memset(root_dir, 0x00, k_bytes);
		persist_on_disk(root_dir, k_bytes, ROOT_ENTRY(0)); // apagou todo o disco (aparentemente impossível na prática).
	}else{
		int last_block = block, j;
		dir_entry_t *dir, *dir_target;
		dir = is_root(bkp); // verificando se o diretório que o contém é a raiz.
		if(dir[ptr_entry].attributes){ // se ele for um diretório.
			dir_target = (dir_entry_t *) read_cluster(block);
			for(i = 2; i < ENTRY_BY_CLUSTER; ++i){
				if(dir_target[i].filename[0] != 0){
					printf("unlink: não foi possível remover '%s': diretório não vazio.\n", arg);
					return;
				}
			}
		}
		while(fat[last_block] != end_file){ // enquanto não for o último bloco do arquivo.
			j = last_block;			
			last_block = fat[last_block];
			fat[j] = not_used; // marcando bloco como disponível.
			persist_on_disk(&fat[j], 2, FAT_ENTRY(j));
		}
		fat[last_block] = not_used; // marcando o último bloco como disponível.
		persist_on_disk(&fat[j], 2, FAT_ENTRY(last_block));
		memset(&dir[ptr_entry], 0x00, ENTRY_BY_CLUSTER); // apagando a entrada no diretório pai.
		if(dir == root_dir){	
			persist_on_disk(&dir[ptr_entry], ENTRY_BY_CLUSTER, ROOT_ENTRY(ptr_entry));
		}else{
			persist_on_disk(&dir[ptr_entry], ENTRY_BY_CLUSTER, CLUSTER_DATA + (bkp * k_bytes) + (ptr_entry * ENTRY_BY_CLUSTER));
		}
	}
}

/// ### GERENCIAMENTO DE BLOCOS ####

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
			fat[index] = not_used;
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