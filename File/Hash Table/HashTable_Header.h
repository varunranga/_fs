#ifndef _STRING_H
	#include <string.h>
#endif

#include "Linked List/LinkedList_Header.h"

#ifndef OPERATION_FAILED
	#define OPERATION_FAILED 0
#endif

#ifndef OPERATION_SUCCESSFUL
	#define OPERATION_SUCCESSFUL 1
#endif

#define HASH_TABLE_SIZE 13

typedef LinkedListElementType HashTableElementType;
typedef char* HashableElementType;

typedef struct hash_table_structure
{
	LinkedList* linkedLists;
} HashTableStructure;

typedef HashTableStructure* HashTable;

typedef struct hash_table_search_structure
{
	int row;
	int column;
	void* address;
} HashTableSearchStructure;

typedef HashTableSearchStructure* HashTableSearch;

HashTable HashTable_Create();
int HashTable_Size(HashTable hashTable);
int HashTable_HashFunction(HashableElementType element);
int HashTable_Add(HashTable hashTable, HashableElementType hashElement, HashTableElementType data);
int HashTable_Traverse(HashTable hashTable, void (*Function)(HashTableElementType));
HashTableSearch HashTable_Search(HashTable hashTable, HashableElementType hashElement);

#include "HashTable_Implementation.c"