// -------------------------------------------------------------------------
/*!
    Exxeno Hub - ExxComm
    \file       ExxComm.h
    Contains the Exxeno communication hub defines, structs, includes, etc.
*/
// ------------------------- CONFIDENTIAL ----------------------------------
//
//              Copyright (c) 2017 by Exxeno, LLC.
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

#ifndef __INTERROGATOR_H_
#define __INTERROGATOR_H_


#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include "../include/Exxeno.h"
#include "../include/debug.h"
#include "../include/curl.h"
#include "../include/cJSON.h"

#define E2_MAX_RECORDS             9999
#define E2_MAX_SHORTNAME_LENGTH	   14

#define E2_MAX_CELL_TYPES          320

#define E2_NAME_SIZE               128
#define E2_MAX_STR_SIZE            128
#define E2_MAX_LONG_STR_SIZE       42
#define E2_MAX_CELL_ENTRIES        2048
#define E2_MAX_CELLTYPE_ENTRIES    480

#define LOG_BUFFER_SIZE            500
#define DEBUG_LINE_LEN             120

#define MAX_VERSION_CHARS          10
#define MAX_HEADER_BUF_SIZE        MAX_VERSION_CHARS + 20

#define BEGIN_CRITICAL(x)          pthread_mutex_lock(x);
#define END_CRITICAL(x)            pthread_mutex_unlock(x);

#define TRUE  1
#define FALSE 0

/*! \brief threeWay */
typedef enum {
    TW_FALSE,
    TW_TRUE,
    TW_NOT_SET
} threeWay;

typedef struct _USER_DEFINED_CONFIG
{
	char				confFileName;
	int					cellTypeQueryList[E2_MAX_CELL_TYPES];
	int					instanceQueryList[E2_MAX_CELLTYPE_ENTRIES];
	int					regIntegers[E2_MAX_RECORDS];
	char				regShortNames[E2_MAX_SHORTNAME_LENGTH];
	char				controllerIpAddress[MAX_IP_ADDR_CHARS];
} USER_DEFINED_CONFIG, *PUSER_DEFINED_CONFIG;

/*! /brief ExxComm Configuration parameters structure  */
typedef struct _CFG_PARAMS {
    char                cfgVersion[MAX_VERSION_CHARS];  /*!< \brief Version number from the .conf file. */
    unsigned int        E2BaseRecordID;                 /*!< \brief Base Record ID for E2 Records (added with Cell Type). */
    int                 logFD;                          /*!< \brief Log File Handle. */
    char *              pLogFileName;                   /*!< \brief Current Log File name. */
    char *              pPathName;                      /*!< \brief Log File Path Name. */
} CFG_PARAMS, *PCFG_PARAMS;

#include "../include/E2.h"

extern PCFG_PARAMS gpcfgParams;

#endif // __INTERROGATOR_H_

