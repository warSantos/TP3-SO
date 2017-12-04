#ifndef shell_h
#define shell_h

#include "fat.h"

#define print(N) printf("comando: %s\n", N);

int find_text(char *str, int indice);

/*
 * Formata o texto bruto capturado pelo scanf e
 * converte para uma matriz de comandos.
 * ### RETORNO ###
 * >=0: quantidade de argumentos (quantidade de vetores de char).
 * -1: erro.
*/
int format(char *str, char ***comandos);

void shell(void);

#endif
