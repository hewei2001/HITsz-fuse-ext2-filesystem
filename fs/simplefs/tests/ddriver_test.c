#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include "fcntl.h"
#include <sys/stat.h>
#include "string.h"
#include "../include/ddriver_ctl_user.h"
#include "fuse.h"

int main(int argc, char const *argv[])
{
    int fd = open("/dev/ddriver", O_RDWR);
    int size;
    struct ddriver_state state;

    /* Cycle 1: read/write test */
    char buffer[512]={'a'};
    char rbuffer[512];
    buffer[511] = '\0';
    if (fd < 0){
        return fd;
    }
    write(fd, buffer, 512);
    lseek(fd, 0, SEEK_SET);
    read(fd, rbuffer, 512);
    printf("%s\n", rbuffer);

    /* Cycle 2: ioctl test - return int */
    ioctl(fd, IOC_REQ_DEVICE_SIZE, &size);
    printf("%d\n", size);

    /* Cycle 3: ioctl test - return struct */
    ioctl(fd, IOC_REQ_DEVICE_STATE, &state);
    printf("read_cnt: %d\n", state.read_cnt);
    printf("write_cnt: %d\n", state.write_cnt);
    printf("seek_cnt: %d\n", state.seek_cnt);

    /* Cycle 4: ioctl test - re-init device */
    ioctl(fd, IOC_REQ_DEVICE_RESET, &size);

    ioctl(fd, IOC_REQ_DEVICE_SIZE, &size);
    printf("%d\n", size);

    ioctl(fd, IOC_REQ_DEVICE_STATE, &state);
    printf("read_cnt: %d\n", state.read_cnt);
    printf("write_cnt: %d\n", state.write_cnt);
    printf("seek_cnt: %d\n", state.seek_cnt);
    close(fd);
    return 0;
}
