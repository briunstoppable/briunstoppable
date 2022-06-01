// -------------------------------------------------------------------------
// 
// PRODUCT:			Expresso Multi-Channel DMA Driver
// MODULE NAME:		ExpressoCli.h
// 
// MODULE DESCRIPTION: 
// 
// Contains common defines for the Command Line Interface application.
// 
// $Revision:  $
//
// ------------------------- CONFIDENTIAL ----------------------------------
// 
//              Copyright (c) 2016 by Northwest Logic, Inc.   
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

#ifndef __ExpressoCli__h_
#define __ExpressoCli__h_

#include <signal.h>
#ifndef WIN32  // Must be for Linux
#include <unistd.h>
//#include <signal.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>

#include <stdint.h>
#include <pthread.h>

#include <aio.h>          /* Asynchronous I/O */

#include "../Include/StdTypes.h"

#define MY_ULONGLONG_MAX	0xFFFFFFFFFFFFFFFFLLU

#define UNREFERENCED_PARAMETER(Value)

#else 

#include "Setupapi.h"

#include "../Include/StdTypes.h"

//#include <windows.h>
//#include <conio.h>
//#include <tchar.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <ctype.h>
//#include <winioctl.h>

#define MY_ULONGLONG_MAX				0xFFFFFFFFFFFFFFFF

#endif // Not Windows version

#define VERSION_2						1

#include "../Include/ExpressoIoctl.h"

#define MY_ULONG_MAX					2147483647

#define MAX_DEVICE_PATH_LENGTH			256
#define	MAX_DEVICE_NAME_EXTENSION		15

#define	NWL_CLI_SIGNATURE				0x6db6aa55

#define	DUMP_BUFFER_SIZE				1000000

// Defines

#define	DEFAULT_CARD_NUMBER				0
#define	DEFAULT_READ_DMA_CHANNEL		0
#define	DEFAULT_WRITE_DMA_CHANNEL		1
#define	DEFAULT_BUFFER_SIZE				16777216
#define	DEFAULT_ITERATIONS				-1 //1000000
#define	DEFAULT_SECONDS					60
#define	DEFAULT_NUMBER_THREAD			1
#define	DEFAULT_VERBOSE					FALSE
#define	DEFAULT_DEBUG					FALSE
#define	DEFAULT_QUIET					FALSE
#define	DEFAULT_SEED_PATTERN			0
#define	DEFAULT_MEM_LENGTH				4
#define	DEFAULT_DMA_LENGTH				8192
#define	DEFAULT_FIFO_DMA_LENGTH			16384
#define	DEFAULT_MAX_FIFO_TRANSFER_SIZE	0x800000
#define	DEFAULT_MAX_TRANSFER_SIZE		2097152 // 32768
#define	DEFAULT_ADDR_DMA_MAX			131072
#define	DEFAULT_STRESS_DMA_LENGTH		131072 // 524288
#define	DEFAULT_MAX_CARD_OFFSET			131072 // 0x100000
#define	DEFAULT_STOP_ON_ERROR			FALSE;
#define	DEFAULT_RANDOMIZE				FALSE;

#define	MAX_THREADS						10
#define MAX_CMD_LENGTH					20
#define MAX_OPTION_LENGTH				3
#define	MAX_CHANNEL_NAME_SIZE			10

#define	PATTERN_INCREMENTING			0
#define	PATTERN_SPECIFIED				1
#define	PATTERN_RANDOM					2

#define	ERROR_DLL_CALL_FAILED			-1
#define	ERROR_TRANSFER_COUNT_MISMATCH	-2
#define ERROR_TRANSFER_DATA_MISMATCH	-3
#define ERROR_NO_DATA_TRANSFER			-4
#define	ERROR_BAD_PARAMETER				-10
#define ERROR_BAD_TEST_COMMAND			-11
#define	ERROR_NO_BOARD_FOUND			-20
#define ERROR_MEMORY_ALLOC_FAILED		-30
#define	ERROR_NO_DMA_CHANNEL_FOUND		-40

// -------------------------------------------------------------------------
// Defines for Stress tests
// -------------------------------------------------------------------------
// Define params structs for Read and Writes.
#define	READ_STRESS						0
#define	WRITE_STRESS					1
#define NUM_STRESS_PARAMS				2

#define _NELEM(arr)            ((INT32) (sizeof(arr) / sizeof(arr[0])))
#ifdef CONFIG_X86_64				
#define _OFFSETOF(t,m)         ((UINT64) &((t *)0)->m)
#else
#define _OFFSETOF(t,m)         ((UINT32) &((t *)0)->m)
#endif

/* \struct CFG_PARAMS
 * \brief Configuration paramters structure
 */
typedef struct _CFG_PARAMS {
	HANDLE			hDrvDevice;
    UINT64			CardAddress;
	UINT32			BufferSize;
	UINT32			MaxTransferSize;
	UINT32			DataSeedPattern;
	UINT32			Iterations;
	UINT32			Seconds;
	UINT32			MaxDMAAddress;
	INT32			Index;
	INT32			CmdArgc;
	const PCHAR *	CmdArgv;
	INT32			Type;
	INT32			cardNum;
	INT32			ReadDMAChannel;
	INT32			WriteDMAChannel;
	INT32			NumberThreads;
	INT32			DataPatternType;
	INT32			DataSeedSize;
	INT32			iPass;
	BOOLEAN			bVerbose;
	BOOLEAN			bDebug;
	BOOLEAN			bQuiet;
	BOOLEAN			bRandomize;
	BOOLEAN			bStopOnError;
	BOOLEAN			bPatternSpecified;
	BOOLEAN			bReadFIFO;
	BOOLEAN			bWriteFIFO;
	BOOLEAN			bNonAligned;
} CFG_PARAMS, *PCFG_PARAMS;

typedef void (*optfunc_t)(
	PCFG_PARAMS		pcfgParams, 
	UINT32			value
);

#define TYPE_NONE				0
#define TYPE_ULONG				1

/* \struct option_t
 * \brief typedef option_t
 */
typedef struct
{
    const char *	opt;
    optfunc_t		func;
	INT32			type;
    const char *	help;
} option_t;

typedef INT32 (*cmdfunc_t)(
	PCFG_PARAMS		pcfgParams
);

#define	TYPE_HELP				0
#define TYPE_GEN				1
#define TYPE_DMA				2

/* \struct command_t
 * \brief typedef command_t
 */
typedef struct
{
    const char *	cmd;
    cmdfunc_t		func;
	INT32			type;
	const char *	help;
} command_t;

/* \struct THREAD_PARAMS
 * \brief Thread Params
 */
typedef struct _THREAD_PARAMS {
	// the overlapped I/O structure os must be first in this structure
    OVERLAPPED		os;				// OVERLAPPED structure for the operation
	//  Do not move os entry from the first position.
	UINT32			Signature;
	HANDLE			hReadDevice;	// Driver open handle for Read DMAs
	HANDLE			hWriteDevice;	// Driver open handle for Write DMAs
	HANDLE			hDoneEvent;		// Event Handle (used to singal completion)

	PUINT8			Buffer;
	PUINT8			FIFOBuffer;
	UINT32			MaxTransferSize;
	UINT32			TransferSize;
	size_t			AllocSize;
#ifdef WIN32
	HANDLE			hThreadHandle;
#else /* Not Windows */
	pthread_t		hThreadHandle;
#endif /* WIN32 */	
	UINT32			TransferCount;
	UINT32			Iterations;
	BOOLEAN			WriteDirection;
	UINT64			CardOffsetMax;
	UINT64			CardOffset;
	UINT32			MaxDMAAddress;
	INT32			iPass;
	BOOLEAN			Running;

	INT32			cardNum;
	INT32			ReadDMAChannel;
	INT32			WriteDMAChannel;

	UINT32			DataSeedPattern;
	INT32			DataPatternType;
	INT32			DataSeedSize;

	UINT64			TotalByteCountTransferred;

	BOOLEAN			bDebug;
	BOOLEAN			bQuiet;
	BOOLEAN			bRandomize;
	BOOLEAN			bStopOnError;
	BOOLEAN			bPatternSpecified;
	BOOLEAN			bUserInfoPatternSpecified;
	BOOLEAN			bReadFIFO;
	BOOLEAN			bWriteFIFO;
	BOOLEAN			bNonAligned;

// -----------------------------------
// Items for FIFO Recieve mode only
// -----------------------------------

	UINT32			DMAOverrunCount;
	UINT32			ElementErrorCount;
	UINT32			ZeroPacketCount;
	UINT32			FullPacketCount;
	UINT32			PartialPacketCount;
} THREAD_PARAMS, *PTHREAD_PARAMS;

/* \struct STRESS_THREAD_PARAMS
 * \brief Structure of Parameters for Stress Threads 
 */
typedef struct _STRESS_THREAD_PARAMS {
	THREAD_PARAMS		WriteParams;
	THREAD_PARAMS		ReadParams;
} STRESS_THREAD_PARAMS, *PSTRESS_THREAD_PARAMS;

/* \struct PASS_STATS
 * \brief Pass Statistics structure 
 */
typedef struct _PASS_STATS {
	UINT64			BytesTransfered;
	UINT64			TransfersPerSecond;
	UINT64			IntsPerSecond;
	UINT64			DPCsPerSecond;
	INT32			CPUUtil;
} PASS_STATS, *PPASS_STATS;

/* \struct SUMMARY_STATS
 * \brief Summary Statistics structure 
 */
typedef struct _SUMMARY_STATS {
	UINT64			Samples;
	UINT64			BytesTransferedTotal;
	UINT64			TransfersPerSecondTotal;
	UINT64			IntsPerSecondTotal;
	UINT64			DPCsPerSecondTotal;
	UINT32			CPUUtilTotal;
} SUMMARY_STATS, *PSUMMARY_STATS;

/* \struct TRAFFIC_CTRL
 * \brief Traffic Control structure 
 */
typedef struct _TRAFFIC_CTRL {
	INT32			DMAChannel;
	BOOLEAN			Generator;
	BOOLEAN			Start;
} TRAFFIC_CTRL, *PTRAFFIC_CTRL;

#define	DMA_CHANNEL_0_GENERATOR_OFFSET		0x0000
#define	DMA_CHANNEL_0_CHECKER_OFFSET		0x0100
#define	DMA_CHANNEL_1_GENERATOR_OFFSET		0x0200
#define	DMA_CHANNEL_1_CHECKER_OFFSET		0x0300
#define	DMA_CHANNEL_2_GENERATOR_OFFSET		0x0400
#define	DMA_CHANNEL_2_CHECKER_OFFSET		0x0500
#define	DMA_CHANNEL_3_GENERATOR_OFFSET		0x0600
#define	DMA_CHANNEL_3_CHECKER_OFFSET		0x0700

/*! \struct GEN_CHECK_CNTRL
 *  \brief Generator Check Control DWORD structure
 */ 
typedef struct _GEN_CHECK_CNTRL
{
	// Hardware specific entries - Do not change or reorder
	union {
		UINT32 	DWORD;							// Control Flags - 32 Bits
		struct {
			UINT32		ENABLE			: 1;	// Bit 0: Enable Generator/Checker
			UINT32		CHECKER_EN		: 1;	// Bit 1: 1 = Enable Checker, 0 = Enable Generator
			UINT32		LOOPBACK_EN		: 1;	// Bit 2: 1 = Enable Loopback, 0 = Gen/Chk
			UINT32		reserved		: 1;	// Bit 3: Reserved
			UINT32		NUM_TABLE_ENTRY : 2;	// Bits 4 - 5: Number of Length Tables
			UINT32		reserved2		: 2;	// Bits 6 - 7: Reserved
			UINT32		DATA_PATTERN	: 2;	// Bits 8 - 9: Data Pattern 0 = Constant, 1 = Incr, 2 = LFSR 3 = Incr DWORD 
			UINT32		reserved3		: 5;	// Bits 10 - 14: Reserved
			UINT32		DATA_CONTINUE	: 1;	// Bits 15: 1 = Continue data from one transfer to the next. 0 = restart on each transfer
			UINT32		DATA_CONTINUOUS : 1;	// Bits 16: 1 = Run indefinitely. 0 = Stop after count is exhausted
			UINT32		reserved4		: 7;	// Bits 17 - 23: Reserved
			UINT32		PRESENT			: 8;	// Bits 24 - 31: Presence Indicator
		} FIELDS;
	} CTRL;
} GEN_CHECK_CNTRL, *PGEN_CHECK_CNTRL;

// -------------------------------------------------------------------------
// Defines for accessing the GEN_CHECK Control DWORD
// -------------------------------------------------------------------------
#define	DEF_ENABLE					(1 << 0)   	// Bit 0: Enable Generator/Checker
#define	DEF_CHECKER_EN				(1 << 1)   	// Bit 1:  1 = Enable Checker, 0 = Enable Generator
#define	DEF_LOOPBACK_EN 			(1 << 2)   	// Bits 2: 1 = Enable Loopback, 0 = Gen/Chk
#define	DEF_ONE_TABLE_ENTRY			(0 << 4)	// Bits 4 - 5: One Length Table entry
#define	DEF_TWO_TABLE_ENTRY			(1 << 4)	// Bits 4 - 5: Two Length Table entries
#define	DEF_THREE_TABLE_ENTRY		(2 << 4)	// Bits 4 - 5: Three Length Table entries
#define	DEF_FOUR_TABLE_ENTRY		(3 << 4)	// Bits 4 - 5: Four Length Table entries
#define	DEF_CONTANT_DATA_PATTERN	(0 << 8)	// Bits 8 - 9: Data Pattern 0 = Constant
#define	DEF_INC_BYTE_DATA_PATTERN	(1 << 8)	// Bits 8 - 9: Data Pattern 1 = Incrementing Byte
#define	DEF_LFSR_DATA_PATTERN		(2 << 8)	// Bits 8 - 9: Data Pattern 2 = LFSR
#define	DEF_INC_DWORD_DATA_PATTERN	(3 << 8)	// Bits 8 - 9: Data Pattern 3 = Incr DWORD 
#define	DEF_DATA_CONTINUE			(1 << 15)	// Bits 15: 1 = Continue data from one transfer to the next. 0 = restart on each transfer
#define	DEF_DATA_CONTINUOUS 		(1 << 16)	// Bits 16: 1 = Run Continuously. 0 = Stop after count exhausted
#define	DEF_GEN_PRESENT 			(8 << 24)	// Bits 24: Determine is packet Generator is present

/*! \struct GENERATOR_CHECKER_CONTROL
 *  \brief Generator/Checker Control Structure 
 *	Control for the traffic Generator
 */
typedef struct _GENENRATOR_CHECKER_CONTROL
{
	GEN_CHECK_CNTRL Control;		// Generator/Checker Control DWORD
} PACKED GENENRATOR_CHECKER_CONTROL, *PGENENRATOR_CHECKER_CONTROL;

/*! \struct GENERATOR_CHECKER_PARAMS
 *  \brief Generator/Checker Parameters Structure 
 *	Parameters for the traffic Generator
 */
typedef struct _GENENRATOR_CHECKER_PARAMS
{
	UINT32		NumTransfers;		// Count of Transfers to generate, 0 = 1, 1 = 2...
	UINT32		DataSeed;			// Data Seed pattern
	UINT16		ErrorCount;			// Error Counter for Checker, write bit 0 == 1 to clear
	UINT16		Reserved;
	UINT32 	TransferLength[4];		// DMA Transfer Length array (Valid bits 0 - 19)
} PACKED GENENRATOR_CHECKER_PARAMS, *PGENENRATOR_CHECKER_PARAMS;

/*! \struct GENENRATOR_CHECKER_STRUCT
 *  \brief Generator/Checker Structure 
 */
typedef struct _GENENRATOR_CHECKER_STRUCT
{
	GEN_CHECK_CNTRL Control;		// Generator/Checker Control DWORD
	UINT32		NumTransfers;		// Count of Transfers to generate, 0 = 1, 1 = 2...
	UINT32		DataSeed;			// Data Seed pattern
	UINT16		ErrorCount;			// Error Counter for Checker, write bit 0 == 1 to clear
	UINT16		Reserved;
	UINT32	 	TransferLength[4];	// DMA Transfer Length array (Valid bits 0 - 19)
} PACKED GENENRATOR_CHECKER_STRUCT, *PGENENRATOR_CHECKER_STRUCT;

const UINT32	GenChkControlTable[] =
{
	DMA_CHANNEL_0_GENERATOR_OFFSET,
	DMA_CHANNEL_0_CHECKER_OFFSET,
	DMA_CHANNEL_1_GENERATOR_OFFSET,
	DMA_CHANNEL_1_CHECKER_OFFSET,
	DMA_CHANNEL_2_GENERATOR_OFFSET,
	DMA_CHANNEL_2_CHECKER_OFFSET,
	DMA_CHANNEL_3_GENERATOR_OFFSET,
	DMA_CHANNEL_3_CHECKER_OFFSET,
};

const UINT32	GenChkParamsTable[] =
{
	DMA_CHANNEL_0_GENERATOR_OFFSET + sizeof(GENENRATOR_CHECKER_CONTROL),
	DMA_CHANNEL_0_CHECKER_OFFSET + sizeof(GENENRATOR_CHECKER_CONTROL),
	DMA_CHANNEL_1_GENERATOR_OFFSET + sizeof(GENENRATOR_CHECKER_CONTROL),
	DMA_CHANNEL_1_CHECKER_OFFSET + sizeof(GENENRATOR_CHECKER_CONTROL),
	DMA_CHANNEL_2_GENERATOR_OFFSET + sizeof(GENENRATOR_CHECKER_CONTROL),
	DMA_CHANNEL_2_CHECKER_OFFSET + sizeof(GENENRATOR_CHECKER_CONTROL),
	DMA_CHANNEL_3_GENERATOR_OFFSET + sizeof(GENENRATOR_CHECKER_CONTROL),
	DMA_CHANNEL_3_CHECKER_OFFSET + sizeof(GENENRATOR_CHECKER_CONTROL),
};

// -------------------------------------------------------------------------
// Function Prototypes
// -------------------------------------------------------------------------

//static void	SignalHandler(int sig);

// Functions located in Utils.cpp.
HANDLE 
OpenDMAChannel(
	UINT32			BoardNumber, 
	INT32			DMAChannel
);

HANDLE 
ConnectDriver(
	UINT32			BoardNumber,
	PCHAR			DMAChannelName
);

INT32
SetupChannelMode(
	PCFG_PARAMS		pcfgParams, 
	HANDLE			Channel, 
	PVOID			Buffer, 
	DWORD			Length,
	BOOLEAN			S2CDirection
);

INT32	
ShutdownChannelMode(
	PCFG_PARAMS		pcfgParams, 
	HANDLE			Channel,
	BOOLEAN			S2CDirection
);

INT32	
BufferAllocAndSet(
	PCFG_PARAMS		pcfgParams, 
	PTHREAD_PARAMS	params
);

BOOLEAN
ValidateDataBuffer(
	PTHREAD_PARAMS	params, 
	PUINT8			Buffer, 
	UINT64			Length
);

VOID 
WriteRandomBuffer(
	PTHREAD_PARAMS	params, 
	UINT32			Length
);

VOID
InitThreadParams(
	PCFG_PARAMS		pcfgParams, 
	PTHREAD_PARAMS	params
);

VOID
FreeThreadParams(
	PTHREAD_PARAMS	params
);

INT32
StartThread(
	PTHREAD_PARAMS	params, 
	LPTHREAD_START_ROUTINE ThreadProc
);

BOOLEAN
TestRunControl(
	PCFG_PARAMS		pcfgParams, 
	PTHREAD_PARAMS	params
);

VOID
ShutdownTestThreads(
	PCFG_PARAMS		pcfgParams, 
	PTHREAD_PARAMS	params
);

INT32	
GetChannelStats(
	HANDLE			Channel, 
	PPASS_STATS		pPassStats
);

VOID	
DisplayPassInfo(
	PCFG_PARAMS		pcfgParams, 
	PPASS_STATS		pPassStats, 
	PSUMMARY_STATS	pSummaryStats
);

VOID
InitPassStats(
	PPASS_STATS		pPassStats
);

VOID	
InitSummaryStats(
	PSUMMARY_STATS	pSummaryStats
);

VOID	
DisplaySummaryInfo(
	PCFG_PARAMS		pcfgParams,	
	PSUMMARY_STATS	pSummaryStats
);

VOID
DisplayFormattedBytes(
	BOOLEAN			bQuiet, 
	UINT64			Offset, 
	PUINT8			pBuffer, 
	INT32			BytesToDisplay
);

UINT32	
DisplayErrorInfo(
	BOOLEAN			bQuiet, 
	UINT32			status
);

UINT32
_hexttoul(
	const PCHAR		value
);

UINT32
__strtoul( 
	const PCHAR		nptr
);

UINT64	
__strtoull(
	const PCHAR		nptr
);

INT32
atohx(
	char *			dst, 
	const char *	src
);

BOOLEAN
isHexStr(
	PCHAR			pHexStr
);

void 
reverse_string(
	PCHAR			str
);


INT32
GetCPUUtil();

INT32
SetupOSSpecifics(
	PCFG_PARAMS		pcfgParams
);

INT32
ShutdownOSSpecifics(
	PCFG_PARAMS		pcfgParams
);

// -------------------------------------------------------------------------
// Functions located in TestCmds.cpp.
// -------------------------------------------------------------------------
INT32	
writePCI(
	PCFG_PARAMS		pcfgParams
);

INT32
readPCI(
	PCFG_PARAMS		pcfgParams
);

INT32	
writeMem(
	PCFG_PARAMS		pcfgParams
);

INT32	
readMem(
	PCFG_PARAMS		pcfgParams
);

INT32 
getBoardCfg(
	PCFG_PARAMS		pcfgParams);

INT32 
ReadDMA(
	PCFG_PARAMS		pcfgParams
);

INT32
WriteDMA(
	PCFG_PARAMS		pcfgParams
);

INT32
WriteReadDMA(
	PCFG_PARAMS		pcfgParams
);

INT32
FIFOC2S(
	PCFG_PARAMS		pcfgParams
);

INT32 
FIFOS2C(
	PCFG_PARAMS		pcfgParams
);

INT32
waitUserInterrupt(
	PCFG_PARAMS		pcfgParams
);

// -------------------------------------------------------------------------
// Functions located in ReadWrite.cpp.
// -------------------------------------------------------------------------
VOID
ReadWriteThreadProc(
	PTHREAD_PARAMS	params
);

VOID
WRLoopbackThreadProc(
	PTHREAD_PARAMS	pparams
);

VOID
FIFOc2sThreadProc(
	PTHREAD_PARAMS	params
);

VOID 
FIFOs2cThreadProc(
	PTHREAD_PARAMS	params
);

VOID
UserIntThreadProc(
	PCFG_PARAMS		pcfgParams
);

INT32
SetupTrafficGenChk(
	PCFG_PARAMS		pcfgParams, 
	PTRAFFIC_CTRL	pTrafficCtrl, 
	PGENENRATOR_CHECKER_PARAMS pGenChkParams
);

VOID 
DrainFIFOc2s(
	PTHREAD_PARAMS		params
);

VOID 
DrainFIFOs2c(
	PTHREAD_PARAMS		params
);


//#if _DEBUG
INT32
DumpChannelInfo(
	HANDLE			Channel
);

INT32	
DumpTrafficGenChk(
	PCFG_PARAMS		pcfgParams, 
	PTRAFFIC_CTRL	pTrafficCtrl
);
//#endif // _DEBUG
#endif /* __ExpressoCli__h_ */
