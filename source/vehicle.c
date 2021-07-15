// Gabriel Victor Cardoso Fernandes nUsp 11878296
// Lourenço de Salles Roselino nUsp 11796805
#include "../includes/vehicle.h"
#include <string.h>

// Tamanho, em bytes, dos campos fixos de todo registro do tipo "veiculo"
#define VEHICLE_FIXED_FIELDS_LEN 36

char* vehicle_from_string(char* entry_string) {
	// 0. Alocar tamanho padrao que garante caber um registro
	char* entry = malloc(200);

	// 1. Percorrer a string de registro, interpretando seus campos
	// e transferindo para representacao binaria.
	// Metacampo 'removido'
	entry[0] = (entry_string[0] == '*')? '0' : '1';
	
	// Campo 1: prefixo
	char* field = entry_string;
	memcpy(&entry[5], field, sizeof(char) * 5);

	// Campo 2: data
	field = strchr(field, ',') + 1; // Jump to next field
	if (strncmp(field, "NULO", 4) == 0) {
		memcpy(&entry[10], "\0@@@@@@@@@", sizeof(char) * 10);
	} else {
		memcpy(&entry[10], field, sizeof(char) * 10);
	}

	// Campo 3: quantidade de lugares
	field = strchr(field, ',') + 1;
	int seat_quantity = atoi(field);
	if (seat_quantity == 0) {
		int null_field = -1;
		memcpy(&entry[20], &null_field, sizeof(int));
	} else {
		memcpy(&entry[20], &seat_quantity, sizeof(int));
	}
	
	// Campo 4: codigo da linha
	field = strchr(field, ',') + 1;
	int line_code = atoi(field);
	if (line_code == 0) {
		int null_field = -1;
		memcpy(&entry[24], &null_field, sizeof(int));
	} else {
		memcpy(&entry[24], &line_code, sizeof(int));
	}

	// Obs: Esse codigo para capturar campos de tamanho variavel adere à especificacao.
	// Caso algm campo nao esteja presente, a sua len = 0 e o campo nao vai ser escrito.
	// Campo 5: tamanho de modelo
	field = strchr(field, ',') + 1;
	int model_len = strcspn(field, ",");
	memcpy(&entry[28], &model_len, sizeof(int));

	// Campo 6: modelo
	memcpy(&entry[32], &field, model_len);
	
	// Campo 7: tamanho de categoria
	field = strchr(field, ',') + 1; // Jump to next field
	int category_len = strcspn(field, ",");
	memcpy(&entry[32+model_len], &category_len, sizeof(int));

	// Campo 8: categoria
	memcpy(&entry[32+model_len+4], &field, category_len);

	// Metacampo "tamanho registro"
	// É a soma dos tamanhos de campos de tam. fixo + campos de tam. variavel
	int entry_size = VEHICLE_FIXED_FIELDS_LEN + model_len + category_len;
	memcpy(&entry[1], &entry_size, sizeof(int));
	
	// 8. Colocar NULL no final do buffer para facilitar uso
	return entry;
};

int vehicle_get_key(char* vehicle_data) {
	// 0. Acessa posicao do campo "prefixo" nos dados binarios e processa sua hash.
	char* prefix = strndup(&vehicle_data[5], sizeof(char) * 5);
	int prefix_hash = convertePrefixo(prefix);

	free(prefix);
	return prefix_hash;
}

int vehicle_get_line_code(char* vehicle_data) {
	int vehicle_line_code;
	memcpy(&vehicle_line_code, &vehicle_data[24], sizeof(int));

	return vehicle_line_code;
}

void display_vehicle_from_data(char* vehicle_data) {
	char* nullField = "campo com valor nulo\0";
	
	// 0. Percorrendo os dados binarios e printando seus campos.
	// Printando prefixo do veiculo -> Nunca é nulo!
	printf("Prefixo do veiculo: %.5s\n",&vehicle_data[0]);

	// Lendo tamanho do campo "modelo"
	int model_len;
	memcpy(&model_len, &vehicle_data[23], sizeof(int));
	
	// Printando modelo do veiculo
	if (model_len > 0) {
		char* vehicle_model = strndup(&vehicle_data[27], model_len);
		printf("Modelo do veiculo: %s\n",vehicle_model);
		free(vehicle_model);
	} else {
		printf("Modelo do veiculo: %s\n",nullField);
	}

	// Lendo tamanho do campo "categoria"
	int category_len;
	memcpy(&category_len, &vehicle_data[27+model_len], sizeof(int));

	// Printando categoria do veiculo
	if (category_len > 0) {
		char* vehicle_category = strndup(&vehicle_data[27+model_len+4], category_len);
		printf("Categoria do veiculo: %s\n",vehicle_category);
		free(vehicle_category);
	} else {
		printf("Categoria do veiculo: %s\n",nullField);
	}

	// Printando data do veiculo
	char* months[12] = {"janeiro","fevereiro","março","abril","maio","junho",
						"julho", "agosto","setembro","outubro","novembro","dezembro"};
	
	if(vehicle_data[5] != '\0'){
		// Valor de ano está no offset 5
		int year = atoi(&vehicle_data[5]);

		// Valor de mes esta no offset 10
		char* month = months[atoi(&vehicle_data[10])-1];

		// Valor de dia esta no offset 13
		char* day_str = strndup(&vehicle_data[13], 2);
		int day = atoi(day_str);
		free(day_str);

		printf("Data de entrada do veiculo na frota: %02d de %s de %d\n",day,month,year);
	} else {
		printf("Data de entrada do veiculo na frota: %s\n",nullField);
	}

	// Lendo campo "quantidade de lugares"
	int seat_quantity;
	memcpy(&seat_quantity, &vehicle_data[15], sizeof(int));

	// Printando campo "quantidade de lugares"
	if (seat_quantity != -1) {
		printf("Quantidade de lugares sentados disponiveis: %d\n\n",seat_quantity);
	} else {
		printf("Quantidade de lugares sentados disponiveis: %s\n\n",nullField);
	}
}
