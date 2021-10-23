#ifndef _DDRIVER_H_
#define _DDRIVER_H_

#include "ddriver_ctl_user.h"
#include "stdio.h"

/**
 * @brief 打开ddriver设备
 * 
 * @param path ddriver设备路径
 * @return int 0成功，否则失败
 */
int ddriver_open(char *path);

/**
 * @brief 移动ddriver磁盘头
 * 
 * @param fd ddriver设备handler
 * @param offset 移动到的位置，注意要和设备IO单位对齐
 * @param whence SEEK_SET即可
 * @return int 0成功，否则失败
 */
int ddriver_seek(int fd, off_t offset, int whence);

/**
 * @brief 写入数据
 * 
 * @param fd ddriver设备handler
 * @param buf 要写入的数据Buf
 * @param size 要写入的数据大小，注意一定要等于单次设备IO单位
 * @return int 0成功，否则失败
 */
int ddriver_write(int fd, char *buf, size_t size);

/**
 * @brief 读出数据
 * 
 * @param fd ddriver设备handler
 * @param buf 要读出的数据Buf
 * @param size 要读出的数据大小，注意一定要等于单次设备IO单位
 * @return int 
 */
int ddriver_read(int fd, char *buf, size_t size);

/**
 * @brief ddriver IO控制
 * 
 * @param fd ddriver设备handler
 * @param cmd 命令号，查看ddriver_ctl_user，IOC_开头
 * @param ret 返回值
 * @return int 0成功，否则失败
 */
int ddriver_ioctl(int fd, unsigned long cmd, void *ret);

/**
 * @brief 关闭ddriver设备
 * 
 * @param fd ddriver设备handler
 * @return int 0成功，否则失败
 */
int ddriver_close(int fd);

#endif /* _DDRIVER_H_ */