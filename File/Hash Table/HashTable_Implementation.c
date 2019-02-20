HashTable HashTable_Create()
{
	HashTable hashTable = (HashTable) MemoryManagement_Malloc(sizeof(HashTableStructure));

	hashTable->linkedLists = (LinkedList*) MemoryManagement_Malloc(sizeof(LinkedList) * HASH_TABLE_SIZE);

	int i = 0;
	for (; i < HASH_TABLE_SIZE; i++)
	{
		hashTable->linkedLists[i] = LinkedList_Create();
	}

	return hashTable;
}

int HashTable_Size(HashTable hashTable)
{
	int size = 0;

	int i = 0;
	for (; i < HASH_TABLE_SIZE; i++)
	{
		size += LinkedList_Size(hashTable->linkedLists[i]);
	}

	return size;
}

int HashTable_HashFunction(HashableElementType element)
{
	unsigned long StringHashFunction(unsigned char *str)
	{
		unsigned long hash = 5381;
		int c;

	    while (c = *str++)
	        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	    return hash;
	}

	return StringHashFunction(element) % HASH_TABLE_SIZE;
}

int HashTable_Add(HashTable hashTable, HashableElementType hashElement, HashTableElementType data)
{
	int index = HashTable_HashFunction(hashElement);
	return LinkedList_AddToFirst(hashTable->linkedLists[index], data);
}

int HashTable_Traverse(HashTable hashTable, void (*Function)(HashTableElementType))
{
	int i = 0;
	for (; i < HASH_TABLE_SIZE; i++)
	{
		LinkedList_Traverse(hashTable->linkedLists[i], Function);
	}

	return OPERATION_SUCCESSFUL;
}

int HashTable_Delete(HashTable hashTable, int row, int column)
{
	return LinkedList_DeleteFromPosition(hashTable->linkedLists[row], column);
}

HashTableSearch HashTable_Search(HashTable hashTable, HashableElementType hashElement)
{
	typedef struct folder_structure
	{
		char* folderName;
		int fileType;
		HashTable folderTable;
	} FolderStructure;

	typedef FolderStructure* Folder;

	typedef struct file_structure
	{
		char* fileName;
		int fileType;
		int fileLength;
		int fileInstancesOpened;
		Folder parentFolder;
		
		void* fileContents;
	} FileStructure;

	typedef FileStructure* File;

	int index = HashTable_HashFunction(hashElement);
	
	int i = 0;
	LinkedListNode currentLinkedListNode = hashTable->linkedLists[index]->head;

	while (currentLinkedListNode)
	{
		File file = (File) currentLinkedListNode->data;

		char* data = file->fileName;

		if (strcmp(hashElement, data) == 0)
		{
			HashTableSearch search = (HashTableSearch) malloc (sizeof(HashTableSearchStructure));
			search->row = index;
			search->column = i;
			search->address = currentLinkedListNode;

			return search;
		}

		currentLinkedListNode = currentLinkedListNode->nextNode;
		i++;
	}

	return NULL;
}