#ifndef shell_h
#define shell_h

#include "fat.h"

#define print(N) printf("comando: %s\n", N);

int format(char *str, char ***comandos);

void shell(void);

#endif
