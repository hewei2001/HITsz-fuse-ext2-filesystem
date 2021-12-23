#ifndef _TYPES_H_
#define _TYPES_H_

/******************************************************************************
* SECTION: Type def
*******************************************************************************/
typedef int          boolean;
typedef uint16_t     flag16;

typedef enum nfs_file_type {
    NFS_REG_FILE,
    NFS_DIR
} NFS_FILE_TYPE;
/******************************************************************************
* SECTION: Macro
*******************************************************************************/
#define TRUE                    1
#define FALSE                   0
#define UINT32_BITS             32
#define UINT8_BITS              8

#define NFS_MAGIC_NUM           0x12345678  /* Define by yourself */
#define NFS_SUPER_OFS           0
#define NFS_ROOT_INO            0

/* 规定各区间占的 BLK 数 */
#define NFS_SUPER_BLKS          1
#define NFS_MAP_INODE_BLKS      1
#define NFS_MAP_DATA_BLKS       1
#define NFS_INODE_BLKS          512
#define NFS_DATA_BLKS           2048

#define NFS_ERROR_NONE          0
#define NFS_ERROR_ACCESS        EACCES
#define NFS_ERROR_SEEK          ESPIPE     
#define NFS_ERROR_ISDIR         EISDIR
#define NFS_ERROR_NOSPACE       ENOSPC
#define NFS_ERROR_EXISTS        EEXIST
#define NFS_ERROR_NOTFOUND      ENOENT
#define NFS_ERROR_UNSUPPORTED   ENXIO
#define NFS_ERROR_IO            EIO     /* Error Input/Output */
#define NFS_ERROR_INVAL         EINVAL  /* Invalid Args */

#define NFS_MAX_FILE_NAME       128
#define NFS_DATA_PER_FILE       4
#define NFS_DEFAULT_PERM        0777     /* 全权限打开 */

#define NFS_IOC_MAGIC           'S'
#define NFS_IOC_SEEK            _IO(NFS_IOC_MAGIC, 0)

#define NFS_FLAG_BUF_DIRTY      0x1
#define NFS_FLAG_BUF_OCCUPY     0x2

/******************************************************************************
* SECTION: Macro Function
*******************************************************************************/
#define NFS_IO_SZ()                     (nfs_super.sz_io)       /* 512B*/
#define NFS_BLK_SZ()                    (nfs_super.sz_blk)      /* 1024B*/
#define NFS_DISK_SZ()                   (nfs_super.sz_disk)     /* 4MB */
#define NFS_DRIVER()                    (nfs_super.driver_fd)

#define NFS_ROUND_DOWN(value, round)    ((value) % (round) == 0 ? (value) : ((value) / (round)) * (round))
#define NFS_ROUND_UP(value, round)      ((value) % (round) == 0 ? (value) : ((value) / (round) + 1) * (round))

#define NFS_BLKS_SZ(blks)               ((blks) * NFS_BLK_SZ())
#define NFS_ASSIGN_FNAME(pnfs_dentry, _fname) memcpy(pnfs_dentry->fname, _fname, strlen(_fname))
#define NFS_INO_OFS(ino)                (nfs_super.inode_offset + (ino) * NFS_BLK_SZ())
#define NFS_DATA_OFS(bno)               (nfs_super.data_offset + (bno) * NFS_BLK_SZ())

#define NFS_IS_DIR(pinode)              (pinode->dentry->ftype == NFS_DIR)
#define NFS_IS_REG(pinode)              (pinode->dentry->ftype == NFS_REG_FILE)
/******************************************************************************
* SECTION: FS Specific Structure - In memory structure
*******************************************************************************/
struct nfs_dentry;
struct nfs_inode;
struct nfs_super;

struct custom_options {
	const char*        device;
};

struct nfs_super {
    int                 driver_fd;          /* 打开的磁盘句柄 */

    int                 sz_io;              /* 512B */
    int                 sz_blk;             /* 1024B */
    int                 sz_disk;            /* 4MB */
    int                 sz_usage;           /* ioctl 相关信息 */

    int                 max_ino;
    int                 max_data;

    uint8_t*            map_inode;          /* 指向 inode 位图的内存起点 */ 
    int                 map_inode_blks;     /* inode 位图占用的块数 */
    int                 map_inode_offset;   /* inode 位图在磁盘上的偏移 */

    uint8_t*            map_data;           /* 指向 data 位图的内存起点 */ 
    int                 map_data_blks;      /* data 位图占用的块数 */
    int                 map_data_offset;    /* data 位图在磁盘上的偏移 */

    int                 inode_offset;       /* 索引结点的偏移 */
    int                 data_offset;        /* 数据块的偏移*/

    boolean             is_mounted;

    struct nfs_dentry*  root_dentry;
};

struct nfs_inode {
    uint32_t            ino;
    int                 size;                               /* 文件已占用空间 */
    int                 dir_cnt;
    struct nfs_dentry*  dentry;                             /* 指向该 inode 的 父dentry */
    struct nfs_dentry*  dentrys;                            /* 如果是 DIR，指向其所有子项（用链表串接）*/
    uint8_t*            block_pointer[NFS_DATA_PER_FILE];   /* 如果是 FILE，指向 4 个数据块，四倍结构 */
    int                 bno[NFS_DATA_PER_FILE];             /* 数据块在磁盘中的块号 */
};

struct nfs_dentry {
    char                fname[NFS_MAX_FILE_NAME];
    struct nfs_dentry*  parent;             /* 父亲 Inode 的 dentry */
    struct nfs_dentry*  brother;            /* 下一个兄弟 Inode 的 dentry */
    uint32_t            ino;
    struct nfs_inode*   inode;              /* 指向inode */
    NFS_FILE_TYPE       ftype;
};


static inline struct nfs_dentry* new_dentry(char * fname, NFS_FILE_TYPE ftype) {
    struct nfs_dentry * dentry = (struct nfs_dentry *)malloc(sizeof(struct nfs_dentry)); /* dentry 在内存空间也是随机分配 */
    memset(dentry, 0, sizeof(struct nfs_dentry));
    NFS_ASSIGN_FNAME(dentry, fname);
    dentry->ftype   = ftype;
    dentry->ino     = -1;
    dentry->inode   = NULL;
    dentry->parent  = NULL;
    dentry->brother = NULL;
    return dentry;                                            
}

/******************************************************************************
* SECTION: FS Specific Structure - Disk structure
*******************************************************************************/
struct nfs_super_d {
    uint32_t    magic_num;
    int         sz_usage;

    int         map_inode_blks;     /* inode 位图占用的块数 */
    int         map_inode_offset;   /* inode 位图在磁盘上的偏移 */

    int         map_data_blks;      /* data 位图占用的块数 */
    int         map_data_offset;    /* data 位图在磁盘上的偏移 */

    int         inode_offset;       /* 索引结点的偏移 */
    int         data_offset;        /* 数据块的偏移*/
};

struct nfs_inode_d {
    uint32_t        ino;            /* 在inode位图中的下标 */
    int             size;           /* 文件已占用空间 */
    int             dir_cnt;
    NFS_FILE_TYPE   ftype;  
    int             bno[NFS_DATA_PER_FILE];       /* 数据块在磁盘中的块号 */
};

struct nfs_dentry_d {
    char            fname[NFS_MAX_FILE_NAME];
    NFS_FILE_TYPE   ftype;
    uint32_t        ino;            /* 指向的 ino 号 */
};

#endif /* _TYPES_H_ */