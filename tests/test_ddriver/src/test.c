#include "../include/ddriver.h"
#include <linux/fs.h>

int main(int argc, char const *argv[])
{
    int size;
    struct ddriver_state state;
    int fd = ddriver_open("/root/ddriver");
    if (fd < 0) {
        return -1;
    }
    /* Cycle 1: read/write test */
    char buffer[512]={'a'};
    char rbuffer[512];
    buffer[511] = '\0';
    if (fd < 0){
        return fd;
    }
    ddriver_write(fd, buffer, 512);
    ddriver_seek(fd, 0, SEEK_SET);
    ddriver_read(fd, rbuffer, 512);
    printf("%s\n", rbuffer);

    /* Cycle 2: ioctl test - return int */
    ddriver_ioctl(fd, IOC_REQ_DEVICE_SIZE, &size);
    printf("%d\n", size);

    /* Cycle 3: ioctl test - return struct */
    ddriver_ioctl(fd, IOC_REQ_DEVICE_STATE, &state);
    printf("read_cnt: %d\n", state.read_cnt);
    printf("write_cnt: %d\n", state.write_cnt);
    printf("seek_cnt: %d\n", state.seek_cnt);

    /* Cycle 4: ioctl test - re-init device */
    ddriver_ioctl(fd, IOC_REQ_DEVICE_RESET, &size);

    ddriver_ioctl(fd, IOC_REQ_DEVICE_SIZE, &size);
    printf("%d\n", size);

    ddriver_ioctl(fd, IOC_REQ_DEVICE_STATE, &state);
    printf("read_cnt: %d\n", state.read_cnt);
    printf("write_cnt: %d\n", state.write_cnt);
    printf("seek_cnt: %d\n", state.seek_cnt);

    ddriver_close(fd);
    return 0;
}
