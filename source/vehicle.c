// Gabriel Victor Cardoso Fernandes nUsp 11878296
// Lourenço de Salles Roselino nUsp 11796805
#include "vehicle.h"

VInfo* VInfoFromString(char* src){
	VInfo* info = malloc(sizeof(VInfo));
	info->stable = '0';
	info->byteOffset = 175;
	info->qty = 0;
	info->rmvQty = 0;

	char** fields = getFields(6,src);
	int shift = 0;
	shift+= memcpyField(info->prefix,fields[0],18) + 1;
	shift+= memcpyField(info->data,fields[1],35) + 1;
	shift+= memcpyField(info->seats,fields[2],42) + 1;
	shift+= memcpyField(info->line,fields[3],26) + 1;
	shift+= memcpyField(info->model,fields[4],17) + 1;
	shift+= memcpyField(info->category,fields[5],20) + 1;

	int pos = 0;
	while(fields[pos]) free(fields[pos++]);
	free(fields);

	return info;
}

VInfo* VInfoFromBytes(char* bytes){
	VInfo* info = malloc(sizeof(VInfo));

	int shift = 0;
	shift += memcpyField(&info->stable,bytes+shift,sizeof(char));
	shift += memcpyField(&info->byteOffset,bytes+shift,sizeof(long));
	shift += memcpyField(&info->qty,bytes+shift,sizeof(int));
	shift += memcpyField(&info->rmvQty,bytes+shift,sizeof(int));
	shift += memcpyField(info->prefix,bytes+shift,18*sizeof(char));
	shift += memcpyField(info->data,bytes+shift,35*sizeof(char));
	shift += memcpyField(info->seats,bytes+shift,42*sizeof(char));
	shift += memcpyField(info->line,bytes+shift,26*sizeof(char));
	shift += memcpyField(info->model,bytes+shift,17*sizeof(char));
	shift += memcpyField(info->category,bytes+shift,20*sizeof(char));

	return info;
}

char* VInfoAsBytes(VInfo* info){
	char* bytes = malloc(176);

	int shift = 0;
	shift += memcpyField(bytes+shift,&info->stable,sizeof(char));
	shift += memcpyField(bytes+shift,&info->byteOffset,sizeof(long));
	shift += memcpyField(bytes+shift,&info->qty,sizeof(int));
	shift += memcpyField(bytes+shift,&info->rmvQty,sizeof(int));
	shift += memcpyField(bytes+shift,info->prefix,18*sizeof(char));
	shift += memcpyField(bytes+shift,info->data,35*sizeof(char));
	shift += memcpyField(bytes+shift,info->seats,42*sizeof(char));
	shift += memcpyField(bytes+shift,info->line,26*sizeof(char));
	shift += memcpyField(bytes+shift,info->model,17*sizeof(char));
	shift += memcpyField(bytes+shift,info->category,20*sizeof(char));

	return bytes;
}

VEntry* VEntryFromString(char* src){
	VEntry* entry = malloc(sizeof(VEntry));
	entry->size = 36;

	char** fields = getFields(6,src);
	memcpy(entry->prefix,fields[0],5);
	if(entry->prefix[0] != '*'){
		entry->isPresent = '1';
	} else {
		entry->prefix[0] = entry->prefix[1];
		entry->prefix[1] = entry->prefix[2];
		entry->prefix[2] = entry->prefix[3];
		entry->prefix[3] = entry->prefix[4];
		entry->prefix[4] = '\0';
		entry->isPresent = '0';
	}
	
	if(!strncmp(fields[1],"NULO",4)){
		memcpy(entry->data,"\0@@@@@@@@@",10);
	} else {
		memcpy(entry->data,fields[1],10);
	}
	
	entry->seatQty = atoi(fields[2]);
	if(entry->seatQty == 0) entry->seatQty = -1;
	
	entry->lineCode = atoi(fields[3]);
	if(entry->lineCode == 0) entry->lineCode = -1;

	strcpy(entry->model,fields[4]);
	if(!strncmp(entry->model,"NULO",4)){
		entry->model[0] = '\0';
		entry->modelLen = 0;
	} else {
		entry->modelLen = strlen(entry->model);
	}

	strcpy(entry->category,fields[5]);
	if(!strncmp(entry->category,"NULO",4)){
		entry->category[0] = '\0';
		entry->categoryLen = 0;
	} else {
		entry->categoryLen = strlen(entry->category);
	}

	entry->size += entry->modelLen + entry->categoryLen;

	int pos = 0;
	while(fields[pos]) free(fields[pos++]);
	free(fields);
	return entry;
}

VEntry* VEntryFromBytes(char* bytes){
	VEntry* entry = malloc(sizeof(VEntry));

	int shift = 0;
	shift += memcpyField(&entry->isPresent,bytes+shift,sizeof(char));
	shift += memcpyField(&entry->size,bytes+shift,sizeof(int));
	entry->size += 5;
	shift += memcpyField(entry->prefix,bytes+shift,5*sizeof(char));
	shift += memcpyField(entry->data,bytes+shift,10*sizeof(char));
	shift += memcpyField(&entry->seatQty,bytes+shift,sizeof(int));
	shift += memcpyField(&entry->lineCode,bytes+shift,sizeof(int));
	shift += memcpyField(&entry->modelLen,bytes+shift,sizeof(int));
	shift += memcpyField(entry->model,bytes+shift,entry->modelLen*sizeof(char));
	entry->model[entry->modelLen] = '\0';
	shift += memcpyField(&entry->categoryLen,bytes+shift,sizeof(int));
	shift += memcpyField(entry->category,bytes+shift,entry->categoryLen*sizeof(char));
	entry->category[entry->categoryLen] = '\0';

	return entry;
}

char* VEntryAsBytes(VEntry* entry){
	char* bytes = malloc(entry->size);

	int shift = 0;
	shift += memcpyField(bytes+shift,&entry->isPresent,sizeof(char));
	entry->size -= 5;	// size = all - isPresent - size
	shift += memcpyField(bytes+shift,&entry->size,sizeof(int));
	entry->size += 5;	// size = all others + isPresent + size
	shift += memcpyField(bytes+shift,entry->prefix,5*sizeof(char));
	shift += memcpyField(bytes+shift,entry->data,10*sizeof(char));
	shift += memcpyField(bytes+shift,&entry->seatQty,sizeof(int));
	shift += memcpyField(bytes+shift,&entry->lineCode,sizeof(int));
	shift += memcpyField(bytes+shift,&entry->modelLen,sizeof(int));
	if(entry->modelLen > -1){
		shift += memcpyField(bytes+shift,entry->model,entry->modelLen*sizeof(char));
	}
	shift += memcpyField(bytes+shift,&entry->categoryLen,sizeof(int));
	if(entry->categoryLen > -1){
		shift += memcpyField(bytes+shift,entry->category,entry->categoryLen*sizeof(char));
	}

	return bytes;
}

// Lê e parsea o CSV do arquivo e armazena em uma struct
VTable* readVehicleCsv(FILE* csv){
	if(csv == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return NULL;
	}

	VTable* table = malloc(sizeof(VTable));

	char* headerString = readline(csv);
	VInfo* info = VInfoFromString(headerString);
	free(headerString);

	// While there are registers in the csv, allocate and process
	table->qty = 1;
	table->fleet = NULL;
	int i = 0;
	// char* entryString = readline(csv);
	char* entryString;
	while((entryString = readline(csv)) != NULL){
		if(i+1 == table->qty){
			table->qty *= 2;
			table->fleet = realloc(table->fleet,table->qty * sizeof(VEntry));
		}

		VEntry* entry = VEntryFromString(entryString);
		table->fleet[i] = *entry;
		free(entry);
		(table->fleet[i].isPresent == '1')? ++info->qty : ++info->rmvQty;
		info->byteOffset += table->fleet[i++].size;
		free(entryString);
	}

	// Bind info to table header and fit fleet to size
	table->qty = i;
	info->stable = '1';
	table->header = info;
	table->fleet = realloc(table->fleet,table->qty * sizeof(VEntry));
	return table;
}

VTable* readVehicleBinary(FILE* bin){
	if(bin == NULL || fgetc(bin) != '1'){
		printf("Falha no processamento do arquivo.\n");
		return NULL;
	}
	rewind(bin);

	VTable* table = malloc(sizeof(VTable));

	char buffer[200];
	fread(buffer,sizeof(char),175,bin);
	VInfo* info = VInfoFromBytes(buffer);

	// Read all entries from binary file
	table->qty = 1;
	table->fleet = NULL;
	int i = 0;
	while(fread(buffer,sizeof(char),5,bin) != 0){
		if(i+1 == table->qty){
			table->qty *= 2;
			table->fleet = realloc(table->fleet,table->qty * sizeof(VEntry));
		}

		int entrySize;
		memcpy(&entrySize,&buffer[1],sizeof(int));
		fread(&buffer[5],sizeof(char),entrySize,bin);

		VEntry* entry = VEntryFromBytes(buffer);
		table->fleet[i++] = *entry;
		free(entry);
		// TEST: Removed entry count, if read binary header is not trustworthy, value will be wrong!
		// TEST: Removed byteoffset count also
	}

	// Bind info to table header and fit fleet to size
	table->qty = i;
	info->stable = '1';
	table->header = info;
	table->fleet = realloc(table->fleet,table->qty * sizeof(VEntry));
	return table;
}

// Destructor que libera memoria alocada de uma struct VehicleData
bool freeVehicleTable(VTable* table){
	if (table == NULL) return false;

	free(table->header);
	free(table->fleet);
	free(table);

	return true;
}

// Transfere os dados de uma VehicleData para um arquivo binario seguindo as regras passadas nas especificaçẽos
void writeVehicleBinary(VTable* table,FILE* bin){
	if(bin == NULL){
		printf("Falha no processamento do arquivo\n");
		return;
	}
	// Write header data - mark the file as unstable until end of write
	table->header->stable = '0';
	char* header = VInfoAsBytes(table->header);
	fwrite(header,sizeof(char),175,bin);
	free(header);

	for(int i = 0;i < table->qty;i++){
		char* entry = VEntryAsBytes(&table->fleet[i]);
		fwrite(entry,sizeof(char),table->fleet[i].size,bin);
		free(entry);
	}

	rewind(bin);
	fwrite("1",sizeof(char),1,bin);
}

// Imprime informações do registro de veiculo
void displayVehicle(VEntry* entry){
	char* nullField = "campo com valor nulo\0";
	printf("Prefixo do veiculo: %.5s\n",entry->prefix);
	printf("Modelo do veiculo: %s\n",(entry->model[0] != '\0')? entry->model : nullField);
	printf("Categoria do veiculo: %s\n",(entry->category[0] != '\0')? entry->category : nullField);

	char* months[12] = {"janeiro","fevereiro","março","abril","maio","junho","julho",
						"agosto","setembro","outubro","novembro","dezembro"};
	
	if(entry->data[0] == '\0'){
		printf("Data de entrada do veiculo na frota: campo com valor nulo\n");
	} else {	// FUTURE FIX: maybe just make all fields larger by 1 so they are null terminated
		char* dayStr = malloc(3);
		strncpy(dayStr,&entry->data[8],2);
		dayStr[2] = '\0';
		int day = atoi(dayStr);
		free(dayStr);

		char* month = months[atoi(&entry->data[5])-1];
		int year = atoi(&entry->data[0]);

		printf("Data de entrada do veiculo na frota: %02d de %s de %d\n",day,month,year);
	}

	if(entry->seatQty != -1){
		printf("Quantidade de lugares sentados disponiveis: %d\n\n",entry->seatQty);
	} else {
		printf("Quantidade de lugares sentados disponiveis: %s\n\n",nullField);
	}
}

// Familia de funções para a selectVehicleWhere
bool matchVehiclePrefix(VEntry* entry,void* prefix){
	return (strncmp(entry->prefix,(char*)prefix,5) == 0)? true : false;
}

bool matchVehicleData(VEntry* entry,void* data){
	if (entry->data[0] == '\0') return false;
	return (strncmp(entry->data,(char*)data,10) == 0)? true : false;
}

bool matchVehicleSeatQty(VEntry* entry,void* seatQty){
	if (entry->seatQty == -1) return false;
	return (entry->seatQty == atoi(seatQty))? true : false;
}

bool matchVehicleModel(VEntry* entry,void* model){
	if (entry->model[0] == '\0') return false;
	return (strcmp(entry->model,(char*)model) == 0)? true : false;
}

bool matchVehicleCategory(VEntry* entry,void* category){
	if (entry->category[0] == '\0') return false;
	return (strcmp(entry->category,(char*)category) == 0)? true : false;
}

void selectVehicle(VTable* table){
	if(table == NULL){
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	bool anyMatched = false;
	for(int i = 0;i < table->qty;i++){
		VEntry* entry = &table->fleet[i];
		if(entry->isPresent == '0') continue;

		displayVehicle(entry);
		anyMatched = true;
	}

	if(!anyMatched) printf("Registro inexistente.\n");
}

void selectVehicleWhere(VTable* table,void* key,bool (*match)(VEntry*,void*)){
	if(table == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	bool anyMatched = false;
	for(int i = 0;i < table->qty;i++){
		VEntry* entry = &table->fleet[i];
		if(entry->isPresent == '0' || !match(entry,key)) continue;

		displayVehicle(entry);
		anyMatched = true;
	}

	if (!anyMatched) printf("Registro inexistente.\n");
}

void insertVehicleEntries(VTable* table,int qty,FILE* bin){
	rewind(bin);	// Rewind and mark binary as unstable
	fwrite("0",sizeof(char),1,bin);
	fseek(bin,0,SEEK_END);

	table->fleet = realloc(table->fleet,(table->qty+qty)*sizeof(VEntry));
	for(int i = 0;i < qty;i++){
		char* entryString = readline(stdin);
		cleanString(entryString);
		VEntry* entry = VEntryFromString(entryString);
		table->header->byteOffset += entry->size;

		char* entryBytes = VEntryAsBytes(entry);
		fwrite(entryBytes,sizeof(char),entry->size,bin);

		(entry->isPresent == '1')? ++table->header->qty : ++table->header->rmvQty;

		table->fleet[i] = *entry;
		free(entryString);
		free(entryBytes);
		free(entry);
	}

	table->qty += qty;
	rewind(bin);	// Rewind and mark binary as stable
	fwrite("1",sizeof(char),1,bin);
	fwrite(&table->header->byteOffset,sizeof(long),1,bin);
	fwrite(&table->header->qty,sizeof(int),1,bin);
	fwrite(&table->header->rmvQty,sizeof(int),1,bin);
}