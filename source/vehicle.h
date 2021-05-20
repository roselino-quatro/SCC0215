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

VehicleData* readVehicleCsv(FILE* csv);
void writeVehicleBinary(VehicleData* vData,FILE* binDest);
void displayVehicle(VehicleReg* vReg);

bool matchVehiclePrefix(VehicleReg* vReg,void* prefix);
bool matchVehicleData(VehicleReg* vReg,void* data);
bool matchVehicleSeatQty(VehicleReg* vReg,void* seatQty);
bool matchVehicleModel(VehicleReg* vReg,void* model);
bool matchVehicleCategory(VehicleReg* vReg,void* category);

void selectVehicleWhere(VehicleData* vData,void* key,bool (*match)(VehicleReg*,void*));