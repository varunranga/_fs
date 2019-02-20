LinkedList LinkedList_Create()
{
	LinkedList linkedList = (LinkedList) MemoryManagement_Malloc(sizeof(LinkedListStructure));
	linkedList->head = NULL;
	linkedList->size = 0;

	return linkedList;
}

int LinkedList_Size(LinkedList linkedList)
{
	return linkedList->size;
}

int LinkedList_AddToPosition(LinkedList linkedList, LinkedListElementType data, int position)
{
	if (position < 0)
	{
		return OPERATION_FAILED;
	}

	LinkedListNode currentLinkedListNode = linkedList->head;
	LinkedListNode previousLinkedListNode = NULL;

	int i = 0;
	while ((currentLinkedListNode) && (i < position))
	{
		previousLinkedListNode = currentLinkedListNode;
		currentLinkedListNode = currentLinkedListNode->nextNode;
	
		i++;
	}

	if (i != position)
	{
		return OPERATION_FAILED;
	}

	LinkedListNode newLinkedListNode = (LinkedListNode) MemoryManagement_Malloc(sizeof(LinkedListNodeStructure));

	if (!newLinkedListNode)
	{
		return OPERATION_FAILED;
	}

	newLinkedListNode->data = data;
	newLinkedListNode->nextNode = currentLinkedListNode;

	if (!previousLinkedListNode)
	{
		linkedList->head = newLinkedListNode;
	}
	else
	{
		previousLinkedListNode->nextNode = newLinkedListNode;
	}

	linkedList->size += 1;

	return OPERATION_SUCCESSFUL;
}

int LinkedList_AddToFirst(LinkedList linkedList, LinkedListElementType data)
{
	return LinkedList_AddToPosition(linkedList, data, 0);
}

int LinkedList_AddToLast(LinkedList linkedList, LinkedListElementType data)
{
	return LinkedList_AddToPosition(linkedList, data, linkedList->size);
}

int LinkedList_DeleteFromPosition(LinkedList linkedList, int position)
{
	if (position < 0)
	{
		return OPERATION_FAILED;
	}

	LinkedListNode currentLinkedListNode = linkedList->head;
	LinkedListNode previousLinkedListNode = NULL;

	int i = 0;
	while ((currentLinkedListNode->nextNode) && (i < position))
	{
		previousLinkedListNode = currentLinkedListNode;
		currentLinkedListNode = currentLinkedListNode->nextNode;

		i++;
	}

	if (i != position)
	{
		return OPERATION_FAILED;
	}

	if (!previousLinkedListNode)
	{
		linkedList->head = currentLinkedListNode->nextNode;
	}
	else
	{
		previousLinkedListNode->nextNode = currentLinkedListNode->nextNode;
	}

	linkedList->size -= 1;

	return OPERATION_SUCCESSFUL;
}

int LinkedList_DeleteFromFirst(LinkedList linkedList)
{
	return LinkedList_DeleteFromPosition(linkedList, 0);
}

int LinkedList_DeleteFromLast(LinkedList linkedList)
{
	return LinkedList_DeleteFromPosition(linkedList, linkedList->size - 1);
}

int LinkedList_Traverse(LinkedList linkedList, void (*Function)(LinkedListElementType))
{
	LinkedListNode currentLinkedListNode = linkedList->head;

	while (currentLinkedListNode)
	{
		Function(currentLinkedListNode->data);

		currentLinkedListNode = currentLinkedListNode->nextNode;
	}
}