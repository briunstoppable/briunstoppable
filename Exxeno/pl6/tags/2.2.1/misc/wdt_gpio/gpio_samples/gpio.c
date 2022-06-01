#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include "matrix_wdt.h"
#include "conio.h"

int
readResult(void)
{
    unsigned short in_data;

    sleep(1);
    if (GPI_Read( &in_data) == 0)
    {
        if (in_data & 0x01)
        {
            printf("\tAdlink Power Supply #1 faulted\n");
        }
        else
        {
            printf("\tAdlink Power Supply #1 functional\n");
        }
        if (in_data & 0x02)
        {
            printf("\tAdlink Power Supply #2 faulted\n");
        }
        else
        {
            printf("\tAdlink Power Supply #2 functional\n");
        }
        if (in_data & 0x04)
        {
            printf("\tEthernet Power Supply #1 faulted\n");
        }
        else
        {
            printf("\tEthernet Power Supply #1 functional\n");
        }
        if (in_data & 0x08)
        {
            printf("\tEthernet Power Supply #2 faulted\n");
        }
        else
        {
            printf("\tEthernet Power Supply #2 functional\n");
        }
    }
    printf("Press return to continue tests.\n\n");
    getch();
    return 0;
}


int main(void)
{
    unsigned short test_pattern = 0;

    setbuf(stdout, NULL);
    clrscr();

    printf("Exxeno Comm Hub GPIO Tests\n\n");
	if( GPIO_Init()<0 ) 
 	{
	    printf("Failed to initialize GPIO Test.\n");
        printf("The mxe5300_wdt driver was not found.\n");
        printf("The operating system load is invalid.\n");
	    return 1;
 	}

    printf("All power supplies should be on:\n");
    test_pattern = (0);
    GPO_Write(test_pattern);
    readResult();

    printf("Ethernet power supplies should be off:\n");
    test_pattern = (0x01);
    GPO_Write(test_pattern);
    readResult();

    printf("All power supplies should be back on:\n");
    test_pattern = (0);
    GPO_Write(test_pattern);
    readResult();

    printf("\n");
	return 0;
}


