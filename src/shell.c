#include "shell.h"

int format(char *str, char ***comandos){
	
	int i = 0, j = 0, bkp, set = 0;	
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
		++i;
	}
	j++;	
	*comandos = malloc(sizeof(char *) * j);
	// separando os comandos.
	int indice = -1, cont = 0;;
	i = bkp;
	char *temp = malloc(500);
	while(1){
		
		if(str[i] != ' ' && str[i] != '\0'){
								
			temp[cont] = str[i];
		}else{
			
			temp[cont] = '\0';
			indice++;			
			(*comandos)[indice] = malloc(sizeof(char) * (strlen(temp) + 1));			
			/* copiando a string para sua respectiva posição na
			cadeia de comandos */
			strcpy((*comandos)[indice], temp);
			cont = -1;
			// removendo excesso de espaços entre arguemtos.
			while(str[i + 1] != '\0' && str[i + 1] == ' '){
				
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
	
	char *str = calloc(1, 1001);
	
	while(1){

		str[0] = '\0';
		char **comando;		
		while(str[0] == '\0'){	
			
			printf("> ");
			scanf("%[^\n]", str);
			__fpurge(stdin);
			//printf("str: %s\n", str);
		}
		int parametros = format(str, &comando), i;	
		//printf("str: %s comando: %s\n", str, comando[0]);
		if(!strcmp("init", comando[0])){				
			init();
		}else if(!strcmp("load", comando[0])){
			load();
		}else if(!strcmp("ls", comando[0])){
			char o = '-'; // parâmetro para arquivos ocultos.		
			if(parametros == 1){
				ls("\0", o); // passa o diretório corrente.
			}else{
				for(i = 1; i < parametros; ++i){
					ls(comando[i], o);
				}
			}				
		}else if(!strcmp("create", comando[0])){
		
		}else if(!strcmp("mkdir", comando[0])){
			mkdir(comando[1]);
		}else if(!strcmp("unlink", comando[0])){		
			
		}else if(!strcmp("write", comando[0])){
			
		}else if(!strcmp("append", comando[0])){
			
		}else if(!strcmp("read", comando[0])){
			
		}else if(!strcmp("quit", comando[0])){
			
			printf("Have a nice day!\n");
			exit(0);
		}else{		
			printf("comando %s não encontrado.\n", comando[0]);
		}	
		free(comando);
	}
}