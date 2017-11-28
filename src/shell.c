#include "shell.h"

int format(char *str, char **comandos){
	
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
	int indice = 0, cont = 0;;
	i = bkp;
	while(str[i] != '\0'){
		
		if(str[i] != ' '){
					
			temp[cont] = str[i];
		}else{
			
			temp[i] = '\0';
			indice++;
			comandos[indice] = malloc(sizeof(char) * (strlen(temp)+1));
			/* copiando a string para sua respectiva posição na
			cadeia de comandos */
			strcpy(comandos[indice -1 ], temp);
			cont = -1;
			// removendo excesso de espaços entre arguemtos.
			while(str[i + 1] != '\0' && str[i + 1] == ' '){
				
				++i;
			}
		}
		++i;
		++cont;
	}	
	// retorna a quantidade de parâmetros.
	return j;
}

void shell(char *comando){
	
	char str[100];
	
	if(!strcmp("init", comando)){
		
		print("init")
	}else if(!strcmp("load", comando)){
	
	}else if(!strcmp("ls", comando)){
	
	}else if(!strcmp("create", comando)){
	
	}else if(!strcmp("unlink", comando)){
	
	}else if(!strcmp("write", comando)){
	
	}else if(!strcmp("append", comando)){
	
	}else if(!strcmp("read", comando)){
	
	}else{
		
		printf("comando %s não encontrado.\n", );
	}
}
int main(void){
	
	
	return 0;
}
