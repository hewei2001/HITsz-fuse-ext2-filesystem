#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include "ddriver_ctl.h"
/******************************************************************************
* SECTION: Macro definitions
*******************************************************************************/
#define DEVICE_NAME   "ddriver"
#define kernel_info(fmt, ...)                                           \
	do {                                                                \
		printk(KERN_INFO DEVICE_NAME " " fmt "\n", ##__VA_ARGS__);      \
	} while(0)                                                          \

#define kernel_alert(fmt, ...)                                          \
	do {                                                                \
		printk(KERN_ALERT DEVICE_NAME " " fmt "\n", ##__VA_ARGS__);     \
	} while(0)                                                          \

#define DRIVER_AUTHOR   "Deadpool <deadpoolmine@qq.com>"
#define DRIVER_DESC     "A Fake disk driver, referring to <https://blog.sourcere"\
                        "r.io/writing-a-simple-linux-kernel-module-d9dc3762c234>"\
                        ".Note we use filp_open to read or write the fake disk, "\
                        "referring to <https://cpp.hotexamples.com/examples/-/-/"\
                        "filp_open/cpp-filp_open-function-examples.html>"
#define DRIVER_VERSION  "0.1.0"

#define CONFIG_DISK_SZ  (4 * 1024 * 1024)
#define CONFIG_BLOCK_SZ (512)
/******************************************************************************
* SECTION: Macro Functions 
*******************************************************************************/
#define IGNORE_ARG(arg)         ((void)arg)
#define IS_ADDR_ALIGN(addr)     (addr % CONFIG_BLOCK_SZ == 0)
#define ADDR_ROUND_UP(addr)     ((addr / CONFIG_BLOCK_SZ) * CONFIG_BLOCK_SZ)

#define GET_HEAD_POS(disk)      (disk.head - disk.layout)
#define FORWARD_HEAD(disk, dis) (disk.head += dis)
#define SET_HEAD(disk, ofs)     (disk.head = disk.layout + ofs)
#define RESET_HEAD(disk)        (SET_HEAD(disk, 0))

#define INC_READCNT(disk)       (disk.read_cnt++)
#define INC_WRITECNT(disk)      (disk.write_cnt++)
#define INC_SEEKCNT(disk)       (disk.seek_cnt++)
/******************************************************************************
* SECTION: Kernel Module Template
*******************************************************************************/
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);	    
MODULE_DESCRIPTION(DRIVER_DESC);	
MODULE_VERSION(DRIVER_VERSION);	
/******************************************************************************
* SECTION: Type definitions
*******************************************************************************/
struct ddriver
{
    char layout[CONFIG_DISK_SZ];                      /* Disk Layout */
    char *head;                                       /* Disk Head */
    int  read_cnt;
    int  write_cnt;
    int  seek_cnt;
    int  major_num;
    int  open_count;
    int  layout_size;
    int  iounit_size;
};

static struct ddriver disk = {
    .head        = NULL,
    .read_cnt    = 0,
    .write_cnt   = 0,
    .seek_cnt    = 0,
    .major_num   = 0,
    .open_count  = 0,
    .layout_size = CONFIG_DISK_SZ,
    .iounit_size = CONFIG_BLOCK_SZ
};
/******************************************************************************
* SECTION: Helper Functions
*******************************************************************************/
int check_valid(size_t size){
    if (GET_HEAD_POS(disk) >=  CONFIG_DISK_SZ) {
        kernel_alert("disk head reach the end");
        return -EINVAL;
    }
    if (size != CONFIG_BLOCK_SZ){
        kernel_alert("io size %ld should align to %d", size, CONFIG_BLOCK_SZ);
        return -EIO;
    }
    return 0;
}
/******************************************************************************
* SECTION: Function definitions
*******************************************************************************/
static int      device_open(struct inode *, struct file *);
static int      device_release(struct inode *, struct file *);
static ssize_t  device_read(struct file *, char *, size_t, loff_t *);
static ssize_t  device_write(struct file *, const char *, size_t, loff_t *);
static loff_t   device_seek(struct file *, loff_t, int);
static long     device_ioctl(struct file *, unsigned int, unsigned long);
/******************************************************************************
* SECTION: Global var or structure definitions
*******************************************************************************/
static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .llseek = device_seek,
    .unlocked_ioctl = device_ioctl,
    .release = device_release
};
/******************************************************************************
* SECTION: Function Implementation
*******************************************************************************/
/**
 * @brief Disk Read
 * 
 * @param file          Ignored
 * @param user_buffer   User space buffer
 * @param size          Must equal to Blocksize @CONFIG_BLOCK_SZ
 * @param offset        Ignored
 * @return ssize_t      Bytes have been read 
 */
static ssize_t 
device_read(struct file *file, char *user_buffer, size_t size, loff_t *offset) {
    IGNORE_ARG(offset);
    IGNORE_ARG(file);
    int res = check_valid(size);
    if(res < 0)
        return res;
    if (copy_to_user(user_buffer, disk.head, CONFIG_BLOCK_SZ))
        return -EFAULT;
    FORWARD_HEAD(disk, CONFIG_BLOCK_SZ);
    INC_READCNT(disk);
    return CONFIG_BLOCK_SZ;
}
/**
 * @brief Disk Write
 * 
 * @param file          Ignored
 * @param user_buffer   User space buffer, copy content from
 * @param size          Must equal to Blocksize @CONFIG_BLOCK_SZ
 * @param offset        Ignored
 * @return ssize_t      Bytes have been written
 */
static ssize_t 
device_write(struct file *file, const char *user_buffer, size_t size, loff_t *offset) {
    IGNORE_ARG(offset);
    IGNORE_ARG(file);
    int res = check_valid(size);
    if(res < 0)
        return res;

    if (copy_from_user(disk.head, user_buffer, CONFIG_BLOCK_SZ))
        return -EFAULT;
    FORWARD_HEAD(disk, CONFIG_BLOCK_SZ);
    INC_WRITECNT(disk);
    return CONFIG_BLOCK_SZ;
}
/**
 * @brief Disk Seek
 * 
 * @param file          Ignored
 * @param offset        Aligned to @CONFIG_BLOCK_SZ
 * @param whence        SEEK_CUR, SEEK_SET
 * @return loff_t       cur pos
 */
static loff_t 
device_seek(struct file *file, loff_t offset, int whence) {
    IGNORE_ARG(file);
    if (!IS_ADDR_ALIGN(offset)) {
        kernel_alert("offset %lld must be aligned to block size %d", 
                      offset, CONFIG_BLOCK_SZ);
        return -EINVAL;
    }
    switch (whence)
    {
    case SEEK_SET:
        SET_HEAD(disk, offset);
        break;
    case SEEK_CUR:
        FORWARD_HEAD(disk, offset);
        break;
    default:
        break;
    }
    INC_SEEKCNT(disk);
    return GET_HEAD_POS(disk);
}
/**
 * @brief Disk ioctl
 * 
 * @param file          Ignored
 * @param cmd           Command
 * @param arg           Args
 * @return long         State
 */
static long 
device_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
    IGNORE_ARG(file);
    int ret;
    struct ddriver_state state;
    switch (cmd)
    {
    case IOC_REQ_DEVICE_SIZE:                         /* Device Size */
        ret = copy_to_user((int __user *)arg, &disk.layout_size, sizeof(int));
        if (ret) 
            return -EFAULT;
        break;
    case IOC_REQ_DEVICE_STATE:                        /* Device State */
        state.read_cnt = disk.read_cnt;
        state.write_cnt = disk.write_cnt;
        state.seek_cnt = disk.seek_cnt;
        ret = copy_to_user((int __user *)arg, &state, sizeof(struct ddriver_state));
        if (ret) 
            return -EFAULT;
        break;
    case IOC_REQ_DEVICE_RESET:                        /* Reset Device */
        disk.head = disk.layout;
        disk.read_cnt = 0;
        disk.write_cnt = 0;
        disk.seek_cnt = 0;
        break;
    case IOC_REQ_DEVICE_IO_SZ:
        ret = copy_to_user((int __user *)arg, &disk.iounit_size, sizeof(int));
        if (ret) 
            return -EFAULT;
        break;
    default:
        break;
    }
    return 0;
}
/**
 * @brief Disk Open
 * 
 * @param inode         Ignored
 * @param file          Ignored
 * @return int          state
 */
static int 
device_open(struct inode *inode, struct file *file) {
    IGNORE_ARG(inode);
    IGNORE_ARG(file);
    
    if (disk.open_count) {                            /* If device is open, return busy */
        return -EBUSY;
    }
    RESET_HEAD(disk);                                 /* Everytime close device, reset head */
    disk.open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}
/**
 * @brief Disk Close
 * 
 * @param inode         Ignored
 * @param file          Ignored
 * @return int          state
 */
static int 
device_release(struct inode *inode, struct file *file) {
                                                      /* Decrement the open counter and usage count. 
                                                         Without this, the module would not unload. */
    IGNORE_ARG(inode);
    IGNORE_ARG(file);
    disk.open_count--;
    module_put(THIS_MODULE);
    return 0;
}
/******************************************************************************
* SECTION: Module Register and Unregister
*******************************************************************************/
static int __init 
ddriver_init(void)
{
    int major_num = register_chrdev(0, DEVICE_NAME, &file_ops);   
                                                      /* Register an device */
    if (major_num < 0) {                              /* Register fail */
        kernel_alert("Can't register device, ret %d", major_num);
        return major_num;
    } 
    else {                                            /* Register success */                                                  
        kernel_info("module loaded with device major number %d", major_num);
        disk.major_num = major_num;
        memset(disk.layout, 0, CONFIG_DISK_SZ);
        return 0;
    }
    return 0;
}

static void __exit 
ddriver_exit(void)
{   
    int major_num = disk.major_num;
    kernel_info("Goodbye %d", major_num);
    if(major_num != 0){
        unregister_chrdev(major_num, DEVICE_NAME);
    }
}

module_init(ddriver_init);
module_exit(ddriver_exit);