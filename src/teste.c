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
        case 100:
            text = set_text(len, 'a');
            break;
        case 500:
            text = set_text(len, 'b');
            break;
        case 1000:
            text = set_text(len, 'c');
            break;
        case 1500:
            text = set_text(len, 'd');
            break;
    }
    return text;
}

char random_algorithm(){

    return "mwmmmw"[random() % 6];
    //return "mmmmmm"[random() % 6];
}

void popular(char *path, int block, int deep){

    int qtde_operacoes = 0; // contador de quantidade de operações para cada diretório.
    int limite_operacoes = 5; // limite de subarquivos para cada diretório.    
    int empty_entry; // recebe entrada vazias do diretório.
    int algoritmo; // recebe o char aleatório correspondente a um algoritmo.
    int size_text[4]; // vetor de configuração de tamanho dos arquivos.
    int cont = 0; // conta quantos arquivos ou diretórios foram criados.
    int len; // tamanho da string para as funções __write e append.
    // Tamanho de arquivos
    size_text[0] = 100;
    size_text[1] = 500;
    size_text[2] = 1000;
    size_text[3] = 1500;
    dir_entry_t *dir = is_root(block);
    char simb;
    char arg[100];
    char *text; // armazena o texto para as funções __write e append.
    empty_entry = 1;
    // enquanto existir entradas de diretório livres e o limite de arquivos não for atingido.
    while((empty_entry = free_entry(dir) > 0) && qtde_operacoes < limite_operacoes){
        
        simb = (char) (97 + cont);
        sprintf(arg, "%s%d%c", path, deep, simb);
        algoritmo = random_algorithm();
        switch(algoritmo){ // defini qual operação será realizada (write, append, unlink e mkdir)

            case 'a': // append
                //append();
                break;
            case 'm': // mkdir
                                
                //printf("arg: %s altura: %d simb: %c\n", arg, deep, simb);
                mkdir(arg);
                cont++;
                break;
            case 'u': // unlink
                //unlink();
                break;
            case 'w': // write
                printf("len: %d %d\n", len, size_text[len]);
                len = 3;//random() % 4;                
                text = get_text(size_text[len]);
                __write(text, arg);
                free(text);
                cont++;
                break;
        }
        qtde_operacoes++;
    }
}

void builder_tree(char *path, int block, int altura, int deep){

    // populando o diretório.
    popular(path, block, deep); 
    if(altura == deep) // se estiver atingido o limite de altura da arvore, então retorne.
        return;
    dir_entry_t *dir = is_root(block);
    char new_arg[200];
    int ptr_entry;
    for(ptr_entry = 2; ptr_entry < ENTRY_BY_CLUSTER; ++ptr_entry){

        if(dir[ptr_entry].attributes){ // se for um diretório então chame a recursão para ele.

            sprintf(new_arg, "%s%s/", path, dir[ptr_entry].filename);  
            // chamando a recursão
            builder_tree(new_arg, dir[ptr_entry].first_block, altura, deep + 1);
        }        
    }
}

void teste_generator(){

    srandom(time(NULL));
    init(); // formatando o disco
    mkdir("/"); // criando o diretório barra.
    int altura = 3; // altura da arvore de diretórios.
    // iniciando a árvore a partir da raiz.
    builder_tree("/", ROOT_BLOCK, altura, 0);
}

/*

    popula
    se nao esta no limite da altura da arvore (deep == altura)
        chama reccursao para seus filhos
    se estiver
        retorne
*/