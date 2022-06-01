#ifndef _GPIB_IOCTL_H
#define _GPIB_IOCTL_H

#include <asm/ioctl.h>

#define MAGIC_NUM 'T'

#define DAS_IOC_SET_PATH  \
	_IOC(_IOC_READ|_IOC_WRITE, MAGIC_NUM, 0, 4*sizeof(__u32))
#define DAS_IOC_SOFT_OUT  \
	_IOC(_IOC_READ|_IOC_WRITE, MAGIC_NUM, 1, sizeof(__u32))
#define DAS_IOC_GET_SBGIO  \
   	_IOC(_IOC_READ|_IOC_WRITE, MAGIC_NUM, 2, sizeof(__u32))
#define DAS_IOC_WDT_LED_CTRL  \
    	_IOC(_IOC_READ|_IOC_WRITE, MAGIC_NUM, 3, 2*sizeof(__u32))
#define DAS_IOC_WDT_CTRL  \
    	_IOC(_IOC_READ|_IOC_WRITE, MAGIC_NUM, 4, 2*sizeof(__u32))

#define DAS_IOC_GET_GPI \
    	_IOC(_IOC_READ|_IOC_WRITE, MAGIC_NUM, 5, sizeof(__u32))
#define DAS_IOC_SET_GPO \
    	_IOC(_IOC_READ|_IOC_WRITE, MAGIC_NUM, 6, sizeof(__u32))
#define DAS_IOC_GET_GPO \
    	_IOC(_IOC_READ|_IOC_WRITE, MAGIC_NUM, 7, sizeof(__u32))

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
#endif
