// -------------------------------------------------------------------------
// 
// PRODUCT:			Expresso Multi-Channel DMA Driver
// MODULE NAME:		Utils.cpp
// 
// MODULE DESCRIPTION: 
// 
// Contains Linux specific utility functions for the CLI application.
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

#include "stdafx.h"
#include "ExpressoCli.h"

// -------------------------------------------------------------------------
// Global variables
// -------------------------------------------------------------------------
UINT64 g_idleTime;
UINT64 g_kernelTime;
UINT64 g_userTime;

static FILE * 	ghProcStat;
UINT64 			g_niceTime;
UINT64 			g_iowaitTime;

/*! OpenDMAChannel
 * 
 * \brief Returns the Handle to the request board and DMA Channel number
 * \param BoardNumber
 * \param DMAChannel
 * \return hDevice
 */
HANDLE 
OpenDMAChannel(
	UINT32			BoardNumber,
	INT32			DMAChannel
)
{
    HANDLE			hDevice = INVALID_HANDLE_VALUE;
    INT8            FileNameBuf[128];

    sprintf((PINT8)&FileNameBuf, "%s%d/DMA%d", PROCFS_PATH, BoardNumber, DMAChannel);
    hDevice = open(FileNameBuf, O_RDWR);
    // Debug Message: fprintf(stderr, "OpenDMAChannel: Name: %s, Handle = %d\n", FileNameBuf, hDevice);
    return hDevice;
}

/*! ConnectDriver
 * 
 * \brief This connects to the board you are interested in talking to.
 * \param BoardNumber
 * \param DMAChannelName
 */
HANDLE 
ConnectDriver(
	UINT32			BoardNumber,
	PINT8			DMAChannelName
)
{
    HANDLE			hDevice = INVALID_HANDLE_VALUE;
   	CHAR            FileNameBuf[128];

   	sprintf((PCHAR)&FileNameBuf, "%s%d/%s", PROCFS_PATH, BoardNumber, PROCFS_FB_NAME);
	hDevice = open(FileNameBuf, O_RDWR);
    //fprintf(stderr, "ConnectDriver: Name: %s, Handle = %d\n", FileNameBuf, hDevice);

	return hDevice;
}

/*! SetupChannelMode
 * 
 * \brief Configures the DMA Channel for FIFO Mode
 * \param pcfgParams
 * \param Channel
 * \param Buffer
 * \param Length
 * \param S2CDirection
 * \return Status
 */
INT32
SetupChannelMode(
	PCFG_PARAMS		pcfgParams, 
	HANDLE			Channel, 
	PVOID			Buffer, 
	DWORD			Length,
	BOOLEAN			S2CDirection
)
{
	SETUP_CHANNEL_STRUCT	SetupChannel;
	INT32			        Status = 0;

	SetupChannel.ChannelNum 	= pcfgParams->ReadDMAChannel;
	SetupChannel.AllocationMode = DMA_MODE_FIFO;
    SetupChannel.Length 		= Length;
    SetupChannel.BufferAddress 	= Buffer;

    SetupChannel.Direction		= 0;	// C2S by default
    if (S2CDirection)
    {
        SetupChannel.ChannelNum		= pcfgParams->WriteDMAChannel;
        SetupChannel.Direction	= 1;	// S2C
    }

	// Send CONFIGURE_CHANNEL_IOCTL
    Status = ioctl(
				Channel, 
				SETUP_CHANNEL_IOCTL, 
				&SetupChannel);
	if (Status != 0)
	{
		printf("SetupChannelMode IOCTL call failed. Status=0x%x, errno = %d\n", Status, errno);
		pcfgParams->iPass = DisplayErrorInfo(pcfgParams->bQuiet, errno);
        Status = errno;
	}
	return Status;
}

/*! ShutdownChannelMode
 *   
 * \brief Shuts down the DMA Channel. 
 * \param pcfgParams
 * \param Channel
 * \return Status
 */
INT32 
ShutdownChannelMode(
	PCFG_PARAMS				pcfgParams,
	HANDLE					Channel,
	BOOLEAN			        S2CDirection
)
{
    SHUTDOWN_CHANNEL_STRUCT	ShutdownChannel;
    INT32			        Status = 0;

    ShutdownChannel.ChannelNum 		= pcfgParams->ReadDMAChannel;
    ShutdownChannel.AllocationMode  = DMA_MODE_FIFO;
    if (S2CDirection)
    {
        ShutdownChannel.ChannelNum	= pcfgParams->WriteDMAChannel;
    }

    // Send Shutdown FIFO Mode IOCTL
    Status = ioctl(Channel, SHUTDOWN_CHANNEL_IOCTL, &ShutdownChannel);
    if (Status != 0)
    {
        printf("ShutdownChannelMode IOCTL call failed. Status=0x%x, errno=%d\n", Status, errno);
        pcfgParams->iPass = DisplayErrorInfo(pcfgParams->bQuiet, Status);
        Status = errno;
    }
    return Status;
}

/*! StartThread
 *
 * \brief Start a thread of execution.
 * \param params
 * \param ThreadProc
 * \return 0 or -2
 */
INT32	
StartThread(
	PTHREAD_PARAMS			params,
	LPTHREAD_START_ROUTINE 	ThreadProc
)
{
	// Create a thread
	if (pthread_create(
			&params->hThreadHandle, 
			NULL, 
			ThreadProc, 
			(PVOID)params))
	{
		fprintf(stderr, "CreateThread failed");
		return -2;
	}
	// Boost the priority of this thread.

	return 0;
}

/*! GetChannelStats
 * 
 * \brief Keeps track of each pass info and optionally display it.
 * \param Channel
 * \param pPassStats
 * \return Status
 */
INT32 
GetChannelStats(
	HANDLE					Channel,
	PPASS_STATS				pPassStats
)
{
	CHANNEL_STAT_STRUCT		DmaPerfStat;
    INT32					Status = 0;

	DmaPerfStat.ChannelNum = 0; 	// Field is not used, init to 0.

	Status = ioctl(Channel, GET_CHANNEL_STATS_IOCTL, &DmaPerfStat);
	if (Status != 0)
	{
		// ioctl failed
//#if DBG
		printf("Get Channel stats IOCTL call failed.  Status=0x%x, Error = 0x%x\n", Status, errno);
//#endif // DBG
		Status = errno;
		pPassStats->TransfersPerSecond = 0;
		pPassStats->IntsPerSecond = 0;
		pPassStats->DPCsPerSecond = 0;
	}
	else
	{
		pPassStats->TransfersPerSecond = DmaPerfStat.RequestCount;
		pPassStats->IntsPerSecond = DmaPerfStat.IntsPerSecond;
		pPassStats->DPCsPerSecond = DmaPerfStat.DPCsPerSecond;
	}

	return Status;
}

/*! GetCPUUtil
 * 
 * \brief Retieves the CPU Utilization since the last time this routine was called
 * \note It is necessary to call once to establish a baseline before collecting stats.
 * \return CPU Utilization since last call to this routine
 */
INT32 
GetCPUUtil(
)
{
	INT32		cpu = 0;

    UINT64 		idleTime;
    UINT64 		kernelTime;
    UINT64 		userTime;
    UINT64 		idl;
    UINT64 		ker;
    UINT64 		usr;
	UINT64 		sys;

	UINT64 		niceTime;
	UINT64 		iowaitTime;
	UINT64 		nice;
	UINT64 		iowait;
	UINT64 		totalTime;
	INT32 		ret;
	INT8 		line[256];

	/*! \note 
	 * Under linux, first try %idle/usage using /proc/stat. 
	 * If that does not work, disable /proc/stat for the future by closing 
	 * the file and use the next-best method.
	 */
	if (ghProcStat != NULL)
	{
		// Get the first line with the data.
		rewind (ghProcStat);
		fflush (ghProcStat);
		if (NULL == fgets (line, 256, ghProcStat))
		{
           printf("Failed to read CPU Status, disabling CPU Stats\n");
		   fclose (ghProcStat);
           ghProcStat = NULL;     // Don't try again.
		}
		else
		{
			iowaitTime = 0;
			ret = sscanf (line, "%*s %qu %qu %qu %qu %qu",
						  &userTime,
						  &kernelTime, &niceTime, &idleTime, &iowaitTime);
			if (ret < 4)
			{
				printf("Failed to parse valid CPU Stats, disabiling CPU Stats\n");
				fclose (ghProcStat);
				ghProcStat = NULL; // Don't try again.
			}
			else
			{
				// Store the current usage.
				usr  = userTime - g_userTime;
				sys  = kernelTime - g_kernelTime;
				nice = niceTime - g_niceTime;
				idl  = idleTime - g_idleTime;
				iowait = iowaitTime - g_iowaitTime;
				// Calculate the % usage.
				sys = usr + sys + nice;
				totalTime = sys + idl + iowait;
				if ((totalTime > 0) && (sys > 0))
				{
					cpu = (int) (100L * sys / totalTime);
				}
				// Store the values for the next calculation.
				g_userTime   = userTime;
				g_kernelTime = kernelTime;
				g_niceTime   = niceTime;
				g_idleTime   = idleTime;
				g_iowaitTime = iowaitTime;
			}
		}
	}
	return cpu;
}

/*! SetupOSSpecifics
 *
 * \brief Sets up any OS Specific functions, i.e Open /proc/stat in Linux
 * \param pcfgParams
 * \return 0 or 1
 */     
 INT32 
 SetupOSSpecifics(
	PCFG_PARAMS	pcfgParams
)
{
	/*! \note
	 * The following method is called in order to initialize the status calls
	 * routines.  After that it is safe to call each of the status calls separately
	 * @return GNUNET_OK on success and GNUNET_SYSERR on error (or calls errexit).
	 */
	ghProcStat = fopen ("/proc/stat", "r");
	if (NULL == ghProcStat)
	{
		printf("Failed to open /proc/stat for CPU Utilization, disabiling CPU Stats.\n");
		return 1;
	}
	return 0;
}

/*! ShutdownOSSpecifics
 * \brief Shutsdown any OS Specific functions, i.e Close /proc/stat in Linux
 * \param pcfgParams
 * \return 0     
 */
INT32 
ShutdownOSSpecifics(
	PCFG_PARAMS		pcfgParams
)
{
	if (ghProcStat != NULL)
	{
		fclose (ghProcStat);
		ghProcStat = NULL;
	}
	return 0;
}


#if _DEBUG
/*! DumpChannelInfo
 *
 * \brief Keeps track of each pass info and optionally display it
 * \param Channel
 * \return Status
 */
INT32
DumpChannelInfo(
	HANDLE			Channel
)
{
	PUINT8							pChanRawInfo;
	PDUMP_CHANNEL_STRUCT			pChan;
	PDMA_CHANNEL_STRUCT				pChannel;
	PDESTINATION_SG_ELEMENT_STRUCT	pDstElem;
	PSTATUS_ELEMENT64_STRUCT		pStat64Elem;
	DWORD							bytesReturned	= DUMP_BUFFER_SIZE;
	DWORD							Status			= 0;
	INT32							i;

	pChanRawInfo = (PUINT8)malloc(DUMP_BUFFER_SIZE);
	if (pChanRawInfo == NULL)
	{
		printf("DumpChannelInfo malloc failed.\n");
		return -1;
	}

	// Send DUMP_CHANNEL_STRUCTS_IOCTL
	Status = ioctl(Channel, DUMP_CHANNEL_STRUCTS_IOCTL, pChanRawInfo);
	// check returned structure size
	if (Status != 0)
	{
		// ioctl failed
        printf("DumpChannelInfo IOCTL call failed. Status=0x%x, errno=%d\n", Status, errno);
		Status = STATUS_INCOMPLETE;
	}
	else
	{
		pChan = (PDUMP_CHANNEL_STRUCT)pChanRawInfo;
		pChannel = (PDMA_CHANNEL_STRUCT)&pChan->DmaChannel;
		printf("DMA Channel Structure:\n");
		//DisplayFormattedBytes(0, 0, (PUCHAR)pChannel, sizeof(DMA_CHANNEL_STRUCT));
		printf("Src: Q_PTR: 0x%llx, Size:0x%x, Limit:0x%x, Enabled:%d\n", 
			pChannel->SOURCE.SRC_Q_PTR.QWORD, pChannel->SOURCE.SRC_Q_SIZE, pChannel->SOURCE.SRC_Q_LIMIT, pChannel->SOURCE.SRC_Q_PTR.FIELDS.QUEUE_ENABLE);
		printf("Dst: Q_PTR: 0x%llx, Size:0x%x, Limit:0x%x, Enabled:%d\n", 
			pChannel->DEST.DST_Q_PTR.QWORD, pChannel->DEST.DST_Q_SIZE, pChannel->DEST.DST_Q_LIMIT, pChannel->DEST.DST_Q_PTR.FIELDS.QUEUE_ENABLE);
		printf("Stat: Q_PTR: 0x%llx, Size:0x%x, Limit:0x%x, Enabled:%d\n", 
			pChannel->STATUS.STA_Q_PTR.QWORD, pChannel->STATUS.STA_Q_SIZE, pChannel->STATUS.STA_Q_LIMIT, pChannel->STATUS.STA_Q_PTR.FIELDS.QUEUE_ENABLE);
		printf("StatD: Q_PTR: 0x%llx, Size:0x%x, Limit:0x%x, Enabled:%d\n", 
			pChannel->STATD.STA_Q_PTR.QWORD, pChannel->STATD.STA_Q_SIZE, pChannel->STATD.STA_Q_LIMIT, pChannel->STATD.STA_Q_PTR.FIELDS.QUEUE_ENABLE);
		printf("HW: SrcQNxt: 0x%x, DstQNxt:0x%x, StatQNxt:0x%x, StatDQNxt:0x%x\n", 
			pChannel->HDWR.SRC_Q_NEXT, pChannel->HDWR.DST_Q_NEXT, pChannel->HDWR.STA_Q_NEXT, pChannel->HDWR.STAD_Q_NEXT);
		//SCRATCHPAD_STRUCT				SCRATCH;
		printf("PCIE: Ctrl: 0x%x, Stat:0x%x\n", 
			pChannel->PCIE_INT.CONTROL.DWORD, pChannel->PCIE_INT.STATUS.DWORD);
		printf("AXI: Ctrl: 0x%x, Stat:0x%x\n", 
			pChannel->AXI_INT.CONTROL.DWORD, pChannel->AXI_INT.STATUS.DWORD);
		printf("DMA: Ctrl: 0x%x, Stat:0x%x, PCIAssert:0x%x, AXIAssert:0x%x\n", 
			pChannel->DMA_CTRL.DMA_CONTROL.DWORD, pChannel->DMA_CTRL.DMA_STATUS.DWORD, 
			pChannel->DMA_CTRL.PCIE_INT_ASSERT.DWORD, pChannel->DMA_CTRL.AXI_INT_ASSERT.DWORD);

		printf("Number of:\n\tDst Elements:%d\n\tUsed St Elements:%d\n\tStat Elements:%d\n\tLast Element processed:%d\n", 
			pChan->NumberOfDstElements,
			pChan->NumberOfUsedDstElements,
			pChan->NumberOfStatElements,
			pChan->LastElemProcessed);

#ifdef WIN32
		pDstElem = (PDESTINATION_SG_ELEMENT_STRUCT)(pChanRawInfo + sizeof(DUMP_CHANNEL_STRUCT));
		printf("DMA Channel Destination SG Structure:\n");
		for (i = 0; i < pChan->NumberOfDstElements; i++)
		{
			printf("Dst#%d, BC/Flags: 0x%x, Addr: 0x%llx, Handle: 0x%x\n",
				i, pDstElem->BYTE_COUNT_FLAGS.DWORD, pDstElem->DEST_ADDRESS, pDstElem->USER_HANDLE);
			pDstElem++;
		}

		pStat64Elem = (PSTATUS_ELEMENT64_STRUCT)pDstElem;
		printf("DMA Channel Status Structure:\n");
		for (i = 0; i < pChan->NumberOfStatElements; i++)
		{
			printf("Stat#%d, Flags: 0x%x, Handle: 0x%x, ID: 0x%x\n",
				i, pStat64Elem->STATUS.DWORD, pStat64Elem->USER_HANDLE, pStat64Elem->USER_ID);
			pStat64Elem++;
		}
#endif // Windows version
	}
	free(pChanRawInfo);
	return Status;
}
#endif // Debug

