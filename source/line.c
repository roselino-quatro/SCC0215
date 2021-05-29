#include "line.h"

LInfo* LInfoFromString(char* src){
	LInfo* info = malloc(sizeof(LInfo));
	info->stable = '0';
	info->byteOffset = 83;
	info->qty = 0;
	info->rmvQty = 0;

	char** fields = getFields(4,src);
	memcpyField(info->code,fields[0],15);
	memcpyField(info->card,fields[1],13);
	memcpyField(info->name,fields[2],13);
	memcpyField(info->line,fields[3],24);

	int pos = 0;
	while(fields[pos]) free(fields[pos++]);
	free(fields);

	return info;
}

LInfo* LInfoFromBytes(char* bytes){
	LInfo* info = malloc(sizeof(LInfo));

	int shift = 0;
	shift += memcpyField(&info->stable,bytes+shift,sizeof(char));
	shift += memcpyField(&info->byteOffset,bytes+shift,sizeof(long));
	shift += memcpyField(&info->qty,bytes+shift,sizeof(int));
	shift += memcpyField(&info->rmvQty,bytes+shift,sizeof(int));
	shift += memcpyField(info->code,bytes+shift,15*sizeof(char));
	shift += memcpyField(info->card,bytes+shift,13*sizeof(char));
	shift += memcpyField(info->name,bytes+shift,13*sizeof(char));
	shift += memcpyField(info->line,bytes+shift,24*sizeof(char));

	return info;
}

char* LInfoAsBytes(LInfo* info){
	char* bytes = malloc(84);

	int shift = 0;
	shift += memcpyField(bytes+shift,&info->stable,sizeof(char));
	shift += memcpyField(bytes+shift,&info->byteOffset,sizeof(long));
	shift += memcpyField(bytes+shift,&info->qty,sizeof(int));
	shift += memcpyField(bytes+shift,info->code,15*sizeof(char));
	shift += memcpyField(bytes+shift,info->card,13*sizeof(char));
	shift += memcpyField(bytes+shift,info->name,13*sizeof(char));
	shift += memcpyField(bytes+shift,info->line,24*sizeof(char));
	shift += memcpyField(bytes+shift,info->line,26*sizeof(char));

	return bytes;
}

LEntry* LEntryFromString(char* src){
	LEntry* entry = malloc(sizeof(LEntry));
	entry->size = 14;

	char** fields = getFields(4,src);
	if(fields[0][0] != '*'){
		entry->lineCode = atoi(fields[0]);
		entry->isPresent = '1';
	} else {
		entry->lineCode = atoi(&fields[0][1]);
		entry->isPresent = '0';
	};

	entry->card = (strlen(fields[1]) == 1)? *fields[1] : '*';
	

	strcpy(entry->name,fields[2]);
	if(!strncmp(entry->name,"NULO",4)){
		entry->name[0] = '\0';
		entry->nameLen = 0;
	} else {
		entry->nameLen = strlen(entry->name);
	}

	strcpy(entry->color,fields[3]);
	if(!strncmp(entry->color,"NULO",4)){
		entry->color[0] = '\0';
		entry->colorLen = 0;
	} else {
		entry->colorLen = strlen(entry->color);
	}

	entry->size += entry->modelLen + entry->categoryLen;

	int pos = 0;
	while(fields[pos]) free(fields[pos++]);
	free(fields);
	return entry;
}

LEntry* LEntryFromBytes(char* bytes){
	LEntry* entry = malloc(sizeof(LEntry));

	int shift = 0;
	shift += memcpyField(&entry->isPresent,bytes+shift,sizeof(char));
	shift += memcpyField(&entry->size,bytes+shift,sizeof(int));
	entry->size += 5;
	shift += memcpyField(entry->lineCode,bytes+shift,sizeof(int));
	shift += memcpyField(entry->card,bytes+shift,sizeof(char));
	shift += memcpyField(&entry->nameLen,bytes+shift,sizeof(int));
	shift += memcpyField(entry->name,bytes+shift,entry->nameLen*sizeof(char));
	shift += memcpyField(&entry->categoryLen,bytes+shift,sizeof(int));
	shift += memcpyField(entry->category,bytes+shift,entry->categoryLen*sizeof(char));
	entry->category[entry->categoryLen] = '\0';

	return entry;
}

char* LEntryAsBytes(LEntry* entry){
	char* bytes = malloc(entry->size);

	int shift = 0;
	shift += memcpyField(bytes+shift,&entry->isPresent,sizeof(char));
	entry->size -= 5;	// size = all - isPresent - size
	shift += memcpyField(bytes+shift,&entry->size,sizeof(int));
	entry->size += 5;	// size = all others + isPresent + size
	shift += memcpyField(bytes+shift,entry->lineCode,sizeof(int));
	shift += memcpyField(bytes+shift,entry->card,sizeof(char));
	shift += memcpyField(bytes+shift,&entry->nameLen,sizeof(int));
	if(entry->nameLen > 0){
		shift += memcpyField(bytes+shift,entry->name,entry->nameLen*sizeof(char));
	}
	shift += memcpyField(bytes+shift,&entry->colorLen,sizeof(int));
	if(entry->colorLen > 0){
		shift += memcpyField(bytes+shift,entry->color,entry->colorLen*sizeof(char));
	}

	return bytes;
}

// Lê e parsea o CSV do arquivo e armazena em uma struct
LTable* readLineCsv(FILE* csv){
	if(csv == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return NULL;
	}

	LTable* table = malloc(sizeof(LTable));

	char* headerString = readline(csv);
	LInfo* info = LInfoFromString(headerString);
	free(headerString);

	// While there are entries in the csv, allocate and process
	table->qty = 1;
	table->fleet = NULL;
	int i = 0;
	// char* entryString = readline(csv);
	char* entryString;
	while((entryString = readline(csv)) != NULL){
		if(i+1 == table->qty){
			table->qty *= 2;
			table->fleet = realloc(table->fleet,table->qty * sizeof(LEntry));
		}

		LEntry* entry = LEntryFromString(entryString);
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
	table->fleet = realloc(table->fleet,table->qty * sizeof(LEntry));
	return table;
}

// Destructor que libera memoria alocada de uma struct LTable
bool freeLineTable(LTable* table){
	if (table == NULL) return false;

	free(table->header);
	free(table->entries);
	free(table);

	return true;
}

LTable* readLineBinary(FILE* bin){
	if(bin == NULL){
		printf("Falha no processamento do arquivo\n");
		return NULL;
	}

	LTable* table = malloc(sizeof(LTable));

	char buffer[140];
	fread(buffer,sizeof(char),83,bin);
	LInfo* info = LInfoFromBytes(buffer);

	// Read all entries from binary file
	table->qty = 1;
	table->fleet = NULL;
	int i = 0;
	while(fread(buffer,sizeof(char),5,bin) != 0){
		if(i+1 == table->qty){
			table->qty *= 2;
			table->fleet = realloc(table->fleet,table->qty * sizeof(LEntry));
		}

		int entrySize;
		memcpy(&entrySize,&buffer[1],sizeof(int));
		fread(&buffer[5],sizeof(char),entrySize,bin);

		LEntry* entry = LEntryFromBytes(buffer);
		table->fleet[i++] = *entry;
		free(entry);
		// TEST: Removed entry count, if read binary header is not trustworthy, value will be wrong!
		// TEST: Removed byteoffset count also
	}

	// Bind info to table header and fit fleet to size
	table->qty = i;
	info->stable = '1';
	table->header = info;
	table->fleet = realloc(table->fleet,table->qty * sizeof(LEntry));
	return table;
}

// Transfere os dados de uma LTable para um arquivo binario seguindo as regras passadas nas especificaçẽos
void writeLineBinary(LTable* table,FILE* bin){
	if(bin == NULL){
		printf("Falha no processamento do arquivo\n");
		return;
	}

	// Write header data - mark the file as unstable until end of write
	table->header->stable = '0';
	char* header = LInfoAsBytes(table->header);
	fwrite(header,sizeof(char),83,bin);
	free(header);

	for(int i = 0;i < table->qty;i++){
		char* entry = LEntryAsBytes(&table->fleet[i]);
		fwrite(entry,sizeof(char),table->fleet[i].size,bin);
		free(entry);
	}

	rewind(bin);
	fwrite("1",sizeof(char),1,bin);
}

void displayLine(LEntry* entry){
	if(!entry) return;

	printf("Nome da linha: %s\n",(entry->nameLen != 0)? entry->name : "campo com valor nulo");
	printf("Cor que descreve a linha: %s\n",entry->color);

	if(entry->card == 'S'){
		printf("Aceita cartao: PAGAMENTO  SOMENTE  COM  CARTAO  SEM  PRESENCA  DE COBRADOR\n\n");
	} else if(entry->card == 'N'){
		printf("Aceita cartao: PAGAMENTO EM CARTAO E DINHEIRO\n\n");
	} else if(entry->card == 'F'){
		printf("Aceita cartao: PAGAMENTO EM CARTAO SOMENTE NO FINAL DE SEMANA\n\n");
	} else{
		printf("Aceita cartao: campo com valor nulo\n\n");
	}
};

// Familia de funções para a selectLineWhere
bool matchLineCode(LEntry* entry,void* code){
	return (entry->lineCode == *(int*)code)? true : false;
};

bool matchLineAcceptCard(LEntry* entry,void* cardStatus){
	if(entry->card == '*') return false;
	return (entry->card == *(char*)cardStatus)? true : false;
};

bool matchLineName(LEntry* entry,void* name){
	if (entry->name[0] == '\0') return false;
	return (strcmp(entry->name,(char*)name) == 0)? true : false;
};

bool matchLineColor(LEntry* entry,void* color){
	if (entry->color[0] == '\0') return false;
	return (strcmp(entry->color,(char*)color) == 0)? true : false;
};


// Imprime todos os registros não removidos de uma struct
void selectLine(LTable* table){
	if(table == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	bool anyMatched = false;
	for(int i = 0;i < table->qty;i++){
		LEntry* entry = &table->entries[i];
		if(entry->isPresent == '0') continue;

		displayLine(entry);
		anyMatched = true;
	}

	if (!anyMatched) printf("Registro inexistente\n");
}


// Imprime os matchs de uma comparação dentro de uma struct Dat
void selectLineWhere(LTable* table,void* key,bool (*match)(LEntry*,void*)){
	if(table == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	bool anyMatched = false;
	for(int i = 0;i < table->qty;i++){
		LEntry* entry = &table->entries[i];
		if (entry->isPresent == '0' || !(match(entry,key)) continue;

		displayLine(entry);
		anyMatched = true;
	}

	if (!anyMatched) printf("Registro inexistente\n");
}

void insertLineEntries(LTable* table,int qty,FILE* bin){
	rewind(bin);	// Rewind and mark binary as unstable
	fwrite("0",sizeof(char),1,bin);
	fseek(bin,0,SEEK_END);

	table->fleet = realloc(table->fleet,table->qty+qty);
	for(int i = 0;i < qty;i++){
		char* entryString = readline(stdin);
		cleanString(entryString);
		LEntry* entry = LEntryFromString(entryString);
		table->header->byteOffset += entry->size;

		char* entryBytes = LEntryAsBytes(entry);
		fwrite(entryBytes,sizeof(char),entry->size,bin);

		(entry->isPresent == '1')? ++table->header->qty : ++table->header->rmvQty;

		table->fleet[i] = *entry;
		free(entry);
	}

	table->qty += qty;
	rewind(bin);	// Rewind and mark binary as stable
	fwrite("1",sizeof(char),1,bin);
	fwrite(&table->header->byteOffset,sizeof(long),1,bin);
	fwrite(&table->header->qty,sizeof(int),1,bin);
	fwrite(&table->header->rmvQty,sizeof(int),1,bin);
}