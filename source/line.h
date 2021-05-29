#ifndef _LINE_H
#define _LINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct _LineInfo{
	bool stable;
	long byteOffset;
	int qty;
	int rmvQty;
	char code[15];
	char card[13];
	char name[13];
	char line[24];
}LInfo;

typedef struct _LineHeader{
	bool isPresent;
	int size;
	int lineCode;
	char card;
	int nameLen;
	char name[100];
	int colorLen;
	char color[100];
}LEntry;

typedef struct _LineTable{
	LInfo* header;
	int qty;
	LEntry* entries;
}LTable;

/****
 * Lê e parsea o CSV do arquivo e armazena em uma struct
 * 
 * @param csv ponteiro para o arquivo a ser lido
 * @return ponteiro para a struct allocada na função
 */
LTable* readLineCsv(FILE* csv);

/****
 * Destructor que libera memoria alocada de uma struct LTable
 * 
 * @param lData ponteiro para struct a ser liberada
 * @return boolean para caso a função tenha sucesso ou não
 */
bool freeLineData(LTable* lData);

/****
 * Transfere os dados de um arquivo binário para uma estrutura LTable
 * Caso o binário seja invalido retorna NULL
 * 
 * @param bin Arquivo binário com os dados
 * @return LTable* estrutura na memoria com os dados alocados
 */
LTable* readLineBinary(FILE* bin);

/****
 * Transfere os dados de uma LTable para um arquivo binario seguindo as regras passadas nas especificaçẽos
 * 
 * @param lData struct a ser transferida
 * @param binDest arquivo alvo
 */
void writeLineBinary(LTable* lData,FILE* binDest);
/****
 * Imprime informações do registro de uma linha
 * 
 * @param lReg registro a ser impresso
 */
void displayLine(LEntry* lReg);

/****
 * Destructor que libera memoria alocada de uma struct LTable
 * 
 * @param lData ponteiro para struct a ser liberada
 * @return boolean para caso a função tenha sucesso ou não
 */
bool freeLineData(LTable* lData);

/****
 * Familia de funções para a selectLineWhere
 * 
 * @param lReg registro a ser verificado
 * @param secondParameter valor a ser comparado
 */
bool matchLineCode(LEntry* lReg,void* code);
bool matchLineAcceptCard(LEntry* lReg,void* cardStatus);
bool matchLineName(LEntry* lReg,void* name);
bool matchLineColor(LEntry* lReg,void* color);

/****
 * Imprime os matchs de uma comparação dentro de uma struct Data
 * 
 * @param lData struct a ser buscada
 * @param match ponteiro de função para uma das funções match
 */
void selectLineWhere(LTable* lData,void* key,bool (*match)(LEntry*,void*));

/****
 * Imprime todos os registros não removidos de uma struct
 * 
 * @param lReg registro a ser verificado 
 */
void selectLine(LTable* lData);

#endif