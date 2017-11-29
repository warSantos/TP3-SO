#include "shell.h"

int format(char *str, char ***comandos){
	
	int i = 0, j = 0, bkp;
	
	// removendo espaços antes do comando
	while(str[i] == ' ' && str[i] != '\0'){
		
		i++;
	}
	
	// verifica se o usuário ficou digitando enter atoa
	if(str[i] == '\0'){ 
		// Comando vazio.
		
		return -1;
	}
	
	// marcando o inicio válido da string.
	bkp = i;
	
	// identificando a quantidade de argumentos.
	while(str[i] != '\0'){
		
		if(str[i] == ' '){
			
			j++;
		}
		++i;
	}
	j++;
	
	*comandos = malloc(sizeof(char *) * j);
	
	// separando os comandos.
	char temp[500];
	int indice = -1, cont = 0;;
	i = bkp;
	while(1){
		
		if(str[i] != ' ' && str[i] != '\0'){
					
			temp[cont] = str[i];	
		}else{
	
			temp[i] = '\0';
			indice++;			
			(*comandos)[indice] = malloc(sizeof(char) * (strlen(temp)+1));
			
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
	
	char str[1000];	
	char **comando;	

	printf("> ");
	scanf("%[^\n]", str);
	__fpurge(stdin);
	int parametros = format(str, &comando);

	if(!strcmp("init", comando[0])){		
		print("init")
	}else if(!strcmp("load", comando[0])){
		print("load")
	}else if(!strcmp("ls", comando[0])){
		print("ls")
	}else if(!strcmp("create", comando[0])){
		print("create")
	}else if(!strcmp("unlink", comando[0])){		
		print("ulink")
	}else if(!strcmp("write", comando[0])){
		print("write")
	}else if(!strcmp("append", comando[0])){
		print("append")
	}else if(!strcmp("read", comando[0])){
		print("read")
	}else if(!strcmp("quit", comando[0])){

		printf("Have a nice day!\n");
		exit(0);
	}else{
		
		printf("comando %s não encontrado.\n", comando[0]);
	}
}