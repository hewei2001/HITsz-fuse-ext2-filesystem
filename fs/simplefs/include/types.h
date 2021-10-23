#ifndef _TYPES_H_
#define _TYPES_H_

/******************************************************************************
* SECTION: Type def
*******************************************************************************/
typedef int          boolean;
typedef uint16_t     flag16;

typedef enum sfs_file_type {
    SFS_REG_FILE,
    SFS_DIR
} SFS_FILE_TYPE;
/******************************************************************************
* SECTION: Macro
*******************************************************************************/
#define TRUE                    1
#define FALSE                   0
#define UINT32_BITS             32
#define UINT8_BITS              8

#define SFS_MAGIC_NUM           0x52415453  
#define SFS_SUPER_OFS           0
#define SFS_ROOT_INO            0



#define SFS_ERROR_NONE          0
#define SFS_ERROR_ACCESS        EACCES
#define SFS_ERROR_SEEK          ESPIPE     
#define SFS_ERROR_ISDIR         EISDIR
#define SFS_ERROR_NOSPACE       ENOSPC
#define SFS_ERROR_EXISTS        EEXIST
#define SFS_ERROR_NOTFOUND      ENOENT
#define SFS_ERROR_UNSUPPORTED   ENXIO
#define SFS_ERROR_IO            EIO     /* Error Input/Output */
#define SFS_ERROR_INVAL         EINVAL  /* Invalid Args */

#define SFS_MAX_FILE_NAME       128
#define SFS_INODE_PER_FILE      1
#define SFS_DATA_PER_FILE       16
#define SFS_DEFAULT_PERM        0777

#define SFS_IOC_MAGIC           'S'
#define SFS_IOC_SEEK            _IO(SFS_IOC_MAGIC, 0)

#define SFS_FLAG_BUF_DIRTY      0x1
#define SFS_FLAG_BUF_OCCUPY     0x2
/******************************************************************************
* SECTION: Macro Function
*******************************************************************************/
#define SFS_IO_SZ()                     (sfs_super.sz_io)
#define SFS_DISK_SZ()                   (sfs_super.sz_disk)
#define SFS_DRIVER()                    (sfs_super.driver_fd)

#define SFS_ROUND_DOWN(value, round)    (value % round == 0 ? value : (value / round) * round)
#define SFS_ROUND_UP(value, round)      (value % round == 0 ? value : (value / round + 1) * round)

#define SFS_BLKS_SZ(blks)               (blks * SFS_IO_SZ())
#define SFS_ASSIGN_FNAME(psfs_dentry, _fname)\ 
                                        memcpy(psfs_dentry->fname, _fname, strlen(_fname))
#define SFS_INO_OFS(ino)                (sfs_super.data_offset + ino * SFS_BLKS_SZ((\
                                        SFS_INODE_PER_FILE + SFS_DATA_PER_FILE)))
#define SFS_DATA_OFS(ino)               (SFS_INO_OFS(ino) + SFS_BLKS_SZ(SFS_INODE_PER_FILE))

#define SFS_IS_DIR(pinode)              (pinode->dentry->ftype == SFS_DIR)
#define SFS_IS_REG(pinode)              (pinode->dentry->ftype == SFS_REG_FILE)
/******************************************************************************
* SECTION: FS Specific Structure - In memory structure
*******************************************************************************/
struct sfs_dentry;
struct sfs_inode;
struct sfs_super;

struct custom_options {
	const char*        device;
	boolean            show_help;
};

struct sfs_inode
{
    int                ino;                           /* 在inode位图中的下标 */
    int                size;                          /* 文件已占用空间 */
    int                dir_cnt;
    struct sfs_dentry* dentry;                        /* 指向该inode的dentry */
    struct sfs_dentry* dentrys;                       /* 所有目录项 */
    uint8_t*           data;           
};  

struct sfs_dentry
{
    char               fname[SFS_MAX_FILE_NAME];
    struct sfs_dentry* parent;                        /* 父亲Inode的dentry */
    struct sfs_dentry* brother;                       /* 兄弟 */
    int                ino;
    struct sfs_inode*  inode;                         /* 指向inode */
    SFS_FILE_TYPE      ftype;
};

struct sfs_super
{
    int                driver_fd;
    
    int                sz_io;
    int                sz_disk;
    int                sz_usage;
    
    int                max_ino;
    uint8_t*           map_inode;
    int                map_inode_blks;
    int                map_inode_offset;
    
    int                data_offset;

    boolean            is_mounted;

    struct sfs_dentry* root_dentry;
};

static inline struct sfs_dentry* new_dentry(char * fname, SFS_FILE_TYPE ftype) {
    struct sfs_dentry * dentry = (struct sfs_dentry *)malloc(sizeof(struct sfs_dentry));
    memset(dentry, 0, sizeof(struct sfs_dentry));
    SFS_ASSIGN_FNAME(dentry, fname);
    dentry->ftype   = ftype;
    dentry->ino     = -1;
    dentry->inode   = NULL;
    dentry->parent  = NULL;
    dentry->brother = NULL;                                            
}
/******************************************************************************
* SECTION: FS Specific Structure - Disk structure
*******************************************************************************/
struct sfs_super_d
{
    uint32_t           magic_num;
    int                sz_usage;
    
    int                max_ino;
    int                map_inode_blks;
    int                map_inode_offset;
    int                data_offset;
};

struct sfs_inode_d
{
    int                ino;                           /* 在inode位图中的下标 */
    int                size;                          /* 文件已占用空间 */
    int                dir_cnt;
    SFS_FILE_TYPE      ftype;   
};  

struct sfs_dentry_d
{
    char               fname[SFS_MAX_FILE_NAME];
    SFS_FILE_TYPE      ftype;
    int                ino;                           /* 指向的ino号 */
};  


#endif /* _TYPES_H_ */