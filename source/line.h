#ifndef _LINE_H
#define _LINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"

typedef struct _LineInfo{
	char stable;
	long byteOffset;
	int qty;
	int rmvQty;
	char code[15];
	char card[13];
	char name[13];
	char line[24];
}LInfo;

typedef struct _LineEntry{
	char isPresent;
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
	LEntry* lines;
}LTable;

/****
 * Lê e parsea o CSV do arquivo e armazena em uma struct
 * 
 * @param csv ponteiro para o arquivo a ser lido
 * @return ponteiro para a struct allocada na função
 */
LTable* readLineCsv(FILE* csv);

/****
 * Transfere os dados de um arquivo binário para uma estrutura LTable
 * Caso o binário seja invalido retorna NULL
 * 
 * @param bin Arquivo binário com os dados
 * @return LTable* estrutura na memoria com os dados alocados
 */

LTable* readLineBinary(FILE* bin);
/****
 * Destructor que libera memoria alocada de uma struct LTable
 * 
 * @param table ponteiro para struct a ser liberada
 * @return boolean para caso a função tenha sucesso ou não
 */
bool freeLineTable(LTable* table);

/****
 * Transfere os dados de uma LTable para um arquivo binario seguindo as regras passadas nas especificaçẽos
 * 
 * @param table struct a ser transferida
 * @param bin arquivo alvo
 */
void writeLineBinary(LTable* table,FILE* bin);

/****
 * Imprime a linha no formato especificado
 * 
 * @param entry linha a ser impressa
 */
void displayLine(LEntry* entry);

/****
 * Imprime todos as entradas dentro da table
 * 
 * @param table table com as linhas
 */
void selectLine(LTable* table);

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
 * Imprime todos linhas, que possuem a chave procurada, numa table
 * 
 * @param table table com as linhas
 * @param key valor a ser comparado em cada linha
 * @param match ponteiro de função para uma das funções match
 */
void selectLineWhere(LTable* table,void* key,bool (*match)(LEntry*,void*));

/****
 * Lê entradas da stdin. Essas entradas são adicionadas na table e depois escritas no bin
 * 
 * @param table tabela a receber as novas entradas
 * @param qty quantidade de entradas que vao ser lidas
 * @param bin arquivo que vai receber as novas entradas
 */
void insertLineEntries(LTable* table,int qty,FILE* bin);
#endif