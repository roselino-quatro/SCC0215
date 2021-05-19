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

LineData readLineCsv(FILE* csv);