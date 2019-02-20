#include <stdio.h>
#include "LinkedList_Header.h"

void Display(LinkedListElementType data)
{
	int* data1 = (int*) data;
	printf("%d\n", *data1);
}

int main()
{
	MemoryManagement_Initialize();

	LinkedList linkedList = LinkedList_Create();

	int* random1 = (int*) malloc (sizeof(int));
	int* random2 = (int*) malloc (sizeof(int));
	int* random3 = (int*) malloc (sizeof(int));
	int* random4 = (int*) malloc (sizeof(int));
	int* random5 = (int*) malloc (sizeof(int));
	
	*random1 = 1;
	*random2 = 2;
	*random3 = 3;
	*random4 = 4;
	*random5 = 5;

	int success = 0;

	success = LinkedList_AddToFirst(linkedList, (void*) random1);
	printf("Size = %d\n", LinkedList_Size(linkedList));

	success = LinkedList_AddToLast(linkedList, (void*) random2);
	printf("Size = %d\n", LinkedList_Size(linkedList));

	success = LinkedList_AddToPosition(linkedList, (void*) random3, 1);
	printf("Size = %d\n", LinkedList_Size(linkedList));

	success = LinkedList_AddToPosition(linkedList, (void*) random4, 3);
	printf("Size = %d\n", LinkedList_Size(linkedList));

	success = LinkedList_AddToPosition(linkedList, (void*) random5, 2);
	printf("Size = %d\n", LinkedList_Size(linkedList));

	LinkedList_Traverse(linkedList, Display);

	success = LinkedList_DeleteFromPosition(linkedList, 2);
	printf("Size = %d\n", LinkedList_Size(linkedList));

	printf("\n");
	LinkedList_Traverse(linkedList, Display);

	success = LinkedList_DeleteFromPosition(linkedList, 3);
	printf("Size = %d\n", LinkedList_Size(linkedList));

	printf("\n");
	LinkedList_Traverse(linkedList, Display);
	
	success = LinkedList_DeleteFromPosition(linkedList, 1);
	printf("Size = %d\n", LinkedList_Size(linkedList));

	printf("\n");
	LinkedList_Traverse(linkedList, Display);
	
	success = LinkedList_DeleteFromFirst(linkedList);
	printf("Size = %d\n", LinkedList_Size(linkedList));
	
	printf("\n");
	LinkedList_Traverse(linkedList, Display);

	success = LinkedList_DeleteFromLast(linkedList);
	printf("Size = %d\n", LinkedList_Size(linkedList));

	printf("\n");
	LinkedList_Traverse(linkedList, Display);

	
	return 0;
}