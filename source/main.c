// Gabriel Victor Cardoso Fernandes nUsp 11878296
// Lourenço de Salles Roselino nUsp 11796805
#include "vehicle.h"
#include "line.h"
#include "utils.h"

// Operações Trab 1
#define CREATE_VEHICLE 1
#define CREATE_LINE 2
#define SELECT_VEHICLE 3
#define SELECT_LINE 4
#define WHERE_VEHICLE 5
#define WHERE_LINE 6
#define INSERTION_VEHICLE 7
#define INSERTION_LINE 8

// Operações Trab 2
#define CREATE_INDEX_VEHICLE 9
#define CREATE_INDEX_LINE 10
#define WHERE_VEHICLE_INDEX 11
#define WHERE_LINE_INDEX 12
#define INSERTION_VEHICLE_INDEX 13
#define INSERTION_LINE_INDEX 14

#define FILE_NAME 1
#define BIN_FILE_NAME 2
#define FIELD_NAME 2
#define INSERT_QNTY 2
#define FIELD_VALUE 3

int main(){
	// Declaração de variaveis
	char** arguments;
	char* command;
	FILE* csvPointer;
	FILE* binPointer;
	VTable* vehiclesTable;
	LTable* linesTable;
	BTree* btree_struct;
	long offset;
	int op;

	// Loop principal
	while(!feof(stdin)) {
		command = readline(stdin);
		if(command == NULL) break;
		cleanString(command);

		op = atoi(command);
		arguments = getFields(4, command);
		switch(op) {
			case CREATE_VEHICLE:
				csvPointer = openFile(arguments[FILE_NAME], "r");
				binPointer = openFile(arguments[BIN_FILE_NAME], "wb");

				vehiclesTable = readVehicleCsv(csvPointer);
				if(vehiclesTable != NULL) {
					closeFile(csvPointer);
					writeVehicleBinary(vehiclesTable, binPointer);
					freeVehicleTable(vehiclesTable);
					closeFile(binPointer);
					binarioNaTela(arguments[BIN_FILE_NAME]);
				} else {
					closeFile(binPointer);
				}
				
				break;
			case CREATE_LINE:
				csvPointer = openFile(arguments[FILE_NAME], "r");
				binPointer = openFile(arguments[BIN_FILE_NAME], "wb");

				linesTable = readLineCsv(csvPointer);
				if(linesTable != NULL) {
					closeFile(csvPointer);
					writeLineBinary(linesTable, binPointer);
					freeLineTable(linesTable);
					closeFile(binPointer);
					binarioNaTela(arguments[BIN_FILE_NAME]);
				} else {
					closeFile(binPointer);
				}
				
				break;
			case SELECT_VEHICLE:
				binPointer = openFile(arguments[FILE_NAME], "r");
				vehiclesTable = readVehicleBinary(binPointer);
				closeFile(binPointer);

				selectVehicle(vehiclesTable);
				break;
			case SELECT_LINE:
				binPointer = openFile(arguments[FILE_NAME], "r");
				linesTable = readLineBinary(binPointer);
				closeFile(binPointer);

				selectLine(linesTable);
				break;
			case WHERE_VEHICLE:
				binPointer = openFile(arguments[FILE_NAME], "r");
				vehiclesTable = readVehicleBinary(binPointer);
				closeFile(binPointer);

				bool (*cmpVehicle)(VEntry*,void*); // Cria um ponteiro de função para ser itoçozada no SelectWhere
				if(!strcmp(arguments[FIELD_NAME], "prefixo")) {
				cmpVehicle = matchVehiclePrefix;
				} else if(!strcmp(arguments[FIELD_NAME], "data")) {
					cmpVehicle = matchVehicleData;
				} else if(!strcmp(arguments[FIELD_NAME], "quantidadeLugares")) {
					cmpVehicle = matchVehicleSeatQty;
				} else if(!strcmp(arguments[FIELD_NAME], "modelo")) {
					cmpVehicle = matchVehicleModel;
				} else if(!strcmp(arguments[FIELD_NAME], "categoria")) {
					cmpVehicle = matchVehicleCategory;
				}

				selectVehicleWhere(vehiclesTable, arguments[FIELD_VALUE], cmpVehicle);
				break;
			case WHERE_LINE:
				binPointer = openFile(arguments[FILE_NAME], "r");
				linesTable = readLineBinary(binPointer);
				closeFile(binPointer);

				bool (*cmpLine)(LEntry*,void*); // Cria um ponteiro de função para ser itoçozada no SelectWhere
				if(!strcmp(arguments[FIELD_NAME], "codLinha")) {
					cmpLine = matchLineCode;
				} else if(!strcmp(arguments[FIELD_NAME], "aceitaCartao")) {
					cmpLine = matchLineAcceptCard;
				} else if(!strcmp(arguments[FIELD_NAME], "nomeLinha")) {
					cmpLine = matchLineName;
				} else if(!strcmp(arguments[FIELD_NAME], "corLinha")) {
					cmpLine = matchLineColor;
				} 

				selectLineWhere(linesTable, arguments[FIELD_VALUE], cmpLine);
				break;
			case INSERTION_VEHICLE:
				binPointer = openFile(arguments[FILE_NAME], "r+");
				vehiclesTable = readVehicleBinary(binPointer);
				if(vehiclesTable == NULL) {
					closeFile(binPointer);
					break;
				}

				insertVehicleEntries(vehiclesTable, atoi(arguments[INSERT_QNTY]), binPointer);
				closeFile(binPointer);
				freeVehicleTable(vehiclesTable);

				binarioNaTela(arguments[FILE_NAME]);
				break;
			case INSERTION_LINE:
				binPointer = openFile(arguments[FILE_NAME], "r+");
				linesTable = readLineBinary(binPointer);
				if(linesTable == NULL) {
					closeFile(binPointer);
					break;
				}

				insertLineEntries(linesTable, atoi(arguments[INSERT_QNTY]), binPointer);
				closeFile(binPointer);
				freeLineTable(linesTable);

				binarioNaTela(arguments[FILE_NAME]);
				break;
			case CREATE_INDEX_VEHICLE:
				btree_delete(vehicleBTreeFromBin(arguments[1], arguments[2]));
				break;
			case CREATE_INDEX_LINE:
				btree_delete(lineBTreeFromBin(arguments[1], arguments[2]));
				break;
			case WHERE_VEHICLE_INDEX:
				btree_struct = btree_read_header(arguments[2]);
				offset = search_btree(btree_struct, convertePrefixo(arguments[4]));
				displayVehicleOffset(arguments[1], offset);
				break;
			case WHERE_LINE_INDEX:
				btree_struct = btree_read_header(arguments[2]);
				offset = search_btree(btree_struct, atoi(arguments[4]));
				displayLineOffset(arguments[1], offset);
				break;
			case INSERTION_VEHICLE_INDEX:
				binPointer = openFile(arguments[FILE_NAME], "r+");
				vehiclesTable = readVehicleBinary(binPointer);
				btree_struct = btree_read_header(arguments[2]);
				if(vehiclesTable == NULL) {
					closeFile(binPointer);
					break;
				}

				insertVehicleEntriesBTree(vehiclesTable, atoi(arguments[3]), binPointer, btree_struct);
				closeFile(binPointer);
				freeVehicleTable(vehiclesTable);

				btree_delete(btree_struct);
				binarioNaTela(arguments[FILE_NAME]);
				break;
			case INSERTION_LINE_INDEX:
				binPointer = openFile(arguments[FILE_NAME], "r+");
				linesTable = readLineBinary(binPointer);
				btree_struct = btree_read_header(arguments[2]);
				if(linesTable == NULL) {
					closeFile(binPointer);
					break;
				}

				insertLineEntriesBTree(linesTable, atoi(arguments[3]), binPointer, btree_struct);
				closeFile(binPointer);
				freeLineTable(linesTable);

				btree_delete(btree_struct);
				binarioNaTela(arguments[FILE_NAME]);
				break;
		}

		int i = 0;
		while(arguments[i] != NULL){
			free(arguments[i]);
			i++;
		}
		free(arguments);
		free(command);
	}

	return 0;
}