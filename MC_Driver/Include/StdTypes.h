// -------------------------------------------------------------------------
// 
// PRODUCT:			DMA Driver
// MODULE NAME:		StdTypes.h
// 
// MODULE DESCRIPTION: 
// 
// Contains standard c extension defines and typedefs.
// 
// $Revision:  $
//
// ------------------------- CONFIDENTIAL ----------------------------------
// 
//              Copyright (c) 2015 by Northwest Logic, Inc.   
//                       All rights reserved. 
// 
// Trade Secret of Northwest Logic, Inc.  Do not disclose. 
// 
// Use of this source code in any form or means is permitted only 
// with a valid, written license agreement with Northwest Logic, Inc. 
// 
// Licensee shall keep all information contained herein confidential  
// and shall protect same in whole or in part from disclosure and  
// dissemination to all third parties. 
// 
// 
//                        Northwest Logic, Inc. 
//                  1100 NW Compton Drive, Suite 100 
//                      Beaverton, OR 97006, USA 
//   
//                        Ph:  +1 503 533 5800 
//                        Fax: +1 503 533 5900 
//                      E-Mail: info@nwlogic.com 
//                           www.nwlogic.com 
// 
// -------------------------------------------------------------------------

/*! @file StdTypes.h
*/

#ifndef __STANDARD_TYPES__h_
#define __STANDARD_TYPES__h_

//#include <stdint.h>

// Unsigned-Integer Values
typedef unsigned long long		UINT64;
typedef unsigned int			UINT32;
typedef unsigned short			UINT16;
typedef unsigned char			UINT8;

// Pointer-to-Unsigned-Integer Values
typedef unsigned long long *	PUINT64;
typedef unsigned int *			PUINT32;
typedef unsigned short *		PUINT16;
typedef unsigned char *			PUINT8;

#ifndef PVOID
typedef void *              PVOID;
#endif // PVOID

#ifndef VOID
typedef void				VOID;
#endif // VOID

#ifndef TRUE
# define TRUE               1
#define true				TRUE
# define FALSE              0
#define false				FALSE
#endif /* TRUE */

#ifndef DWORD
typedef unsigned long       DWORD;
#endif // DWORD

// Signed-Integer Values
#ifndef INT16
typedef short				INT16;
#endif // INT16

#ifndef INT32
typedef int					INT32;
#endif // INT32

#ifndef INT64
typedef long long    		INT64;
#endif // INT64

#ifndef LONG
typedef long                LONG;
#endif // LONG

#ifndef LONGLONG
typedef long long			LONGLONG;
#endif

// Pointer-to-Signed-Integers
#ifndef PINT16 
typedef short *				PINT16;
#endif // INT16

#ifndef PINT32
typedef int	*				PINT32;
#endif // INT32

#ifndef PINT64
typedef long long *    		PINT64;
#endif // INT64

#ifndef PLONG
typedef long *               PLONG;
#endif // LONG


#ifndef SIZE_T
//#define SIZE_T(arg)       ((size_t)(arg))
#define SIZE_T              size_t
//#typedef size_t 			SIZE_T;
#endif // SIZE_T

#ifndef WIN32

#ifndef boolean_t
typedef unsigned char       BOOLEAN;
#endif // boolean_t

// Signed-Integer Values
#ifndef INT8
typedef char				INT8;
#endif // INT8

// Pointer-to-Signed-Integers
#ifndef PINT8
typedef char *				PINT8;
#endif // INT8

// Many operating system calls still require a char instead of a byte, 
// or an 8-bit integer. While the types aren't truly different, the capitalized
// typedef convention is used throughout the Expresso PCIe Solution
#ifndef CHAR
typedef char				CHAR;
#endif

#ifndef PCHAR
typedef char *				PCHAR;
#endif // PCHAR

#ifndef PUCHAR
typedef unsigned char *		PUCHAR;
#endif // PUCHAR

#ifndef HANDLE
typedef int					HANDLE;
#endif

#ifndef FLOAT
typedef float				FLOAT;
#endif

// Redefine CloseHandle (Windows) to close (Linux).
#ifndef CloseHandle
#define CloseHandle(a)		close(a)
#endif

// Redefine OVERLAPPED (Windows) to close (Linux)

#ifndef OVERLAPPED
typedef off_t				OVERLAPPED;
#endif

#ifndef Sleep
#define Sleep(a)            usleep(a * 1000)
#endif // Sleep

#ifndef SleepEx
#define SleepEx(a, b)       usleep(a * 1000)
#endif // SleepEx

#ifndef LPTHREAD_START_ROUTINE
typedef void *(*LPTHREAD_START_ROUTINE)(void *);
#endif // LPTHREAD_START_ROUTINE

#define	INVALID_HANDLE_VALUE		-1

#endif // Not Windows.

#endif /* !defined(__STANDARD_TYPES_h__) */

