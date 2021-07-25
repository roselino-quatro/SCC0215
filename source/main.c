// Gabriel Victor Cardoso Fernandes nUsp 11878296
// Lourenço de Salles Roselino nUsp 11796805
#include "../includes/btree.h"
#include "../includes/utils.h"
#include "../includes/vehicle.h"
#include "../includes/line.h"

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

// Funcao 15 do trabalho 3  ——— Iterar sobre veiculos e sobre linhas, printrando
//                                 registros correspondentes por "codigo de linha".
void join_bruteforce(char* vehicle_name,char* line_name) {
	// 0. Abrindo e validando arquivos utilizados
	FILE* vehicle_bin = fopen_valid(vehicle_name, "rb");
	FILE* line_bin = fopen_valid(line_name, "rb");

	if (vehicle_bin == NULL || line_bin == NULL){
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	
	// 1. Variavel rastreando se — QUALQUER — par foi feito.
	// Caso nenhum seja feito, printar "Registro inexistente.\n"
	char any_code_found = 0;

	// 1.1 Pulando header do arquivo veiculo
	fseek(vehicle_bin, VEHICLE_DESCRIPTION_LEN+17, SEEK_SET);
	
	// 2. Iterando sobre cada veiculo em veiculos
	char removed;
	while ((fread(&removed, sizeof(char), 1, vehicle_bin)) > 0) {
		char* vehicle = read_entry(removed, vehicle_bin);
		if (vehicle == NULL) continue;

		// 2.1 Pega, em veiculo, campo que vai ser comparado — codigo de linha
		int vehicle_line_code = vehicle_get_line_code(vehicle);

		fseek(line_bin, LINE_DESCRIPTION_LEN+17, SEEK_SET);
		// 3. Iterando sobre cada linha em linhas
		// Inicia busca pelo "codigo de linha" correspondente em linha.bin
		while ((fread(&removed, sizeof(char), 1, line_bin)) > 0) {
			char* line = read_entry(removed, line_bin);
			if (line == NULL) continue;

			// 3.1 Pega, em veiculo, campo que vai ser comparado — codigo de linha
			int line_code = line_get_key(line);

			// 3.2 Caso seja linha correspondente: printa veiculo -> printa linha
			// Tambem marcar que pelo menos 1 par foi achado, entao nao printa "Registro inexistente.\n"
			if (vehicle_line_code == line_code) {
				display_vehicle_from_data(vehicle);
				display_line_from_data(line);
				any_code_found = 1;

				free(line);
				break;
			}

			free(line);
		}

		free(vehicle);
	}

	// 4. Caso não haja nenhuma junção
	if (!any_code_found) {
		printf("Registro inexistente.\n");
	}

	// 5. Fechar os arquivos abertos
	fclose(vehicle_bin);
	fclose(line_bin);
}

// Funcao 16 do trabalho 3 ——— Iterar sobre veiculos e buscar, no indice das linhas, um registro
//                                correspondente em "codigo de linha", depois printar o par.
void join_simple(char* vehicle_name,char* line_name,char* line_btree_name) {
	// 0. Abrindo arquivos utilizados
	Btree* line_btree = btree_read_header(line_btree_name);
	FILE* vehicle_bin = fopen_valid(vehicle_name, "rb");
	FILE* line_bin = fopen_valid(line_name, "rb");

	if (line_btree == NULL || vehicle_bin == NULL || line_bin == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	// 1. Variavel rastreando se — QUALQUER — par foi feito.
	// Caso nenhum seja feito, printar "Registro inexistente.\n"
	char any_code_found = 0;
	
	// 1.1 Pulando header do veiculo
	fseek(vehicle_bin, VEHICLE_DESCRIPTION_LEN+17, SEEK_SET);

	// 2. Iterando sobre cada veiculo em veiculos
	char removed;
	while ((fread(&removed, sizeof(char), 1, vehicle_bin)) > 0) {
		char* vehicle = read_entry(removed, vehicle_bin);
		if (vehicle == NULL) continue;

		// 2.5 Pega, em veiculo, campo que vai ser comparado — codigo de linha
		int vehicle_line_code = vehicle_get_line_code(vehicle);

		// 2. Procurar pela key no arquivo de indice (btree)
		long line_offset = search_btree(line_btree, vehicle_line_code);

		// 3. Se a chave foi achada, pular pra posicao do registro
		if (line_offset >= 0) {
			fseek(line_bin, line_offset, SEEK_SET);

			// 3.1 Pegar o registro correspondente, no arquivo binario
			char removed;
			fread(&removed, sizeof(char), 1, line_bin);

			char* line = read_entry(removed, line_bin);
			if (line == NULL) continue;


			// 3.2 Printar registro veiculo e registro linha correspondentes
			display_vehicle_from_data(vehicle);
			display_line_from_data(line);
			any_code_found = 1;

			free(line);
		}
		free(vehicle); // Free no registro veiculo lido
	}

	// 5. Caso não haja nenhuma junção
	if (!any_code_found) {
		printf("Registro inexistente.\n");
	}

	// 6. Fechar os arquivos abertos
	fclose(vehicle_bin);	
	fclose(line_bin);
	btree_delete(line_btree);
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

// Funcao 17 e 18 do trabalho 3 ——— Ler todos registros de um arquivo entrada, ordenar os registros
//                                     em memória, e escrever os registros ordenados em um novo arquivo.
void sort_table(int op,char* in_name,char* out_name) {
	// 0. Especificar qual tipo de dado estamos ordenando: veiculo ou linha
	int (*entry_compare)(const void*,const void*);
	int header_description_len;
	if (op == 17) {
		entry_compare = vehicle_cmp;
		header_description_len = VEHICLE_DESCRIPTION_LEN;
	} else {
		entry_compare = line_cmp;
		header_description_len = LINE_DESCRIPTION_LEN;
	}

	// 1. Carregando registros do arquivo para memória
	FILE* bin = fopen_valid(in_name, "rb");
	if (bin == NULL){
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	Bin_header* bin_header = header_read(bin, header_description_len);
	char** binary_entries = binary_load_to_memory(bin, bin_header);
	fclose(bin);	

	// 2. Ordenando vetor de registros
	qsort(binary_entries, bin_header->nroRegistros, sizeof(char*), entry_compare);

	// 3. Salvando quantidade de registros
	int entry_qty = bin_header->nroRegistros;

	// 4. Criando arquivo de saida.
	FILE* bin_out = fopen(out_name, "wb");
	bin_header->nroRegistros = 0;
	bin_header->nroRegistrosRemovidos = 0;
	bin_header->status = '0';
	bin_header->byteProxReg = header_description_len + 17;
	header_write(out_name, bin_header);

	// 4.1 Pulando header no arquivo de saida
	fseek(bin_out, header_description_len+17, SEEK_SET);
	
	// 5. Escrevendo o vetor de registros, ordenado, no arquivo de saida.
	for (int i = 0; i < entry_qty; i++) {
		// 5.1 Lendo tamanho do registro atual
		char* entry = binary_entries[i];
		int entry_data_len;
		memcpy(&entry_data_len, &entry[1], sizeof(int));

		// 5.2 Escrevendo registro atual no arquivo de saida
		fwrite(entry, sizeof(char)+sizeof(int)+entry_data_len, 1, bin_out);

		// 5.3 Liberando memoria do registro atual
		free(entry);

		// 5.4 Atualizando informacao do cabecalho
		bin_header->nroRegistros++;
		bin_header->byteProxReg += sizeof(char)+sizeof(int)+entry_data_len;
	}

	// 6. Escrevendo cabecalho no arquivo de saida
	bin_header->status = '1';
	header_write(out_name, bin_header);

	// 7. Fechando arquivos e liberando memoria
	header_free(bin_header);
	free(binary_entries);
	fclose(bin_out);
}

// Funcao 19 do trabalho 3 ——— Carregar veiculos e linhas na memoria, depois
//                                printar matches usando two-pointer approach.
void merge_tables(char* vehicle_bin_name,char* line_bin_name) {
	// 0. Criando arquivos intermediarios, ordenados, que vao ser utilizados
	sort_table(17, vehicle_bin_name, "veiculo_merge.bin");
	sort_table(18, line_bin_name, "line_merge.bin");

	// 1. Carregando registros de veiculo na memória
	FILE* vehicle_bin = fopen_valid("veiculo_merge.bin", "rb");
	if (vehicle_bin == NULL) return;

	Bin_header* vehicle_header = header_read(vehicle_bin, VEHICLE_DESCRIPTION_LEN);
	char** vehicle_entries = binary_load_to_memory(vehicle_bin, vehicle_header);
	fclose(vehicle_bin);

	// 2. Carregando registros de linha na memória
	FILE* line_bin = fopen_valid("line_merge.bin", "rb");
	if (line_bin == NULL) return;
	
	Bin_header* line_header = header_read(line_bin, LINE_DESCRIPTION_LEN);
	char** line_entries = binary_load_to_memory(line_bin, line_header);
	fclose(line_bin);

	// 3. Criando ponteiros de indice para registros de veiculo e registros de linha
	int vehicle_qty = vehicle_header->nroRegistros;
	header_free(vehicle_header);
	int vehicle_pos = 0;

	int line_qty = line_header->nroRegistros;
	header_free(line_header);
	int line_pos = 0;

	char any_match_occured = 0;
	// 4. Loop fazendo o merge
	while (vehicle_pos < vehicle_qty && line_pos < line_qty) {
		// 4.1 Pegar o campo "codigo de linha" dos dois registrados apontados
		char* vehicle = vehicle_entries[vehicle_pos];
		int vehicle_line_code = vehicle_get_line_code(vehicle);
		
		char* line = line_entries[line_pos];
		int line_code = line_get_key(line);

		// 4.2 Comparar o "codigo de linha" dos dois registros apontados
		if (vehicle_line_code == line_code) {
			// 5. Merge! Printar o registro veiculo e o registro linha correspondente
			display_vehicle_from_data(vehicle);
			display_line_from_data(line);
			any_match_occured = 1;
			
			// 5.1 Liberar memoria dos registros mostrados
			free(vehicle);
			
			// 5.2 Andar ponteiro
			vehicle_pos++;
		} else if(vehicle_line_code < line_code) {
			// 6. Liberar memoria do registro veiculo e andar seu ponteiro
			free(vehicle);
			vehicle_pos++;
		} else {
			// 7. Liberar memoria do registro linha e andar seu ponteiro
			free(line);
			line_pos++;
		}
	}

	// 8. Caso nenhum par seja printado, printar "Registro inexistente."
	if (!any_match_occured) {
		printf("Registro inexistente.\n");
	}

	// 9. Terminar de liberar a memoria do vetor de maior tamanho
	while (vehicle_pos < vehicle_qty) {
		free(vehicle_entries[vehicle_pos]);
		vehicle_pos++;
	}
	free(vehicle_entries);

	while (line_pos < line_qty) {
		free(line_entries[line_pos]);
		line_pos++;
	}
	free(line_entries);
}

void trabalho3_menu(char** arguments) {
	int operation = atoi(arguments[0]);
	switch (operation) {
		case 15:
			join_bruteforce(arguments[1], arguments[2]);
			break;
		case 16:
			join_simple(arguments[1], arguments[2], arguments[5]);
			break;
		case 17:
		case 18:
			sort_table(operation, arguments[1], arguments[2]);

			FILE* sorted_table = fopen_valid(arguments[2], "rb");
			if (sorted_table) {
				binarioNaTela(arguments[2]);
				fclose(sorted_table);
			}
			
			break;
		case 19:
			merge_tables(arguments[1], arguments[2]);
			break;
	}
}

int main(){
	// 1. Lê o comando na stdin e extrai os argumentos dele
	char* operation = readline(stdin);
	char** arguments = arguments_get(operation);
	free(operation);

	// 2. Chama o menu e realiza a operacao selecionada
	trabalho3_menu(arguments);

	// 3. Limpa a memoria do menu e sai do programa
	arguments_free(arguments);
	return 0;
}