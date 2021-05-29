#ifndef _VEHICLE_H
#define _VEHICLE_H

#include <stdbool.h>
#include "utils.h"

// Cabecalho do arquivo binário / Informações sobre a frota
typedef struct VehiclesInfo{
	char stable;
	long byteOffset;
	int qty;
	int rmvQty;
	char prefix[18];
	char data[35];
	char seats[42];
	char line[26];
	char model[17];
	char category[20];
}VInfo;

// Um registro de onibus na frota / Informações sobre 1 entrada no frota
typedef struct VehicleEntry{
	char isPresent;
	int size;	// Size of all fields. When writing, subtract 5 to ignore first 2 (isPresent & size).
	char prefix[5];
	char data[10];
	int seatQty;
	int lineCode;
	int modelLen;
	char model[100];
	int categoryLen;
	char category[100];
}VEntry;

// Status da frota inteira / Informações sobre metadados + todos registros da frota
typedef struct VehiclesTable{
	VInfo* header;
	int qty;
	VEntry* fleet;
}VTable;

/****
 * Cria uma table a partir do csv
 * 
 * @param csv arquivo csv com os dados formatados
 * @return VTable* table com os dados carregados
 */
VTable* readVehicleCsv(FILE* csv,char* delim);

/****
 * Cria uma tabela a partir de um arquivo binario
 * 
 * @param bin arquivo binario com os dados formatados
 * @return VTable* table com os dados carregados
 */
VTable* readVehicleBinary(FILE* bin);

/****
 * Destructor que libera memoria alocada de uma struct VehicleData
 * 
 * @param vData ponteiro para struct a ser liberada
 * @return boolean para caso a função tenha sucesso ou não
 */
bool freeVTable(VTable* table);

/****
 * Escreve a table, na formatação especificada, em um arquivo binario
 * 
 * @param vData table com os dados
 * @param bin arquivo a ser criado com os dados da tabela
 */
void writeVehicleBinary(VTable* table,FILE* bin);

/****
 * Imprime o registro no formatado especificado
 * 
 * @param entry veiculo a ser impresso
 */
void displayVehicle(VEntry* entry);

/****
 * Familia de funções para a selectVehicleWhere
 * 
 * @param entry registro a ser verificado
 * @param secondParameter valor a ser comparado
 */
bool matchVehiclePrefix(VEntry* entry,void* prefix);
bool matchVehicleData(VEntry* entry,void* data);
bool matchVehicleSeatQty(VEntry* entry,void* seatQty);
bool matchVehicleModel(VEntry* entry,void* model);
bool matchVehicleCategory(VEntry* entry,void* category);

/****
 * Imprime todos veiculos de uma table
 * 
 * @param table table com os veiculos
 */
void selectVehicle(VTable* table);

/****
 * Imprime todos veiculos, que possuem a chave procurada, numa table
 * 
 * @param table table com os veiculos
 * @param key valor a ser comparado em cada veiculo
 * @param match ponteiro de função para uma das funções match
 */
void selectVehicleWhere(VTable* table,void* key,bool (*match)(VEntry*,void*));

/****
 * Lê entradas da stdin. Essas entradas são adicionadas na table e depois escritas no bin
 * 
 * @param table tabela a receber as novas entradas
 * @param qty quantidade de entradas que vao ser lidas
 * @param delim caractere que vai separar os campos da entrada
 * @param bin arquivo que vai receber as novas entradas
 */
void insertVehicleEntries(VTable* table,int qty,char* delim,FILE* bin);
#endif