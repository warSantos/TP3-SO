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
/// marca o cluster inicial do espaço de dados.
#define CLUSTER_DATA (sizeof(boot_block) + sizeof(fat)  + sizeof(root_dir))
/// Mapeia posição de blocos em bytes da fat no disco através do id de um bloco.
#define FAT_ENTRY(ENTRY_BLOCK) (sizeof(boot_block) + (2 * ENTRY_BLOCK))
/// Mapeia entrada de diretórios da raiz no disco.
#define ROOT_ENTRY(ENTRY_DIR) (sizeof(boot_block) + sizeof(fat) + (ENTRY_DIR) * 32)
/// nome dado a partição (arquivo onde é realizado o descarregamento de dados).
#define fat_name	"fat.part"
/// define o bloco de referẽncia para o diretório root.
#define ROOT_BLOCK 65534

/// para operações como divisão e multiplicação (falha com define).
int k_bytes;

/// armazena o valor correspondente ao de final de arquivo (65533).
unsigned short end_file;

/// armazena o valor correspondente a bloc não usado.
unsigned short not_used;

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

/// Tabela fat
unsigned short fat[NUM_CLUSTER];

/// Tabele de armazenamento id de blocos livres.
unsigned short fat_sucessors[NUM_CLUSTER];

/// Aponta para o póximo bloco livre.
unsigned short next_available_block;

/// Bloco de boot
unsigned char boot_block[CLUSTER_SIZE];

/// Diretório root global
dir_entry_t root_dir[ENTRY_BY_CLUSTER];

/// Espaço de dados.
data_cluster clusters[4086];

/// Diretório atual no shell.
char pwd[500];

/// Define o cluster atual (para fins de otimização).
int current_block;

/* ### FUNÇÕES DE LEITURA E ESCRITA ### */

/*
 * Recebe um caminho e o bloco atual deste caminho. 
 * retorna o bloco do próximo diretório ou arquivo
 * e também o restante da string.
 * ex.: /home/user home == bloco 5
 * 
 * ### RETORNO ###
 * 65534: representa o bloco do diretório root
 * -2: caminho válido, porém arquivo ou diretório inexiste.
 * -3: caminho inválido.
 * >= 0: arquivo existente.
*/
int findCluster(char **path, int bloco_atual, int *ptr_entry);

/// Persiste um fluxo de dados no disco (fat_name).
void persist_on_disk(void *data, int size_data, int block);

/// Persiste um novo diretório no disco
void create_dir(dir_entry_t *parent_dir, int block_parent_dir, char *str);

/// Configura os parâmetros de uma entrada de diretório.
void set_dir_entry(dir_entry_t *parent_dir, int block_parent_dir, char *str, int new_entry, 
		int block, int size, int attributes);

/* 
 * Faz leitura de um cluster do "disco".
 * ### RETORNO ###
 * retornar um ponteiro data_cluster *,
 * ou NULL caso o endereço vá para fora do disco.
*/
data_cluster *read_cluster(int block);

/// É a função read_cluster com casting para (dir_entry *).
dir_entry_t *is_root(int block);

/*
 * Verifica se existe espaço para inserir mais uma entrada no diretório.
 * ### RETORNO ###
 * >=2, <32: posição vázia.
 * -1: não existe mais posições livres 
*/
int free_entry(dir_entry_t *t);

/* ### FUNÇÕES DE MANIPULAÇÃO DE BLOCOS ### */

/*
 * Procura blocos livres em O(n), utilizada para fins de debug.
 * ## RETORNO ###
 * >=0: se existir blocos livres.
 * -1: se não existir mais blocos livres.
*/
int free_blocks(int init);

/*
 * Retorna a quantidade de blocos necessários por um arquivo.
 * a partir do tamanho de sua string.
*/
int size_in_block(int n_bytes);

/*
 * Retorna a quantidade de espaço ocupado por uma string em 
 * blocos e mapeia os blocos disponíveis em um vetor. 
 * Retorna -1 quando não há espaço suficiente.
*/
int limit_disk(char *arg, char *path, short int *buff);

/* ### FUNÇÕES DE SUPORTE ###*/

/// Cria um diretório padrão. 
dir_entry_t *new_dir(int block, int block_parent_dir);

/// Imprime os atributos das entradas de um diretório
void lista_dir(dir_entry_t *dir, char oculto, char info);

/// Similar a strtok porém preserva a string.
char *__strtok(char *str, char delim);

/// Retorna o ultimo token de um caminho.
char *last_token(char *str, char delim);

/* ### FUNÇÕES DO SISTEMA DE ARQUIVO ### */

/// Formata o disco (fat.part)
void init(void);

/// Carrega o diretório ROOT e a FAT  para memória.
void load(void);

/*
 * Lista as entradas de diretório de um diretório.
 * Ex.: ls /home
 * ### PARÂMETROS ### 
 * -o: mostra diretórios ocultos.
 * Ex.: ls -o /home
*/
void ls(char *arg, char oculto, char info);

/*
 * Recebe o caminho e o nome do diretório a ser criado.
 * Se nenhum caminho for passado antes do diretório,
 * o diretório base é tomado como referência.
*/
void mkdir(char *arg);

/*
 * Recebe o caminho e o nome do arquivo a ser criado.
 * Se nenhum caminho for passado antes do diretório,
 * o diretório base é tomado como referência.
*/
int create_file(char *arg, int size_file, int ignore);

/*
 * Recebe o caminho e o nome do arquivo a ser criado.
 * Se nenhum caminho for passado antes do diretório,
 * o diretório base é tomado como referência.
 * Se o arquivo nao existir ele é criado.
 * Se o arquivo existir ele é sobrescrito.
*/
void __write(char *arg, char *path);

/*
 * Recebe o caminho e o nome do arquivo a ser criado.
 * Se nenhum caminho for passado antes do diretório,
 * o diretório base é tomado como referência.
 * Se o arquivo nao existir ele é criado.
 * Se o arquivo existir o texto é adicionado ao seu final.
*/
void append(char *arg, char *path);

/*
 * Recebe o caminho e o nome do arquivo e armazena o texto
 * em um buffer, printando ao final.
*/
void __read(char *arg);

/*
 * Recebe o caminho de um arquivo ou diretório, e remove-o
 * se possível, marcando os blocos do arquivo com livre
 * Diretório são removidos somente se estiverem vazios.
*/
void __unlink(char *arg);

#endif
