#include "shell.h"

int find_text(char *str, int indice){
	
	++indice;
	while(str[indice] != '\0' && str[indice] != '"'){	
		++indice;
	}
	if(str[indice] == '\0'){ // verifica se aspas foram abertas e não fechadas
		return -1;
	}
	return indice; // retorna posição onde as aspas foram fechadas.
}

int format(char *str, char ***comandos){
	
	int i = 0, j = 0, bkp, set = 0;
	int n_text = 0, text_end;
	// removendo espaços antes do comando
	while(str[i] == ' ' && str[i] != '\0'){
		
		i++;
	}		
	// verifica se a entrada esta vazia.
	if(str[i] == '\0'){ 
		// Comando vazio.		
		return -1;
	}
	// copiando o inicio válido da string.
	bkp = i;
	// identificando a quantidade de argumentos.
	while(str[i] != '\0'){
				
		if(str[i] == ' '){					
			
			set = 1;			
		}else if(set && str[i] != ' '){
						
			set = 0;
			j++;			
		}
		if(str[i] == '"'){ // identificando possíveis textos para write e append.
			
			if(n_text){ // se mais de um bloco de texto for passado.

				printf("espressão mal formada: excesso de parâmetros.\n");
				return -1;
			}			
			i = find_text(str, i);
			if(i == -1){ // se as aspas não foram fechadas.
				printf("expressão mal formada: \" esperado.\n");
				return i;
			}else if(str[i + 1] != ' '){ // se depóis do texto nao existir um espaço
				j++;					// aumente em um a quantidade de parãmetros.
			}
			text_end = i;
			n_text++;
		}	
		++i;
	}
	j++;
	*comandos = malloc(sizeof(char *) * j);
	
	// separando os comandos.
	int indice = -1, cont = 0;
	i = bkp;
	stage(200)
	char *temp = calloc(1,500);
	stage(300)

	while(1){

		if(str[i] != ' ' && str[i] != '\0'){
			
			if(str[i] == '"'){ // copiando a string completa para o novo arguemnto.				
				indice++;
				(*comandos)[indice] = malloc(text_end - i);				
				strncpy((*comandos)[indice], &str[i + 1], text_end - i - 1);
				(*comandos)[indice][text_end - i - 1] = '\0';
				for(i = text_end; str[i + 1] == ' '; ++i); // movendo o indice para depois da " (aspas) e
														   // também de possívies espaços.
				
				cont--;
			}else{		
				temp[cont] = str[i];				
			}			
		}else{
			temp[cont] = '\0';
			indice++;		
			(*comandos)[indice] = malloc((strlen(temp) + 1));			
			// copiando a string para sua respectiva posição na
			// cadeia de comandos.
			strcpy((*comandos)[indice], temp);
			cont = -1;
			// removendo excesso de espaços entre arguemtos.
			while(str[i] != '\0' && str[i + 1] != '\0' && str[i + 1] == ' '){				
				++i;
			}
			if(str[i] == '\0'){

				break;
			}
		}
		++i;
		++cont;
	}	
	// retorna a quantidade de parâmetros.
	return j;
}

void shell(void){
	
	char *str = malloc(1025);
	mkdir("/");
	//__write("teste write", "/file");
	append("teste append", "/file");
	while(1){
		inicio:
		str[0] = '\0';
		char **comando;		
		while(str[0] == '\0'){				
			printf("> ");
			scanf("%[^\n]", str);
			__fpurge(stdin);
		}
		int parametros = format(str, &comando), i;	
		if(parametros < 0){
			goto inicio;
		}
		
		if(!strcmp("init", comando[0])){				
			init();
		}else if(!strcmp("load", comando[0])){
			load();
		}else if(!strcmp("ls", comando[0])){
			char o = '-'; // parâmetro para arquivos ocultos.		
			if(parametros == 1){
				ls("\0", 'o', 'i'); // passa o diretório corrente.
			}else{
				for(i = 1; i < parametros; ++i){
					ls(comando[i], o, 'i');
				}
			}				
		}else if(!strcmp("create", comando[0])){
			if(parametros == 1){
				printf("create: não foi possível criar o arquivo: parâmetros insuficcientes.\n");
			}else{
				for(i = 1; i < parametros; ++i){
					create_file(comando[i], 0, 0);	
				}				
			}			
		}else if(!strcmp("mkdir", comando[0])){			
			if(parametros == 1){
				printf("mkdir: não foi possível criar o diretório: parâmetros insuficcientes.\n");
			}else{
				for(i = 1; i < parametros; ++i){
					mkdir(comando[i]);
				}				
			}
		}else if(!strcmp("unlink", comando[0])){		
			
		}else if(!strcmp("write", comando[0])){
			if(parametros != 3){
				printf("write: não foi possível escrever no arquivo: parâmetros inválidos.\n");
				goto inicio;
			}
			__write(comando[1], comando[2]);
		}else if(!strcmp("append", comando[0])){
			stage(1000)
			append(comando[1], comando[2]);
		}else if(!strcmp("read", comando[0])){
			if(parametros == 1){
				printf("read: não foi possível ler o arquivo: parâmetros insuficcientes.\n");
			}else{
				for(i = 1; i < parametros; ++i){
					__read(comando[i]);
				}				
			}			
		}else if(!strcmp("quit", comando[0])){
			if(parametros == 1){
				printf("Have a nice day!\n");
				exit(0);
			}
			printf("quit: este comando não possui parâmetros.\n");
			goto inicio;			
		}else{		
			printf("comando %s não encontrado.\n", comando[0]);
		}	
		free(comando);
	}
}