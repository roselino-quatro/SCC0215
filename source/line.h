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

/****
 * Lê e parsea o CSV do arquivo e armazena em uma struct
 * 
 * @param csv ponteiro para o arquivo a ser lido
 * @return ponteiro para a struct allocada na função
 */
LineData* readLineCsv(FILE* csv);

/****
 * Destructor que libera memoria alocada de uma struct LineData
 * 
 * @param lData ponteiro para struct a ser liberada
 * @return boolean para caso a função tenha sucesso ou não
 */
bool freeLineData(LineData* lData);


/****
 * Transfere os dados de uma LineData para um arquivo binario seguindo as regras passadas nas especificaçẽos
 * 
 * @param lData struct a ser transferida
 * @param binDest arquivo alvo
 */
void writeLineBinary(LineData* lData,FILE* binDest);
/****
 * Imprime informações do registro de uma linha
 * 
 * @param lReg registro a ser impresso
 */
void displayLine(LineReg* lReg);

/****
 * Destructor que libera memoria alocada de uma struct LineData
 * 
 * @param lData ponteiro para struct a ser liberada
 * @return boolean para caso a função tenha sucesso ou não
 */
bool freeLineData(LineData* lData);

/****
 * Familia de funções para a selectLineWhere
 * 
 * @param lReg registro a ser verificado
 * @param secondParameter valor a ser comparado
 */
bool matchLineCode(LineReg* lReg,void* code);
bool matchLineAcceptCard(LineReg* lReg,void* cardStatus);
bool matchLineName(LineReg* lReg,void* name);
bool matchLineColor(LineReg* lReg,void* color);

/****
 * Imprime os matchs de uma comparação dentro de uma struct Data
 * 
 * @param lData struct a ser buscada
 * @param match ponteiro de função para uma das funções match
 */
void selectLineWhere(LineData* lData,void* key,bool (*match)(LineReg*,void*));

/****
 * Imprime todos os registros não removidos de uma struct
 * 
 * @param lReg registro a ser verificado 
 */
void selectLine(LineData* lData) {

#endif