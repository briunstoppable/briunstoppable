#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include "matrix_wdt.h"

int main(void)
{
	int i;

 	if( InitWDT()<0 ) 
 	{
	    printf("Failed to initialize Watchdog Timer\n");
	    return 1;
 	}
   
	SetWDT(50, 0);  // Set timeout 50 seconds
	//SetWDT(3, 1);  // Set timeout 3 minites

	StartWDT();     // Enable Watchdog Timer
	
	for( i=0; i<30; i++ )
	{	
	    ResetWDT();  // Reflesh the watchdog timer
	    sleep( 2 );
	    printf(".");
	    fflush(stdout);
	}
	printf("\n");
	
	StopWDT();  // Disable Watchdog Timer

	return 0;
}

