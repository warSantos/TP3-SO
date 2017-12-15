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

char **format(char *str, int *n_parametros){
	char **comando;
	int i = 0, j = 0, bkp, set = 0;
	int n_text = 0, text_end;
	// removendo espaços antes do comando
	while(str[i] == ' ' && str[i] != '\0'){	
		i++;
	}		
	// verifica se a entrada esta vazia.
	if(str[i] == '\0'){ 
		// Comando vazio.		
		return NULL;
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
				return NULL;
			}			
			i = find_text(str, i);
			if(i == -1){ // se as aspas não foram fechadas.
				printf("expressão mal formada: \" esperado.\n");
				return NULL;
			}else if(str[i + 1] != ' '){ // se depóis do texto nao existir um espaço
				j++;					// aumente em um a quantidade de parãmetros.
			}
			text_end = i;
			n_text++;
		}	
		++i;
	}
	j++;
	comando = malloc(j);
	
	// separando os comandos.
	int indice = -1, cont = 0;
	i = bkp;
	//char *aux = malloc(500);
	char aux[500];
	while(1){

		if(str[i] != ' ' && str[i] != '\0'){
			
			if(str[i] == '"'){ // copiando a string completa para o novo arguemnto.				
				indice++;				
				comando[indice] = malloc(text_end - i);								
				strncpy(comando[indice], &str[i + 1], text_end - i - 1);
				comando[indice][text_end - i - 1] = '\0';
				for(i = text_end; str[i + 1] == ' '; ++i); // movendo o indice para depois da " (aspas) e
				cont--;								   	   // também de possívies espaços.								
			}else{		
				aux[cont] = str[i];				
			}			
		}else{
			aux[cont] = '\0';
			indice++;				
			comando[indice] = malloc((strlen(aux) + 1));		
			// copiando a string para sua respectiva posição na
			// cadeia de comandos.
			strcpy(comando[indice], aux);
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
	*n_parametros = j;
	return comando;
}

void shell(void){
	
	char *str = malloc(1025);
	mkdir("/");
	__write("teste write", "/file");
	//append("teste append", "/file");
	while(1){
		inicio:
		str[0] = '\0';
		char **comando;		
		int parametros, i;
		while(str[0] == '\0'){				
			printf("> ");
			scanf("%[^\n]", str);
			__fpurge(stdin);
		}
		comando = format(str, &parametros);	
		if(comando == NULL){
			goto inicio;
		}		
		if(!strcmp("init", comando[0])){				
			init();
		}else if(!strcmp("load", comando[0])){
			load();
		}else if(!strcmp("ls", comando[0])){
			char oculto = '-'; // parâmetro para arquivos ocultos.		
			char info = '-'; // parâmetros para mostrar informações.
			int optc = 0, options = 0;
			optind = 0;
			while((optc = getopt(parametros, comando, "oi")) != -1){
				
				switch(optc){
					case 'o' :
						oculto = 'o';						
						options++;
						break;
					case 'i' :
						info = 'i';
						options++;
						break;
					default :
						break;
				}
			}
			if((parametros - options) == 1){
				ls("\0", oculto, info); // passa o diretório corrente.
			}else{				
				for(i = 1; i < parametros; ++i){
					if(comando[i][0] != '-' || strlen(comando[i]) > 2){
						ls(comando[i], oculto, info);
					}
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
			if(parametros == 1){
				printf("unlink: não foi possível remover o arquivo: parâmetros insuficcientes.\n");
			}else{
				for(i = 1; i < parametros; ++i){
					__unlink(comando[i]);
				}				
			}
		}else if(!strcmp("write", comando[0])){
			if(parametros != 3){				
				printf("write: não foi possível escrever no arquivo: parâmetros inválidos.\n");
				goto inicio;
			}
			//printf("\n\nparametro: %s\n\n", comando[1]);
			__write(comando[1], comando[2]);
		}else if(!strcmp("append", comando[0])){
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
		}else if(!strcmp("fat", comando[0])){
			
			int limit = atoi(comando[1]), i;
			for(i = 0; i < limit; ++i){
				printf("indice: %d, próximo: %d\n", i, fat[i]);
			}
		}else{
			printf("comando %s não encontrado.\n", comando[0]);
		}	
		free(comando);
	}
}