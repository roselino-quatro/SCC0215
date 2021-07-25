// Gabriel Victor Cardoso Fernandes nUsp 11878296
// Lourenço de Salles Roselino nUsp 11796805
#include "../includes/binary_file.h"

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

char* read_entry(char is_removed, FILE* bin) {
	// 1. Le tamanho do registro
	int entry_len;
	fread(&entry_len, sizeof(int), 1, bin);

	// 2. Registro logicamente removido: retorna NULL
	if (is_removed == '0') {
		fseek(bin, entry_len, SEEK_CUR);
		return NULL;
	}

	// 3 Aloca memoria para o registro
	char* entry = malloc(entry_len + sizeof(char) + sizeof(int));

	// 4 Copia os campos 'removido', 'tamanho do registro' e lê o resto
	entry[0] = is_removed;
	memcpy(&entry[1], &entry_len, sizeof(int));
	fread(&entry[5], sizeof(char), entry_len, bin);

	return entry;
}

// Lê um arquivo binario de dados, e retorna um vetor de todos registros
// logicamente presentes no arquivo
char** binary_load_to_memory(FILE* bin,Bin_header* bin_header) {
	// 0. Alocando vetor de registros que vai armazenar os registros do arquivo binario de dados
	char** binary_entries = malloc((bin_header->nroRegistros) * sizeof(char*));
	int entries_pos = 0; // Posicao livre atual do vetor de registros

	fseek(bin, bin_header->descreve_len+17, SEEK_SET);
	// 1. Loop carregando registros do arquivo para memória: lê registro -> guarda no vetor de registros
	char removed;
	while ((fread(&removed, sizeof(char), 1, bin)) > 0) {
		char* entry = read_entry(removed, bin);
		if (entry == NULL) continue;

		binary_entries[entries_pos++] = entry;
	}

	return binary_entries;
}