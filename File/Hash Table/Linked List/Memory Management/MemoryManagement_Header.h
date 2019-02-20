#ifndef _STDIO_H
	#include <stdio.h>
#endif

#ifndef _STDLIB_H
	#include <stdlib.h>
#endif

#ifndef _STRING_H
	#include <string.h>
#endif

#ifndef _UNISTD_H
	#include <unistd.h>
#endif

#ifndef _DIRENT_H
	#include <dirent.h>
#endif

#ifndef _ERROR_H
	#include <errno.h>
#endif

#define GIGABYTE 1073741824
#define MEGABYTE 1048576
#define KILOBYTE 1024

#define BLOCK_SIZE 4*KILOBYTE
#define TOTAL_SIZE 100*MEGABYTE
#define NUMBER_OF_BLOCKS ((TOTAL_SIZE)/(BLOCK_SIZE))  

#define MEMORY_SIZE 10*MEGABYTE

#define OPERATION_FAILED 0
#define OPERATION_SUCCESSFUL 1
#define OPERATION_SUCCESSFUL_BEFORE 2

#define BITMAP			NUMBER_OF_BLOCKS/sizeof(int)
#define SetBit(A,k)		(A[(k/32)] |= (1 << (k%32)))  
#define ClearBit(A,k)	(A[(k/32)] &= ~(1 << (k%32)))  
#define TestBit(A,k)    (A[(k/32)] & (1 << (k%32)))  
#define MIN(A,B)		(A<B)?A:B
#define MAX(A,B)		(A>B)?A:B    


typedef struct 
{
	int usedBlocks[BITMAP];	// bit array of size 8. int A[8] => 32*8 bits => 256 bits
	void* memoryStart;
	long long int _remainingSpace;	
} MountStructure;

typedef MountStructure* Mount;

static void* _MemoryManagement_MemorySpace;
static Mount _mount;
extern int errno;
//static int _remainingSpace = MEMORY_SIZE;

int MemoryManagement_UpdateMeta(Mount _mount);
int MemoryManagement_LoadMeta();
int MemoryManagement_Initialize();
void* MemoryManagement_Malloc(long long int requiredSize);
int* MemoryManagement_Balloc(long long int requiredSize);
int MemoryManagement_WriteToBlock(int blockNumber, int offset, char* toWrite);
char* MemoryManagement_ReadFromBlock(int blockNumber, int length, int offset);
int MemoryManagement_FreeBlocks(int* bitmap);
char* MemoryManagement_ReadFromBlocks(int* bitmap, int numberOfBytes, int offset);
int MemoryManagement_WriteToBlocks(int* bitmap, int offset, char* contentToWrite);

#include "MemoryManagement_Implementation.c"
