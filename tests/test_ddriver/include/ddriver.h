#ifndef _DDRIVER_H_
#define _DDRIVER_H_

#include "ddriver_ctl_user.h"
#include "stdio.h"

int ddriver_open(char *path);
int ddriver_seek(int fd, off_t offset, int whence);
int ddriver_write(int fd, char *buf, size_t size);
int ddriver_read(int fd, char *buf, size_t size);
int ddriver_ioctl(int fd, unsigned long cmd, void *ret);
int ddriver_close(int fd);

#endif /* _DDRIVER_H_ */