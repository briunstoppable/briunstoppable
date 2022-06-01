// -------------------------------------------------------------------------
/*!
    Exxeno - Manufacturing Test Box
    \file       TestBox.h
    Contains the Exxeno Test box defines, structs, includes, etc.
*/
// ------------------------- CONFIDENTIAL ----------------------------------
//
//              Copyright (c) 2015 by Exxeno, LLC.
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

#ifndef __TEST_BOX_H_
#define __TEST_BOX_H_

//  #define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
#define _BSD_SOURCE
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include <unistd.h>
#include <syslog.h>

#define RESPONDER_PORT          9806

#define MAX_STR_SIZE            60

#define KEEPALIVE_INTERVAL_SECONDS  30  // Send keep alive every 30 seconds.

#define LOG_BUFFER_SIZE         500

#define DEBUG_LINE_LEN          120

#define MAX_VERSION_CHARS       10

#define MAX_NET_IFS             5

#define MAX_MAC_ADDR_CHARS      20
#define MAX_IP_ADDR_CHARS       18
#define MAX_URL_STR_SIZE        256
#define MAX_EXEC_CMD_SIZE       256

#define MAX_FILE_NAME_SIZE      512 //PATH_MAX

#undef FALSE
#define FALSE   0
#undef TRUE
#define TRUE    1

#ifndef boolean
typedef unsigned char   boolean;
#endif // boolean
#ifndef uint8_t
typedef	unsigned char	    uint8_t;
#endif // uint8_t
#ifndef uint16_t
typedef unsigned short	    uint16_t;
#endif // uint16_t
#ifndef uint32_t
//typedef unsigned long       uint32_t;
#endif // uint32_t

#include "../include/debug.h"

extern volatile boolean gStop;

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
    NET_ADDRESS         IPAddr[MAX_NET_IFS];            /*!< \brief IP addresses for each ethernet interfaces. */
    char                netIFName[MAX_NET_IFS][IFNAMSIZ]; /*!< \brief Array of Network interface names, eth0, eth1, etc. */

    unsigned char       Eth0MACAddr[MAX_MAC_ADDR_CHARS];  /*!< \brief Hub MAC address read from Eth0. */

    pthread_t	        UploadThread;                   /*!< \brief Upload thread handle. */
    int                 socketUDP;                      /*!< \brief Packet Blast UDP Socket. */
    int                 netIfIdx;                       /*!< \brief Number of Ethernet interfaces. */
} CFG_PARAMS, *PCFG_PARAMS;


extern PCFG_PARAMS gpcfgParams;

//extern void DiscoverPowerScouts(PCFG_PARAMS pcfgParams);

#endif // __TEST_BOX_H_

