/*Arquivo extendendo o Header_Binario genérico.
* Define constantes e funcoes especifcas para arquivos de dados com Veiculos.
* Interpreta os dados binarios de acordo com a especificação do trabalho. */
#ifndef VEHICLE_HEADER_
#define VEHICLE_HEADER_

#include "../includes/binary_header.h" // Re-exportar header binario e add funcoes para dados binario de veiculo
#include "../includes/utils.h" // Usa funcao convertePrefixo

#define VEHICLE_DESCRIPTION_LEN 158

/* 
* Turns a "text" representation of a vehicle into a "binary" representation.
* Example input: DN020,2002-12-18,18,560,MARCOPOLO SENIOR,MICRO
*/
char* vehicle_from_string(char* entry_string);

/*
* Gets the vehicle's key field from a binary representation.
* Key field: vehicle prefix — hash fn —> hashed prefix
*/
int vehicle_get_key(char* vehicle_data);

/*
* Gets the vehicle's "line code" field from a binary representation.
*/
int vehicle_get_line_code(char* vehicle_data);

/*
* Display the binary representation is a "text"/human readable format.
*/
void display_vehicle_from_data(char* vehicle_data);

#endif