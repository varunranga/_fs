#define FUSE_USE_VERSION 31

#ifndef _FUSE_H
	#include <fuse.h>
#endif

#ifndef _STRING_H
	#include <string.h>
#endif

#ifndef _ERRNO_H
	#include <errno.h>
#endif

#ifndef _STDIO_H
	#include <stdio.h>
#endif

#include "File/File_Header.h"

static Folder _root;
static Folder _pwd;

static void* __init__(struct fuse_conn_info *conn, struct fuse_config *cfg)
{
	(void) conn;
		
	// cfg->use_ino = 1;
	// cfg->entry_timeout = 0;
	// cfg->attr_timeout = 0;
	// cfg->negative_timeout = 0;

	int success = MemoryManagement_Initialize() ;
	if (success == OPERATION_SUCCESSFUL_BEFORE)
	{
		_root = (Folder) _mount->memoryStart;
		_pwd = _root;

		// printf("\n\n\nROOT: %s\n\n\n", _root->folderName);

		return NULL;
	}

	_root = Initialize_Root();
	_pwd = _root;

	// File_Create(_root, "file1.txt");
	// Folder_Create(_root, "folder1");

	// File file = File_Open(_root, "file1.txt");

	// File_Write(file, "Hello, world!\n", 0);

	return NULL;
}

static int __getattr__(const char *constPath, struct stat *stbuf, struct fuse_file_info *fi)
{
	(void) fi;
	int res = 0;

	if (strcmp(constPath, "/") == 0)
	{
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;

		return 0;
	}
	else
	{
		int pathLength = strlen(constPath);
		char* path = (char*) malloc (sizeof(char) * pathLength);
		strncpy(path, constPath, pathLength);
		path++;

		memset(stbuf, 0, sizeof(struct stat));

		char* buf = (char*) malloc (sizeof(char) * pathLength);

		int i = 0;
		int j = 0;

		Folder currentFolder = _root;

		for (; i < pathLength-1; i++)
		{
			if (path[i] == '/')
			{
				buf[j] = '\0';
				j = 0;

				Search search = Find(currentFolder, buf);
				if (!search)
				{
					return -ENOENT;
				}
				else
				if (search->fileType == FILE_TYPE)
				{
					return -ENOENT;
				}
				else
				{
					currentFolder = (Folder)search->address;
				}

				memset(buf, 0, pathLength);
			}
			else
			{
				buf[j] = path[i];
				j++;
			}
		}

		Search search = Find(currentFolder, buf);

		if (!search)
		{
			printf("__getattr__: could not find! %s\n", path);

			res = -ENOENT;
		}
		else
		if (search->fileType == FOLDER_TYPE)
		{
			stbuf->st_mode = S_IFDIR | 0755;
			stbuf->st_nlink = 2;
		}
		else
		if (search->fileType == FILE_TYPE)
		{
			stbuf->st_mode = S_IFREG | 0755;
			stbuf->st_nlink = 1;
			File file = (File) search->address;
			stbuf->st_size = file->fileLength;
		}
		else
		{
			res = -ENOENT;
		}

		return res;
	}
}

static int __readdir__(const char* constPath, void *buff, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi /*, enum fuse_readdir_flags flags */)
{

	printf("__readdir__ : %s\n", constPath) ;

	void _filler(LinkedListElementType data)
	{
		File file = (File) data;
		char fileName[100];
		(int*) data;
		strcpy(fileName, file->fileName);

		filler(buff, fileName, NULL, 0/*, 0*/);
	}

	Folder currentFolder;

	if (strcmp(constPath, "/") == 0)
	{
		currentFolder = _root;
	}
	else
	{
		int pathLength = strlen(constPath);
		char* path = (char*) malloc (sizeof(char) * pathLength);
		strncpy(path, constPath, pathLength);
		path++;

		char* buf = (char*) malloc (sizeof(char) * pathLength);

		int i = 0;
		int j = 0;

		currentFolder = _root;

		for (; i < pathLength; i++)
		{
			if ((path[i] == '/') || (path[i] == '\0'))
			{
				buf[j] = '\0';
				j = 0;

				Search search = Find(currentFolder, buf);
				if (!search)
				{
					return -ENOENT;
				}
				else
				if (search->fileType == FILE_TYPE)
				{
					return -ENOENT;
				}
				else
				{
					currentFolder = (Folder)search->address;
				}

				memset(buf, 0, pathLength);
			}
			else
			{
				buf[j] = path[i];
				j++;
			}
		}
	}

	HashTable_Traverse(currentFolder->folderTable, _filler);

	return 0;
}

static int __open__(const char* constPath, struct fuse_file_info *fi)
{
	int pathLength = strlen(constPath);
	char* path = (char*) malloc (sizeof(char) * pathLength);
	strncpy(path, constPath, pathLength);
	path++;

	Folder currentFolder;

	char* buf;

	if (strcmp(constPath, "/") == 0)
	{
		currentFolder == _root;
		buf = path;
	}
	else
	{
		int pathLength = strlen(constPath);
		char* path = (char*) malloc (sizeof(char) * pathLength);
		strncpy(path, constPath, pathLength);
		path++;

		buf = (char*) malloc (sizeof(char) * pathLength);

		int i = 0;
		int j = 0;

		currentFolder = _root;

		for (; i < pathLength-1; i++)
		{
			if ((path[i] == '/') || (path[i] == '\0'))
			{
				buf[j] = '\0';
				j = 0;

				Search search = Find(currentFolder, buf);
				if (!search)
				{
					return -ENOENT;
				}
				else
				if (search->fileType == FILE_TYPE)
				{
					return -ENOENT;
				}
				else
				{
					currentFolder = (Folder)search->address;
				}

				memset(buf, 0, pathLength);
			}
			else
			{
				buf[j] = path[i];
				j++;
			}
		}
	}

	Search search = Find(currentFolder, buf);

	if (!search)
	{
		return -ENOENT;
	}

	// if ((fi->flags & O_ACCMODE) != O_RDONLY)
	// {
	// 	return -EACCES;
	// }

	File file = (File) search->address;
	file->fileInstancesOpened += 1;

	return 0;
}

static int __mknod__(const char* constPath, mode_t mode, dev_t rdev)
{
	int res;

	int pathLength = strlen(constPath);
	char* path = (char*) malloc (sizeof(char) * pathLength);
	strncpy(path, constPath, pathLength);
	path++;

	char* buf = (char*) malloc (sizeof(char) * pathLength);

	int i = 0;
	int j = 0;

	Folder currentFolder = _root;

	for (; i < pathLength-1; i++)
	{
		if (path[i] == '/')
		{
			buf[j] = '\0';
			j = 0;

			Search search = Find(currentFolder, buf);
			if (!search)
			{
				return -ENOENT;
			}
			else
			if (search->fileType == FILE_TYPE)
			{
				return -ENOENT;
			}
			else
			{
				currentFolder = (Folder)search->address;
			}

			memset(buf, 0, pathLength);
		}
		else
		{
			buf[j] = path[i];
			j++;
		}
	}

	printf("__mknod__: Filename %s\n", buf);

	File_Create(currentFolder, buf);

	return 0;
}

static int __read__(const char* constPath, char* buff, size_t size, off_t offset, struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;

	int pathLength = strlen(constPath);
	char* path = (char*) malloc (sizeof(char) * pathLength);
	strncpy(path, constPath, pathLength);
	path++;

	Folder currentFolder;

	char* buf;

	if (strcmp(constPath, "/") == 0)
	{
		currentFolder == _root;
		buf = path;
	}
	else
	{
		int pathLength = strlen(constPath);
		char* path = (char*) malloc (sizeof(char) * pathLength);
		strncpy(path, constPath, pathLength);
		path++;

		buf = (char*) malloc (sizeof(char) * pathLength);

		int i = 0;
		int j = 0;

		currentFolder = _root;

		for (; i < pathLength-1; i++)
		{
			if ((path[i] == '/') || (path[i] == '\0'))
			{
				buf[j] = '\0';
				j = 0;

				Search search = Find(currentFolder, buf);
				if (!search)
				{
					return -ENOENT;
				}
				else
				if (search->fileType == FILE_TYPE)
				{
					return -ENOENT;
				}
				else
				{
					currentFolder = (Folder)search->address;
				}

				memset(buf, 0, pathLength);
			}
			else
			{
				buf[j] = path[i];
				j++;
			}
		}
	}

	Search search = Find(currentFolder, buf);

	if ((!search) || (search->fileType == FOLDER_TYPE))
	{
		return -ENOENT;
	}

	char* content = File_Read((File) search->address, size, offset);
	memcpy(buff, content, size);

	return strlen(content);
}


static int __write__(const char* constPath, const char* contentToWrite, size_t size, off_t offset, struct fuse_file_info *fi)
{
	int res;
	(void) fi;

	int pathLength = strlen(constPath);
	char* path = (char*) malloc (sizeof(char) * pathLength);
	strncpy(path, constPath, pathLength);
	path++;

	char* buf = (char*) malloc (sizeof(char) * pathLength);

	int i = 0;
	int j = 0;

	Folder currentFolder = _root;

	for (; i < pathLength-1; i++)
	{
		if (path[i] == '/')
		{
			buf[j] = '\0';
			j = 0;

			Search search = Find(currentFolder, buf);
			if (!search)
			{
				return -ENOENT;
			}
			else
			if (search->fileType == FILE_TYPE)
			{
				return -ENOENT;
			}
			else
			{
				currentFolder = (Folder)search->address;
			}

			memset(buf, 0, pathLength);
		}
		else
		{
			buf[j] = path[i];
			j++;
		}
	}

	Search search = Find(currentFolder, buf);

	if (!search)
	{
		return -ENOENT;
	}
	else
	if (search->fileType == FOLDER_TYPE)
	{
		return -ENOENT;
	}
	else
	{
		printf("__write__: Writing\n");

		File file = File_Open(currentFolder, buf);
		File_Write(file, contentToWrite, offset);
	}

	return 0;
}

static int __access__(const char* constPath, int mask)
{
	if (strcmp(constPath, "/") == 0)
	{
		return 0;
	}

	int pathLength = strlen(constPath);
	char* path = (char*) malloc (sizeof(char) * pathLength);
	strncpy(path, constPath, pathLength);
	path++;

	Folder currentFolder;
	
	char* buf = (char*) malloc (sizeof(char) * pathLength);

	int i = 0;
	int j = 0;

	currentFolder = _root;

	for (; i < pathLength-1; i++)
	{
		if (path[i] == '/')
		{
			buf[j] = '\0';
			j = 0;

			Search search = Find(currentFolder, buf);
			if (!search)
			{
				return -ENOENT;
			}
			else
			if (search->fileType == FILE_TYPE)
			{
				return -ENOENT;
			}
			else
			{
				currentFolder = (Folder)search->address;
			}			

			memset(buf, 0, pathLength);
		}
		else
		{
			buf[j] = path[i];
			j++;
		}
	}	

	Search search = Find(currentFolder, buf);
	if (!search)
	{
		return -ENOENT;
	}

	return 0;	
}

static int __mkdir__(const char* constPath, mode_t mode)
{
	int res;

	int pathLength = strlen(constPath);
	char* path = (char*) malloc (sizeof(char) * pathLength);
	strncpy(path, constPath, pathLength);
	path++;

	char* buf = (char*) malloc (sizeof(char) * pathLength);

	int i = 0;
	int j = 0;

	Folder currentFolder = _root;

	for (; i < pathLength-1; i++)
	{
		if (path[i] == '/')
		{
			buf[j] = '\0';
			j = 0;

			Search search = Find(currentFolder, buf);
			if (!search)
			{
				return -ENOENT;
			}
			else
			if (search->fileType == FILE_TYPE)
			{
				return -ENOENT;
			}
			else
			{
				currentFolder = (Folder)search->address;
			}

			memset(buf, 0, pathLength);
		}
		else
		{
			buf[j] = path[i];
			j++;
		}
	}

	printf("__mkdir__: Foldername %s\n", buf);

	Folder_Create(currentFolder, buf);

	return 0;
}

static int __create__(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	return __open__(path, fi);
}

static int __rmdir__(const char *constPath)
{
	int pathLength = strlen(constPath);
	char* path = (char*) malloc (sizeof(char) * pathLength);
	strncpy(path, constPath, pathLength);
	path++;

	Folder currentFolder;

	char* buf;

	if (strcmp(constPath, "/") == 0)
	{
		currentFolder == _root;

		return -ENOENT;
	}
	else
	{
		int pathLength = strlen(constPath);
		char* path = (char*) malloc (sizeof(char) * pathLength);
		strncpy(path, constPath, pathLength);
		path++;

		buf = (char*) malloc (sizeof(char) * pathLength);

		int i = 0;
		int j = 0;

		currentFolder = _root;

		for (; i < pathLength-1; i++)
		{
			if ((path[i] == '/') || (path[i] == '\0'))
			{
				buf[j] = '\0';
				j = 0;

				Search search = Find(currentFolder, buf);
				if (!search)
				{
					return -ENOENT;
				}
				else
				if (search->fileType == FILE_TYPE)
				{
					return -ENOENT;
				}
				else
				{
					currentFolder = (Folder)search->address;
				}

				memset(buf, 0, pathLength);
			}
			else
			{
				buf[j] = path[i];
				j++;
			}
		}
	}

	// Search search = Find(currentFolder, buf);

	printf("__rmdir__: Deleted : %s\n", buf);

	Search search = Find(currentFolder, buf);

	if (HashTable_Size(((Folder) (search->address))->folderTable) != 0)
	{
		return -ENOTEMPTY;
	}

	Folder_Delete(currentFolder, buf);

	return 0 ;
}

static int __unlink__(const char *constPath)
{
	int pathLength = strlen(constPath);
	char* path = (char*) malloc (sizeof(char) * pathLength);
	strncpy(path, constPath, pathLength);
	path++;

	Folder currentFolder;

	char* buf;

	if (strcmp(constPath, "/") == 0)
	{
		currentFolder == _root;

		return -ENOENT;
	}
	else
	{
		int pathLength = strlen(constPath);
		char* path = (char*) malloc (sizeof(char) * pathLength);
		strncpy(path, constPath, pathLength);
		path++;

		buf = (char*) malloc (sizeof(char) * pathLength);

		int i = 0;
		int j = 0;

		currentFolder = _root;

		for (; i < pathLength-1; i++)
		{
			if ((path[i] == '/') || (path[i] == '\0'))
			{
				buf[j] = '\0';
				j = 0;

				Search search = Find(currentFolder, buf);
				if (!search)
				{
					return -ENOENT;
				}
				else
				if (search->fileType == FILE_TYPE)
				{
					return -ENOENT;
				}
				else
				{
					currentFolder = (Folder)search->address;
				}

				memset(buf, 0, pathLength);
			}
			else
			{
				buf[j] = path[i];
				j++;
			}
		}
	}

	printf("__unlink__ : Deleted file : %s\n", buf) ;

	File_Delete(currentFolder, buf) ;

	return 0 ;
}

static int __rename__(const char *constPath, const char *newPath, unsigned int flags)
{	
	int pathLength = strlen(constPath);
	char* path = (char*) malloc (sizeof(char) * pathLength);
	strncpy(path, constPath, pathLength);
	path++;

	char* buf = (char*) malloc (sizeof(char) * pathLength);

	int i = 0;
	int j = 0;

	Folder currentFolder = _root;

	for (; i < pathLength-1; i++)
	{
		if (path[i] == '/')
		{
			buf[j] = '\0';
			j = 0;

			Search search = Find(currentFolder, buf);
			if (!search)
			{
				return -ENOENT;
			}
			else
			if (search->fileType == FILE_TYPE)
			{
				return -ENOENT;
			}
			else
			{
				currentFolder = (Folder)search->address;
			}

			memset(buf, 0, pathLength);
		}
		else
		{
			buf[j] = path[i];
			j++;
		}
	}

	File file = File_Open(currentFolder, buf) ;

	i = 0;
	j = 0;
	int pathLength2 = strlen(newPath);
	char* path2 = (char*) malloc (sizeof(char) * pathLength2);
	strncpy(path2, newPath, pathLength2);
	path2++;
	char* buf2 = (char*) malloc (sizeof(char) * pathLength2);

	currentFolder = _root;

	for (; i < pathLength2-1; i++)
	{
		if (path2[i] == '/')
		{
			buf2[j] = '\0';
			j = 0;

			Search search = Find(currentFolder, buf2);
			if (search->fileType == FILE_TYPE)
			{
				return -ENOENT;
			}
			else
			{
				currentFolder = (Folder)search->address;
			}

			memset(buf2, 0, pathLength2);
		}
		else
		{
			buf2[j] = path2[i];
			j++;
		}
	}

	file->fileName = buf2 ;
	return 0 ;
}


static struct fuse_operations _operations = 
{
	.init 		= __init__,
	.getattr 	= __getattr__,
	.readdir 	= __readdir__,
	.open 		= __open__,
	.read 		= __read__,
	.mknod 		= __mknod__,
	.write 		= __write__,
	.access 	= __access__,
	.mkdir 		= __mkdir__,
	.rmdir 		= __rmdir__,
	.unlink     = __unlink__,
	.rename     = __rename__
};

static void show_help(const char *progname)
{
    printf("usage: %s [options] <mountpoint>\n\n", progname);
}

int main(int argc, char* argv[])
{
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
/*
	if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
	{
		return 1;
	}

	if (options.show_help)
	{
		show_help(argv[0]);
		assert(fuse_opt_add_arg(&args, "--help") == 0);
		args.argv[0] = (char*) "";
	}
*/
	return fuse_main(args.argc, args.argv, &_operations, NULL);
}