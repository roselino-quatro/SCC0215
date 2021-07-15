// Gabriel Victor Cardoso Fernandes nUsp 11878296
// Lourenço de Salles Roselino nUsp 11796805
#include "../includes/btree.h"
#include "../includes/utils.h"
#include "../includes/vehicle.h"
#include "../includes/line.h"

// Função 9 e 10 do trabalho 2  ——— Ler registro do csv, inserir no .bin e inserir na btree
void create_index(int op,char* bin_name,char* btree_name) {
	// 0. Abrindo arquivos utilizados
	FILE* bin = fopen_safe(bin_name, "rb");
	Btree* btree = btree_new(btree_name);

	// 1. Lendo header e escrevendo com status '0'
	int description_len = (op == 9)? VEHICLE_DESCRIPTION_LEN : LINE_DESCRIPTION_LEN;
	Bin_header* header = header_read(bin, description_len);
	header->status = '0'; // Comeca marcado com instavel
	header_write(bin_name, header);

	// 2. Ponteiro para funcao que vai pegar a chave a partir dos dados
	int (*entry_get_key)(char*) = vehicle_get_key;
	if (op == 10) entry_get_key = line_get_key;

	// 3. Loop processando binario: lê registro -> adiciona na btree
	char removed;
	while ((fread(&removed, sizeof(char), 1, bin)) > 0) {
		// 3.1 Le tamanho do registro
		int entry_len;
		fread(&entry_len, sizeof(int), 1, bin);

		// 3.2 Registro logicamente removido: pula para proximo loop
		if (removed == '0') {
			fseek(bin, entry_len, SEEK_CUR);
			continue;
		}

		// 3.3 Aloca memoria para o registro
		int entry_data_len = sizeof(char) + sizeof(int) + entry_len;
		char* entry_data = malloc(entry_data_len);

		// 3.4 Copia os campos 'removido', 'tamanho do registro' e lê o resto
		entry_data[0] = removed;
		memcpy(&entry_data[1], &entry_len, sizeof(int));
		fread(&entry_data[5], sizeof(char), entry_len, bin);

		// 3.5 Insere dados na btree (indexando)
		int key = entry_get_key(entry_data);
		long offset = ftell(bin) - sizeof(char) - sizeof(int) - entry_len;
		insert_btree(btree, key, offset);

		free(entry_data); // Free no registro lido
	}

	// 4. Fechar os arquivos abertos
	// 4.1 Escrevendo header do arquivo binario
	header->status = '1';
	header_write(bin_name, header);
	fclose(bin);

	// 4.2 Escrevendo header da btree só após todas inserções
	btree->status = '1';
	btree_write_header(btree);
	btree_delete(btree);

	// 5. Chamando binario na tela para arquivo de indice (btree)
	binarioNaTela(btree_name);
}

// Função 11 e 12 do trabalho 2 ——— Procurar chave "valor" no indice e printar sua entrada do .bin
void select_where(int op,char* bin_name,char* btree_name,char* key_str) {
	// 1. Extraindo `int key` da string `key_str`
	int key;
	if (op == 11) {
		char* prefix = strndup(&key_str[1], 5 * sizeof(char));
		key = convertePrefixo(prefix);
		free(prefix);
	} else {
		key = atoi(key_str);
	}

	// 2. Procurar pela key no arquivo de indice (btree)
	Btree* btree = btree_read_header(btree_name);
	if (btree == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return;
	}
	long offset_in_bin = search_btree(btree, key);

	// 3. Fechando o arquivo da btree e removendo ela da memoria
	btree->status = '1';
	btree_write_header(btree);
	btree_delete(btree);

	// 4. Se a chave nao foi achada, offset < 0
	if (offset_in_bin < 0){ // Nao achou a chave: sair da funcao
		printf("Registro inexistente.\n");
		return;
	}

	// 5. Abrir o arquivo binario
	FILE* bin = fopen_safe(bin_name, "rb");
	fseek(bin, offset_in_bin, SEEK_SET);

	// 5. Pegar o registro correspondente, no arquivo binario
	char removed;
	fread(&removed, sizeof(char), 1, bin);

	if (removed == '0'){ // Registro está logicamente removido: sair da funcao
		fclose(bin);
		printf("Registro inexistente.\n");
		return;
	}

	int entry_size;
	fread(&entry_size, sizeof(int), 1, bin);

	// 6. Ler dados de registro no arquivo binario, e fechar o arquivo
	char entry_data[entry_size];
	fread(entry_data, sizeof(char), entry_size, bin);

	fclose(bin);

	// 7. Printar registro encontrado
	if (op == 11) {
		display_vehicle_from_data(entry_data);
	} else {
		display_line_from_data(entry_data);
	}
}

// Funcao 13 e 14 do traalho 2  ——— Inserir n entradas no .bin e na btree, simultaneamente
void insert_into(int op,char* bin_name,char* btree_name,int quantity) {
	// 0. Abrindo os arquivos utilizados
	FILE* bin = fopen_safe(bin_name, "rb+");
	Btree* btree = btree_read_header(btree_name);
	if (btree == NULL) { // Algum dos arquivos nao existe: sair da funcao
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	// 1. Lendo header e escrevendo com status '0'
	int description_len = (op == 9)? VEHICLE_DESCRIPTION_LEN : LINE_DESCRIPTION_LEN;
	Bin_header* header = header_read(bin,description_len);
	header->status = '0'; // Comeca marcado instavel
	header_write(bin_name, header);

	// 2. Definir funcoes que vao processar cada linha do input
	char* (*process_line)(char*) = vehicle_from_string;
	int (*get_entry_key)(char*)  = vehicle_get_key;
	if (op == 14) {
		process_line  = line_from_string;
		get_entry_key = line_get_key;
	}

	fseek(bin, 0, SEEK_END);
	// 3. Loop processando input: lê registro -> adiciona ao .bin -> adiciona na btree
	for (int i = 0; i < quantity; i++) {
		// 3.1 Lê o input e passa pro formato csv
		char* input_line = readline(stdin);
		string_to_csv(input_line);

		// 3.2 Processa o input: string -> memória
		char* entry_data = process_line(input_line);
		free(input_line); // Free na linha do input

		// 3.3 Registro logicamente removido: pula para proximo loop
		if (entry_data[0] == '0') {
			header->nroRegistrosRemovidos++;
			free(entry_data);
			continue;
		}

		// 3.3 Insere dados no binario
		int entry_data_len;
		memcpy(&entry_data_len, &entry_data[1], sizeof(int));
		fwrite(entry_data, sizeof(char), entry_data_len, bin);
		header->nroRegistros++;

		// 3.4 Insere dados na btree (indexando)
		int key = get_entry_key(entry_data);
		long offset = ftell(bin) - entry_data_len; // se nao subtrair, ftell pega offset do prox registro
		insert_btree(btree, key, offset);

		free(entry_data); // Free no registro lido
	}

	// 4. Escrever headers, restaurando estabilidade, e fechar arquivos
	header->status = '1';
	header->byteProxReg = ftell(bin);
	header_write(bin_name, header);

	header_free(header);
	fclose(bin);

	// Escrevendo header da btree só após todas inserções
	btree->status = '1';
	btree_write_header(btree);
	btree_delete(btree);

	// 5. Chamando binario na tela para arquivo de indice (btree)
	binarioNaTela(btree_name);
}

void trabalho2_menu(char** arguments) {
	int operation = atoi(arguments[0]);
	switch (operation) {
		case 9:
		case 10:
			create_index(operation, arguments[1], arguments[2]);
			break;
		case 11:
		case 12:
			select_where(operation, arguments[1], arguments[2], arguments[4]);
			break;
		case 13:
		case 14: {
			int insert_quantity = atoi(arguments[3]);
			insert_into(operation, arguments[1], arguments[2], insert_quantity);
			break;
		}
	}
}

int main(){
	// 1. Lê o comando na stdin e extrai os argumentos dele
	char* operation = readline(stdin);
	char** arguments = arguments_get(operation);
	free(operation);

	// 2. Chama o menu e realiza a operacao selecionada
	trabalho2_menu(arguments);

	// 3. Limpa a memoria do menu e sai do programa
	arguments_free(arguments);
	return 0;
}