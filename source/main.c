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
	LTable* linesTables;
	int op;
	do {
		command = readline(stdin);
		op = atoi(command);
		switch(op) {
			case CREATE_VEHICLE:
				cleanString(command);
				arguments = getFields(3, command);
				csvPointer = openFile(arguments[FILE_NAME], "rb");
				free(arguments[FILE_NAME]);
				binPointer = openFile(arguments[BIN_FILE_NAME], "wb");

				vehiclesTable = readVehicleCsv(csvPointer, ",");
				fclose(csvPointer);
				writeVehicleBinary(vehiclesTable, binPointer);
				fclose(binPointer);
				freeVTable(vehiclesTable);
				
				binarioNaTela(arguments[BIN_FILE_NAME]);
				free(arguments[BIN_FILE_NAME]);
				break;
			case CREATE_LINE:
				cleanString(command);
				arguments = getFields(3, command);
				csvPointer = openFile(arguments[FILE_NAME], "rb");
				free(arguments[FILE_NAME]);
				binPointer = openFile(arguments[BIN_FILE_NAME], "wb");

				linesTables = readLineCsv(csvPointer);
				fclose(csvPointer);
				writeLineBinary(linesTables, binPointer);
				fclose(binPointer);
				freeLTable(linesTables);
				
				binarioNaTela(arguments[BIN_FILE_NAME]);
				free(arguments[BIN_FILE_NAME]);
				free(argumentes[1]);
				break;
			case SELECT_VEHICLE:
				cleanString(command);
				arguments = getFields(2, command);
				binPointer = openFile(arguments[FILE_NAME], "rb");
				free(arguments[FILE_NAME]);
				vehiclesTable = readVehicleBinary(binPointer);
				fclose(binPointer);

				selectVehicle(vehiclesTable);

				freeVehicleTable(vehiclesTable);
				break;
			case SELECT_LINE:
				cleanString(command);
				arguments = getFields(2, command);
				binPointer = openFile(arguments[FILE_NAME], "rb");
				free(arguments[FILE_NAME]);
				linesTables = readLineBinary(binPointer);
				fclose(binPointer);

				selectLine(linesTables);

				freeLineTable(linesTables);
				break;
			case WHERE_VEHICLE:
				break;
			case WHERE_LINE:
				break;
			case INSERTION_VEHICLE:
				break;
			case INSERTION_LINE:
				break;
		}
		
	} while(!feof(stdin));


	char* command = readline(stdin);
	int op = atoi(command);
	while(command){
		char* field = strchr(command,' ')+1;
		switch(op){
			case 1:{
				char* csvName = strndup(field,strcspn(field," "));
				field = strchr(field,' ')+1;
				char* binName = strndup(field,strcspn(field,"\0"));
				FILE* csv = fopen(csvName,"rb");
				FILE* bin = fopen(binName,"wb");
				free(csvName);

				VTable* vtable = readVehicleCsv(csv,",");
				fclose(csv);
				writeVehicleBinary(vtable,bin);
				fclose(bin);
				freeVTable(vtable);
				
				binarioNaTela(binName);
				free(binName);
				break;
			}
			case 2:{
				char* csvName = strndup(field,strcspn(field," "));
				field = strchr(command,' ')+1;
				char* binName = strndup(field,strcspn(field,"\0"));
				FILE* csv = fopen(csvName,"rb");
				FILE* bin = fopen(binName,"wb");
				free(csvName);

				LTable* ltable = readLineCsv(csv);
				fclose(csv);
				writeLineBinary(ltable,bin);
				fclose(bin);
				
				binarioNaTela(binName);
				free(binName);
				break;
			}
			case 3:{
				char* binName = strndup(field,strcspn(field,"\0"));
				FILE* bin = fopen(binName,"rb");
				VTable* vtable = readVehicleBinary(bin);
				fclose(bin);

				selectVehicle(vtable);
				
				binarioNaTela(binName);
				free(binName);
				break;
			}
			case 4:{
				char* binName = strndup(field,strcspn(field,"\0"));
				FILE* bin = fopen(binName,"rb");
				LTable* ltable = readLineBinary(bin);
				fclose(bin);
				
				selectLine(ltable);
				
				// binarioNaTela(binName);
				free(binName);
				break;
			}
			case 5:{
				char* binName = strndup(field,strcspn(field,"\0"));
				field = strchr(command,' ')+1;
				char* cmpField = strndup(field,strcspn(field," "));
				field = strchr(command,' ')+1;
				char* fieldValue = strndup(field,strcspn(field,"\0"));

				void* key = fieldValue;
				bool (*cmpVehicle)(VEntry*,void*);
				if(cmpField[0] == 'p'){	// FUTURE-FIX: maybe replace with switch? block will be bigger tho
					cmpVehicle = matchVehiclePrefix;
				} else if(cmpField[0] == 'd'){
					cmpVehicle = matchVehicleData;
				} else if(cmpField[0] == 's'){
					cmpVehicle = matchVehicleSeatQty;
					int seatQty = atoi(fieldValue);
					key = &seatQty;
				} else if(cmpField[0] == 'm'){
					cmpVehicle = matchVehicleModel;
				} else {
					cmpVehicle = matchVehicleCategory;
				}

				FILE* bin = fopen(binName,"rb");
				VTable* vtable = readVehicleBinary(bin);
				fclose(bin);

				selectVehicleWhere(vtable,key,cmpVehicle);
				freeVTable(vtable);
				
				free(binName);
				break;
			}
			case 6:{
				char* binName = strndup(field,strcspn(field," "));
				field = strchr(field,' ')+1;
				char* cmpField = strndup(field,strcspn(field," "));
				field = strchr(field,' ')+1;
				char* fieldValue = strndup(field,strcspn(field,"\0"));
				cleanString(fieldValue);

				int code;
				void* key = fieldValue;
				bool (*cmpLine)(LEntry*,void*);
				if(!strncmp(cmpField,"cod",3)){	// FUTURE-FIX: maybe replace with switch? block will be bigger tho
					cmpLine = matchLineCode;
					code = atoi(fieldValue);
					key = &code;
				} else if(cmpField[0] == 'a'){
					cmpLine = matchLineAcceptCard;
					key = &fieldValue[0];
				} else if(cmpField[0] == 'n'){
					cmpLine = matchLineName;
					int seatQty = atoi(fieldValue);
					key = &seatQty;
				} else if(!strncmp(cmpField,"cor",3)){
					cmpLine = matchLineColor;
				}

				FILE* bin = fopen(binName,"rb");
				LTable* ltable = readLineBinary(bin);
				fclose(bin);

				// printf("bin name:%s\n",binName);
				// printf("compare field:%s\n",cmpField);
				// printf("key:%s\n",(char*)key);
				selectLineWhere(ltable,key,cmpLine);
				freeLineTable(ltable);
				
				free(binName);
				break;
			}
			case 7:{
				char* binName = strndup(field,strcspn(field,"\0"));
				field = strchr(command,' ')+1;
				char* quantityString = strndup(field,strcspn(field," "));
				int insertQty = atoi(quantityString);

				
				FILE* bin = fopen(binName,"rb");
				VTable* vtable = readVehicleBinary(bin);
				fclose(bin);

				insertVehicleEntries(vtable,insertQty,",",bin);
				freeVTable(vtable);

				binarioNaTela(binName);
				
				free(binName);
			}
			case 8:{
				char* binName = strndup(field,strcspn(field," "));
				field = strchr(field,' ')+1;
				char* quantityString = strndup(field,strcspn(field,"\0"));
				int insertQty = atoi(quantityString);

				printf("bin name:%s\n",binName);
				printf("insert:%d\n",insertQty);
				
				FILE* bin = fopen(binName,"r+");
				LTable* ltable = readLineBinary(bin);

				insertLineEntries(ltable,insertQty,bin);
				freeLineTable(ltable);
				fclose(bin);

				binarioNaTela(binName);
				
				free(binName);
			}
			default:
				printf("O comando veio zuado, nao tava entre [1..9]\n");
				exit(1);
		}

		free(command);
		command = readline(stdin);
	}
	

	free(command);
	return 0;
}