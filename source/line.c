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

LEntry* LEntryFromString(char* src,char* delim){
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

	entry->card = *fields[1];
	

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
bool freeLineData(LTable* table) {
	if (table == NULL) return false;

	free(table->entries);
	free(table);

	return true;
}

LTable* readLineBinary(FILE* bin) {
	if(bin == NULL) return NULL;	// Caso arquivo passado seja invalido

	LTable* lData = malloc(sizeof(LTable));

	// Read header data - mark the file as unstable until end of read
	char header[82];
	fread(&header,sizeof(char),82,bin);

	lData->header->stable = false;
	memcpy(&lData->header->byteOffset,&header[1],8);
	memcpy(&lData->header->qty,&header[9],4);
	memcpy(&lData->header->rmvQty,&header[13],4);
	memcpy(&lData->header->code,&header[17],15);
	memcpy(&lData->header->card,&header[32],13);
	memcpy(&lData->header->name,&header[45],13);
	memcpy(&lData->header->line,&header[58],24);

	// Iterate through entries and write their data
	lData->regQty = 1;
	lData->entries = malloc(lData->regQty * sizeof(LEntry));
	int rpos = 0;
	char regBuffer[14];
	while (fread(&regBuffer,sizeof(char),14,bin) != 0) {
		// Exponential reallocation check on every entry
		if (rpos+1 == lData->regQty){
			lData->regQty *= 2;
			lData->entries = realloc(lData->entries,lData->regQty * sizeof(LEntry));
		}

		LEntry* cReg = &lData->entries[rpos++];

		memcpy(&cReg->isPresent,&regBuffer[0], sizeof(char));
		memcpy(&cReg->size,&regBuffer[1], sizeof(int));
		memcpy(&cReg->lineCode,&regBuffer[5], sizeof(int));
		memcpy(&cReg->card,&regBuffer[9], sizeof(char));
		memcpy(&cReg->nameLen,&regBuffer[10], sizeof(int));

		fread(&cReg->name,sizeof(char),cReg->nameLen,bin);
		
		fread(&cReg->colorLen,sizeof(int),1,bin);
		fread(&cReg->color,sizeof(char),cReg->colorLen,bin);
	}
	
	// Rewind and mark file as stable
	lData->header->stable = true;
	lData->regQty = rpos;

	return lData;
}

// Transfere os dados de uma LTable para um arquivo binario seguindo as regras passadas nas especificaçẽos
void writeLineBinary(LTable* lData,FILE* binDest){
	// Write header data - mark the file as unstable until end of write
	lData->header->stable = false;
	fwrite(&lData->header->stable, sizeof(char), 1, binDest);
	fwrite(&lData->header->byteOffset, sizeof(long), 1, binDest);
	fwrite(&lData->header->qty, sizeof(int), 1, binDest);
	fwrite(&lData->header->rmvQty, sizeof(int), 1, binDest);
	fwrite(&lData->header->code, sizeof(char), 15, binDest);
	fwrite(&lData->header->card, sizeof(char), 13, binDest);
	fwrite(&lData->header->name, sizeof(char), 13, binDest);
	fwrite(&lData->header->line, sizeof(char), 24, binDest);

	// Iterate through entries and write their data
	int regpos = 0;
	while (regpos < lData->regQty){
		LEntry* curReg = &lData->entries[regpos++];
		fwrite(&curReg->isPresent, sizeof(char), 1, binDest);
		fwrite(&curReg->size, sizeof(int), 1, binDest);
		fwrite(&curReg->lineCode, sizeof(int), 1, binDest);
		fwrite(&curReg->card, sizeof(char), 1, binDest);
		fwrite(&curReg->nameLen, sizeof(int), 1, binDest);
		fwrite(&curReg->name, sizeof(char), curReg->nameLen, binDest);
		fwrite(&curReg->colorLen, sizeof(int), 1, binDest);
		fwrite(&curReg->color, sizeof(char), curReg->colorLen, binDest);
	}

	// Rewind and mark file as stable
	rewind(binDest);
	lData->header->stable = true;
	fwrite(&lData->header->stable,1,1,binDest);
}

void displayLine(LEntry* lReg){
	if(!lReg) return;

	printf("Nome da linha: %s\n",(lReg->nameLen != 0)? lReg->name : "campo com valor nulo");
	printf("Cor que descreve a linha: %s\n",lReg->color);

	if(lReg->card == 'S'){
		printf("Aceita cartao: PAGAMENTO  SOMENTE  COM  CARTAO  SEM  PRESENCA  DE COBRADOR\n\n");
	} else if(lReg->card == 'N'){
		printf("Aceita cartao: PAGAMENTO EM CARTAO E DINHEIRO\n\n");
	} else if(lReg->card == 'F'){
		printf("Aceita cartao: PAGAMENTO EM CARTAO SOMENTE NO FINAL DE SEMANA\n\n");
	} else{
		printf("Aceita cartao: campo com valor nulo\n\n");
	}
};

// Familia de funções para a selectLineWhere
bool matchLineCode(LEntry* lReg,void* code) {
	return (lReg->lineCode == *(int*)code)? true : false;
};

bool matchLineAcceptCard(LEntry* lReg,void* cardStatus) {
	return (lReg->card == *(char*)cardStatus)? true : false;
};

bool matchLineName(LEntry* lReg,void* name) {
	if (lReg->name[0] == '\0') return false;
	return (strcmp(lReg->name,(char*)name) == 0)? true : false;
};

bool matchLineColor(LEntry* lReg,void* color) {
	if (lReg->color[0] == '\0') return false;
	return (strcmp(lReg->color,(char*)color) == 0)? true : false;
};

// Imprime os matchs de uma comparação dentro de uma struct Dat
void selectLineWhere(LTable* lData,void* key,bool (*match)(LEntry*,void*)) {
	if(lData == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	bool anyMatched = false;
	int regPos = 0;
	while(regPos < lData->regQty) {
		LEntry* curReg = &lData->entries[regPos++];
		if (!curReg->isPresent) continue;

		if (match(curReg,key)) {
			displayLine(curReg);
			anyMatched = true;
		}
	}

	if (!anyMatched) printf("Registro inexistente\n");
}

// Imprime todos os registros não removidos de uma struct
void selectLine(LTable* lData) {
	if(lData == NULL) {
		printf("Falha no processamento do arquivo.\n");
		return;
	}

	bool anyMatched = false;
	int regPos = 0;
	while(regPos < lData->regQty) {
		LEntry* curReg = &lData->entries[regPos++];
		if (curReg->isPresent) {
			displayLine(curReg);
			anyMatched = true;
		}
	}

	if (!anyMatched) printf("Registro inexistente\n");
}
