#include <stdio.h>

#include "File_Header.h"

int main()
{
	Folder root = Initialize_Root();
	int success = File_Create(root, "file1");

	if (success)
	{
		printf("File 1 has been created successfully.\n");
	}

	File file1 = File_Open(root, "file1");

	if (file1)
	{
		printf("File 1 has been opened successfully.\n");
	}

	success = File_Write(file1, "Hello, world!", 0);

	if (success)
	{
		printf("File 1 has been been written with content.\n");
	}

	char* readContents = File_Read(file1, 4096, 0);

	printf("Contents Read 1 = %s\n", readContents);

	success = File_Create(root, "file2");

	if (success)
	{
		printf("File 2 has been created successfully.\n");
	}

	File file2 = File_Open(root, "file2");

	if (file2)
	{
		printf("File 2 has been opened successfully.\n");
	}

	success = File_Write(file2, "This is DJ SURFACe", 4);

	if (success)
	{
		printf("File 2 has been been written with content.\n");
	}

	readContents = File_Read(file2, 13, 5);

	printf("Contents Read 2 = %s\n", readContents);

	success = File_Delete(root, "file1");

	if (success)
	{
		printf("File 1 has been deleted successfully.\n");
	}

	success = Folder_Create(root, "folder1");

	if (success)
	{
		printf("Folder 1 has been created successfully.\n");
	}

	Search search = Find(root, "folder1");

	if (!search)
	{
		printf("Could not find.\n");
	}
	else
	{
		printf("Found file type:%d\n", search->fileType);
		success = File_Create((Folder) search->address, "file3");
	
		if (success)
		{
			printf("File 3 created successfully.\n");
		}
	}
	
	
	
	return 0;
}