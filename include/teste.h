#ifndef teste_h
#define teste_h

#include "fat.h"
#include <time.h>

/// preenche um vetor de tamanho len com um caracter.
char *set_text(int len, char simb);

/// retorna uma string preenchida de simbolos de tamanho len.
char *get_text(int len);

/// retorna um char (utilizado para sortear um algoritmo).
char random_algorithm();

/*
 * Realiza operações em um vetor, principalmente criar arquivos
 * e diretórios porém também pode remove-los.
*/
void popular(char *path, int block, int altura, int *n_operacao);

/*
 * Cria uma arvore de diretórios com arquivos
 * e também remove arquivos durante o processo
 * de recursão.
 * Possui o objetivo de gerar a segmentação de blocos
 * na FAT de forma automática.
*/
void builder_tree(char *path, int block, int altura, int deep, int *n_operacao);

/*
 * Chama a função builder_tree e inicia as recursões a partir
 * do diretório raiz.
*/
void teste_generator();

/*
 * Calcula os nível de fragmentação da fat, baseado no númeoro
 * de aruqivos maiores que 1024 e no número de acessos não 
 * adjacentes.
*/
void fragmentacao();

#endif