#include "line.h"

LineData* readLineCsv(FILE* csv){
	LineData* lData = malloc(sizeof(LineData));

	// Initialize and read header from csv file
	lData->header.isStable = true;
	lData->header.regByteOff = 81;
	lData->header.regQty = 0;
	lData->header.regRemovedQty = 0;

	fscanf(csv,"%[^,],",lData->header.descCode);
	fscanf(csv,"%[^,],",lData->header.descCard);
	fscanf(csv,"%[^,],",lData->header.descName);
	fscanf(csv,"%[^\n]\n",lData->header.descLine);

	// While there are registers in the csv, allocate and process
	lData->regQty = 1;
	lData->registers = malloc(lData->regQty * sizeof(LineReg));
	int rpos = 0;
	char code[4];
	while (fscanf(csv,"%[^,],",code) != EOF) {
		if (rpos+1 == lData->regQty){
			lData->regQty *= 2;
			lData->registers = realloc(lData->registers,lData->regQty * sizeof(LineReg));
		}

		LineReg* cReg = &lData->registers[rpos++];
		if (code[0] == '*') {
			cReg->lineCode = atoi(&code[1]);
			cReg->isPresent = true;
		} else {
			cReg->lineCode = atoi(code);
			cReg->isPresent = false;
		}
		
		fscanf(csv,"%c,",&cReg->acceptsCard);
		fscanf(csv,"%[^,],",cReg->name);
		cReg->nameSize = strlen(cReg->name);
		fscanf(csv,"%[^\n]\n",cReg->color);
		cReg->colorSize = strlen(cReg->color);
		cReg->regSize = 23 + cReg->nameSize + cReg->colorSize;
	}

	// Shrink registers array back to appropriate size
	lData->regQty = rpos;
	lData->registers = realloc(lData->registers,(lData->regQty) * sizeof(LineReg));
	return lData;
}

void writeLineBinary(LineData* lData,FILE* binDest){
	// Write header data - mark the file as unstable until end of write
	lData->header.isStable = false;
	fwrite(&lData->header.isStable,1,1,binDest);
	fwrite(&lData->header.regByteOff,8,1,binDest);
	fwrite(&lData->header.regQty,4,1,binDest);
	fwrite(&lData->header.regRemovedQty,4,1,binDest);
	fwrite(&lData->header.descCode,1,15,binDest);
	fwrite(&lData->header.descCard,1,13,binDest);
	fwrite(&lData->header.descName,1,13,binDest);
	fwrite(&lData->header.descLine,1,24,binDest);

	// Iterate through registers and write their data
	int regpos = 0;
	while (regpos < lData->regQty){
		LineReg* curReg = &lData->registers[regpos++];
		fwrite(&curReg->isPresent,1,1,binDest);
		fwrite(&curReg->regSize,4,1,binDest);
		fwrite(&curReg->lineCode,4,1,binDest);
		fwrite(&curReg->acceptsCard,1,1,binDest);
		fwrite(&curReg->nameSize,4,1,binDest);
		fwrite(&curReg->name,1,curReg->nameSize,binDest);
		fwrite(&curReg->colorSize,4,1,binDest);
		fwrite(&curReg->color,1,curReg->colorSize,binDest);
	}

	// Rewind and mark file as stable
	rewind(binDest);
	lData->header.isStable = true;
	fwrite(&lData->header.isStable,1,1,binDest);
}
