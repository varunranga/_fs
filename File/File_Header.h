#ifndef _STRINGS_H
	#include <strings.h>
#endif

#ifndef _STRING_H
	#include <string.h>
#endif

#ifndef _STDLIB_H
	#include <stdlib.h>
#endif

#include "Hash Table/HashTable_Header.h"

#define FOLDER_TYPE 0
#define FILE_TYPE 1

#ifndef OPERATION_FAILED
	#define OPERATION_FAILED 0
#endif

#ifndef OPERATION_SUCCESSFUL
	#define OPERATION_SUCCESSFUL 1
#endif

typedef struct file_structure FileStructure;
typedef FileStructure* File;

typedef struct folder_structure FolderStructure;
typedef FolderStructure* Folder;

typedef struct folder_structure
{
	char* folderName;
	int fileType;
	Folder parentFolder;
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
	int* bitmap;
	//void* fileContents;
} FileStructure;

typedef FileStructure* File;

typedef struct $
{
	int fileType;
	void *address;
} SearchStructure;

typedef SearchStructure* Search;

static Folder _root;
static Folder _presentWorkingDirectory;

int File_Create(Folder folder, char* fileName);
int File_Delete(Folder folder, char* fileName);

File File_Open(Folder folder, char* fileName);
int File_Close(File file);

char* File_Read(File file, int numberOfBytes, int offset);
int File_Write(File file, char* contentToWrite, int offset);

File File_Find(Folder folder, char* path);

Folder Initialize_Root();

int Folder_Create(Folder folder, char* folderName);
int Folder_Delete(Folder folder, char* folderName);

Folder Folder_Find(Folder folder, char* path);

#include "File_Implementation.c"
