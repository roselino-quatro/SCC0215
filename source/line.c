// Gabriel Victor Cardoso Fernandes nUsp 11878296
// Lourenço de Salles Roselino nUsp 11796805
#include "../includes/line.h"

// Tamanho, em bytes, dos campos fixos de todo registro do tipo "veiculo"
#define LINE_FIXED_FIELDS_LEN 18

char* line_from_string(char* entry_string) {
	// 0. Alocar tamanho padrao que garante caber um registro
	char* entry = malloc(200);

	// 1. Percorrer a string de registro, interpretando seus campos
	// e transferindo para representacao binaria.
	// Metacampo 'removido'
	entry[0] = (entry_string[0] == '*')? '0' : '1';
	
	// Campo 1: codigo da linha | Não aceita valor nulo!
	char* field = entry_string;
	int line_code = atoi(field);
	memcpy(&entry[5], &line_code, sizeof(int));

	// Campo 2: "aceita cartao?" | Nulo quando campo esta vazio (contem apenas delimitador ,)
	field = strchr(field, ',') + 1; // Jump to next field
	entry[9] = (field[0] != ',')? field[0] : '\0';

	// Campo 3: tamanho do campo "nome"
	field = strchr(field, ',') + 1;
	int name_len = strcspn(field, ",");
	memcpy(&entry[10], &name_len, sizeof(int));

	// Campo 4: nome
	memcpy(&entry[14], &field, name_len);

	// Campo 5: tamanho do campo "cor da linha"
	field = strchr(field, ',') + 1;
	int color_len = strcspn(field, ",");
	memcpy(&entry[14+name_len], &color_len, sizeof(int));

	// Campo 6: cor da linha
	memcpy(&entry[14+name_len+4], &field, color_len);

	// Metacampo "tamanho registro"
	// É a soma dos tamanhos de campos de tam. fixo + campos de tam. variavel
	int entry_size = LINE_FIXED_FIELDS_LEN + name_len + color_len;
	memcpy(&entry[1], &entry_size, sizeof(int));
	
	return entry;
};

int line_get_key(char* line_data) {
	// 0. Acessa posicao do campo "codigo da linha" nos dados binarios.
	int line_code;
	memcpy(&line_code, &line_data[5], sizeof(int));

	return line_code;
}

void display_line_from_data(char* line_data) {
	// 1. Printando codigo da linha
	int line_code;
	memcpy(&line_code, &line_data[5], sizeof(int));
	printf("Codigo da linha: %d\n",line_code);

	// 2. Printando nome da linha
	int name_len;
	memcpy(&name_len, &line_data[10], sizeof(int));
	
	if (name_len > 0) {
		char* name = strndup(&line_data[14], name_len);
		printf("Nome da linha: %s\n",name);
		free(name);
	} else {
		printf("Nome da linha: campo com valor nulo\n");
	}

	// 3. Printando cor da linha
	int color_len;
	memcpy(&color_len, &line_data[14+name_len], sizeof(int));

	if (color_len > 0) {
		char* color = strndup(&line_data[14+name_len+4], color_len);
		printf("Cor que descreve a linha: %s\n",color);
		free(color);
	} else {
		printf("Nome da linha: campo com valor nulo\n");
	}

	char aceita_cartao = line_data[9];
	if(aceita_cartao == 'S'){
		printf("Aceita cartao: PAGAMENTO SOMENTE COM CARTAO SEM PRESENCA DE COBRADOR");
	} else if(aceita_cartao == 'N'){
		printf("Aceita cartao: PAGAMENTO EM CARTAO E DINHEIRO");
	} else if(aceita_cartao == 'F'){
		printf("Aceita cartao: PAGAMENTO EM CARTAO SOMENTE NO FINAL DE SEMANA");
	} else{
		printf("Aceita cartao: campo com valor nulo");
	}
	return;
}
