#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
* Transforma uma string, com campos separados por espaco,
* em um vetor de argumentos
*/
char** arguments_get(char* arg_string);

/*
* Free no vetor de argumentos
*/
void arguments_free(char** arguments);

/** Lê um arquivo até achar o primeiro finalizador de linha.
 * Caso a linha seja vazia, retorna NULL
 * @param fstream Arquivo a ser lido
 * @return char* line String contendo a linha
 */
char* readline(FILE* fstream);

/** Limpa a string inplace, extraindo os campos para fora das aspas duplas
 * @param string String a ser limpada
 */
void string_to_csv(char* string);

/**
* Abre arquivo de acordo com a especificação do trabalho.
* Caso não consiga abrir, ou arquivo esteja invalido -> sai do programa com exit(0).
*/
FILE* fopen_safe(char* name,char* mode);

void binarioNaTela(char *nomeArquivoBinario);

/*  
    Converte o prefixo do veículo para int

    OBS1:   retorna -1 se o primeiro caracter é '*'

    OBS2:   retorna LIXO se a string contém algo diferente de números e letras 
            maiúsculas (com excessão do caso da OBS1)

    COMO FUNCIONA:

        Para converter os prefixos para int e garantir que prefixos diferentes 
        deem números diferentes interpretamos eles como números em base 36

        Um número N com 5 digitos dI em base 36 é escrito como:

            N = d0 * 36^0 + d1 * 36^1 + d2 * 36^2 + d3 * 36^3 + d4 * 36^4

        Nota-se que estamos escrevendo do digito menos significativo para o 
        mais significativo

        Como o nosso prefixo têm 5 bytes e eles podem assumir 36 valores
        cada, podemos interpretar cada char dele como um digito em base 36, 
        prefixos diferentes formaram números diferentes em base 36 e portanto 
        números diferentes quando convertemos para um int normal
*/
int convertePrefixo(char* str);

#endif