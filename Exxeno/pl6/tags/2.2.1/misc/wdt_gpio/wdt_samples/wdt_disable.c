#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/watchdog.h>
#include "matrix_wdt.h"

int main(void)
{
 	if(InitWDT()<0) 
 	{
	    printf("Failed to initialize Watchdog Timer\n");
	    return 1;
 	}
   
	StopWDT();  // Disable Watchdog Timer

	return 0;
}

