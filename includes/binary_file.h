#ifndef BINARY_HEADER_
#define BINARY_HEADER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct BINARY_HEADER {
	char status;
	long byteProxReg;
	int  nroRegistros;
	int  nroRegistrosRemovidos;
	char* descreve;
	int descreve_len;
} Bin_header;

Bin_header* header_read(FILE* bin,int description_len);

void header_free(Bin_header* header);

void header_write(char* bin_name, Bin_header* header);

char* read_entry(char is_removed, FILE* bin);

char** binary_load_to_memory(FILE* bin,Bin_header* bin_header);

#endif