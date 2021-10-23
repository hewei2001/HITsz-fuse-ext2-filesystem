#include "../include/sfs.h"
/******************************************************************************
* SECTION: Macro
*******************************************************************************/
#define OPTION(t, p)        { t, offsetof(struct custom_options, p), 1 }

/******************************************************************************
* SECTION: Global Static Var
*******************************************************************************/
static const struct fuse_opt option_spec[] = {
	OPTION("--device=%s", device),
	OPTION("-h", show_help),
	OPTION("--help", show_help),
	FUSE_OPT_END
};

static struct fuse_operations operations = {
	.init = sfs_init,						          /* mount文件系统 */		
	.destroy = sfs_destroy,							  /* umount文件系统 */
	.mkdir = sfs_mkdir,								  /* 建目录，mkdir */
	.getattr = sfs_getattr,							  /* 获取文件属性，类似stat，必须完成 */
	.readdir = sfs_readdir,							  /* 填充dentrys */
	.mknod = sfs_mknod,							      /* 创建文件，touch相关 */
	.write = sfs_write,								  /* 写入文件 */
	.read = sfs_read,								  /* 读文件 */
	.utimens = sfs_utimens,							  /* 修改时间，忽略，避免touch报错 */
	.truncate = sfs_truncate,						  /* 改变文件大小 */
	.unlink = sfs_unlink,							  /* 删除文件 */
	.rmdir	= sfs_rmdir,							  /* 删除目录， rm -r */
	.rename = sfs_rename,							  /* 重命名，mv */

	.open = sfs_open,							
	.opendir = sfs_opendir,
	.access = sfs_access
};
/******************************************************************************
* SECTION: Function Implementation
*******************************************************************************/
void* sfs_init(struct fuse_conn_info * conn_info) {
	if (sfs_mount(sfs_options) != SFS_ERROR_NONE) {
        SFS_DBG("[%s] mount error\n", __func__);
		fuse_exit(fuse_get_context()->fuse);
		return NULL;
	} 
	return NULL;
}

void sfs_destroy(void* p) {
	if (sfs_umount() != SFS_ERROR_NONE) {
		SFS_DBG("[%s] unmount error\n", __func__);
		fuse_exit(fuse_get_context()->fuse);
		return;
	}
	return;
}
/**
 * @brief 
 * 
 * @param path 
 * @param mode 
 * @return int 
 */
int sfs_mkdir(const char* path, mode_t mode) {
	(void)mode;
	boolean is_find, is_root;
	char* fname;
	struct sfs_dentry* last_dentry = sfs_lookup(path, &is_find, &is_root);
	struct sfs_dentry* dentry;
	struct sfs_inode*  inode;

	if (is_find) {
		return -SFS_ERROR_EXISTS;
	}

	if (SFS_IS_REG(last_dentry->inode)) {
		return -SFS_ERROR_UNSUPPORTED;
	}

	fname  = sfs_get_fname(path);
	dentry = new_dentry(fname, SFS_DIR); 
	dentry->parent = last_dentry;
	inode  = sfs_alloc_inode(dentry);
	sfs_alloc_dentry(last_dentry->inode, dentry);
	
	return SFS_ERROR_NONE;
}
/**
 * @brief 获取文件属性
 * 
 * @param path 相对于挂载点的路径
 * @param sfs_stat 返回状态
 * @return int 
 */
int sfs_getattr(const char* path, struct stat * sfs_stat) {
	boolean	is_find, is_root;
	struct sfs_dentry* dentry = sfs_lookup(path, &is_find, &is_root);
	if (is_find == FALSE) {
		return -SFS_ERROR_NOTFOUND;
	}

	if (SFS_IS_DIR(dentry->inode)) {
		sfs_stat->st_mode = S_IFDIR | SFS_DEFAULT_PERM;
		sfs_stat->st_size = dentry->inode->dir_cnt * sizeof(struct sfs_dentry_d);
	}
	else if (SFS_IS_REG(dentry->inode)) {
		sfs_stat->st_mode = S_IFREG | SFS_DEFAULT_PERM;
		sfs_stat->st_size = dentry->inode->size;
	}

	sfs_stat->st_nlink = 1;
	sfs_stat->st_uid 	 = getuid();
	sfs_stat->st_gid 	 = getgid();
	sfs_stat->st_atime   = time(NULL);
	sfs_stat->st_mtime   = time(NULL);
	sfs_stat->st_blksize = SFS_IO_SZ();

	if (is_root) {
		sfs_stat->st_size	= sfs_super.sz_usage; 
		sfs_stat->st_blocks = SFS_DISK_SZ() / SFS_IO_SZ();
		sfs_stat->st_nlink  = 2;		/* !特殊，根目录link数为2 */
	}
	return SFS_ERROR_NONE;
}
/**
 * @brief 
 * 
 * @param path 
 * @param buf 
 * @param filler 参数讲解:
 * 
 * typedef int (*fuse_fill_dir_t) (void *buf, const char *name,
 *				const struct stat *stbuf, off_t off)
 * buf: name会被复制到buf中
 * name: dentry名字
 * stbuf: 文件状态，可忽略
 * off: 下一次offset从哪里开始，这里可以理解为第几个dentry
 * 
 * @param offset 
 * @param fi 
 * @return int 
 */
int sfs_readdir(const char * path, void * buf, fuse_fill_dir_t filler, off_t offset,
			    struct fuse_file_info * fi) {
    boolean	is_find, is_root;
	int		cur_dir = offset;

	struct sfs_dentry* dentry = sfs_lookup(path, &is_find, &is_root);
	struct sfs_dentry* sub_dentry;
	struct sfs_inode* inode;
	if (is_find) {
		inode = dentry->inode;
		sub_dentry = sfs_get_dentry(inode, cur_dir);
		if (sub_dentry) {
			filler(buf, sub_dentry->fname, NULL, ++offset);
		}
		return SFS_ERROR_NONE;
	}
	return -SFS_ERROR_NOTFOUND;
}
/**
 * @brief 
 * 
 * @param path 
 * @param mode 
 * @param fi 
 * @return int 
 */
int sfs_mknod(const char* path, mode_t mode, dev_t dev) {
	boolean	is_find, is_root;
	
	struct sfs_dentry* last_dentry = sfs_lookup(path, &is_find, &is_root);
	struct sfs_dentry* dentry;
	struct sfs_inode* inode;
	char* fname;
	
	if (is_find == TRUE) {
		return -SFS_ERROR_EXISTS;
	}

	fname = sfs_get_fname(path);
	
	if (S_ISREG(mode)) {
		dentry = new_dentry(fname, SFS_REG_FILE);
	}
	else if (S_ISDIR(mode)) {
		dentry = new_dentry(fname, SFS_DIR);
	}
	dentry->parent = last_dentry;
	inode = sfs_alloc_inode(dentry);
	sfs_alloc_dentry(last_dentry->inode, dentry);

	return SFS_ERROR_NONE;
}
/**
 * @brief 
 * 
 * @param path 
 * @param buf 
 * @param size 
 * @param offset 
 * @param fi 
 * @return int 
 */
int sfs_write(const char* path, const char* buf, size_t size, off_t offset,
		        struct fuse_file_info* fi) {
    boolean	is_find, is_root;
	struct sfs_dentry* dentry = sfs_lookup(path, &is_find, &is_root);
	struct sfs_inode*  inode;
	
	if (is_find == FALSE) {
		return -SFS_ERROR_NOTFOUND;
	}

	inode = dentry->inode;
	
	if (SFS_IS_DIR(inode)) {
		return -SFS_ERROR_ISDIR;	
	}

	if (inode->size < offset) {
		return -SFS_ERROR_SEEK;
	}

	memcpy(inode->data + offset, buf, size);
	inode->size = offset + size > inode->size ? offset + size : inode->size;
	
	return size;
}
/**
 * @brief 
 * 
 * @param path 
 * @param buf 
 * @param size 
 * @param offset 
 * @param fi 
 * @return int 
 */
int sfs_read(const char* path, char* buf, size_t size, off_t offset,
		       struct fuse_file_info* fi) {
	boolean	is_find, is_root;
	struct sfs_dentry* dentry = sfs_lookup(path, &is_find, &is_root);
	struct sfs_inode*  inode;

	if (is_find == FALSE) {
		return -SFS_ERROR_NOTFOUND;
	}

	inode = dentry->inode;
	
	if (SFS_IS_DIR(inode)) {
		return -SFS_ERROR_ISDIR;	
	}

	if (inode->size < offset) {
		return -SFS_ERROR_SEEK;
	}

	memcpy(buf, inode->data + offset, size);

	return size;			   
}
/**
 * @brief 
 * 
 * @param path 
 * @return int 
 */
int sfs_unlink(const char* path) {
	boolean	is_find, is_root;
	struct sfs_dentry* dentry = sfs_lookup(path, &is_find, &is_root);
	struct sfs_inode*  inode;

	if (is_find == FALSE) {
		return -SFS_ERROR_NOTFOUND;
	}

	inode = dentry->inode;

	sfs_drop_inode(inode);
	sfs_drop_dentry(dentry->parent->inode, dentry);
	return SFS_ERROR_NONE;
}
/**
 * @brief 删除路径时的步骤
 * rm ./tests/mnt/j/ -r
 *  1) Step 1. rm ./tests/mnt/j/j
 *  2) Step 2. rm ./tests/mnt/j
 * @param path 
 * @return int 
 */
int sfs_rmdir(const char* path) {
	return sfs_unlink(path);
}
/**
 * @brief 
 * 
 * @param from 
 * @param to 
 * @return int 
 */
int sfs_rename(const char* from, const char* to) {
	int ret = SFS_ERROR_NONE;
	boolean	is_find, is_root;
	struct sfs_dentry* from_dentry = sfs_lookup(from, &is_find, &is_root);
	struct sfs_inode*  from_inode;
	struct sfs_dentry* to_dentry;
	mode_t mode = 0;
	if (is_find == FALSE) {
		return -SFS_ERROR_NOTFOUND;
	}

	if (strcmp(from, to) == 0) {
		return SFS_ERROR_NONE;
	}

	from_inode = from_dentry->inode;
	
	if (SFS_IS_DIR(from_inode)) {
		mode = S_IFDIR;
	}
	else if (SFS_IS_REG(from_inode)) {
		mode = S_IFREG;
	}
	
	ret = sfs_mknod(to, mode, NULL);
	if (ret != SFS_ERROR_NONE) {					  /* 保证目的文件不存在 */
		return ret;
	}
	
	to_dentry = sfs_lookup(to, &is_find, &is_root);	  
	sfs_drop_inode(to_dentry->inode);				  /* 保证生成的inode被释放 */	
	to_dentry->ino = from_inode->ino;				  /* 指向新的inode */
	to_dentry->inode = from_inode;
	
	sfs_drop_dentry(from_dentry->parent->inode, from_dentry);
	return ret;
}
/**
 * @brief 
 * 
 * @param path 
 * @param fi 
 * @return int 
 */
int sfs_open(const char* path, struct fuse_file_info* fi) {
	return SFS_ERROR_NONE;
}
/**
 * @brief 
 * 
 * @param path 
 * @param fi 
 * @return int 
 */
int sfs_opendir(const char* path, struct fuse_file_info* fi) {
	return SFS_ERROR_NONE;
}
/**
 * @brief 
 * 
 * @param path 
 * @param type 
 * @return boolean 
 */
boolean sfs_access(const char* path, int type) {
	boolean	is_find, is_root;
	boolean is_access_ok = FALSE;
	struct sfs_dentry* dentry = sfs_lookup(path, &is_find, &is_root);
	struct sfs_inode*  inode;

	switch (type)
	{
	case R_OK:
		is_access_ok = TRUE;
		break;
	case F_OK:
		if (is_find) {
			is_access_ok = TRUE;
		}
		break;
	case W_OK:
		is_access_ok = TRUE;
		break;
	case X_OK:
		is_access_ok = TRUE;
		break;
	default:
		break;
	}
	return is_access_ok ? SFS_ERROR_NONE : -SFS_ERROR_ACCESS;
}	
/**
 * @brief 修改时间，为了不让touch报错
 * 
 * @param path 
 * @param tv 
 * @return int 
 */
int sfs_utimens(const char* path, const struct timespec tv[2]) {
	(void)path;
	return SFS_ERROR_NONE;
}
/**
 * @brief 
 * 
 * @param path 
 * @param offset 
 * @return int 
 */
int sfs_truncate(const char* path, off_t offset) {
	boolean	is_find, is_root;
	struct sfs_dentry* dentry = sfs_lookup(path, &is_find, &is_root);
	struct sfs_inode*  inode;
	
	if (is_find == FALSE) {
		return -SFS_ERROR_NOTFOUND;
	}
	
	inode = dentry->inode;

	if (SFS_IS_DIR(inode)) {
		return -SFS_ERROR_ISDIR;
	}

	inode->size = offset;

	return SFS_ERROR_NONE;
}
/**
 * @brief 展示sfs用法
 * 
 */
void sfs_usage() {
	printf("Sample File System (SFS)\n");
	printf("=================================================================\n");
	printf("Author: Deadpool <deadpoolmine@qq.com>\n");
	printf("Description: A Filesystem in UserSpacE (FUSE) sample file system \n");
	printf("\n");
	printf("Usage: ./sfs-fuse --device=[device path] mntpoint\n");
	printf("mount device to mntpoint with SFS\n");
	printf("=================================================================\n");
	printf("FUSE general options\n");
	return;
}
/******************************************************************************
* SECTION: FS Specific Structure
*******************************************************************************/
int main(int argc, char **argv)
{
    int ret;
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	sfs_options.device = strdup("/dev/ddriver");

	if (fuse_opt_parse(&args, &sfs_options, option_spec, NULL) == -1)
		return -SFS_ERROR_INVAL;
	
	if (sfs_options.show_help) {
		sfs_usage();
		fuse_opt_add_arg(&args, "--help");
		args.argv[0][0] = '\0';
	}
	
	ret = fuse_main(args.argc, args.argv, &operations, NULL);
	fuse_opt_free_args(&args);
	return ret;
}
