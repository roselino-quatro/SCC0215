#ifndef _VEHICLE_H
#define _VEHICLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Cabecalhos:
// 1 - nenhum byte da struct deve deve ficar vazio / padding!
// 2 - quando escrever no arquivo não precisa por \0 nas strings
typedef struct VEHICLES_HEADER {
	bool isStable;
	long regByteOff;
	int regQty;
	int regRemovedQty;
	char descPrefix[18];
	char descData[35];
	char descSeats[42];
	char descLine[26];
	char descModel[17];
	char descCategory[20];
}VehicleHeader;

// Registros:
// 1 - nao precisa por \0 quando escreve strings
// 2 - prefix nao pode ser nulo
// 3 - nulos para campos fixos: int = -1; string = "\0@@@@"
// 4 - nulos para campos variaveis: tamanho = 0; string = null / nao escreve
typedef struct VEHICLE_REGISTER {
	bool isPresent;
	int regSize;
	char prefix[5];
	char data[10];
	int seatQty;
	int lineCode;
	int modelSize;
	char model[100];
	int categorySize;
	char category[100];
}VehicleReg;

typedef struct VEHICLE_DATA {
	VehicleHeader header;
	int regQty;
	VehicleReg* registers;
}VehicleData;

/****
 * Lê e parsea o CSV do arquivo e armazena em uma struct
 * 
 * @param csv ponteiro para o arquivo a ser lido
 * @return ponteiro para a struct allocada na função
 */
VehicleData* readVehicleCsv(FILE* csv);

/****
 * Transfere os dados de um arquivo binário para uma estrutura VehicleData
 * Caso o binário seja invalido retorna NULL
 * 
 * @param bin Arquivo binário com os dados
 * @return VehicleData* estrutura na memoria com os dados alocados
 */
VehicleData* readVehicleBinary(FILE* bin);

/****
 * Destructor que libera memoria alocada de uma struct VehicleData
 * 
 * @param vData ponteiro para struct a ser liberada
 * @return boolean para caso a função tenha sucesso ou não
 */
bool freeVehicleData(VehicleData* vData);

/****
 * Transfere os dados de uma VehicleData para um arquivo binario seguindo as regras passadas nas especificaçẽos
 * 
 * @param vData struct a ser transferida
 * @param binDest arquivo alvo
 */
void writeVehicleBinary(VehicleData* vData,FILE* binDest);

/****
 * Adiciona registros lidos na stdin para os registros de vData
 * 
 * @param vData struct a receber os registros
 * @param qty quantidade de registros a serem lidos
 */
void appendVehicleRegisters(VehicleData* vData,int qty);

/****
 * Imprime informações do registro de veiculo
 * 
 * @param vReg registro a ser impresso
 */
void displayVehicle(VehicleReg* vReg);

/****
 * Familia de funções para a selectVehicleWhere
 * 
 * @param vReg registro a ser verificado
 * @param secondParameter valor a ser comparado
 */
bool matchVehiclePrefix(VehicleReg* vReg,void* prefix);
bool matchVehicleData(VehicleReg* vReg,void* data);
bool matchVehicleSeatQty(VehicleReg* vReg,void* seatQty);
bool matchVehicleModel(VehicleReg* vReg,void* model);
bool matchVehicleCategory(VehicleReg* vReg,void* category);

/****
 * Imprime os matchs de uma comparação dentro de uma struct Data
 * 
 * @param vData struct a ser buscada
 * @param match ponteiro de função para uma das funções match
 */
void selectVehicleWhere(VehicleData* vData,void* key,bool (*match)(VehicleReg*,void*));

/****
 * Imprime todos os registros não removidos de uma struct
 * 
 * @param vReg registro a ser verificado 
 */
void selectVehicle(VehicleData* vData);

#endif