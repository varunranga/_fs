int MemoryManagement_UpdateMeta(Mount _mount)
{
	FILE *meta0Pointer = fopen("./.blocks/.meta0", "wb") ;		// meta0 contains _mount structure
	FILE *meta1Pointer = fopen("./.blocks/.meta1", "wb") ;		// meta1 contains hashtable structure

	// Writing the metadata to the files
	fwrite(_mount, sizeof(MountStructure), 1, meta0Pointer);
	fwrite(_mount->memoryStart, sizeof(char), MEMORY_SIZE, meta1Pointer);

	fclose(meta0Pointer);
	fclose(meta1Pointer);
	
	return OPERATION_SUCCESSFUL;	
}

int MemoryManagement_LoadMeta()
{
	DIR* dir = opendir(".blocks");

	// printf("\n\n\nDIR: %p\n\n\n", dir);

	if (dir)
	{
		FILE *meta0Pointer = fopen("./.blocks/.meta0", "rb") ;		// meta0 contains _mount structure
		FILE *meta1Pointer = fopen("./.blocks/.meta1", "rb") ;		// meta1 contains hashtable structure

		_mount = (Mount) sbrk (sizeof(MountStructure));
		fread(_mount, sizeof(MountStructure), 1, meta0Pointer);
		void* memorySpace = (void*) sbrk (MEMORY_SIZE);
		_mount->memoryStart = memorySpace;
		fread(_mount->memoryStart, sizeof(char), MEMORY_SIZE, meta1Pointer);

		fclose(meta0Pointer);
		fclose(meta1Pointer);    

	    closedir(dir);

	    return OPERATION_SUCCESSFUL;
	}
	else if (ENOENT == errno)
	{
	    return OPERATION_FAILED;
	}

	return OPERATION_FAILED ;
}

int MemoryManagement_Initialize()
{
	if (MemoryManagement_LoadMeta()) return OPERATION_SUCCESSFUL_BEFORE;


	int i = NUMBER_OF_BLOCKS;
	void* start = (void*) sbrk(TOTAL_SIZE);

	while (i--)
	{	
		mkdir(".blocks", 0777);

		char _blockNumberInString[30];
		sprintf(_blockNumberInString, "./.blocks/.%d", i);

		FILE *filePointer = fopen(_blockNumberInString, "wb");
		fwrite(start, BLOCK_SIZE, 1, filePointer);
		fclose(filePointer);
	}

	sbrk(-TOTAL_SIZE);

	// Initializing Mount structure
	_mount = (Mount) sbrk (sizeof(MountStructure));
	void* memorySpace = (void*) sbrk (MEMORY_SIZE);

	_mount->memoryStart = memorySpace;
	_mount->_remainingSpace = MEMORY_SIZE ;

	return MemoryManagement_UpdateMeta(_mount) ;
	//return OPERATION_SUCCESSFUL;
}


void* MemoryManagement_Malloc(long long int requiredSize)
{
	if (_mount->_remainingSpace < requiredSize)
	{
		return 0;
	}

	void* toBeReturned = _mount->memoryStart + (MEMORY_SIZE - _mount->_remainingSpace);

	_mount->_remainingSpace -= requiredSize;

	MemoryManagement_UpdateMeta(_mount);
	return (void*) toBeReturned;
}

int* MemoryManagement_Balloc(long long int requiredSize)
{
	int* allocatedBlocks = (int*) MemoryManagement_Malloc(sizeof(int) * BITMAP) ;
	memset(allocatedBlocks, 0, sizeof(int) * BITMAP) ;

	int mainBlockCounter = 0;

	while (requiredSize > 0)
	{
		while (TestBit(_mount->usedBlocks, mainBlockCounter)) mainBlockCounter++;
		SetBit(_mount->usedBlocks, mainBlockCounter);
		SetBit(allocatedBlocks, mainBlockCounter);
		requiredSize -= BLOCK_SIZE;
	}

	MemoryManagement_UpdateMeta(_mount);
	return allocatedBlocks;
}

int MemoryManagement_WriteToBlock(int blockNumber, int offset, char* toWrite)
{
	int sizeToWrite = strlen(toWrite);

	if ((offset + sizeToWrite) > BLOCK_SIZE)
	{
		return OPERATION_FAILED;
	}

	char _blockNumberInString[30];
	sprintf(_blockNumberInString, "./.blocks/.%d", blockNumber);

	FILE *filePointer = fopen(_blockNumberInString, "rb");
	fseek(filePointer, 0, SEEK_SET);
	
	char* block = (char*) malloc(BLOCK_SIZE);
	fread(block, sizeof(char), BLOCK_SIZE, filePointer);
	fclose(filePointer);

	int i = 0;
	for (; i < sizeToWrite; i++)
	{
		block[i+offset] = toWrite[i];
	}

	filePointer = fopen(_blockNumberInString, "wb");
	printf("FWRITE: %d\n", fwrite(block, sizeof(char), BLOCK_SIZE, filePointer));
	fclose(filePointer);

	return OPERATION_SUCCESSFUL;
}

char* MemoryManagement_ReadFromBlock(int blockNumber, int length, int offset)
{
	char* buf = (char*) malloc (sizeof(char) * length);

	char _blockNumberInString[30];
	sprintf(_blockNumberInString, "./.blocks/.%d", blockNumber);

	FILE *filePointer = fopen(_blockNumberInString, "rb");
	// fseek(filePointer, 0, SEEK_SET);

	char* block = (char*) malloc (BLOCK_SIZE);

	printf("%p\n", block);

	fread(block, sizeof(char), BLOCK_SIZE, filePointer);

	printf("Block : %s \n", block); // NULL

	memcpy(buf, (void*) block+offset, ((BLOCK_SIZE - offset) > length) ? length : (BLOCK_SIZE - offset));
	fclose(filePointer) ;
	printf("Buf : %s \n", buf) ;

	return buf;
}

int MemoryManagement_FreeBlocks(int* bitmap)
{
	int i = 0 ;
	while(i < (BITMAP*sizeof(int)*8))
	{
		if(TestBit(bitmap, i))
			ClearBit(_mount->usedBlocks, i) ;
		i++ ;
	}
	return OPERATION_SUCCESSFUL ;
}

char* MemoryManagement_ReadFromBlocks(int* bitmap, int numberOfBytes, int offset)
{
	int i = 0 ;
	int bytesRemaining = numberOfBytes ;
	char* buf = (char*) malloc (sizeof(char) * numberOfBytes);
	char* temp;

	int j = 0;

//  if numberOfBytes < (BLOCK_SIZE + offset) we have one block to read atmost
	while(i < (BITMAP*sizeof(int)*8) && (bytesRemaining > 0))
	{
		if(TestBit(bitmap, i))
		{
			temp = MemoryManagement_ReadFromBlock(i, MIN(bytesRemaining, BLOCK_SIZE), offset) ;
			
			int k = 0;

			int _min = MIN(bytesRemaining,BLOCK_SIZE);
			for (; k < _min; k++)
			{
				buf[j] = temp[k];
				j++;
			}

			// buf = strncat(buf, temp, MIN(bytesRemaining, BLOCK_SIZE));		// Appends n bytes (3rd param) from temp to buf
			bytesRemaining -= _min ;
			offset = 0 ;									// Offset is used for the 1st time, it's 0 for future blocks
		}

		i++ ;
	}

	buf[j] = '\0';

	return buf ;
}

int MemoryManagement_WriteToBlocks(int* bitmap, int offset, char* contentToWrite)
{
	int i = 0, pos = 0;
	char* temp = (char*) malloc (sizeof(char) * BLOCK_SIZE);

	while(i < (BITMAP*sizeof(int)*8) && (pos < strlen(contentToWrite)))
	{
		if(TestBit(bitmap, i))
		{
			int j = 0 ;
			
			while(pos < strlen(contentToWrite) && j < BLOCK_SIZE)
				temp[j++] = contentToWrite[pos++] ;

			MemoryManagement_WriteToBlock(i, offset, temp) ;
			offset = 0 ;
			memset(temp, 0, sizeof(char) * BLOCK_SIZE) ;
		}

		i++ ;
	}

	return OPERATION_SUCCESSFUL ;
}