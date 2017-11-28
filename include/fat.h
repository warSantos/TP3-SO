#ifndef fat_h
#define fat_h

/*INCLUDE*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*DATA DECLARATION*/
unsigned short fat[NUM_CLUSTER];
unsigned char boot_block[CLUSTER_SIZE];
dir_entry_t root_dir[ENTRY_BY_CLUSTER];
data_cluster clusters[4086];

void init();

void load();

#endif
