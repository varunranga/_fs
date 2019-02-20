#include <stdio.h>
#include "HashTable_Header.h"

void Display(LinkedListElementType data)
{
	char* data1 = (char*) data;
	printf("%s\n", data1);
}

int main()
{
	HashTable hashTable = HashTable_Create();

	char* random1 = "hello";
	char* random2 = "world";
	char* random3 = "surface";
	char* random4 = "music";
	char* random5 = "tree";
	
	HashTable_Add(hashTable, random1, (void*) random1);
	HashTable_Add(hashTable, random2, (void*) random2);
	HashTable_Add(hashTable, random3, (void*) random3);
	HashTable_Add(hashTable, random4, (void*) random4);
	HashTable_Add(hashTable, random5, (void*) random5);

	HashTable_Traverse(hashTable, Display);

	HashTableSearch search = HashTable_Search(hashTable, "hello");
	printf("%d %d\n", search->row, search->column);

	search = HashTable_Search(hashTable, "world");
	printf("%d %d\n", search->row, search->column);

	search = HashTable_Search(hashTable, "surface");
	printf("%d %d\n", search->row, search->column);

	search = HashTable_Search(hashTable, "music");
	printf("%d %d\n", search->row, search->column);

	search = HashTable_Search(hashTable, "tree");
	printf("%d %d\n", search->row, search->column);

	printf("Size = %d\n", HashTable_Size(hashTable));

	return 0;
}