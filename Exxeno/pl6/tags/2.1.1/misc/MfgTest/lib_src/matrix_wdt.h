/****************************************************************************/
/*  Copyright (c) 1995-2008, ADLink Technology Inc.  All rights reserved.   */
/*                                                                          */
/*  File Name   :   PXI3900_WDT.H                                           */
/*  Purpose     :   Header file for ADLINK PXI3900_WDT                      */
/*  Date        :   07/01/2010                                              */
/*  Revision    :   2.00                                                    */
/****************************************************************************/
#ifndef ADLINK_MATRIX_WDT_API
#define ADLINK_MATRIX_WDT_API

#ifdef __cplusplus
extern "C" {
#endif

#define clrscr()	printf("\033[H\033[J")

/****************************************************************************/
/*      Function  Declerations                                              */
/****************************************************************************/
short InitWDT(void);
short StartWDT(void);
short StopWDT(void);
short SetWDT(unsigned char tick, unsigned char unit);
short ResetWDT(void);

short GPIO_Init(void);
short GPI_Read(unsigned short* pwState);
short GPO_Write(unsigned short wState);
short GPO_Read(unsigned short* pwState);

#ifdef __cplusplus
}
#endif


#endif // ADLINK_MATRIX_WDT_API
