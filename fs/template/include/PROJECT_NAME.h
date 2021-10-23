#ifndef _PROJECT_NAME^^_H_
#define _PROJECT_NAME^^_H_

#define FUSE_USE_VERSION 26
#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include "fcntl.h"
#include "string.h"
#include "fuse.h"
#include <stddef.h>
#include "ddriver.h"
#include "errno.h"
#include "types.h"

#define PROJECT_NAME^^_MAGIC                  /* TODO: Define by yourself */
#define PROJECT_NAME^^_DEFAULT_PERM    0777   /* 全权限打开 */

/******************************************************************************
* SECTION: PROJECT_NAME.c
*******************************************************************************/
void* 			   PROJECT_NAME_init(struct fuse_conn_info *);
void  			   PROJECT_NAME_destroy(void *);
int   			   PROJECT_NAME_mkdir(const char *, mode_t);
int   			   PROJECT_NAME_getattr(const char *, struct stat *);
int   			   PROJECT_NAME_readdir(const char *, void *, fuse_fill_dir_t, off_t,
						                struct fuse_file_info *);
int   			   PROJECT_NAME_mknod(const char *, mode_t, dev_t);
int   			   PROJECT_NAME_write(const char *, const char *, size_t, off_t,
					                  struct fuse_file_info *);
int   			   PROJECT_NAME_read(const char *, char *, size_t, off_t,
					                 struct fuse_file_info *);
int   			   PROJECT_NAME_access(const char *, int);
int   			   PROJECT_NAME_unlink(const char *);
int   			   PROJECT_NAME_rmdir(const char *);
int   			   PROJECT_NAME_rename(const char *, const char *);
int   			   PROJECT_NAME_utimens(const char *, const struct timespec tv[2]);
int   			   PROJECT_NAME_truncate(const char *, off_t);
			
int   			   PROJECT_NAME_open(const char *, struct fuse_file_info *);
int   			   PROJECT_NAME_opendir(const char *, struct fuse_file_info *);

#endif  /* _PROJECT_NAME_H_ */