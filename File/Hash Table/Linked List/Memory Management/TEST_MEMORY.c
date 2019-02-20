#include "MemoryManagement_Header.h"

int main()
{
	MemoryManagement_Initialize();

	int success = MemoryManagement_WriteToBlock(10, 0, "Hello, world!\n");
	
	if (success)
	{
		printf("Write done!\n");
	}
	else
	{
		printf("Write failed!\n");
	}

	char* buf = MemoryManagement_ReadFromBlock(10, 14, 0);	

	if (buf)
	{
		printf("Read done! %s\n", buf);
	}
	else
	{
		printf("Read failed!\n");
	}

	printf("goodbye\n");
}