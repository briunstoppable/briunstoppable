// -------------------------------------------------------------------------
/*!
    Exxeno Hub - ExxComm
    \file       ExxComm.h
    Contains the Exxeno communication hub defines, structs, includes, etc.
*/
// ------------------------- CONFIDENTIAL ----------------------------------
//
//              Copyright (c) 2014 - 2015 by Exxeno, LLC.
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

#ifndef __EXX_COMM_H_
#define __EXX_COMM_H_

//  #define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
//#define _BSD_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <signal.h>
#include <assert.h>
#include "../include/Exxeno.h"
#include "../include/debug.h"
#include "../include/curl.h"
#include "../include/cJSON.h"


//#include "../include/Exxeno.h"
//#include "../include/Exx_PHub.h"

#define MAX_PS_RECORDS          20
#define MAX_RS_RECORDS          20
#define MAX_E2_RECORDS          9999

#define MAX_PARITY_SIZE         3

#define MAX_CELL_TYPES          320

#define E2_NAME_SIZE        128
#define MAX_STR_SIZE        128
#define MAX_E2_STR_SIZE     16
#define MAX_E2_LONG_STR_SIZE  42
#define MAX_CELL_ENTRIES    2048

#define KEEPALIVE_INTERVAL_SECONDS  30  // Send keep alive every 30 seconds.
#define CHECK_CONFIG_INTERVAL       60  // Check once a minute

#define LOG_BUFFER_SIZE         500
#define MAX_MB_SLOTS            24

#define DEBUG_LINE_LEN          120

#define MAX_VERSION_CHARS       10
#define MAX_HEADER_BUF_SIZE     MAX_VERSION_CHARS + 20

#define HUB_STATUS_CHECK_ADDRESS    "http://www.exxeno.com/HubStatusCheck.aspx?MAC="
#define HUB_CONFIG_CHECK_ADDRESS    "http://www.exxeno.com/checkhubconfig.aspx?MAC="
#define HUB_GET_CONFIG_ADDRESS      "http://www.exxeno.com/gethubconfig.aspx?MAC="

#define BEGIN_CRITICAL(x)       pthread_mutex_lock(x);
#define END_CRITICAL(x)         pthread_mutex_unlock(x);

#define TRUE 1
#define FALSE 0

/*! /brief  Enums for running thread types */
typedef enum
{
	DISCOVERY   = 0x1,
	UDP_LISTEN  = 0x02,
	MODBUS_POLL = 0x04,
    UPLOAD      = 0x08,
    E2_POLL     = 0x10,
    RS485_POLL  = 0x20,
} THREAD_PROCS;

#define RT_RS_POLL_MODE         0xA000
#define RT_PS_POLL_MODE         0x9000
#define RT_PS_TIME_STAMP        0x9100
#define RT_E2_POLL_MODE         0x8000

#define DEFAULT_UPLOAD_SLEEP            5                   // Check for need to upload every x seconds
#define DEFAULT_UPLOAD_INTERVAL         5 * (60 / DEFAULT_UPLOAD_SLEEP)   // Upload every x minutes
#define DEFUALT_UPLOAD_SIZE             1000000             // Upload when file is x bytes

#define PS_CONTROL_OFF          0x01
#define PS_CONTROL_ON           0x00

#define DEFAULT_POWER_SUPPLY_MASK   0x00

#define HUB_PS_A_FAULT          0x01
#define HUB_PS_B_FAULT          0x02
#define SW_1_PS_FAULT           0x04
#define SW_2_PS_FAULT           0x08

#define PS_CLEAR_KEY            1234

extern volatile boolean gStop;
extern volatile boolean gReconfig;

extern const char * sHubStatusCheckAddress;
extern const char * sHubConfigCheckAddress;
extern const char * sHubGetConfigAddress;

//typedef void *(*LPTHREAD_START_ROUTINE)(void *);

/*! \brief threeWay */
typedef enum {
    TW_FALSE,
    TW_TRUE,
    TW_NOT_SET
} threeWay;

/*! /brief Structure for time stamps */
typedef struct _TIME_STAMP {
	union {
		unsigned long long  time;   				/* 64 bit Access */
		struct {
            unsigned long    uSeconds;
            unsigned long    Seconds;
		};
	};
} TIME_STAMP, *PTIME_STAMP;

typedef struct _MAC_ADDRESS {
	union {
		unsigned long long  ADDRESS;   				/* 64 bit Access */
		struct {
            unsigned char   A1;
            unsigned char   A2;
            unsigned char   A3;
            unsigned char   A4;
            unsigned char   A5;
            unsigned char   A6;
		};
	};
} MAC_ADDRESS, *PMAC_ADDRESS;

typedef struct _NET_ADDRESS {
	union {
		unsigned long       lADDRESS;   				/* 32 bit Access */
        unsigned char       bADDRESS[4];
	};
} NET_ADDRESS, *PNET_ADDRESS;


/*! /brief ExxComm Configuration parameters structure  */
typedef struct _CFG_PARAMS {
    NET_ADDRESS         CommIPAddr;                     /*!< \brief Hub IP Address. */
    NET_ADDRESS         CommBcastAddr;                  /*!< \brief Hub Broadcast address. */
//    unsigned char       Eth0MACAddr[MAX_MAC_ADDR_CHARS];  /*!< \brief Hub MAC address read from Eth0. */

    char                cfgVersion[MAX_VERSION_CHARS];  /*!< \brief Version number from the ExxComm.conf file. */

//    char                controllerNetIF[MAX_NET_IFS];   /*!< \brief Name of Ethernet interfaces for controllers. */

//    pthread_mutex_t     logFileSemaphore;               /*!< \brief Log File Access semaphore. */
//	pthread_t	        UDPThread;                      /*!< \brief Packet Blast Thread handle. */
//    pthread_t	        UploadThread;                   /*!< \brief Upload thread handle. */
//    pthread_t	        E2PollThread;                   /*!< \brief E2 Polling thread handle. */
//    pthread_t	        RS485PollThread;                /*!< \brief RS485 polling thread handle. */
//    THREAD_PROCS        RunningThreads;                 /*!< \brief Running threads bit map. */


    int                 uploadSize;                     /*!< \brief Size of current upload. */
    int                 uploadInterval;                 /*!< \brief Upload interval. */
    int                 uploadSleep;                    /*!< \brief Lenght of time to sleep between uploads. */

    unsigned int        PSModBusPollRecordID;           /*!< \brief Record ID for PowerScout ModBUs Poll Mode. */
    unsigned int        PSTimeStampRecordID;            /*!< \brief Record ID for Packet Blast time stamp. */
    unsigned int        RS485RecordID;                  /*!< \brief Record ID for RS485 data. */
    unsigned int        E2BaseRecordID;                 /*!< \brief Base Record ID for E2 Records (added with Cell Type). */

    int                 logFD;                          /*!< \brief Log File Handle. */
    char *              pLogFileName;                   /*!< \brief Current Log File name. */
    char *              pPathName;                      /*!< \brief Log File Path Name. */
    unsigned short      BlastDuration;                  /*!< \brief Packet Blast Duration. */
    unsigned long long  bmPacketBlast;                  /*!< \brief Current Packet Blast bit map. */
    int                 PSCount;                        /*!< \brief PowerScout count. */
    unsigned long long  bmThreadMapArray[32];           /*!< \brief Active Threads bit map. */
 //   struct list_head    PSList;
} CFG_PARAMS, *PCFG_PARAMS;


/*! /brief ExxComm E2 Record configuration parameters structure  */
typedef struct _PS_RECORD {
    int 				RecordID;                       /*!< \brief ModBus Poll Record ID returned in the csv. */
    int	    			ModBusPollInterval;             /*!< \brief ModBus polling frequency. */
    short				ModBusRegNumSlots;
    short				ModBusRegSlot[MAX_MB_SLOTS];    /*!< \brief PowerScout ModBus Registers to read. */
} PS_RECORD, *PPS_RECORD;

#include "../include/E2.h"

extern PCFG_PARAMS gpcfgParams;

//extern void DiscoverPowerScouts(PCFG_PARAMS pcfgParams);
extern void UDPListen(PCFG_PARAMS pcfgParams);
//extern void ModBusPoll(PMB_PARAMS pMbParams);
extern void UploadThread(PCFG_PARAMS pcfgParams);
//extern void E2PollThread(PE2_PARAMS pE2Params);
//extern void RS485Poll(PRS485_PARAMS pRS485Params);

extern int createLogFile(PCFG_PARAMS pcfgParams);

extern boolean getSystemConfig(PCFG_PARAMS params);

extern int getProgramConfig(PCFG_PARAMS pcfgParams);
extern int createConfigFile(PCFG_PARAMS pcfgParams);
//extern int createPSEntry(PMB_PARAMS pMbParams);

extern int getPBState(PCFG_PARAMS pcfgParams, unsigned char SensorIP);
void resetPBState(PCFG_PARAMS pcfgParams, unsigned char SensorIP);

//extern boolean getPSSensorConfig(PMB_PARAMS pPScout, int index);
//extern boolean getE2Config(PE2_PARAMS pE2, int index);
//extern boolean getRS485Config(PRS485_PARAMS pRS485);

extern int getThreadRun(PCFG_PARAMS pcfgParams, unsigned char SensorNum);
extern void setThreadRun(PCFG_PARAMS pcfgParams, unsigned char SensorNum);
extern void resetThreadRun(PCFG_PARAMS pcfgParams, unsigned char SensorNum);
extern unsigned char getRunningThreads(PCFG_PARAMS pcfgParams);

extern int KeepAlive(PCFG_PARAMS pcfgParams);
extern int CheckNewConfig(PCFG_PARAMS pcfgParams);

extern void logMessages(int level, char * logData);

#endif // __EXX_COMM_H_

