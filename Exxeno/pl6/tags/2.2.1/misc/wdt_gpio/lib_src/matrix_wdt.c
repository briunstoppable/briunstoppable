#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/user.h>
#include <asm/types.h>
#include <linux/watchdog.h>

#include "pciioctl.h"
#include "matrix_wdt.h"

#define MAX_PCI_CARDS 	  1 

typedef struct _Card_Info_Type
{
   unsigned short initFlag;
   unsigned short cardType;
   int hDev;
} Card_Info_Type;


Card_Info_Type card_Info[MAX_PCI_CARDS] = { {0, 0, -1} };

short InitWDT(void)
{
    int fd;
    char driverStr[256];
    unsigned short dev_number = 0;
    int value;

    if(!card_Info[dev_number].initFlag) 
    {
       // sprintf(driverStr,"/dev/watchdog");
       sprintf(driverStr,"/dev/matrix");

       //Open the device driver
       fd = open( driverStr, O_RDWR );
       if( fd < 0 ) 
       {
           printf("open: %s failed!\n", driverStr);
           return -1;
       }
       card_Info[dev_number].hDev=fd;
       card_Info[dev_number].initFlag=1;
   }
   else
   {
       fd = card_Info[dev_number].hDev;
   }
   value=60;  // change the timeout-unit to 60 seconds
   if ( ioctl(fd, WDIOC_SETTIMEOUT, &value ) < 0 )  
   {	
       printf("WDT doesn't support WDIOC_SETOPTIONS\n");
       return -1;
   }
   return 0;
}

/****************************************************************************/
/* Function: _SetSOFT                                           */
short SetWDT(unsigned char tick, unsigned char unit)
{
  int fd;
  unsigned short wCardNumber = 0;
  
  int value;
    
  if(!card_Info[wCardNumber].initFlag) 
        return -1;    
 
  if( unit == 1 ) 
  	value = (int)( 0x100 | tick);
  else if( unit == 0 )
  	value = (int)( tick );
  else  return -1;
  	
  fd = card_Info[wCardNumber].hDev;  	
	  
  if ( ioctl(fd, WDIOC_SETTIMEOUT, &value ) < 0 )  
  {	
        printf("WDT doesn't support WDIOC_SETTIMEOUT\n");
        return -1;
  }	
	
   return 0;
}

short StartWDT(void)
{
  int fd;    
  unsigned short wCardNumber = 0;
  int io_setting = WDIOS_ENABLECARD; 
    
  if(!card_Info[wCardNumber].initFlag) 
        return -1;
        
	fd = card_Info[wCardNumber].hDev;
  if ( ioctl(fd, WDIOC_SETOPTIONS, &io_setting) < 0 )
  {	
        printf("WDT doesn't support WDIOS_ENABLECARD\n");
        return -1;
  }	
       
   return 0;
}

short StopWDT(void)
{
  int fd;    
  unsigned short wCardNumber = 0;
  int io_setting = WDIOS_DISABLECARD; 
    
  if(!card_Info[wCardNumber].initFlag) 
        return -1;
        
  fd = card_Info[wCardNumber].hDev;
  if ( ioctl(fd, WDIOC_SETOPTIONS, &io_setting) < 0 )
  {	
        printf("WDT doesn't support WDIOS_DISABLECARD\n");
        return -1;
  }	
       
  return 0;
}

short ResetWDT(void)
{
  int fd;    
  unsigned short wCardNumber = 0;
    
  if(!card_Info[wCardNumber].initFlag) 
        return -1;
        
  fd = card_Info[wCardNumber].hDev;
  if ( ioctl(fd, WDIOC_KEEPALIVE) < 0 )
  {	
        printf("WDT doesn't support WDIOC_KEEPALIVE\n");
        return -1;
  }	
       
	return 0;
}

short GPIO_Init(void)
{
    int fd;
    char driverStr[256];
    unsigned short dev_number = 0;
 
    if(!card_Info[dev_number].initFlag) 
    {
        // sprintf(driverStr,"/dev/watchdog");
        sprintf(driverStr,"/dev/matrix");
        //Open the device driver
        fd = open( driverStr, O_RDWR );
        if( fd < 0 ) 
        {
            printf("open: %s failed!\n", driverStr);
            return -1;
        }
  
        card_Info[dev_number].hDev=fd;
        card_Info[dev_number].initFlag=1;
    }
    else
    {
        fd = card_Info[dev_number].hDev;
    }
  return 0;
}

short GPI_Read(unsigned short* pwState)
{	

  int fd;    
  unsigned short wCardNumber = 0;
  unsigned long  settings;
    
  if(!card_Info[wCardNumber].initFlag) 
        return -1;
        
   fd = card_Info[wCardNumber].hDev;

  if ( ioctl(fd, DAS_IOC_GET_GPI, &settings) < 0 )
  {	
        printf("DAS_IOC_GET_GPI Error\n");
        return -1;
  }	

 *pwState = settings;

  return 0;
	
}

short GPO_Write(unsigned short wState)
{

  int fd;    
  unsigned short wCardNumber = 0;
  unsigned long  settings;
     
  if(!card_Info[wCardNumber].initFlag) 
        return -1;
        
   fd = card_Info[wCardNumber].hDev;
  settings = wState;

  if ( ioctl(fd, DAS_IOC_SET_GPO, &settings) < 0 )
  {	
        printf("DAS_IOC_SET_GPO Error\n");
        return -1;
  }	
				
  return 0;
	
}

short GPO_Read(unsigned short* pwState)
{	

  int fd;    
  unsigned short wCardNumber = 0;
  unsigned long  settings;
    
  if(!card_Info[wCardNumber].initFlag) 
        return -1;
        
   fd = card_Info[wCardNumber].hDev;

  if ( ioctl(fd, DAS_IOC_GET_GPO, &settings) < 0 )
  {	
        printf("DAS_IOC_GET_GPO Error\n");
        return -1;
  }	

 *pwState = settings;

  return 0;
	
}
