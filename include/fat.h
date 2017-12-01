#ifndef fat_h
#define fat_h

/*INCLUDE*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <stdint.h>
#include <stdbool.h>

/*DEFINE*/
#define SECTOR_SIZE	512
#define CLUSTER_SIZE	2 * SECTOR_SIZE /// 1024
#define ENTRY_BY_CLUSTER CLUSTER_SIZE /sizeof(dir_entry_t) /// 32
#define NUM_CLUSTER	4096
#define fat_name	"fat.part"

typedef struct _dir_entry_t
{
	unsigned char filename[18];
	unsigned char attributes;
	unsigned char reserved[7];
	unsigned short first_block;
	unsigned int size;
}dir_entry_t;

typedef union _data_cluster{

	dir_entry_t dir[ENTRY_BY_CLUSTER]; /// 1024 bytes
	uint8_t data[CLUSTER_SIZE]; /// 1024 bytes
}data_cluster;

/*DATA DECLARATION*/

/// tabela fat
unsigned short fat[NUM_CLUSTER];

/// bloco de boot
unsigned char boot_block[CLUSTER_SIZE];

/// diretório root global
dir_entry_t root_dir[ENTRY_BY_CLUSTER];

/// espaço de dados.
data_cluster clusters[4086];

/// diretório atual no shell.
char pwd[500];

/// define o cluster atual (para fins de otimização).
int cluster_atual;

/*
 * Recebe um caminho e o bloco atual deste caminho. 
 * retorna o bloco do próximo diretório ou arquivo
 * e também o restante da string.
 * ex.: /home/user home == bloco 5
 * 
 * ### RETORNO ###
 * -1: representa o bloco do diretório root
 * -2: caminho inválido.
 * -3: caminho válido, porém arquivo ou diretório inexiste.
 * >= 0: arquivo existente.
*/

int findCluster(char **path, int bloco_atual, char atribute, int *ptr_enter);

void init();

void load();

/*
 * verifica se existe espaço para inserir mais uma entrada no diretório.
 * ### RETORNO ###
 * >=2, <32: posição vázia.
 * -1: não existe mais posições livres 
*/
int free_enter(dir_entry_t *t);

/*
 * Recebe o caminho e o nome do diretório a ser criado.
 * Se nenhum caminho for passado antes do diretório,
 * o diretório base é tomado como referência.
*/
void mkdir(char *arg);

#endif
