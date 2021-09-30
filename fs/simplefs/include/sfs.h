#ifndef _SFS_H_ 
#define _SFS_H_

#define FUSE_USE_VERSION 26
#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include "fcntl.h"
#include "string.h"
#include "fuse.h"
#include <stddef.h>
#include "ddriver_ctl_user.h"
#include "errno.h"
#include "types.h"
/******************************************************************************
* SECTION: global region
*******************************************************************************/
struct sfs_super      sfs_super; 
struct custom_options sfs_options;
/******************************************************************************
* SECTION: macro debug
*******************************************************************************/
#define SFS_DBG(fmt, ...) do { printf("SFS_DBG: " fmt, ##__VA_ARGS__); } while(0) 
/******************************************************************************
* SECTION: sfs_utils.c
*******************************************************************************/
char* 			   sfs_get_fname(const char* path);
int 			   sfs_calc_lvl(const char * path);
int 			   sfs_driver_read(int offset, uint8_t *out_content, int size);
int 			   sfs_driver_write(int offset, uint8_t *in_content, int size);


int 			   sfs_mount(struct custom_options options);
int 			   sfs_umount();

int 			   sfs_alloc_dentry(struct sfs_inode * inode, struct sfs_dentry * dentry);
int 			   sfs_drop_dentry(struct sfs_inode * inode, struct sfs_dentry * dentry);
struct sfs_inode*  sfs_alloc_inode(struct sfs_dentry * dentry);
int 			   sfs_sync_inode(struct sfs_inode * inode);
int 			   sfs_drop_inode(struct sfs_inode * inode);
struct sfs_inode*  sfs_read_inode(struct sfs_dentry * dentry, int ino);
struct sfs_dentry* sfs_get_dentry(struct sfs_inode * inode, int dir);

struct sfs_dentry* sfs_lookup(const char * path, boolean * is_find, boolean* is_root);
/******************************************************************************
* SECTION: sfs.c
*******************************************************************************/
void* sfs_init(struct fuse_conn_info *);
void  sfs_destroy(void *);
int   sfs_mkdir(const char *, mode_t);
int   sfs_getattr(const char *, struct stat *);
int   sfs_readdir(const char *, void *, fuse_fill_dir_t, off_t,
			      struct fuse_file_info *);
int   sfs_mknod(const char *, mode_t, dev_t);
int   sfs_write(const char *, const char *, size_t, off_t,
		        struct fuse_file_info *);
int   sfs_read(const char *, char *, size_t, off_t,
		       struct fuse_file_info *);
int   sfs_unlink(const char *);
int   sfs_rmdir(const char *);
int   sfs_rename(const char *, const char *);
int   sfs_utimens(const char *, const struct timespec tv[2]);
int   sfs_truncate(const char *, off_t);

int   sfs_open(const char *, struct fuse_file_info *);
int   sfs_opendir(const char *, struct fuse_file_info *);
int   sfs_access(const char *, int);
/******************************************************************************
* SECTION: debug.c
*******************************************************************************/
void sfs_dump_map();
#endif