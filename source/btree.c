#include "btree.h"

// Get the byte offset of a Node / Page File, given it's `rrn`.
#define node_byteoffset(rrn) (PAGE_SIZE + PAGE_SIZE*rrn)

/**
* Binary search for a key inside an array.
* Returns pos of key on array.
* If key isn't present, return pos it would be at.
*/
int binary_search_pos(int* array,int arr_len,int key) {
	int left = 0, right = arr_len-1, middle = 0;
	while (left <= right) {
		middle = left + (right-left)/2;
		if (key == array[middle]){
			return middle; // Key is found
		}
		
		if (key > array[middle]) {
			left = middle + 1;
		} else {
			right = middle - 1;
		}
	}

	return middle;
}

/**
* Write current Btree header into it's file.
* Will `fopen` and then `fclose` internally.
*/
void btree_write_header(BTree* btree) {
	//printf("Writing header\n");
	FILE* btree_file = fopen(btree->file_name, "r+");
	rewind(btree_file);

	fwrite(&(btree->status), sizeof(char), 1, btree_file);
	fwrite(&btree->noRaiz, sizeof(int), 1, btree_file);
	fwrite(&btree->RRNproxNo, sizeof(int), 1, btree_file);
	fwrite(btree->_padding, sizeof(char), HEADER_PADDING_LEN, btree_file);
	fclose(btree_file);
}

void btree_write_status(FILE* btree_file,char status) {
	rewind(btree_file);
	fwrite(&status, sizeof(char), 1, btree_file);
}

BTree* btree_read_header(char* file_name) {
	if (!file_name) return NULL;

	FILE* btree_file = openFile(file_name, "rb");
	BTree* btree_struct = malloc(sizeof(BTree));

	fread(&btree_struct->status, sizeof(char), 1, btree_file);
	fread(&btree_struct->noRaiz, sizeof(int), 1, btree_file);
	fread(&btree_struct->RRNproxNo, sizeof(int), 1, btree_file);
	fread(btree_struct->_padding, sizeof(char), HEADER_PADDING_CHAR, btree_file);
	btree_struct->file_name = file_name;

	fclose(btree_file);

	return btree_struct;
}

BTree* btree_new(char* file_name) {
	if (!file_name) return NULL;
	
	// 1. Initiliaze Btree struct with default values.
	BTree* btree = malloc(sizeof(BTree));
	//printf("Alocando %lu bytes do tamanho da btree\n",sizeof(BTree));
	btree->status = '1';
	btree->noRaiz = -1;
	btree->RRNproxNo = -1;
	for (int i = 0; i < HEADER_PADDING_LEN; i++) {
		btree->_padding[i] = HEADER_PADDING_CHAR;
	}
	btree->file_name = strdup(file_name);

	// 2. Initialize Btree File with default header.
	btree_write_header(btree);

	return btree;
}

void btree_delete(BTree* btree) {
	if(btree == NULL || btree->file_name == NULL) {

	}
	free(btree->file_name);
	free(btree);
}
/*
    Node structure:
                [key     ,   key    ,      key,    key] <-------- Node / Page File
               /        /           |          \       \  
    (left-child)  (1-child) (middle-child) (3-child) (right-child)
****/
/**
* Struct representing Node / Page File.
* Quantity of keys and children follows `BTREE_ORDER` macro.
* Internal structure doesn't exactly match file written counterpart.
* Total byte size = 77 (`PAGE_SIZE`).
 */
typedef struct BTREENODE {
	char is_leaf;                  // '1' when leaf node, '0' otherwise
	int key_quantity;              // number of inserted keys in this node
	int node_rrn;                  // this node's RRN
	int children[CHILD_QUANTITY];  // RRN of child nodes
	int keys[KEY_QUANTITY];        // key identifying indexed object
	long offsets[KEY_QUANTITY];    // byte offset, in binary file, of object
} BtreeNode;

/* 
* Create a new Node with no keys and no children.
* By default, new Nodes are considered leaf nodes.
*/
BtreeNode* node_new(BTree* btree) {
	// 1. Initialize node with default values and next btree rrn
	BtreeNode* node = malloc(sizeof(BtreeNode));
	node->is_leaf = '1';
	node->key_quantity = 0;
	if (btree->RRNproxNo == -1) btree->RRNproxNo = 0;
	node->node_rrn = btree->RRNproxNo++; // Update then assign new node rrn

	// 2. Iterate in specified order
	for (int i = 0; i < KEY_QUANTITY; i++) {
		node->children[i] = -1;
		node->keys[i] = -1;
		node->offsets[i] = -1;
	}
	node->children[CHILD_QUANTITY-1] = -1;

	return node;
}

/**
* Reads a `Node*` from `btree->file` in given `rrn`.
* Does nothing if `rrn == -1`.
*/
BtreeNode* node_read(BTree* btree,int rrn){
	if (rrn == -1) return NULL;

	// 0. Open btree_file
	FILE* btree_file = fopen(btree->file_name, "rb+");

	// 1. Read Node data to buffer.
	//printf("Reading node of rrn %d at byteoffset %d\n",rrn,node_byteoffset(rrn));
	char node_data[PAGE_SIZE];
	fseek(btree_file,node_byteoffset(rrn),SEEK_SET);
	fread(&node_data,sizeof(char),PAGE_SIZE,btree_file);

	// 2. Build Node from buffered data
	BtreeNode* node = malloc(sizeof(BtreeNode));

	memcpy(&node->is_leaf,      node_data,   sizeof(char));
	memcpy(&node->key_quantity, node_data+1, sizeof(int));
	memcpy(&node->node_rrn,     node_data+5, sizeof(int));

	// 3. Iterate in specified order
	for (int i = 0; i < KEY_QUANTITY; i++) {
		int off = 9 + (i*16);
		memcpy(&node->children[i], node_data+off,   sizeof(int));
		memcpy(&node->keys[i],     node_data+off+4, sizeof(int));
		memcpy(&node->offsets[i],  node_data+off+8, sizeof(long));
	}
	int off = 9 + (16 * KEY_QUANTITY);
	memcpy(&node->children[CHILD_QUANTITY-1],node_data+off,sizeof(int));

	fclose(btree_file);
	return node;
}

/**
* Write a given Node / Page File into binary file.
* Since `node` carries it's `rrn` internally, this function
* is self contained.
* Does nothing if `node == NULL`.
*/
void node_write(BTree* btree,BtreeNode* node){
	if (!node) return;

	// 0. Open btree_file
	FILE* btree_file = fopen(btree->file_name, "rb+");

	// 1. Seek to Node byteoffset
	//printf("Writing Node of rrn %d at byteoffset %d\n",node->node_rrn,node_byteoffset(node->node_rrn));
	fseek(btree_file,node_byteoffset(node->node_rrn),SEEK_SET);

	// 2. Build buffer with Node internal data, in specified file representation
	char node_data[PAGE_SIZE];
	memcpy(node_data,   &node->is_leaf,      sizeof(char));
	memcpy(node_data+1, &node->key_quantity, sizeof(int));
	memcpy(node_data+5, &node->node_rrn,     sizeof(int));

	// 3. Iterating through keys, keys_rrn and children in specified order
	for (int i = 0; i < KEY_QUANTITY; i++) {
		int off = 9 + (i*16);
		memcpy(node_data+off  , &node->children[i] , sizeof(int));
		memcpy(node_data+off+4, &node->keys[i]     , sizeof(int));
		memcpy(node_data+off+8, &node->offsets[i]  , sizeof(long));
	}
	int off = 9 + (16 * KEY_QUANTITY);
	memcpy(node_data+off,&node->children[CHILD_QUANTITY-1],sizeof(int));
	
	// 4. Write buffered Node data
	fwrite(node_data,sizeof(char),PAGE_SIZE,btree_file);
	fclose(btree_file);
}

/**
* Splits a node maintaining inner key sorting.
* Interiorly mutates node, transferring some keys to returned node.
* Interiorly mutates key, turning it into new median value (the value that actually gets promoted).
* @returns right_node resulted from splitting.
*/
BtreeNode* node_split(BTree* btree,BtreeNode* node,int* key,int right_child,long* offset) {
	// 0. Simulate overflowing insertion
	int overflow_keys[KEY_QUANTITY+1];
	int overflow_children[CHILD_QUANTITY+1];
	long overflow_offsets[KEY_QUANTITY+1];

	int key_pos = 0;
	while (*key > node->keys[key_pos]) {
		overflow_keys[key_pos]     = node->keys[key_pos];
		overflow_children[key_pos] = node->children[key_pos];
		overflow_offsets[key_pos]  = node->offsets[key_pos];
		key_pos++;
	}
	overflow_children[key_pos] = node->children[key_pos];

	// Insert key, offset and right_child_pointer of previous split
	overflow_keys[key_pos]       = *key;
	overflow_children[key_pos+1] = right_child;
	overflow_offsets[key_pos]    = *offset;

	for (int i = KEY_QUANTITY; i > key_pos; i--) { // Right shift elements after key
		overflow_keys[i]     = node->keys[i-1];
		overflow_children[i+1] = node->children[i];
		overflow_offsets[i]  = node->offsets[i-1];
	}

	// 1. Find median key on overflowed_keys
	int split_pos = (KEY_QUANTITY+1) / 2;
	*key    = overflow_keys[split_pos];
	*offset = overflow_offsets[split_pos];

	// 2. Update current node (left part of split) with results from overflowing insertion
	for (int i = 0; i < split_pos; i++) {
		node->keys[i]     = overflow_keys[i];
		node->children[i] = overflow_children[i];
		node->offsets[i]  = overflow_offsets[i];
	}

	// 3. Build new Node (right part of split) that receives all keys to the right of key_pos
	BtreeNode* right = node_new(btree);
	right->is_leaf = node->is_leaf;
	int r_pos = split_pos+1;
	for (int i = 0; i < split_pos; i++) {
		right->keys[i]     = overflow_keys[r_pos];
		right->children[i] = overflow_children[r_pos];
		right->offsets[i]  = overflow_offsets[r_pos];
		right->key_quantity++;

		r_pos++;
	}
	right->children[split_pos] = overflow_children[CHILD_QUANTITY]; // Copy last child pointer
	
	// 4. Set the right half left_split to NULL
	for (int i = split_pos; i < KEY_QUANTITY; i++) {
		node->keys[i]     = -1;
		node->children[i+1] = -1;
		node->offsets[i]  = -1;
		node->key_quantity--;
	}

	// 5. Return the newly created right node
	return right;
}

/** 
* Inserts a key in a Node, assuming it's NOT full.
* Returns the position that key was inserted inside Node's keys.
* DO NOT call this function if a node is already full
*/
int node_add_key(BtreeNode* node,int new_key,long byteoffset) {
	int key_pos = 0;
	while (new_key > node->keys[key_pos]) { // Iterate until new_key position is found
		if (node->keys[key_pos] == -1) break;
		key_pos++;
	}

	for (int i = node->key_quantity; i > key_pos; i--) {
		node->children[i+1] = node->children[i];
		node->keys[i] = node->keys[i-1];
		node->offsets[i] = node->offsets[i-1];
	}
	node->children[key_pos+1] = node->children[key_pos];

	node->children[key_pos] = -1;         // Set subtree pointer to NULL
	node->keys[key_pos] = new_key;        // Put new_key where it should be
	node->offsets[key_pos] = byteoffset;  // Put byteoff corresponding to new_key_pos
	node->key_quantity++;

	return key_pos;
}

long search_btree(BTree* btree,int searched_key){
	if (btree->noRaiz == -1 || searched_key == -1) return -1; // RRN = -1 -> node doesn't exist

	// 0. Open btree_file
	FILE* btree_file = fopen(btree->file_name, "rb+");
	btree_write_status(btree_file, '0');

	// 1. read root node using root_RRN from btree_header 
	BtreeNode* node = node_read(btree,btree->noRaiz);
	while (node) {
		// 2. Binary searching for the key inside a node
		int searched_key_pos = binary_search_pos(node->keys, node->key_quantity, searched_key);
		if (node->keys[searched_key_pos] == searched_key) {
			btree_write_status(btree_file, '1');
			fclose(btree_file);
			return node->offsets[searched_key_pos]; // If we find the key, return the associated byteoffset!
		}

		// 3. Follow child pointer to next node and try searching again
		if (searched_key > node->keys[searched_key_pos]) {
			searched_key_pos++; // If searched_key_pos is bigger than key, go to next pointer
		}
		int child_rrn = node->children[searched_key_pos];
		
		free(node);
		node = node_read(btree,child_rrn);
	}

	btree_write_status(btree_file, '1');
	fclose(btree_file);
	return -1; // node was not found, return -1
}

void insert_btree(BTree* btree,int new_key,long byteoffset){
	if (new_key == -1 || byteoffset == -1) return;
	// 0. open btree_file
	FILE* btree_file = fopen(btree->file_name, "rb+");
	btree_write_status(btree_file, '0');

	// 1. Initiliaze Node* stack. Necessary because algorithm is naturally recursive
	// PLACEHOLDER: constant stack size - use log2(element_quantity) to get height/depth
	int btree_depth = 30; // int btree_depth = log2(btree->RRNproxNo-1);
	BtreeNode** node_stack = malloc(btree_depth * sizeof(BtreeNode*));
	node_stack[0] = NULL;
	int stack_top = 0;

	// 2. Read root node using root_RRN from btree_header
	BtreeNode* node = node_read(btree,btree->noRaiz);
	while (node) {
		node_stack[++stack_top] = node; // Push valid node into stack

		// 3. Binary searching for the key inside a node
		int new_key_pos = binary_search_pos(node->keys,node->key_quantity,new_key);
		if (node->keys[new_key_pos] == new_key) {
			for (int i = stack_top; i >= 0; i--) free(node_stack[i]);
			free(node_stack);
			btree_write_status(btree_file, '1');
			fclose(btree_file);
			return; // Exit function if key is found - no duplicates allowed
		}

		// 4. Follow child pointer to next node and try searching again!
		if (new_key > node->keys[new_key_pos]) new_key_pos++; // If new_key is bigger than key at it's pos, go to next pointer
		int child_rrn = node->children[new_key_pos];
		node = node_read(btree,child_rrn);
	}
	int stack_len = stack_top;
	// End of the loop, we have all traversed nodes on stack and stack_top is leaf node

	// 6. Handling nodes that need splitting
	BtreeNode* receiving_node = node_stack[stack_top--];
	BtreeNode* left = NULL, *right = NULL; // dummy left, right splitted node pointers
	while (receiving_node && receiving_node->key_quantity >= KEY_QUANTITY) { // If receiving node is full - split!
		int right_child = -1; // 
		if (right) { // If this isn't the first consecutive split, we have a right pointer
			right_child = right->node_rrn; // The right child is previous split's right half
			free(right);
		}
		
		// Split full node, promote new_key and byteoffset and finally add reference to right_child
		right = node_split(btree, receiving_node, &new_key, right_child, &byteoffset);
		left = receiving_node;

		node_write(btree, left);
		node_write(btree, right);

		receiving_node = node_stack[stack_top--];
	}

	// 7. If there is no valid receiving node, create a new Parent!
	if (receiving_node == NULL) {
		receiving_node = node_new(btree);
		if (btree->RRNproxNo > 1) receiving_node->is_leaf = '0'; // Only the first node created is a leaf!
		btree->noRaiz = receiving_node->node_rrn;

		node_stack[++stack_len] = receiving_node; // Push to stack so it get's free'd
	}

	// 8. Finally add key in vacant node. At this point, key might be a promoted key instead of argument
	int new_key_pos = node_add_key(receiving_node, new_key, byteoffset);
	if (left) receiving_node->children[new_key_pos]   = left->node_rrn;  // Add pointer to left_splitted in parent
	if (right) {
		receiving_node->children[new_key_pos+1] = right->node_rrn; // Add pointer to right_splitted in parent
		node_stack[++stack_len] = right; // Push to stack so it get's free'd
	}

	node_write(btree, receiving_node);

	for (int i = stack_len; i > 0; i--) free(node_stack[i]);
	free(node_stack);
	btree_write_header(btree);
	fclose(btree_file);
}