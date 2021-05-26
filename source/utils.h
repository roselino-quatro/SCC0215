#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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