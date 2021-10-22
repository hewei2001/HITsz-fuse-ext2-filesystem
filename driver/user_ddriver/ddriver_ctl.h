#ifndef _DDRIVER_CTL_H_ 
#define _DDRIVER_CTL_H_

#include <sys/ioctl.h>   
/******************************************************************************
* SECTION: IO ctl protocol definitions
*******************************************************************************/
#define IOC_MAGIC               'A'

struct ddriver_state
{
    int write_cnt;
    int read_cnt;
    int seek_cnt;
};

#define IOC_REQ_DEVICE_SIZE     _IOR(IOC_MAGIC, 0, int)
#define IOC_REQ_DEVICE_STATE    _IOR(IOC_MAGIC, 1, struct ddriver_state)
#define IOC_REQ_DEVICE_RESET    _IO(IOC_MAGIC, 2)
#define IOC_REQ_DEVICE_IO_SZ    _IOR(IOC_MAGIC, 3, int)
#endif