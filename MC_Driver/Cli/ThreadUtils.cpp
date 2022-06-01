// -------------------------------------------------------------------------
// 
// PRODUCT:			Expresso Multi-Channel DMA Driver
// MODULE NAME:		ThreadUtils.cpp
// 
// MODULE DESCRIPTION: 
// 
// Contains Thread related utility functions for the CLI application.
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

/*! InitThreadParams
 * 
 * \brief Initializes the common thread parameter structure.
 * \param pcfgParams
 * \param params
 * \return None
 */
VOID
InitThreadParams(
	PCFG_PARAMS				pcfgParams, 
	PTHREAD_PARAMS			params
)
{
	params->Buffer			= NULL;
	params->FIFOBuffer		= NULL;
	params->hThreadHandle	= 0;
	params->WriteDirection	= FALSE;

	params->TransferCount	= 0;
	params->Running			= TRUE;
	params->iPass			= 0;

	params->Signature		= NWL_CLI_SIGNATURE;

	/*! \note
		The main configuration information is copied to the thread param 
		struct to avoid any cross thread issues.
	 */
	params->Iterations		= pcfgParams->Iterations;
	params->CardOffset		= 0;
	params->CardOffsetMax	= pcfgParams->CardAddress;
	params->MaxDMAAddress 	= pcfgParams->MaxDMAAddress; 
	params->MaxTransferSize	= pcfgParams->MaxTransferSize;

	params->cardNum			= pcfgParams->cardNum;
	params->ReadDMAChannel  = pcfgParams->ReadDMAChannel;
	params->WriteDMAChannel = pcfgParams->WriteDMAChannel;

	params->DataSeedPattern	= pcfgParams->DataSeedPattern;
	params->DataPatternType	= pcfgParams->DataPatternType;
	params->DataSeedSize	= pcfgParams->DataSeedSize;

	params->TotalByteCountTransferred = 0;

	params->bDebug			= pcfgParams->bDebug;
	params->bQuiet			= pcfgParams->bQuiet;
	params->bRandomize		= pcfgParams->bRandomize;
	params->bStopOnError	= pcfgParams->bStopOnError;
	params->bPatternSpecified = pcfgParams->bPatternSpecified;
	params->bReadFIFO		= pcfgParams->bReadFIFO;
	params->bWriteFIFO		= pcfgParams->bWriteFIFO;
	params->bNonAligned		= pcfgParams->bNonAligned;

#ifdef WIN32	
	// Create the event handles, make sure to free them when done.
	params->os.hEvent		= CreateEvent(NULL, FALSE, FALSE, NULL);
	params->hDoneEvent		= CreateEvent(NULL, TRUE, FALSE, NULL);
	ResetEvent(params->hDoneEvent);
#else	// Linux
#endif	// Windows vs. Linux
}

/*! FreeThreadParams
 * 
 * \brief Frees the thread parameters, and memory previous 
 *	allocated by BufferAllocAndSet.
 * \param params
 * \return None
 */
VOID
FreeThreadParams(
	PTHREAD_PARAMS		params
)
{
#ifdef WIN32	
	if (params->os.hEvent != NULL)
	{
		CloseHandle(params->os.hEvent);
		params->os.hEvent = NULL;
	}
	if (params->hDoneEvent)
	{
		CloseHandle(params->hDoneEvent);
		params->hDoneEvent = NULL;
	}

	if (params->Buffer != NULL)
	{
		if (params->bNonAligned)
		{
			free(params->Buffer);
		}
		else
		{
			if (!VirtualFree(params->Buffer, 0, MEM_RELEASE))
			{
				printf("Error %d: while freeing buffer\n", GetLastError());
			}
		}
		params->Buffer = NULL;
	}
#else // Linux
	free(params->Buffer);
    params->Buffer = NULL;
#endif	// Windows vs. Linux
}

/*! TestRunControl
 * 
 * \brief Controls the test and if necessary shuts down the test threads
 * \param pcfgParams
 * \param params
 * \return Running
 */
BOOLEAN
TestRunControl(
	PCFG_PARAMS		pcfgParams, 
	PTHREAD_PARAMS	params
)
{
	PTHREAD_PARAMS	pparams = params;
	UINT32			i;
	BOOLEAN			Running = FALSE;

	if ((pcfgParams->iPass != 0) && (pcfgParams->bStopOnError))
	{
		pcfgParams->Seconds = 0;
	}
	// Count down the seconds and if zero shut the test down
	if (!--pcfgParams->Seconds)
	{
		//printf("Test timed out, shutting down.\n");
		Running = FALSE;
		pparams = params;
		for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
		{
			pparams->Running = FALSE;
			pparams++;
		}
		Sleep(100);
	}
	else
	{
		pparams = params;
		for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
		{
			if (pparams->Running)
			{
				Running = TRUE;
				if (pcfgParams->bDebug)
				{
					printf("%d DMA Transfers on thread 0x%lx\n", pparams->TransferCount, pparams->hThreadHandle);
				}
				pparams->TransferCount = 0;
			}
			else
			{
				//printf("Traffic processing thread shutdown.\n");
			}
			pparams++;
		}
	}
	// If we are shutting down collect the result codes from each thread and copy it
	// to the main test thread. Stop on the first error code.
	if (!Running)
	{
		if (pcfgParams->iPass == 0)
		{
			pparams = params;
			for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
			{
				if (pparams->Running)
				{
					Sleep(500);
				}
				if (pparams->iPass != 0)
				{
					pcfgParams->iPass = pparams->iPass;
					break;
				}
				pparams++;
			}
		}
	}	
	return Running;
}

/*! ShutdownTestThreads
 * 
 * \brief Stops all running test threads
 * \param pcfgParams
 * \param params
 * \return None
 */
VOID
ShutdownTestThreads(
	PCFG_PARAMS		pcfgParams, 
	PTHREAD_PARAMS	params
)
{
	PTHREAD_PARAMS	pparams = params;
	UINT32			i;

	// Shutdown all threads
	pparams = params;
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		pparams->Running = FALSE;
		pparams++;
	}
	// Allow time for the threads to shutdown
	Sleep(1000);
	return;
}
