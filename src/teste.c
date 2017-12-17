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

    return "mmmmwwwaau"[random() % 10];
}

void popular(char *path, int block, int deep){

    int qtde_operacoes = 0; // contador de quantidade de operações para cada diretório.
    int limite_operacoes = 10; // limite de subarquivos para cada diretório.    
    int empty_entry; // recebe entrada vazias do diretório.
    int algoritmo; // recebe o char aleatório correspondente a um algoritmo.    
    int cont = 0; // conta quantos arquivos ou diretórios foram criados.
    int len; // recebe valor aleatório entre 0 e 4.
    int i;
    int files[10] = {0, 0, 0, 0, 0}; // vetor de marcar posições de arquivos.
    // Tamanho de arquivos
    int size_text[4]; // vetor de configuração de tamanho dos arquivos.
    size_text[0] = 100;
    size_text[1] = 500;
    size_text[2] = 1000;
    size_text[3] = 1500;
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
                        //printf("append: new_path: %s altura: %d simb: %c\n", new_path, deep, simb);
                        len = random() % 4;
                        text = get_text(size_text[len]);
                        append(text, new_path);
                        files[i] = 0;
                        break;
                    }
                }
                break;
            case 'm': // mkdir
                simb = (char) (97 + cont);
                sprintf(new_path, "%s%d%c", path, deep, simb);
                //printf("new_path: %s altura: %d simb: %c\n", new_path, deep, simb);
                mkdir(new_path);
                files[cont] = 2;
                cont++;
                break;
            case 'u': // unlink
                for(i = 0; i < cont; ++i){
                    if(files[i] == 1 || files[i] == 2){
                        simb = (char) (97 + i);
                        sprintf(new_path, "%s%d%c", path, deep, simb);
                        printf("unlink: new_path: %s altura: %d simb: %c\n", new_path, deep, simb);
                        __unlink(new_path);
                        files[i] = 0;
                        break;
                    }   
                }
                break;
            case 'w': // write
                simb = (char) (97 + cont);
                sprintf(new_path, "%s%d%c", path, deep, simb);
                //printf("new_path: %s altura: %d simb: %c\n", new_path, deep, simb);
                len = random() % 4;
                text = get_text(size_text[len]);
                __write(text, new_path);
                files[cont] = 1;
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
    int altura = 5; // altura da arvore de diretórios.
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