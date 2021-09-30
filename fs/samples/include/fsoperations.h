#ifndef OPERATIONS
#define OPERATIONS

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "fstree.h"
#include "fsdisk.h"

/*
Pointer to the root node of the metadata tree
*/
extern FStree * root;

/*
Return file attributes
*/
int do_getattr(const char *path, struct stat *st);

/*
Return one or more directory entries (struct dirent) to the caller
*/
int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi );

/*
Create a directory
*/
int do_mkdir(const char * path, mode_t x);

/*
Delete an empty directory
*/
int do_rmdir(const char * path);

/*
Make a special (device) file, FIFO, or socket
*/
int do_mknod(const char * path, mode_t x, dev_t y);

/*
Open a file
*/
int do_open(const char *path, struct fuse_file_info *fi);

/*
Remove (delete) the given file
*/
int do_unlink(const char * path);

/*
Read size bytes from the given file into the buffer buf, beginning offset bytes into the file.
*/
int do_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi);

/*
Change the mode (permissions) of the given object to the given new permissions
*/
int do_chmod(const char *path, mode_t new);

/*
Read size bytes from the given buffer buf to the file, beginning offset bytes into the file
*/
int do_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

/*
Update the last access time of the given object
*/
int do_utimens(const char *path, struct utimbuf *tv);

/*
Change the size of a file
*/
int do_truncate(const char *path, off_t size);

/*
Rename / move a file
*/
int do_rename(const char* from, const char* to);

/*
Check file access permissions
*/
int do_access(const char* path,int mask);
#endif
