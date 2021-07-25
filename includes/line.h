/* Arquivo extendendo o Header_Binario genérico.
* Define constantes e funcoes especifcas para arquivos de dados com Linhas.
* Interpreta os dados binarios de acordo com a especificação do trabalho. */
#ifndef LINE_HEADER_
#define LINE_HEADER_

#include "../includes/binary_file.h"

#define LINE_DESCRIPTION_LEN 65

/*
* Turns a "text" representation of a line into a "binary" representation.
* Example input: 150,S,C. MUSICA-V. ALEGRE,AMARELA
*/
char* line_from_string(char* entry_string);

/*
* Gets the line's key field from a binary representation.
* Key field: line code
*/
int line_get_key(char* line_data);

/*
* qsort() comparator function.
*/
int line_cmp(const void* line1,const void* line2);

/*
* Display the binary representation in a "text"/human readable format.
*/
void display_line_from_data(char* line_data);

#endif