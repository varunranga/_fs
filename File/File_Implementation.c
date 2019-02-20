int File_Create(Folder folder, char* fileName)
{
	printf("File_Create: Creating file %s\n", fileName);

	File file = (File) MemoryManagement_Malloc(sizeof(FileStructure));
	
	char* memFileName = (char*) MemoryManagement_Malloc((sizeof(char) * strlen(fileName)) + 1);
	memcpy(memFileName, fileName, strlen(fileName));
	memFileName[strlen(fileName)] = '\0';

	file->fileName = memFileName;
	file->fileType = FILE_TYPE;
	file->fileLength = 0;
	file->parentFolder = folder;
	file->fileInstancesOpened = 0;

	//file->fileContents = NULL;
	file->bitmap = NULL;

	if (HashTable_Add(folder->folderTable, fileName, (void*) file) == OPERATION_SUCCESSFUL)
	{
		printf("File_Create: Successfully created file %s\n", fileName);
		return OPERATION_SUCCESSFUL;
	}
	else
	{
		printf("File_Create: Could not create file %s\n", fileName);
		return OPERATION_FAILED;
	}
}

int File_Delete(Folder folder, char* fileName)
{
	File file = File_Open(folder, fileName) ;
	MemoryManagement_FreeBlocks(file->bitmap) ;
	File_Close(file) ; 

	printf("File_Delete: Delete File %s\n", fileName);

	HashTableSearch searchResult = HashTable_Search(folder->folderTable, fileName);
	if (searchResult == NULL)
	{
		printf("File_Open: Couldn't find file %s\n", fileName);
		return OPERATION_FAILED;
	}

	if (LinkedList_DeleteFromPosition(folder->folderTable->linkedLists[searchResult->row], searchResult->column) == OPERATION_SUCCESSFUL)
	{
		return OPERATION_SUCCESSFUL;
	}
	else
	{
		return OPERATION_FAILED;
	}
}

File File_Open(Folder folder, char* fileName)
{
	printf("File_Open: Opening File %s\n", fileName);

	HashTableSearch searchResult = HashTable_Search(folder->folderTable, fileName);

	if (searchResult == NULL)
	{
		printf("File_Open: Couldn't find file %s\n", fileName);
		return OPERATION_FAILED;
	}

	LinkedListNode currentLinkedListNode = (LinkedListNode) searchResult->address;
	File file = currentLinkedListNode->data;

	file->fileInstancesOpened += 1;

	printf("File_Open: Opened file %s\n", fileName);

	return file;
}

int File_Close(File file)
{
	printf("File_Close: Closing file %s\n", file->fileName);

	if (file->fileInstancesOpened)
	{	
		file->fileInstancesOpened -= 1;

		printf("File_Close: Closing file %s\n", file->fileName);
		return OPERATION_SUCCESSFUL;
	}
	else
	{
		printf("File_Close: Couldn't close file %s\n", file->fileName);
		return OPERATION_FAILED;
	}	
}

char* File_Read(File file, int numberOfBytes, int offset)
{
	int* allocatedBlock = file->bitmap;

	char* buffer = MemoryManagement_ReadFromBlocks(allocatedBlock, numberOfBytes, offset) ;

	printf("File_Read: Reading %d bytes from position %d to file %s\n", numberOfBytes, offset, file->fileName);

	printf("File_Read: Read file %s\n", file->fileName);

	return buffer;
}

int File_Write(File file, char* contentToWrite, int offset)
{
	if (file->fileLength != 0)
	{
		return File_WriteNotFromStart(file, contentToWrite, offset);
	}

	int* allocatedBlock = MemoryManagement_Balloc(strlen(contentToWrite) + offset) ;

	printf("File_Write: Writing %s to file %s at position %d\n", contentToWrite, file->fileName, offset);

	MemoryManagement_WriteToBlocks(allocatedBlock, offset, contentToWrite);
	file->fileLength = strlen(contentToWrite) + offset;	
	file->bitmap = allocatedBlock ;	// Updating the bitmap field in file

	printf("File_Write: Wrote to file %s\n", file->fileName);


	return OPERATION_SUCCESSFUL;
}

int File_WriteNotFromStart(File file, char* contentToWrite, int offset)
{
	printf("File_WriteNotFromStart: Writing %s to file %s at position %d\n", contentToWrite, file->fileName, offset);
	
	char* buf = (char*) malloc (file->fileLength + offset + strlen(contentToWrite));
	int j = 0;

	char* buffer = MemoryManagement_ReadFromBlocks(file->bitmap, file->fileLength, 0);
	memcpy(buf, buffer, file->fileLength);
	memcpy(buf + offset, contentToWrite, strlen(contentToWrite));

	printf("File_WriteNotFromStart: Writing: %s\n", buf);

	MemoryManagement_FreeBlocks(file->bitmap);

	int* allocatedBlock = MemoryManagement_Balloc(file->fileLength + strlen(contentToWrite) + offset) ;

	// printf("File_WriteNotFromStart: Writing %s to file %s at position %d\n", contentToWrite, file->fileName, offset);

	MemoryManagement_WriteToBlocks(allocatedBlock, 0, buf);
	file->fileLength +=  offset + strlen(contentToWrite);
	file->bitmap = allocatedBlock ;	// Updating the bitmap field in file	

	return OPERATION_SUCCESSFUL;
}

Folder Initialize_Root()
{
	printf("Initialize_Root: Initializing root\n");

	Folder folder = (Folder) MemoryManagement_Malloc(sizeof(FolderStructure));
	folder->folderName = "/";

	folder->fileType = FOLDER_TYPE;
	folder->parentFolder = folder;
	folder->folderTable = HashTable_Create();

	printf("Initialize_Root: Done\n");

	return folder;
}

int Folder_Create(Folder folder, char* folderName)
{
	printf("File_Create: Creating folder %s\n", folderName);

	Folder newFolder = (Folder) MemoryManagement_Malloc (sizeof(FolderStructure));
	
	char* memFolderName = (char*) MemoryManagement_Malloc(sizeof(char) * strlen(folderName) + 1);
	memcpy(memFolderName, folderName, strlen(folderName));
	memFolderName[strlen(folderName)] = '\0';

	newFolder->folderName = memFolderName;
	newFolder->fileType = FOLDER_TYPE;
	newFolder->folderTable = HashTable_Create();
	newFolder->parentFolder = (Folder) folder;

	return HashTable_Add(folder->folderTable, folderName, (void*) newFolder);
}

int Folder_Delete(Folder folder, char* folderName)
{
	printf("Folder_Delete: Delete folder %s\n", folderName);

	HashTableSearch searchResult = HashTable_Search(folder->folderTable, folderName);
	if (searchResult == NULL)
	{
		printf("Folder_Delete: Couldn't find folder %s\n", folderName);
		return OPERATION_FAILED;
	}

	return LinkedList_DeleteFromPosition(folder->folderTable->linkedLists[searchResult->row], searchResult->column);
}

Search Find(Folder folder, char *path)
{
	if (strlen(path) == 0)
	{
		return NULL;
	}

	if (path[0] == '/')
	{
		path++;
	}

	char bufferName[100];

	int i = 0;
	while ((path[0] != '/') && (path[0] != '\0'))
	{
		bufferName[i] = path[0];
		path++;
		i++;
	}

	bufferName[i] = '\0';

	HashTableSearch hashTableSearch = HashTable_Search(folder->folderTable, (char*) bufferName);
	if (hashTableSearch)
	{
		if (strlen(path) == 0)
		{
			LinkedListNode node = (LinkedListNode) hashTableSearch->address;
			void* data = (void*) node->data;
			int fileType = ((File) data)->fileType;

			Search search = (Search) malloc (sizeof(SearchStructure));
			search->fileType = fileType;
			search->address = data;

			return search;
		}
		else
		{
			LinkedListNode node = (LinkedListNode) hashTableSearch->address;
			void* data = (void*) node->data;
			int fileType = ((File) data)->fileType;

			if (fileType == FILE_TYPE)
			{
				return NULL;
			}

			return Find((Folder) data, path);
		}
	}
	else
	{
		return NULL;
	}
}