#include "line.h"

// Lê e parsea o CSV do arquivo e armazena em uma struct
LTable* readLineCsv(FILE* csv){
	if(csv == NULL) {
		printf("Falha no processamentodo arquivo.\n");
		return NULL;
	}
	LTable* lData = malloc(sizeof(LTable));

	// Initialize and read header from csv file
	lData->header->stable = true;
	lData->header->byteOffset = 81;
	lData->header->qty = 0;
	lData->header->rmvQty = 0;

	fscanf(csv,"%[^,],",lData->header->code);
	fscanf(csv,"%[^,],",lData->header->card);
	fscanf(csv,"%[^,],",lData->header->name);
	fscanf(csv,"%[^\n]\n",lData->header->line);

	// While there are registers in the csv, allocate and process
	lData->regQty = 1;
	lData->registers = malloc(lData->regQty * sizeof(LEntry));
	int rpos = 0;
	char code[4];
	while (fscanf(csv,"%[^,],",code) != EOF) {
		if (rpos+1 == lData->regQty){
			lData->regQty *= 2;
			lData->registers = realloc(lData->registers,lData->regQty * sizeof(LEntry));
		}

		LEntry* cReg = &lData->registers[rpos++];
		if (code[0] != '*') {
			cReg->lineCode = atoi(code);
			cReg->isPresent = true;
		} else {
			cReg->lineCode = atoi(&code[1]);
			cReg->isPresent = false;
		}
		
		fscanf(csv,"%c,",&cReg->card);

		fscanf(csv,"%[^,],",cReg->name);
		cReg->nameLen = strlen(cReg->name);
		if(strncmp(cReg->name,"NULO",4) == 0){
			cReg->name[0] = '\0';
			cReg->nameLen = 0;
		}
		
		fscanf(csv,"%[^\n]\n",cReg->color);
		cReg->colorLen = strlen(cReg->color);
		cReg->size = 23 + cReg->nameLen + cReg->colorLen;
	}

	// Shrink registers array back to appropriate size
	lData->regQty = rpos;
	lData->registers = realloc(lData->registers,(lData->regQty) * sizeof(LEntry));
	return lData;
}

// Destructor que libera memoria alocada de uma struct LTable
bool freeLineData(LTable* lData) {
	if (lData == NULL) return false;

	free(lData->registers);
	free(lData);

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

	// Iterate through registers and write their data
	lData->regQty = 1;
	lData->registers = malloc(lData->regQty * sizeof(LEntry));
	int rpos = 0;
	char regBuffer[14];
	while (fread(&regBuffer,sizeof(char),14,bin) != 0) {
		// Exponential reallocation check on every entry
		if (rpos+1 == lData->regQty){
			lData->regQty *= 2;
			lData->registers = realloc(lData->registers,lData->regQty * sizeof(LEntry));
		}

		LEntry* cReg = &lData->registers[rpos++];

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

	// Iterate through registers and write their data
	int regpos = 0;
	while (regpos < lData->regQty){
		LEntry* curReg = &lData->registers[regpos++];
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
		LEntry* curReg = &lData->registers[regPos++];
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
		LEntry* curReg = &lData->registers[regPos++];
		if (curReg->isPresent) {
			displayLine(curReg);
			anyMatched = true;
		}
	}

	if (!anyMatched) printf("Registro inexistente\n");
}
