#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/****
 * Lê uma string e a separa em pedaços de acordo com delims.
 * Não altera a string original de qualquer forma.
 * 
 * @param qty Quantidade de campos a extrair
 * @param src String fonte com os campos a serem extraidos
 * @return Vetor de campos extraidos terminado em NULL
 */
char** getFields(int qty,char* src);

/****
 * Simples wrapper/encapsulamento em cima da memcpy que faz o memcpy e retorna qtd de bytes copiada
 * 
 * @param dest Pointeiro que vai receber os bytes (deve ter espaço suficiente)
 * @param src Pointeiro fonte dos bytes
 * @param n Quantidade de bytes
 * @return Quantidade de bytes inseridos = n
 */
int memcpyField(void* dest,const void* src,size_t n);

/** Lê um arquivo até achar o primeiro finalizador de linha.
 * Caso a linha seja vazia, retorna NULL
 * @param fstream Arquivo a ser lido
 * @return char* line String contendo a linha
 */
char* readline(FILE* fstream);


/** Limpa a string inplace, extraindo os campos para fora das aspas duplas
 * @param string String a ser limpada
 */
void cleanString(char* string);

void binarioNaTela(char *nomeArquivoBinario);
void scan_quote_string(char *str);

/****
 * Abre um arquivo tratando erros
 * 
 * @param name ponteiro para o arquivo a ser aberto
 * @param mode ponteiro para o modo que o arquivo deve ser aberto
 * @return Ponteiro para o arquivo
 */
FILE* openFile(const char* name, const char* mode);

#endif