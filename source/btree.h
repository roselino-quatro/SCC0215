#ifndef BTREE_HEADER_
#define BTREE_HEADER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define BTREE_ORDER 5
#define CHILD_QUANTITY (BTREE_ORDER)
#define KEY_QUANTITY (BTREE_ORDER-1)

#define PAGE_SIZE 77
#define HEADER_PADDING_LEN (PAGE_SIZE-9)
#define HEADER_PADDING_CHAR '@'

/**
* Struct representing the Btree file Header.
* Needs padding to serve as a dummy node / not interfere with RRN seeks.
* Total byte size = 77 (`PAGE_SIZE`).
*/
typedef struct BTREE_INDEX_HEADER {
	char status;       // File consistency: '0' when corrupted, otherwise '1'
	int noRaiz;        // Root node RRN in FILE, -1 when tree is empty 
	int RRNproxNo;     // RRN of next insertion in FILE. Starts with 0 and increases with evey addition
	char _padding[HEADER_PADDING_LEN]; // Char padding filled with '@' for FILE consistency
	char* file_name;   // Internal btree file name used for `fopen` and `fclose` in an easier way.
} BTree;

/**
* Initiliaze a btree struct and file with name `file_name`.
* Write the btree header into file.
* In case `file_name == NULL` , do nothing and return `NULL`
*/
BTree* btree_new(char* file_name);

/****
 * Returns the reader from a btree file
 */
BTree* btree_read_header(char* file_name);

/**
* Free all memory of `btree`.
*/
void btree_delete(BTree* btree);

/**
* Search the `btree` for `searched_key`.
* @return `byteoffset` on sucess.
* @return `-1` if not found.
*/
long search_btree(BTree* btree,int searched_key);

/**
* Insert a {new_key: byteoffset} pair into the btree.
* May do several file writes internally.
*/
void insert_btree(BTree* btree,int new_key,long byteoffset);

#endif