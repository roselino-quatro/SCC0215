// Gabriel Victor Cardoso Fernandes nUsp 11878296
// Lourenço de Salles Roselino nUsp 11796805
#include "../includes/binary_header.h"

Bin_header* header_read(FILE* bin,int description_len) {
	// 0. Volta para inicio do arquivo
	rewind(bin);

	// 1. Inicializa o header lendo os campos comuns
	Bin_header* header = malloc(sizeof(Bin_header));
	fread(&header->status,                sizeof(char), 1, bin);
	fread(&header->byteProxReg,           sizeof(long), 1, bin);
	fread(&header->nroRegistros,          sizeof(int), 1, bin);
	fread(&header->nroRegistrosRemovidos, sizeof(int), 1, bin);

	// 2. Lê a descrição do header, de acordo com o tamanho passado
	char* description = malloc(sizeof(char) * description_len);
	fread(description, sizeof(char), description_len, bin);

	// 3. Seta a descrição do header para o lido no binário
	header->descreve = description;
	header->descreve_len = description_len;
	
	return header;
}

void header_free(Bin_header* header) {
	free(header->descreve);
	free(header);
}

void header_write(char* bin_name, Bin_header* header) {
	FILE* bin = fopen(bin_name, "rb+");
	fwrite(&header->status,                sizeof(char), 1, bin);
	fwrite(&header->byteProxReg,           sizeof(long), 1, bin);
	fwrite(&header->nroRegistros,          sizeof(int),  1, bin);
	fwrite(&header->nroRegistrosRemovidos, sizeof(int),  1, bin);
	fwrite(header->descreve,               sizeof(char), header->descreve_len, bin);
	fclose(bin);
}
