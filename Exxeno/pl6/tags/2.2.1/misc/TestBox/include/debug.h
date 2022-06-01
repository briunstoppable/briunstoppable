// -------------------------------------------------------------------------
/*!
    Exxeno Hub
    \file       debug.h
    Contains the Exxeno program debug functions.
*/
// ------------------------- CONFIDENTIAL ----------------------------------
// 
//              Copyright (c) 2014 by Exxeno, LLC.    
//                       All rights reserved. 
// 
// Trade Secret of Exxeno, LLC.  Do not disclose. 
// 
// Use of this source code in any form or means is permitted only 
// with a valid, written license agreement with Exxeno, Inc. 
// 
// Licensee shall keep all information contained herein confidential  
// and shall protect same in whole or in part from disclosure and  
// dissemination to all third parties. 
// 
// 
// Exxeno, LLC. 
// Hillsboro, OR 97124, USA 
//   
// 
// -------------------------------------------------------------------------

// 	Portions copyright Pro Code Works, LLC.

extern  int gMsgLevel;

//#if DBG
#define LOG_BASE        0xFFFF

#ifdef USE_DEBUG_MSG
// Debug version (Messages to console)
#define DEBUGP(level, ...) \
{\
    if (level <= gMsgLevel) \
    { \
        printf("TestBox:"); \
        printf( __VA_ARGS__); \
        printf("\n"); \
    } \
} 

#else // Not USE_DEBUG_MSG

#ifdef USE_SYSLOG

// Release version
#define DEBUGP(level, ...) \
{\
	syslog(level, "Exxeno:"); \
    syslog(level, __VA_ARGS__); \
	syslog(level, "\n"); \
} 

#else

//#define LOG_EMERG       0xFFFF
//#define LOG_ALERT       0xFFFE
//#define LOG_CRIT        0xFFFD
//#define LOG_ERR         0xFFFC
//#define LOG_WARNING     0xFFFB
//#define LOG_NOTICE      0xFFFA
//#define LOG_INFO        0xFFF9
//#define LOG_DEBUG       0xFFF8
// Codes 0xFFF0 - 0xFFF7 are reserved for future use

#define DEBUGP(level, ...) \
{\
    if (level <= gMsgLevel) \
    { \
        sprintf(pLogInfo, __VA_ARGS__); \
        logMessages(level, pLogInfo); \
    } \
} 

#endif // USE_SYSLOG
#endif // USE_DEBUG_MSG
