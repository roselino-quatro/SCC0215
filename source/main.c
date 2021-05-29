#include "vehicle.h"
#include "line.h"
#include "utils.h"

// Exemplo de formatar string pra csv
// char* input = readline(stdin);
// cleanString(input);
// char** args = getFields(quantidade,input);
// args[0], args[1] ... vai ter <quantidade> strings (todas alocadas, lembra de dar free dps)

int main(){
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