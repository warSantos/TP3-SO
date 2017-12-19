#include "teste.h"

char *set_text(int len, char simb){
    int i;
    char *text = malloc(len + 1);
    for(i = 0; i < len; ++i)
        text[i] = simb;
    return text;
}

char *get_text(int len){

    char *text;
    switch(len){
        case 500:
            text = set_text(len, 'a');
            break;
        case 750:
            text = set_text(len, 'b');
            break;
        case 1000:
            text = set_text(len, 'c');
            break;
        case 1250:
            text = set_text(len, 'd');
            break;
        case 1500:
            text = set_text(len, 'e');
            break;
        case 1750:
            text = set_text(len, 'f');
            break;
        case 2000:
            text = set_text(len, 'g');
            break;
    }
    return text;
}

char random_algorithm(){

    //return "mmmmwwwwaaaaaauuuuuu"[random() % 20];
    return "mwamwmuwuauawmuauwum"[random() % 20];
}

void popular(char *path, int block, int deep, int limite_operacoes, int *n_operacao){

    int qtde_operacoes = 0; // contador de quantidade de operações para cada diretório.    
    int empty_entry; // recebe entrada vazias do diretório.
    int algoritmo; // recebe o char aleatório correspondente a um algoritmo.    
    int cont = 0; // conta quantos arquivos ou diretórios foram criados.
    int len; // recebe valor aleatório entre 0 e 4.
    int i;
    int *files = calloc(limite_operacoes, sizeof(int)); // vetor para marcar posições de arquivos.
    // Tamanho de arquivos
    int size_text[7]; // vetor de configuração de tamanho dos arquivos.
    size_text[0] = 500;
    size_text[1] = 750;
    size_text[2] = 1000;
    size_text[3] = 1250;
    size_text[4] = 1500;
    size_text[5] = 1750;
    size_text[6] = 2000;
    dir_entry_t *dir = is_root(block);
    char simb;
    char new_path[100];
    char *text;
    empty_entry = 1;
    // enquanto existir entradas de diretório livres e o limite de arquivos não for atingido.
    while((empty_entry = free_entry(dir) > 0) && qtde_operacoes < limite_operacoes){
        
        algoritmo = random_algorithm();
        switch(algoritmo){ // defini qual operação será realizada (write, append, unlink e mkdir)

            case 'a': // append
                for(i = 0; i < cont; ++i){
                    if(files[i] == 1){
                        simb = (char) (97 + i);
                        sprintf(new_path, "%s%d%c", path, deep, simb);
                        len = random() % 7;
                        text = get_text(size_text[len]);
                        append(text, new_path);
                        files[i] = 0;
                        break;
                    }
                }
                n_operacao[0]++;
                break;
            case 'm': // mkdir
                simb = (char) (97 + cont);
                sprintf(new_path, "%s%d%c", path, deep, simb);
                mkdir(new_path);
                files[cont] = 2;
                cont++;
                n_operacao[1]++;
                break;
            case 'u': // unlink
                for(i = 0; i < cont; ++i){
                    if(files[i] == 1 || files[i] == 2){
                        simb = (char) (97 + i);
                        sprintf(new_path, "%s%d%c", path, deep, simb);                        
                        __unlink(new_path);
                        files[i] = 0;
                        break;
                    }   
                }
                n_operacao[2]++;
                break;
            case 'w': // write
                simb = (char) (97 + cont);
                sprintf(new_path, "%s%d%c", path, deep, simb);
                len = random() % 7;
                text = get_text(size_text[len]);
                __write(text, new_path);
                files[cont] = 1;
                cont++;
                n_operacao[3]++;
                break;
        }
        qtde_operacoes++;
    }
    free(files);
}

void builder_tree(char *path, int block, int altura, int deep, int limite_operacoes, int *n_operacao){

    // populando o diretório.
    popular(path, block, deep, limite_operacoes, n_operacao);
    if(altura == deep) // se estiver atingido o limite de altura da arvore, então retorne.
        return;
    dir_entry_t *dir = is_root(block);
    char new_arg[200];
    int ptr_entry;
    for(ptr_entry = 2; ptr_entry < ENTRY_BY_CLUSTER; ++ptr_entry){

        if(dir[ptr_entry].attributes){ // se for um diretório então chame a recursão para ele.

            sprintf(new_arg, "%s%s/", path, dir[ptr_entry].filename);  
            // chamando a recursão
            builder_tree(new_arg, dir[ptr_entry].first_block, altura, deep + 1, limite_operacoes, n_operacao);
        }       
    }
}

void teste_generator(int altura, int limite_operacoes){

    srandom(time(NULL));
    init(); // formatando o disco
    mkdir("/"); // criando o diretório barra.
    
    // iniciando a árvore a partir da raiz.
     int n_operacao[4] = {0, 0, 0, 0}; // vetor para contabilizar a qtde de cada uma operação
    builder_tree("/", ROOT_BLOCK, altura, 0, limite_operacoes, n_operacao);
    printf("appends:\t%d\n", n_operacao[0]);
    printf("mkdir:\t\t%d\n", n_operacao[1]);
    printf("unlinks:\t%d\n", n_operacao[2]);
    printf("writes:\t\t%d\n", n_operacao[3]);
}

void fragmentacao(void){

    int single_block = 0; // contabiliza a quantidade de arquivos com tamanho <= 1024 (um bloco).
    int mult_block = 0; // contabiliza a quantidade de arquivos com tamanho > 1024.
    int first_block; // recebe o bloco inicial de um arquivo.
    int n_adj = 0; // contabiliza o número de leituras adjacentes.
    int n_not_adj = 0;
    int i;
    int *vetor = calloc(4086, sizeof(int)); // marca os arquivos já acessados.
    for(i = 0; i < 4086; ++i){
        if(fat[i] != not_used){ // se estiver ocupado
            if(!vetor[i]){ // se ainda não estiver passado por este bloco.                
                if(fat[i] == end_file){ // se for um arquivo de um único bloco e ainda não analisado.
                    single_block++;
                    vetor[i] = 1;
                }else if(fat[i] < not_used){
                    mult_block++;
                    first_block = i;                                      
                    while(fat[first_block] != end_file){ // enquanto não cheagar no fim do arquivo.
                        vetor[first_block] = 1;
                        if(first_block == fat[first_block] - 1){ // se o bloco for adjacente ao último lido.
                            n_adj++;
                        }else{
                            n_not_adj++;
                        }
                        first_block = fat[first_block];
                    }
                    vetor[first_block] = 1;                    
                }
            }
        }
    }
    printf("Qtde de arquivos e diretórios......: %d\n", mult_block + single_block);
    printf("Arquivos <= 1024 bytes.............: %d\n", single_block);
    printf("Arquivos > 1024 bytes..............: %d\n", mult_block);
    printf("Acessos contíguos..................: %d\n", n_adj);
    printf("Acessos não contíguos..............: %d\n", n_not_adj);
    printf("Média de arquivos segmentados......: %.2f\n", mult_block / (float) (mult_block + single_block));
    printf("Média de seguimentação por arquivo.: %.2f\n", (n_not_adj / (float) mult_block));
    free(vetor);
}