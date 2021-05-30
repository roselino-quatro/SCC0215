// Gabriel Victor Cardoso Fernandes nUsp 11878296
// Lourenço de Salles Roselino nUsp 11796805
#include "vehicle.h"
#include "line.h"
#include "utils.h"

#define CREATE_VEHICLE 1
#define CREATE_LINE 2
#define SELECT_VEHICLE 3
#define SELECT_LINE 4
#define WHERE_VEHICLE 5
#define WHERE_LINE 6
#define INSERTION_VEHICLE 7
#define INSERTION_LINE 8

#define FILE_NAME 1
#define BIN_FILE_NAME 2
#define FIELD_NAME 2
#define INSERT_QNTY 2
#define FIELD_VALUE 3

// Exemplo de formatar string pra csv
// char* input = readline(stdin);
// cleanString(input);
// char** args = getFields(quantidade,input);
// args[0], args[1] ... vai ter <quantidade> strings (todas alocadas, lembra de dar free dps)

int main(){
	// Declaração de variaveis
	char** arguments;
	char* command;
	FILE* csvPointer;
	FILE* binPointer;
	VTable* vehiclesTable;
	LTable* linesTable;
	int op;

	// Loop principal
	while(!feof(stdin)) {
		command = readline(stdin);

		if(command == NULL) break;

		op = atoi(command);
		switch(op) {
			case CREATE_VEHICLE:
				cleanString(command);
				arguments = getFields(3, command);
				csvPointer = openFile(arguments[FILE_NAME], "r");
				free(arguments[FILE_NAME]);
				binPointer = openFile(arguments[BIN_FILE_NAME], "wb");

				vehiclesTable = readVehicleCsv(csvPointer, ",");
				if(vehiclesTable != NULL) {
					fclose(csvPointer);
					writeVehicleBinary(vehiclesTable, binPointer);
					freeVTable(vehiclesTable);
					fclose(binPointer);
					binarioNaTela(arguments[BIN_FILE_NAME]);
				}
				if(vehiclesTable == NULL) fclose(binPointer);
				
				free(arguments[BIN_FILE_NAME]);
				free(arguments[0]);
				free(arguments);

				break;
			case CREATE_LINE:
				cleanString(command);
				arguments = getFields(3, command);
				csvPointer = openFile(arguments[FILE_NAME], "r");
				free(arguments[FILE_NAME]);
				binPointer = openFile(arguments[BIN_FILE_NAME], "wb");

				linesTable = readLineCsv(csvPointer);
				if(linesTable != NULL) {
					fclose(csvPointer);
					writeLineBinary(linesTable, binPointer);
					freeLineTable(linesTable);
					fclose(binPointer);
					binarioNaTela(arguments[BIN_FILE_NAME]);
				}
				if(vehiclesTable == NULL) fclose(binPointer);
				
				free(arguments[BIN_FILE_NAME]);
				free(arguments[0]);
				free(arguments);

				break;
			case SELECT_VEHICLE:
				cleanString(command);
				arguments = getFields(2, command);
				binPointer = openFile(arguments[FILE_NAME], "r");
				free(arguments[FILE_NAME]);
				vehiclesTable = readVehicleBinary(binPointer);
				fclose(binPointer);

				selectVehicle(vehiclesTable);

				free(arguments[0]);
				freeVTable(vehiclesTable);
				free(arguments);

				break;
			case SELECT_LINE:
				cleanString(command);
				arguments = getFields(2, command);
				binPointer = openFile(arguments[FILE_NAME], "r");
				free(arguments[FILE_NAME]);
				linesTable = readLineBinary(binPointer);
				fclose(binPointer);

				selectLine(linesTable);

				free(arguments[0]);
				freeLineTable(linesTable);
				free(arguments);

				break;
			case WHERE_VEHICLE:
				cleanString(command);
				arguments = getFields(4, command);
				binPointer = openFile(arguments[FILE_NAME], "r");
				free(arguments[FILE_NAME]);
				vehiclesTable = readVehicleBinary(binPointer);
				fclose(binPointer);

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
				free(arguments[0]);
				free(arguments[FIELD_NAME]);
				free(arguments[FIELD_VALUE]);
				freeVTable(vehiclesTable);
				free(arguments);

				break;
			case WHERE_LINE:
				cleanString(command);
				arguments = getFields(4, command);
				binPointer = openFile(arguments[FILE_NAME], "r");
				free(arguments[FILE_NAME]);
				linesTable = readLineBinary(binPointer);
				fclose(binPointer);

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
				free(arguments[0]);
				free(arguments[FIELD_NAME]);
				free(arguments[FIELD_VALUE]);
				freeLineTable(linesTable);
				free(arguments);

				break;
			case INSERTION_VEHICLE:
				cleanString(command);
				arguments = getFields(3, command);
				binPointer = openFile(arguments[FILE_NAME], "r");
				vehiclesTable = readVehicleBinary(binPointer);
				if(vehiclesTable == NULL) {
					fclose(binPointer);
					free(arguments[0]);
					free(arguments[FILE_NAME]);
					free(arguments[INSERT_QNTY]);
					free(arguments);
					break;
				}

				insertVehicleEntries(vehiclesTable, atoi(arguments[INSERT_QNTY]), ",", binPointer);
				fclose(binPointer);
				freeVTable(vehiclesTable);

				binarioNaTela(arguments[FILE_NAME]);

				free(arguments[0]);
				free(arguments[FILE_NAME]);
				free(arguments[INSERT_QNTY]);
				free(arguments);

				break;
			case INSERTION_LINE:
				cleanString(command);
				arguments = getFields(3, command);
				binPointer = openFile(arguments[FILE_NAME], "r");
				linesTable = readLineBinary(binPointer);
				if(linesTable == NULL) {
					fclose(binPointer);
					free(arguments[0]);
					free(arguments[FILE_NAME]);
					free(arguments[INSERT_QNTY]);
					free(arguments);
					break;
				}
				fclose(binPointer);

				insertLineEntries(linesTable, atoi(arguments[INSERT_QNTY]), binPointer);
				freeLineTable(linesTable);

				binarioNaTela(arguments[FILE_NAME]);

				free(arguments[0]);
				free(arguments[FILE_NAME]);
				free(arguments[INSERT_QNTY]);
				free(arguments);
				break;
		}
		free(command);
		
	}

	return 0;
}