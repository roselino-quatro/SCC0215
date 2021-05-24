#ifndef _LINE_H
#define _LINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct LINE_HEADER {
	bool isStable;
	long regByteOff;
	int regQty;
	int regRemovedQty;
	char descCode[15];
	char descCard[13];
	char descName[13];
	char descLine[24];
}LineHeader;

typedef struct LINE_REGISTER {
	bool isPresent;
	int regSize;
	int lineCode;
	char acceptsCard;
	int nameSize;
	char name[100];
	int colorSize;
	char color[100];
}LineReg;

typedef struct LINE_DATA {
	LineHeader header;
	int regQty;
	LineReg* registers;
}LineData;

LineData* readLineCsv(FILE* csv);
void writeLineBinary(LineData* lData,FILE* binDest);
void displayLine(LineReg* lReg);

bool freeLineData(LineData* lData);

bool matchLineCode(LineReg* lReg,void* code);
bool matchLineAcceptCard(LineReg* lReg,void* cardStatus);
bool matchLineName(LineReg* lReg,void* name);
bool matchLineColor(LineReg* lReg,void* color);

void selectLineWhere(LineData* lData,void* key,bool (*match)(LineReg*,void*));

#endif