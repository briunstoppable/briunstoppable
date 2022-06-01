// -------------------------------------------------------------------------
// 
// PRODUCT:			Expresso Multi-Channel DMA Driver
// MODULE NAME:		Utils.cpp
// 
// MODULE DESCRIPTION: 
// 
// Contains utility functions for the CLI application.
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

UINT32	
GetNextPattern(
	INT32	Type, 
	INT32	Size, 
	UINT32	Seed
);

/*! BufferAllocAndSet
 * 
 * Description:
 *   Allocates a buffer based on the contents of cfgParams BufferSize and 
 *      initializes the buffer based on DataPatternType and SeedPattern
 */
INT32	
BufferAllocAndSet(
	PCFG_PARAMS		pcfgParams, 
	PTHREAD_PARAMS	params
)
{
	UINT32			t;
	UINT32			seed = 0;
	PUINT32			ulBuffer;
	UINT32			InitLen;
	UINT32			InitValue;

	// Allocate the buffer in multiples of 1024 bytes (minimum of 1024)
	params->TransferSize	= pcfgParams->BufferSize;
	params->Buffer			= NULL;

	params->AllocSize = SIZE_T((pcfgParams->BufferSize + 1023) & -1024);
	if ((params->AllocSize == 0) || (params->AllocSize < 0))
	{
#ifdef WIN32
		printf("Invalid Buffer Allocation Size = %d\n", params->AllocSize);
#else // Linux
		printf("Invalid Buffer Allocation Size = %zd\n", params->AllocSize);
#endif // Windows vs. Linux
		pcfgParams->iPass = ERROR_MEMORY_ALLOC_FAILED;
		return -1;
	}
	// Allocate the buffer
	
#ifdef WIN32
	if (pcfgParams->bNonAligned)
	{
		params->Buffer = (PUINT8) malloc(params->AllocSize);
		printf("Using malloc for buffer, Address=0x%p, Size = %d\n", params->Buffer, params->AllocSize);
	}
	else
	{
		params->Buffer = (PUINT8)VirtualAlloc(NULL, params->AllocSize, MEM_COMMIT, PAGE_READWRITE);
	}
	if (params->Buffer == NULL)
	{
		printf("Buffer allocation failed, Size = %d\n", params->AllocSize);
		params->Buffer = NULL;
		pcfgParams->iPass = ERROR_MEMORY_ALLOC_FAILED;
		return -1;
	}
#else // Linux
	if (pcfgParams->bNonAligned)
	{
		params->Buffer = (PUINT8) malloc(params->AllocSize);
		if (params->Buffer == NULL)
		{
			printf("Buffer malloc failed, Size = 0x%zd\n", params->AllocSize);
			params->Buffer = NULL;
			pcfgParams->iPass = ERROR_MEMORY_ALLOC_FAILED;
			return -1;
		}
	}
	else
    {
		if (posix_memalign((PVOID *)&params->Buffer, 4096, params->AllocSize))
		{
			printf("Buffer malloc failed, Size = 0x%zd\n", params->AllocSize);
			params->Buffer = NULL;
			pcfgParams->iPass = ERROR_MEMORY_ALLOC_FAILED;
			return -1;
		}
	}
#endif // Windows vs. Linux
	if (params->AllocSize != pcfgParams->BufferSize)
	{
		if (pcfgParams->bDebug)
		{
#ifdef WIN32
			printf("Buffer size optimized to %d for better performance from the requested size of %ld\n", 
				params->AllocSize, pcfgParams->BufferSize);
#else // Linux
			printf("Buffer size optimized to %zd for better performance from the requested size of %d\n", 
				params->AllocSize, pcfgParams->BufferSize);
#endif // Windows vs. Linux
		}
	}
	if (params->AllocSize > 0x7FFFFFFF)
	{
		// Turn off pattern setup and checking if using larger the 2GB buffers.
		pcfgParams->bPatternSpecified = FALSE;
	}
	// Make sure the buffer gets touch, just in case it is paged out (Win 7)
	memset(params->Buffer, 0, params->AllocSize);

	if (pcfgParams->bPatternSpecified)
	{
		ulBuffer = (PUINT32)params->Buffer;
		InitLen = params->AllocSize >> 2;

		switch (pcfgParams->DataPatternType)
		{
			case PATTERN_INCREMENTING:
			{
				seed = pcfgParams->DataSeedPattern;
				for (t = 0; t < InitLen; t++)
				{
					InitValue = seed;
					if (pcfgParams->DataSeedSize == 1)
					{
						InitValue = ((seed+3 & 0xFF) << 24) | 
									((seed+2 & 0xFF) << 16) | 
									((seed+1 & 0xFF) << 8) | 
									(seed & 0xFF);
						seed += 4;
					}
					else
					{	
						seed++;
					}
					ulBuffer[t] = InitValue;
				}
			}
			break;

			case PATTERN_SPECIFIED:
			{
				seed = pcfgParams->DataSeedPattern;
				if (pcfgParams->DataSeedSize == 1)
				{
					InitValue = ((seed & 0xFF) << 24) | 
								((seed & 0xFF) << 16) | 
								((seed & 0xFF) << 8) | 
								(seed & 0xFF);
				}
				else
				{
					InitValue = seed;
				}
				for (t = 0; t < InitLen; t++)
				{
					ulBuffer[t] = InitValue;
				}
			}
			break;
	
			case PATTERN_RANDOM:
			{
				srand(pcfgParams->DataSeedPattern);
				for (t = 0; t < InitLen; t++)
				{
					InitValue = ((rand() & 0xffff) << 16); // | rand();
					ulBuffer[t] = InitValue;
				}
			}
			break;
	
			default:
				memset(params->Buffer, 0xAA, params->AllocSize);
		}
	}
	return 0;
}

/*!
 * Function:
 *   ValidateDataBuffer
 *
 * Description:
 *   Compares the buffer to the data pattern specified
 */
BOOLEAN 
ValidateDataBuffer(
	PTHREAD_PARAMS		params,
	PUINT8				Buffer,
	UINT64				Length
)
{
	BOOLEAN				result = TRUE;
	UINT64				BufferOffset;
	UINT32				LeftToDo = 0;
	PUINT32				ulBuffer;
	UINT32				seed;
	UINT32				CmpLen;

	// If validation is not enabled, just return TRUE.
	if (!params->bPatternSpecified)
	{
		return result;
	}

	ulBuffer		= (PUINT32)Buffer;
	BufferOffset	= Buffer - params->Buffer;
	CmpLen			= (UINT32)(Length >> 2);

	if ((BufferOffset + Length) > params->AllocSize)
	{
		CmpLen		= (UINT32)((params->AllocSize - BufferOffset) >> 2);
		LeftToDo	= (UINT32)((Length - (params->AllocSize - BufferOffset)) >> 2);
	}

	seed = params->DataSeedPattern;
	while (CmpLen)
	{
		if (params->bPatternSpecified)
		{
			for (UINT32 t = 0; t < CmpLen; t++)
			{
				if (ulBuffer[t] != (UINT32)seed)
				{
					if (!params->bQuiet)
					{
						printf("Data mistatch at offset %d (out of %lld), Expected Value: 0x%08x, Buffer Value: 0x%08x\n",
							t * 4, Length, (UINT32)(seed & 0xFFFFFFFF), (ulBuffer[t] & 0xFFFFFFFF));
					}
					result = FALSE;
					params->iPass = ERROR_TRANSFER_DATA_MISMATCH;
					if (params->bStopOnError)
					{
						// stop here
						break;
					}
				}
				seed = GetNextPattern(params->DataPatternType, params->DataSeedSize, seed);
			}
			// If we were to run a continious pattern then we would save the seed value thus:
			//params->DataSeedPattern = seed;
		}
		CmpLen = LeftToDo;
		LeftToDo = 0;
		ulBuffer = (PUINT32)params->Buffer;
	}
	return result;
}

/*!
 * Function:
 *   WriteRandomBuffer
 *
 * Description:
 *   Writes random data to the buffer pointed to in the params struct.
 */
VOID 
WriteRandomBuffer(
	PTHREAD_PARAMS 	params,
	UINT32			Length
)
{
	UINT32			t;
	PUINT8			ucBuffer;
#if 0
	PUINT32			ulBuffer = (PUINT32)params->Buffer;
	UINT32			InitLen = 0;

	InitLen = Length >> 2;
	srand(params->DataSeedPattern);
	for (t = 0; t < InitLen; t++)
	{
		*ulBuffer = (UIINT32)rand();
		ulBuffer++;
	}
	ucBuffer = (PUINT8)ulBuffer;
	InitLen = Length & 0x3;
	if (InitLen > 0)
	{
		for (t = 0; t < InitLen; t++)
		{
			*ucBuffer = (UINT8)(rand() & 0xFF);
		}
	}
#else
	srand(params->DataSeedPattern);
	ucBuffer = (PUINT8)params->Buffer;
	for (t = 0; t < Length; t++)
	{
		*ucBuffer = (UINT8)(rand() & 0xFF);
		ucBuffer++;
	}
#endif 
}

/*!
 * Function:
 *   GetNextpattern
 *
 * Description:
 *   Calculates the next 32 bit pattern based on the Type and Size
 */
UINT32
GetNextPattern(
	INT32				Type,
	INT32				Size,
	UINT32				Seed
)
{
	UINT32				NewPattern	= 0;
	UINT8 				ucSeqNum	= 0;
	UINT8 				ucSeed		= 0;
	BOOLEAN				y1, 
						y2, 
						y3, 
						y4, 
						y5;

	if ((Type == PATTERN_INCREMENTING) ||
		(Type == PATTERN_SPECIFIED))
	{
		NewPattern = Seed;
		if (Type == PATTERN_INCREMENTING)
		{
			if (Size == 1)
			{
				ucSeed		= (UINT8)Seed & 0xFF;
				ucSeqNum	= (UINT8)(Seed >> 8) & 0xFF;
				ucSeed		+= 4;
				ucSeqNum	+= 4;
				NewPattern	=	((ucSeqNum & 0xFF) << 24) | 
								((ucSeqNum & 0xFF) << 16) | 
								((ucSeqNum & 0xFF) << 8)  | 
								(ucSeed & 0xFF);
			}
			else
			{	
				NewPattern++;
			}
		}
	}
	else if (Type == PATTERN_RANDOM)
	{
		y1 = (Seed & 0x80000000) ? 1 : 0;
		y2 = (Seed & 0x00200000) ? 1 : 0;
		y3 = (Seed & 0x00000400) ? 1 : 0;
		y4 = (Seed & 0x00000002) ? 1 : 0;
		y5 = (Seed & 0x00000001) ? 1 : 0;
		NewPattern = Seed << 1 |((~(y1 ^ y2 ^ y3 ^ y4 ^ y5)) & 0x01);
		//printf("pattern: 0x%x\n", NewPattern);
	}
	return NewPattern;
}

//
// Setup the traffic Generator so we get DMA transfers
//
INT32
SetupTrafficGenChk(
	PCFG_PARAMS					pcfgParams,
	PTRAFFIC_CTRL				pTrafficCtrl,
	PGENENRATOR_CHECKER_PARAMS	pGenChkParams
)
{
#ifdef WIN32
    OVERLAPPED			os;			// OVERLAPPED structure for the operation
#endif // Windows vs. Linux
	MEM_COPY_STRUCT				MemWrite;
	GENENRATOR_CHECKER_CONTROL	GenChkCtrl;
	DWORD						bytesReturned = 0;
	INT32						tableIndex;
	UINT32						Status = 0;

	if (pTrafficCtrl->DMAChannel > 4)
	{
		return -1;
	}
	tableIndex = pTrafficCtrl->DMAChannel << 1;
	if (!pTrafficCtrl->Generator)
	{
		tableIndex |= 0x01;
	}

	MemWrite.BarNum		= 1;	// Traffic Generator controls are located in BAR 1.
	MemWrite.CardOffset = (UINT64)GenChkParamsTable[tableIndex];
	MemWrite.Length		= sizeof(GENENRATOR_CHECKER_PARAMS);

	GenChkCtrl.Control.CTRL.DWORD = 0;
	pGenChkParams->Reserved	= 0;

	if (pTrafficCtrl->Start)
	{
		if (pTrafficCtrl->Generator)
		{
			GenChkCtrl.Control.CTRL.DWORD = (DEF_ENABLE | DEF_FOUR_TABLE_ENTRY);
		}
		else
		{
			GenChkCtrl.Control.CTRL.DWORD = (DEF_ENABLE | 
				DEF_CHECKER_EN | 
				DEF_FOUR_TABLE_ENTRY); // |
//				DEF_INC_BYTE_DATA_PATTERN |
//				DEF_DATA_CONTINUOUS);
			//GenChkCtrl.Control.CTRL.DWORD = 0x00010103;
			pGenChkParams->ErrorCount = 0x01;
		}
		if (pcfgParams->bPatternSpecified)
		{
			if (pcfgParams->DataPatternType == PATTERN_RANDOM)
			{
				GenChkCtrl.Control.CTRL.DWORD |= DEF_LFSR_DATA_PATTERN;
			}
			else if (pcfgParams->DataPatternType == PATTERN_SPECIFIED)
			{
				GenChkCtrl.Control.CTRL.DWORD |= DEF_CONTANT_DATA_PATTERN;
				pGenChkParams->DataSeed = pcfgParams->DataSeedPattern;
			}
			else if (pcfgParams->DataPatternType == PATTERN_INCREMENTING)
			{
				if (pcfgParams->DataSeedSize == 1)
				{
					GenChkCtrl.Control.CTRL.DWORD |= DEF_INC_BYTE_DATA_PATTERN;
				}
				else
				{
					GenChkCtrl.Control.CTRL.DWORD |= DEF_INC_DWORD_DATA_PATTERN;
				}
				pGenChkParams->DataSeed = pcfgParams->DataSeedPattern;
			}
		}
		else
		{
			GenChkCtrl.Control.CTRL.DWORD |= DEF_INC_BYTE_DATA_PATTERN;
		}
		GenChkCtrl.Control.CTRL.DWORD |= DEF_DATA_CONTINUOUS;
	}
	else
	{
		GenChkCtrl.Control.CTRL.DWORD = DEF_LOOPBACK_EN;
	}

	// Debug Print Message:: printf("SetupTrafficGenChk Control:0x%x, ChannelIndex:%d\n", GenChkCtrl.Control.CTRL.DWORD, tableIndex);
	
#ifdef WIN32
	os.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	//if (GenChkCtrl.Control.CTRL.DWORD != 0)
	{
		// Send Mem Write IOCTL with the GenChk Parameters Structure
		if (!DeviceIoControl(pcfgParams->hDrvDevice, WRITE_MEM_ACCESS_IOCTL,
				(LPVOID) &MemWrite, sizeof(MEM_COPY_STRUCT),
				(LPVOID)pGenChkParams, (DWORD)MemWrite.Length,
				&bytesReturned, &os))
		{
			Status = GetLastError();
			if (Status == ERROR_IO_PENDING)
			{
				// Wait here (forever) for the Overlapped I/O to complete
				if (!GetOverlappedResult(pcfgParams->hDrvDevice, &os, &bytesReturned, TRUE))
				{
					Status = GetLastError();
					printf("SetupTrafficGenChk IOCTL call failed. Error = %d\n", Status);
				}
				else
				{
					Status = 0;
				}
			}
		}
		ResetEvent(os.hEvent);
	}

	// Now that we have the parameters set, send the control dword
	MemWrite.BarNum		= 1;	// Traffic Generator controls are located in BAR 1.
	MemWrite.CardOffset = (UINT64)GenChkControlTable[tableIndex];
	MemWrite.Length		= sizeof(GENENRATOR_CHECKER_CONTROL);

	if (!DeviceIoControl(pcfgParams->hDrvDevice, WRITE_MEM_ACCESS_IOCTL,
			(LPVOID) &MemWrite, sizeof(MEM_COPY_STRUCT),
			(LPVOID) &GenChkCtrl, (DWORD)MemWrite.Length,
			&bytesReturned, &os))
	{
		Status = GetLastError();
		if (Status == ERROR_IO_PENDING)
		{
			// Wait here (forever) for the Overlapped I/O to complete
			if (!GetOverlappedResult(pcfgParams->hDrvDevice, &os, &bytesReturned, TRUE))
			{
				Status = GetLastError();
				printf("SetupTrafficGenChk IOCTL call failed. Error = %d\n", Status);
			}
			else
			{
				Status = 0;
			}
		}
	}
    CloseHandle(os.hEvent);
#else // Linux
    MemWrite.Buffer = (PUINT8)pGenChkParams;
	// Send DoMem IOCTL to write to the Traffic Generator registers in BAR 1
	Status = ioctl (pcfgParams->hDrvDevice, WRITE_MEM_ACCESS_IOCTL, &MemWrite);
	if (Status != 0)
	{
		// ioctl failed
        printf("SetupTrafficGenChk IOCTL call failed. Statu=0x%x, errno = %d\n", Status, errno);
		return errno;
	}

    // Now that we have the parameters set, send the control dword
    MemWrite.BarNum		= 1;	// Traffic Generator controls are located in BAR 1.
    MemWrite.CardOffset = (UINT64)GenChkControlTable[tableIndex];
    MemWrite.Length		= sizeof(GENENRATOR_CHECKER_CONTROL);
    MemWrite.Buffer = (PUINT8)&GenChkCtrl;

    Status = ioctl (pcfgParams->hDrvDevice, WRITE_MEM_ACCESS_IOCTL, &MemWrite);
	if (Status != 0)
	{
		// ioctl failed
        printf("SetupTrafficGenChk IOCTL call failed. Status= 0x%x, errno = %d\n", Status, errno);
		return errno;
	}

#endif // Windows vs. Linux
	return Status;
}

#if _DEBUG
//
// Setup the traffic Generator so we get DMA transfers
//
INT32
DumpTrafficGenChk(
	PCFG_PARAMS			pcfgParams,
	PTRAFFIC_CTRL		pTrafficCtrl
)
{
#ifdef WIN32
    OVERLAPPED			os;			// OVERLAPPED structure for the operation
#endif // Windows vs. Linux
	MEM_COPY_STRUCT		MemRead;
	GENENRATOR_CHECKER_STRUCT	GenChk;
	DWORD				bytesReturned = 0;
	INT32				tableIndex;
	UINT32				Status = 0;

	if (pTrafficCtrl->DMAChannel > 4)
	{
		return -1;
	}
	tableIndex = pTrafficCtrl->DMAChannel << 1;
	if (!pTrafficCtrl->Generator)
	{
		tableIndex |= 0x01;
	}
	MemRead.BarNum		= 1;	// Traffic Generator controls are located in BAR 1.
	MemRead.CardOffset	= (UINT64)GenChkControlTable[tableIndex];
	MemRead.Length		= sizeof(GENENRATOR_CHECKER_STRUCT);

#ifdef WIN32
	os.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Send Mem Write IOCTL with the GenChk Parameters Structure
	if (!DeviceIoControl(pcfgParams->hDrvDevice, READ_MEM_ACCESS_IOCTL,
			(LPVOID) &MemRead, sizeof(MEM_COPY_STRUCT),
			(LPVOID)&GenChk, (DWORD)MemRead.Length,
			&bytesReturned, &os))
	{
		Status = GetLastError();
		if (Status == ERROR_IO_PENDING)
		{
			// Wait here (forever) for the Overlapped I/O to complete
			if (!GetOverlappedResult(pcfgParams->hDrvDevice, &os, &bytesReturned, TRUE))
			{
				Status = GetLastError();
				printf("DumpTrafficGenChk IOCTL call failed. Error = %d\n", Status);
			}
			else
			{
				Status = 0;
			}
		}
	}
    CloseHandle(os.hEvent);
#else // Linux
    MemRead.Buffer = (PUINT8)&GenChk;

    Status = ioctl (pcfgParams->hDrvDevice, READ_MEM_ACCESS_IOCTL, &MemRead);
	if (Status != 0)
	{
		// ioctl failed
        printf("DumpTrafficGenChk IOCTL call failed. Status= 0x%x, errno = %d\n", Status, errno);
		return errno;
	}
#endif // Windows vs. Linux
//#if _DEBUG
	printf("Reading Pkt Gen/Chk BAR1 offset 0x%llx\n", 
		MemRead.CardOffset);
	if (Status == 0)
	{
		printf("Pkt Gen/Chk Ctrl:0x%x, NumTransfers:0x%x\n", 
			GenChk.Control.CTRL.DWORD, GenChk.NumTransfers);
		printf("Pkt Gen/Chk Data Seed:0x%x, Error Count:0x%x\n", 
			GenChk.DataSeed, GenChk.ErrorCount);
		printf("Pkt Gen/Chk xferLen1:0x%x, xferLen2:0x%x, xferLen3:0x%x, xferLen4:0x%x\n", 
			GenChk.TransferLength[0], GenChk.TransferLength[1], GenChk.TransferLength[2], GenChk.TransferLength[3]);
	}
//#endif // Debug
	return Status;
}
#endif // Debug
