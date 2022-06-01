// -------------------------------------------------------------------------
// 
// PRODUCT:			Expresso Multi-Channel DMA Driver
// MODULE NAME:		ReadWrite.cpp
// 
// MODULE DESCRIPTION: 
// 
// Contains the Read & Write command functions for the CLI
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

VOID
PrepareTransfer(
	PTHREAD_PARAMS		params
)
{
	params->TransferSize	= params->MaxTransferSize;
	params->CardOffset		= params->CardOffsetMax; 
	// Check for random size and start offset option
	if (params->bRandomize)
	{
		// If randomize and Addressable FIFO mode
		if ((params->bWriteFIFO && params->WriteDirection) ||
			params->bReadFIFO)
		{
			// Only randomize the transfer length
			params->TransferSize = rand() % params->MaxTransferSize + 1;
		}
		else
		{
			params->CardOffset = 0;
			if (params->CardOffsetMax)
			{
				params->CardOffset = rand() % params->CardOffsetMax; 
			}
			params->TransferSize = rand() % params->MaxTransferSize + 1;
			if ((params->CardOffset + params->TransferSize) > DEFAULT_MAX_CARD_OFFSET)
			{
				params->TransferSize = (UINT32)(DEFAULT_MAX_CARD_OFFSET - params->CardOffset);
			}
		}
	}
	if ((params->DataPatternType == PATTERN_RANDOM) &&
		params->WriteDirection)
	{
		WriteRandomBuffer(params, params->TransferSize);
    }
	if (params->bDebug)
	{
		printf ("Preparing to transfer length of %d to card address %lld\n", 
			params->TransferSize, params->CardOffset);
	}
#ifdef WIN32
	params->os.OffsetHigh	= (DWORD)((params->CardOffset >> 32) & 0xFFFFFFFF);
	params->os.Offset		= (DWORD)(params->CardOffset & 0xFFFFFFFF);
#else // Linux
    params->os = params->CardOffset;
#endif // WIN32
}

#ifdef WIN32

//---------------------------------------------------------------------------------------
//			Windows Read and Write Overlapped I/O routines
//---------------------------------------------------------------------------------------

/*! ReadOverlappedCallback - called by the asynchronous I/O Completion handler.
 *
 * \brief Callback function called by the system or manually for each operation
 * \param dwErrorCode
 * \param dwBytesXfer
 * \param lpOS
 * \return None
 */
VOID WINAPI 
ReadOverlappedCallback(
	DWORD			dwErrorCode, 
	DWORD			dwBytesXfer, 
	LPOVERLAPPED	lpOS
)
{
	PTHREAD_PARAMS params = (PTHREAD_PARAMS) lpOS;

	// report error if any
    if (dwErrorCode)
	{
        printf("Error %d: Overlapped IO Completion Routine\n", dwErrorCode);
		if (params->bStopOnError)
		{
			// stop here
			params->Running = FALSE;
			SetEvent(params->hDoneEvent);
			return;
		}
    }
	if (params->Signature != NWL_CLI_SIGNATURE)
	{
        printf("Error: Overlapped callback parameters do not match\n");
		// stop here
		return;
	}
	else
	{
		params->TotalByteCountTransferred += (UINT64)dwBytesXfer;
		if (params->bDebug)
		{
			printf("Overlapped callback transferred %ld bytes, Read Buffer:\n", dwBytesXfer);
			if (dwBytesXfer > 0)
			{
				DisplayFormattedBytes(params->bQuiet, 0, params->Buffer, dwBytesXfer);
			}
		}
		if (params->Iterations--)
		{
			PrepareTransfer(params);

			if (!ReadFileEx(params->hReadDevice, params->Buffer, params->TransferSize, 
				(LPOVERLAPPED)&params->os, ReadOverlappedCallback)) 
			{
				printf ("Error on Read Handle 0x%x\n", params->hReadDevice);
				// error from the ReadFileEx
				params->iPass = DisplayErrorInfo(params->bQuiet, GetLastError());
				if (params->bStopOnError)
				{
					// stop here
					params->Running = FALSE;
					SetEvent(params->hDoneEvent);
					return;
				}
			}
			params->TransferCount++;
		}
		else
		{
			if (params->bDebug)
			{
			    printf("Overlapped Read Finished\n");
			}
			params->Running = FALSE;
			// Signal the event
			SetEvent(params->hDoneEvent);
		}
	}
}

/*! WriteOverlappedCallback
 * 
 * \brief called by the asynchronous I/O Completion handler.
 *  Callback function called by the system or manually for each operation
 * \param dwErrorCode
 * \param dwBytesXfer
 * \return None
 */
VOID WINAPI 
WriteOverlappedCallback(
	DWORD			dwErrorCode, 
	DWORD			dwBytesXfer, 
	LPOVERLAPPED	lpOS
)
{
	PTHREAD_PARAMS params = (PTHREAD_PARAMS) lpOS;

	// report error if any
    if (dwErrorCode)
	{
        printf("Error %d: Overlapped IO Completion Routine\n", dwErrorCode);
		if (params->bStopOnError)
		{
			// stop here
			params->Running = FALSE;
			SetEvent(params->hDoneEvent);
			return;
		}
    }
	if (params->Signature != NWL_CLI_SIGNATURE)
	{
        printf("Error: Overlapped callback parameters do not match\n");
		// stop here
		return;
	}
	else
	{
		params->TotalByteCountTransferred += (UINT64)dwBytesXfer;
		if (params->bDebug)
		{
	        printf("Overlapped callback transferred %ld bytes\n", dwBytesXfer);
		}
		if (params->Iterations--)
		{
			PrepareTransfer(params);

			// Issue async writes
			if (!WriteFileEx(params->hWriteDevice, params->Buffer, params->TransferSize, 
				(LPOVERLAPPED)&params->os, WriteOverlappedCallback)) 
			{
				// error from the ReadFileEx
				params->iPass = DisplayErrorInfo(params->bQuiet, GetLastError());
				if (params->bStopOnError)
				{
					// stop here
					params->Running = FALSE;
					SetEvent(params->hDoneEvent);
					return;
				}
			}
			params->TransferCount++;
		}
		else
		{
			if (params->bDebug)
			{
			    printf("Overlapped Write Finished\n");
			}
			params->Running = FALSE;
			// Signal the event
			SetEvent(params->hDoneEvent);
		}
	}
}

//---------------------------------------------------------------------------------------
//			Read Write Stress Thread routines
//---------------------------------------------------------------------------------------

/*! ReadWriteThreadProc
 *
 * \brief Write or Read Thread(s) routine
 * \param params
 * \return None.
 */
VOID	
ReadWriteThreadProc(
	PTHREAD_PARAMS	params
)
{
    DWORD			dwRes;

	PrepareTransfer(params);

	if (params->WriteDirection)
	{
		// Prime the pump by doing an overlapped write
		WriteOverlappedCallback(0, 0, (LPOVERLAPPED) &params->os);
	}
	else
	{
		// Prime the pump by doing an overlapped read
		ReadOverlappedCallback(0, 0, (LPOVERLAPPED) &params->os);
	}

	/*
	 *	Operation issued, allow the callback to get called
	 *	The following do/while loop waits until the overlapped call completes
	 *	and does a SetEvent.
	 */
	do 
	{
		// See if we are still supposed to run, another thread may want to shut us down.
		if (!params->Running)
		{
			break;
		}
		dwRes = WaitForSingleObjectEx(params->hDoneEvent, 1000, TRUE);
		switch (dwRes) 
		{
			case WAIT_IO_COMPLETION:
				if (params->bDebug)
				{
					printf("WaitForSingleObjectEx - WAIT_IO_COMPLETION\n");
				}
				/*
				 *	WAIT_IO_COMPLETION means that a completion routine was called.
				 *	We still want to wait for the Overlapped I/O routine to complete
				 *	so we will just loop here until it completes (dwRes == 0)
				 */
				break;
			case WAIT_TIMEOUT:
				if (params->bDebug)
				{
					printf("WaitForSingleObjectEx - WAIT_TIMEOUT\n");
				}
				//if (params->bStopOnError)
				//	params->Running = FALSE;
				break;
			case WAIT_FAILED:
				if (params->bDebug)
				{
					printf("Error %d: Write WaitForSingleObjectEx\n", GetLastError());
				}
				params->iPass = GetLastError();
				params->Running = FALSE;
				break;
			case WAIT_OBJECT_0:
				params->Running = FALSE;
				if (params->bDebug)
				{
					printf("WaitForSingleObjectEx - WAIT_OBJECT_0\n");
				}
				break;
			default:
				if (params->bDebug)
				{
					printf("WaitForSingleObjectEx - dwRes = 0x%x\n", dwRes);
				}
				break;
		}
	} while (dwRes == WAIT_IO_COMPLETION);

	if (params->bDebug)
	{
		printf ("*****Thread 0x%lx, shutting down successfully\n", params->hThreadHandle);
	}
	params->Running = FALSE;
	Sleep(10);
}

/* WRLoopbackThreadProc
 *
 * \brief Windows Write, Read, Compare Loop Stress Thread
 * \param pparams
 * \return None
 */
VOID	
WRLoopbackThreadProc(
	PTHREAD_PARAMS	pparams
)
{
	PTHREAD_PARAMS	ReadParams;
	PTHREAD_PARAMS	WriteParams;
	DWORD			BytesWritten;
	DWORD			BytesRead = 0;
	DWORD			LastErrorStatus;
	UINT32			status = 0;

	ReadParams	= pparams;
	pparams++;
	WriteParams = pparams;

	WriteParams->TransferSize	= WriteParams->MaxTransferSize;
	WriteParams->CardOffset		= WriteParams->CardOffsetMax;

	for (UINT64 i = 0; i < WriteParams->Iterations; i++)
	{
		PrepareTransfer(WriteParams);
		BytesWritten = 0;
		
		// TEST ONLY
		//WriteRandomBuffer(WriteParams, WriteParams->TransferSize);

		if (!WriteFile(WriteParams->hWriteDevice, WriteParams->Buffer, WriteParams->TransferSize, 
			NULL, (LPOVERLAPPED)&WriteParams->os)) 
		{
			LastErrorStatus = GetLastError();
			if (LastErrorStatus == ERROR_IO_PENDING)
			{
				// Wait here (forever) for the Overlapped I/O to complete
				if (!GetOverlappedResult(WriteParams->hWriteDevice, &WriteParams->os, &BytesWritten, TRUE))
				{
					LastErrorStatus = GetLastError();
					printf("WriteFile Overlapped failed. Error = %d\n", LastErrorStatus);
					status = LastErrorStatus;
				}
			}
			else
			{
				// ioctl failed
				printf("WriteFile failed, Error = %d\n", LastErrorStatus);
				status = LastErrorStatus;
			}
		}

		// Check the status
		if (status != 0)
		{
			WriteParams->iPass = DisplayErrorInfo(WriteParams->bQuiet, status);
			// Break out of the loop and cleanup
			break;
		}
		WriteParams->TotalByteCountTransferred += (UINT64)BytesWritten;
		WriteParams->TransferCount++;

		ReadParams->TransferSize = WriteParams->TransferSize;

		// TEST ONLY
		// Overwrite the buffer to make sure it gets changed by the read.
		memset(ReadParams->Buffer, 0xFF, ReadParams->TransferSize);

		ReadParams->os.OffsetHigh = (DWORD)((WriteParams->CardOffset >> 32) & 0xFFFFFFFF);
		ReadParams->os.Offset = (DWORD)(WriteParams->CardOffset & 0xFFFFFFFF);

		if (!ReadFile(ReadParams->hReadDevice, ReadParams->Buffer, ReadParams->TransferSize, 
				NULL, (LPOVERLAPPED)&ReadParams->os)) 
		{
			LastErrorStatus = GetLastError();
			if (LastErrorStatus == ERROR_IO_PENDING)
			{
				// Wait here (forever) for the Overlapped I/O to complete
				if (!GetOverlappedResult(ReadParams->hReadDevice, &ReadParams->os, &BytesRead, TRUE))
				{
					LastErrorStatus = GetLastError();
					printf("ReadFile Overlapped failed. Error = %d\n", LastErrorStatus);
					status = LastErrorStatus;
				}
			}
			else
			{
				// ioctl failed
				printf("ReadFile failed, Error = %d\n", LastErrorStatus);
				status = LastErrorStatus;
			}
		}
		// Check the status
		if (status != 0)
		{
			ReadParams->iPass = DisplayErrorInfo(ReadParams->bQuiet, status);
			// Break out of the loop and cleanup
			break;
		}
		else
		{
			ReadParams->TotalByteCountTransferred += (UINT64)BytesRead;
			ReadParams->TransferCount++;
			// Make sure the length is valid
			if (ReadParams->TransferSize != 0)
			{
				if (WriteParams->TransferSize != ReadParams->TransferSize)
				{
					printf("Receive Size mismatch, Write Value: %ld, Read Value: %ld\n", WriteParams->TransferSize, ReadParams->TransferSize);
				}
				for (UINT32 t = 0; t < WriteParams->TransferSize; t++)
				{
					if (ReadParams->Buffer[t] != WriteParams->Buffer[t])
					{
						if (!ReadParams->bQuiet)
						{
							printf("Data mistatch at offset %ld, Write Value: 0x%x, Read Value: 0x%x\n",
									t, WriteParams->Buffer[t], ReadParams->Buffer[t]);
						}
						ReadParams->iPass = ERROR_TRANSFER_DATA_MISMATCH;
						if (ReadParams->bStopOnError)
						{
							// stop here
							ReadParams->Running = FALSE;
							break;
						}
					}
				}
				if (ReadParams->iPass == 0)
				{
					if (ReadParams->bDebug)
					{
						printf ("Read length of %ld, Read matched Write.\n", ReadParams->TransferSize);
						DisplayFormattedBytes(ReadParams->bQuiet, 0, 
							ReadParams->Buffer, ReadParams->TransferSize);
					}
				}
				else
				{
					if (ReadParams->bDebug)
					{
						printf ("Write Buffer:\n");
						DisplayFormattedBytes(WriteParams->bQuiet, 0, 
							WriteParams->Buffer, WriteParams->TransferSize);
						printf ("Read Buffer:\n");
						DisplayFormattedBytes(ReadParams->bQuiet, 0, 
							ReadParams->Buffer, ReadParams->TransferSize);
					}
				}
			}
			else
			{
				if (!ReadParams->bQuiet)
				{
					printf("Invalid transfer recieved, Length=%ld\n", ReadParams->TransferSize);
				}
				ReadParams->iPass = ERROR_TRANSFER_COUNT_MISMATCH;
			}
			if (ReadParams->bDebug)
			{
				if ((ReadParams->TransferCount % 100) == 0)
				{
					printf ("Read/Written %ld\n", ReadParams->TransferCount);
				}
			}
		}
		// See if we are still supposed to run, another thread may want to shut us down.
		if (!WriteParams->Running || !ReadParams->Running)
		{
			break;
		}
	}
	if (ReadParams->bDebug)
	{
		printf ("*****Test shutting down successfully.\n");
	}
	WriteParams->Running = FALSE;
	ReadParams->Running  = FALSE;
	Sleep(10);
}

/*! FIFOc2sThreadProc
 *
 * \brief FIFO C2S Thread routine
 * \param params
 * \return None
 */
VOID 
FIFOc2sThreadProc(
	PTHREAD_PARAMS		params
)
{
	PFIFO_C2S_STRUCT	pFIFOc2s;
	DWORD				ReqSize;
	DWORD				LastErrorStatus;
	PUINT8				Buffer;
	UINT32				Offset;
	UINT32				Length;
	DWORD				bytesReturned		= 0;
	UINT32				ReleaseElementCount = 0;
	UINT32				TotalTransferCount	= 0;
	UINT32				t;

	if (params->FIFOBuffer == NULL)
	{
		printf("FIFOc2sThreadProc FIFOBuffer has not been setup, exiting\n");
		return;
	}

	ReqSize = sizeof(FIFO_C2S_STRUCT) + (MAX_NUMBER_FIFO_ENTRIES * sizeof(FIFOC2S_ENTRY_STRUCT));
	pFIFOc2s = (PFIFO_C2S_STRUCT)malloc(ReqSize);
	if (pFIFOc2s == NULL)
	{
		printf("FIFOc2sThreadProc FIFO request could not be allocated, exiting\n");
		return;
	}

	params->ZeroPacketCount = 0;
	params->FullPacketCount = 0;
	params->PartialPacketCount = 0;

	do 
	{
		// See if we are still supposed to run, another thread may want to shut us down.
		if (!params->Running)
		{
			break;
		}
		if (params->bDebug)
		{
			//printf ("Thread 0x%lx, Set to receive %ld packets.\n", params->hThreadHandle, params->Iterations);
		}

		// (re)set the structure calling parameters
		pFIFOc2s->AvailNumEntries	= MAX_NUMBER_FIFO_ENTRIES;
		pFIFOc2s->RetNumEntries		= 0;
		pFIFOc2s->ChannelNum		= (UINT16)params->ReadDMAChannel;
		pFIFOc2s->ReleaseCount		= ReleaseElementCount;

		LastErrorStatus = 0;
		// Send the FIFO Receives structure down to the driver to fill out
		if (!DeviceIoControl(params->hReadDevice, FIFO_C2S_IOCTL,
			NULL, 0,
			(LPVOID)pFIFOc2s, ReqSize,
			&bytesReturned, &params->os))
		{
			LastErrorStatus = GetLastError();
			if (LastErrorStatus == ERROR_IO_PENDING)
			{
				// Wait here (forever) for the Overlapped I/O to complete
				if (!GetOverlappedResult(params->hReadDevice, 
										&params->os, 
										&bytesReturned, 
										TRUE))
				{
					LastErrorStatus = GetLastError();
					if (params->Running)
					{
						printf("FIFOReadThreadProc GetOverlapped call failed. Error = %d\n", LastErrorStatus);
						params->Running = FALSE;
						break;
					}
				}
				else
				{
					LastErrorStatus = 0;
				}
			}
			else
			{
				// ioctl failed
				LastErrorStatus = GetLastError();
				if (params->Running)
				{
					printf("FIFOReadThreadProc IOCTL call failed. Error = %d\n", LastErrorStatus);
					params->Running = FALSE;
					break;
				}
			}
		}
		// Check the status
		if (LastErrorStatus != 0)
		{
			params->iPass = DisplayErrorInfo(params->bQuiet, LastErrorStatus);
			break;
		}
		if (params->bDebug)
		{
			printf ("FIFO C2S returned %d transfers for thread 0x%lx ...\n", pFIFOc2s->RetNumEntries, params->hThreadHandle);
			if (pFIFOc2s->ChannelStatus)
			{
				if (params->bDebug)
				{
					printf ("FIFO C2S returned DMA Engine error: 0x%x.\n", pFIFOc2s->ChannelStatus);
				}
				if (params->bStopOnError)
				{
					// Exit the loop and cleanup
					params->Running = FALSE;
					break;
				}
			}
		}

		if (pFIFOc2s->ChannelStatus == STAT_DMA_OVERRUN_ERROR)
		{
			params->DMAOverrunCount++;
		}
		if (pFIFOc2s->ChannelStatus == STAT_ERROR_MALFORMED)
		{
			params->ElementErrorCount++;
		}
		// Special Statistics
		if (pFIFOc2s->RetNumEntries == 0)
			params->ZeroPacketCount++;
		else if (pFIFOc2s->RetNumEntries == pFIFOc2s->AvailNumEntries)
			params->FullPacketCount++;
		else
			params->PartialPacketCount++;

		ReleaseElementCount = 0;
		TotalTransferCount += pFIFOc2s->RetNumEntries;
		for (t = 0; t < pFIFOc2s->RetNumEntries; t++)
		{
			Offset = pFIFOc2s->Transfers[t].Offset;
			Length = pFIFOc2s->Transfers[t].Length;

			// Keep a count of the total number of elements to release
			ReleaseElementCount += pFIFOc2s->Transfers[t].NumberElements;

			if (params->bDebug)
			{
				printf("FIFO C2S entry %d (Offset 0x%lx) of size %ld on thread 0x%lx\n", 
					t, Offset, Length, params->hThreadHandle);
			}
			// Make sure the Buffer pointer and length are valid
			if ((Offset < params->AllocSize) && 
				(Length != 0) && 
				(pFIFOc2s->Transfers[t].Status == 0))
			{
				params->TransferCount++;
				params->TotalByteCountTransferred += (UINT64)Length;
				Buffer = params->FIFOBuffer + Offset;
				if (params->bDebug)
				{
					printf("FIFOBuffer 0x%p, Buffer 0x%p, Offset 0x%lx\n", 
						params->FIFOBuffer, Buffer, Offset);
				}
				if (!ValidateDataBuffer(params, Buffer, Length))
				{	
					if (params->bDebug)
					{
						printf("FIFO C2S Transfer entry %d (Buffer 0x%p) of size %ld on thread 0x%lx\n", 
								t, Buffer, Length, params->hThreadHandle);
					}
					if (params->bStopOnError)
					{
						// Exit the loop and cleanup
						params->Running = FALSE;
						break;
					}
				}
			}
			else
			{
				if (!params->bQuiet)
				{
					printf("Invalid Transfer recieved, Offset=0x%lx, Length=%ld, Status=0x%x\n", 
							Offset, Length, pFIFOc2s->Transfers[t].Status);
				}
				params->iPass = ERROR_TRANSFER_COUNT_MISMATCH;
				if (params->bStopOnError)
				{
					// Exit the loop and cleanup
					params->Running = FALSE;
					break;
				}
			}
		} // End of 'for' loop

		// Check to see if we are at or past our transfer count
		params->Iterations--;
	} while ((LastErrorStatus == 0) &&
			(params->Iterations != 0));

	// Free the transfer structure allocated above.
	free(pFIFOc2s);

	if (params->bDebug)
	{
		printf ("*****Thread 0x%lx, shutting down successfully.\n", params->hThreadHandle);
	}
	params->Running = FALSE;
	Sleep(10);
}



/*! FIFOs2cThreadProc
 *
 * \brief FIFO S2C Thread routine
 * \param params
 * \return None
 */
VOID 
FIFOs2cThreadProc(
	PTHREAD_PARAMS		params
)
{
	PFIFO_S2C_STRUCT	pFIFOs2c;
	DWORD				ReqSize;
	DWORD				LastErrorStatus;
	UINT32				Offset = 0;
	UINT32				Length = params->MaxTransferSize;
	DWORD				bytesReturned		= 0;
	UINT32				TotalTransferCount	= 0;
	UINT32				t;

	if (params->FIFOBuffer == NULL)
	{
		printf("FIFOs2cThreadProc FIFOBuffer has not been setup, exiting\n");
		return;
	}

	ReqSize = sizeof(FIFO_S2C_STRUCT) + (MAX_NUMBER_FIFO_ENTRIES * sizeof(FIFOS2C_ENTRY_STRUCT));
	pFIFOs2c = (PFIFO_S2C_STRUCT)malloc(ReqSize);
	if (pFIFOs2c == NULL)
	{
		printf("FIFOs2cThreadProc FIFO request could not be allocated, exiting\n");
		return;
	}

	params->ZeroPacketCount = 0;
	params->FullPacketCount = 0;
	params->PartialPacketCount = 0;
	pFIFOs2c->NumEntries = MAX_NUMBER_FIFOS2C_ENTRIES;

	do 
	{
		// See if we are still supposed to run, another thread may want to shut us down.
		if (!params->Running)
		{
			break;
		}

		// (re)set the structure calling parameters
		pFIFOs2c->ChannelNum		= (UINT16)params->WriteDMAChannel;

		TotalTransferCount += pFIFOs2c->NumEntries;
		for (t = 0; t < pFIFOs2c->NumEntries; t++)
		{
			pFIFOs2c->Transfers[t].Offset = Offset;
			pFIFOs2c->Transfers[t].Length = Length;

			if (params->bDebug)
			{
				printf("FIFO S2C entry %d (Offset 0x%lx) of size %ld on thread 0x%lx\n", 
					t, Offset, Length, params->hThreadHandle);
			}

			Offset += Length;
			if (Offset > params->AllocSize) 
			{
				Offset = 0;
			}
		} // End of 'for' loop

		LastErrorStatus = 0;
		// Send the FIFO Receives structure down to the driver to fill out
		if (!DeviceIoControl(params->hWriteDevice, FIFO_S2C_IOCTL,
			NULL, 0,
			(LPVOID)pFIFOs2c, ReqSize,
			&bytesReturned, &params->os))
		{
			LastErrorStatus = GetLastError();
			if (LastErrorStatus == ERROR_IO_PENDING)
			{
				// Wait here (forever) for the Overlapped I/O to complete
				if (!GetOverlappedResult(params->hWriteDevice, 
										&params->os, 
										&bytesReturned, 
										TRUE))
				{
					LastErrorStatus = GetLastError();
					if (params->Running)
					{
						printf("FIFOs2cThreadProc GetOverlapped call failed. Error = %d\n", LastErrorStatus);
						params->Running = FALSE;
						break;
					}
				}
				else
				{
					LastErrorStatus = 0;
				}
			}
			else
			{
				// ioctl failed
				LastErrorStatus = GetLastError();
				if (params->Running)
				{
					printf("FIFOs2cThreadProc IOCTL call failed. Error = %d\n", LastErrorStatus);
					params->Running = FALSE;
					break;
				}
			}
		}
		// Check the status
		if (LastErrorStatus != 0)
		{
			params->iPass = DisplayErrorInfo(params->bQuiet, LastErrorStatus);
			break;
		}
		else
		{
			// Special Statistics
			if (pFIFOs2c->NumEntriesSent == 0)
				params->ZeroPacketCount++;
			else if (pFIFOs2c->NumEntries == pFIFOs2c->NumEntriesSent)
				params->FullPacketCount++;
			else
				params->PartialPacketCount++;

			for (t = 0; t < pFIFOs2c->NumEntriesSent; t++)
			{
				params->TotalByteCountTransferred += (UINT64)pFIFOs2c->Transfers[t].Length;
				params->TransferCount++;
			} // End of 'for' loop
		}
		params->Iterations--;
	} while ((LastErrorStatus == 0) &&
			(params->Iterations != 0));

	// Free the transfer structure allocated above.
	free(pFIFOs2c);

	if (params->bDebug)
	{
		printf ("*****Thread 0x%lx, shutting down successfully.\n", params->hThreadHandle);
	}
	params->Running = FALSE;
	Sleep(10);
}


/*! DrainFIFOc2s
 *
 * \brief 
 * \param params
 * \return None
 */
VOID 
DrainFIFOc2s(
	PTHREAD_PARAMS		params
)
{
	PFIFO_C2S_STRUCT	pFIFOc2s;
	DWORD				ReqSize;
	DWORD				bytesReturned		= 0;

	if (params->FIFOBuffer == NULL)
	{
		printf("DrainFIFOc2s FIFOBuffer has not been setup, exiting\n");
		return;
	}

	ReqSize = sizeof(FIFO_C2S_STRUCT) + (1 * sizeof(FIFOC2S_ENTRY_STRUCT));
	pFIFOc2s = (PFIFO_C2S_STRUCT)malloc(ReqSize);
	if (pFIFOc2s == NULL)
	{
		printf("DrainFIFOc2s FIFO request could not be allocated, exiting\n");
		return;
	}

	do {
		// (re)set the structure calling parameters
		pFIFOc2s->AvailNumEntries	= 1;
		pFIFOc2s->RetNumEntries		= 0;
		pFIFOc2s->ChannelNum		= (UINT16)params->ReadDMAChannel;
		pFIFOc2s->ReleaseCount		= 0;

		// Send the FIFO Receives structure down to the driver to fill out
		if (!DeviceIoControl(params->hReadDevice, FIFO_C2S_IOCTL,
			NULL, 0,
			(LPVOID)pFIFOc2s, ReqSize,
			&bytesReturned, &params->os))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				// Wait here (forever) for the Overlapped I/O to complete
				GetOverlappedResult(params->hReadDevice, 
										&params->os, 
										&bytesReturned, 
										TRUE);
			}
		}
	} while (pFIFOc2s->RetNumEntries != 0);
	// Free the transfer structure allocated above.
	free(pFIFOc2s);
}

/*! DrainFIFOs2c
 *
 * \brief Drain FIFO S2C
 * \param params
 * \return None
 */
VOID 
DrainFIFOs2c(
	PTHREAD_PARAMS		params
)
{
	PFIFO_S2C_STRUCT	pFIFOs2c;
	DWORD				ReqSize;
	DWORD				bytesReturned		= 0;

	if (params->FIFOBuffer == NULL)
	{
		printf("DrainFIFOs2c FIFOBuffer has not been setup, exiting\n");
		return;
	}

	ReqSize = sizeof(FIFO_S2C_STRUCT) + (1 * sizeof(FIFOS2C_ENTRY_STRUCT));
	pFIFOs2c = (PFIFO_S2C_STRUCT)malloc(ReqSize);
	if (pFIFOs2c == NULL)
	{
		printf("DrainFIFOs2c FIFO request could not be allocated, exiting\n");
		return;
	}

	do {
		pFIFOs2c->NumEntries = 1;
		// (re)set the structure calling parameters
		pFIFOs2c->ChannelNum		= (UINT16)params->WriteDMAChannel;
		pFIFOs2c->Transfers[0].Offset = 0;
		pFIFOs2c->Transfers[0].Length = 4096;
		pFIFOs2c->NumEntriesSent = 0;

		// Send the FIFO Receives structure down to the driver to fill out
		if (!DeviceIoControl(params->hWriteDevice, FIFO_S2C_IOCTL,
			NULL, 0,
			(LPVOID)pFIFOs2c, ReqSize,
			&bytesReturned, &params->os))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				// Wait here (forever) for the Overlapped I/O to complete
				GetOverlappedResult(params->hWriteDevice, 
										&params->os, 
										&bytesReturned, 
										TRUE);
			}
		}
	} while (pFIFOs2c->NumEntriesSent != 0);
	// Free the transfer structure allocated above.
	free(pFIFOs2c);
}


/*! UserIntThreadProc
 *
 * \brief User Interrupt Thread routine
 * \param pcfgParams
 * \return None
 */
VOID	
UserIntThreadProc(
	PCFG_PARAMS			pcfgParams
)
{
	UINT32				Status = 0;

	for (;;)			// Loop forever.
	{
		Status = waitUserInterrupt(pcfgParams);
		if (!((Status == 6) || (Status == 995)))
		{
			printf("UserIntThreadProc: waitUserInterrupt returned = %d\n", Status);
		}
	}
}


#else // Linux

//---------------------------------------------------------------------------------------
//			Linux Read and Write I/O routines
//---------------------------------------------------------------------------------------

/*! ReadWriteThreadProc

 * \brief Linux Write or Read Thread(s) routine
 * \param params
 * \return None
 */
VOID	
ReadWriteThreadProc(
	PTHREAD_PARAMS	params
)
{
    ssize_t         BytesXfer = 0;

	params->CardOffset				  = params->CardOffsetMax;
    params->TotalByteCountTransferred = 0;

	if (params->WriteDirection)
	{
	    do 
	    {
		    // See if we are still supposed to run, another thread may want to shut us down.
		    if (!params->Running)
		    {   
			    break;
		    }

            //.. fix the offset            
            lseek(params->hWriteDevice, params->os, SEEK_SET);
            // Do the Blocked write...
            BytesXfer = write(
							params->hWriteDevice, 
							params->Buffer, 
							params->TransferSize);
            if (BytesXfer == -1)
            {
                BytesXfer = 0;
				// error from the Write
				params->iPass = DisplayErrorInfo(params->bQuiet, errno);
				if (params->bStopOnError)
				{
					// stop here
					params->Running = FALSE;
					break;
				}
            }
   		    params->TotalByteCountTransferred += (UINT64)BytesXfer;
			params->TransferCount++;
            PrepareTransfer(params);
		    params->Iterations--;
	    } while (params->Iterations);
		if (params->bDebug)
		{
		    printf("Write Finished\n");
		}
		params->Running = FALSE;
	}
	else
	{
	    do 
	    {
		    // See if we are still supposed to run, another thread may want to shut us down.
		    if (!params->Running)
		    {   
			    break;
		    }
            //.. fix the offset
            lseek(params->hReadDevice, params->os, SEEK_SET);
            // Do the Blocked read...
            BytesXfer = read(params->hReadDevice, params->Buffer, params->TransferSize);
            if (BytesXfer == -1)
            {
                BytesXfer = 0;
				// error from the Read
				params->iPass = DisplayErrorInfo(params->bQuiet, errno);
				if (params->bStopOnError)
				{
					// stop here
					params->Running = FALSE;
					break;
				}
            }
   		    params->TotalByteCountTransferred += (UINT64)BytesXfer;
			params->TransferCount++;
		    if (params->bDebug)
		    {
			    printf("Transferred %zd bytes, Read Buffer:\n", BytesXfer);
			    if (BytesXfer > 0)
                {
				    DisplayFormattedBytes(
									params->bQuiet, 
									0, 
									params->Buffer, 
									BytesXfer);
			    }
		    }
            PrepareTransfer(params);
		    params->Iterations--;
	    } while (params->Iterations);
		if (params->bDebug)
		{
		    printf("Read Finished\n");
		}
		params->Running = FALSE;
	} // if read...

	if (params->bDebug)
	{
		printf ("*****Thread 0x%lx, shutting down successfully\n", params->hThreadHandle);
	}
	params->Running = FALSE;
	Sleep(10);
}

/*! WRLoopbackThreadProc
 * 
 * \brief Linux Write, Read, Compare Loop Stress Thread
 * \param ppparms
 * \return None
 */
VOID	
WRLoopbackThreadProc(
	PTHREAD_PARAMS	pparams
)
{
	PTHREAD_PARAMS	ReadParams;
	PTHREAD_PARAMS	WriteParams;
	ssize_t			BytesWritten;
	ssize_t			BytesRead;
	UINT32			Mode = 0;
	UINT32			status;

	ReadParams	=	pparams;
	pparams++;
	WriteParams =	pparams;

	WriteParams->TransferSize = WriteParams->MaxTransferSize;
	WriteParams->CardOffset	  = WriteParams->CardOffsetMax;

	for (UINT64 i = 0; i < WriteParams->Iterations; i++)
	{
		PrepareTransfer(WriteParams);
		BytesWritten = 0;

        //.. Fix the offset
        lseek(WriteParams->hWriteDevice, 
			WriteParams->os, 
			SEEK_SET);
        // Do the Blocked write...
        BytesWritten = write(
						WriteParams->hWriteDevice, 
						WriteParams->Buffer, 
						WriteParams->TransferSize);
        if (BytesWritten == -1)
        {
            BytesWritten = 0;
			// error from the Write
			WriteParams->iPass = DisplayErrorInfo(WriteParams->bQuiet, errno);
			if (WriteParams->bStopOnError)
			{
				// stop here
				WriteParams->Running = FALSE;
				break;
			}
        }
		WriteParams->TotalByteCountTransferred += (UINT64)BytesWritten;
		WriteParams->TransferCount++;

		ReadParams->TransferSize = WriteParams->TransferSize;

		// Overwrite the buffer to make sure it gets changed by the read.
    	if (ReadParams->DataPatternType == PATTERN_RANDOM)
    	{
	    	memset(ReadParams->Buffer, 0xFF, ReadParams->TransferSize);
        }

		ReadParams->os = WriteParams->CardOffset;

        // .. fix the offset        
        lseek(ReadParams->hReadDevice, ReadParams->os, SEEK_SET);
        // Do the Blocked read...
        BytesRead = read(ReadParams->hReadDevice, ReadParams->Buffer, ReadParams->TransferSize);
        if (BytesRead == -1)
        {
            BytesRead = 0;
			// error from the Read
			ReadParams->iPass = DisplayErrorInfo(ReadParams->bQuiet, errno);
			if (ReadParams->bStopOnError)
			{
				// stop here
				ReadParams->Running = FALSE;
				break;
			}
        }
		else
		{
			ReadParams->TotalByteCountTransferred += (UINT64)BytesRead;
			ReadParams->TransferCount++;
			// Make sure the length is valid
			if (ReadParams->TransferSize != 0)
			{
				if (WriteParams->TransferSize != ReadParams->TransferSize)
				{
					printf("Receive Size mismatch, Write Value: %d, Read Value: %d\n", WriteParams->TransferSize, ReadParams->TransferSize);
				}
				for (UINT32 t = 0; t < WriteParams->TransferSize; t++)
				{
					if (ReadParams->Buffer[t] != WriteParams->Buffer[t])
					{
						if (!ReadParams->bQuiet)
						{
							printf("Data mistatch at offset %d, Write Value: 0x%x, Read Value: 0x%x\n",
									t, WriteParams->Buffer[t], ReadParams->Buffer[t]);
						}
						ReadParams->iPass = ERROR_TRANSFER_DATA_MISMATCH;
						if (ReadParams->bStopOnError)
						{
							// stop here
							ReadParams->Running = FALSE;
							break;
						}
					}
				}
				if (ReadParams->iPass == 0)
				{
					if (ReadParams->bDebug)
					{
						printf ("Read length of %d, Read matched Write.\n", ReadParams->TransferSize);
						DisplayFormattedBytes(
											ReadParams->bQuiet, 
											0, 
											ReadParams->Buffer, 
											ReadParams->TransferSize);
					}
				}
				else
				{
					if (ReadParams->bDebug)
					{
						printf ("Write Buffer:\n");
						DisplayFormattedBytes(
											WriteParams->bQuiet, 
											0, 
											WriteParams->Buffer, 
											WriteParams->TransferSize);
						printf ("Read Buffer:\n");
						DisplayFormattedBytes(
											ReadParams->bQuiet, 
											0, 
											ReadParams->Buffer, 
											ReadParams->TransferSize);
					}
				}
			}
			else
			{
				if (!ReadParams->bQuiet)
				{
					printf("Invalid transfer recieved, Length=%d\n", ReadParams->TransferSize);
				}
				ReadParams->iPass = ERROR_TRANSFER_COUNT_MISMATCH;
			}
			if (ReadParams->bDebug)
			{
				if ((ReadParams->TransferCount % 100) == 0)
				{
					printf ("Read/Written %d\n", ReadParams->TransferCount);
				}
			}
		}
		// See if we are still supposed to run, another thread may want to shut us down.
		if (!WriteParams->Running || !ReadParams->Running)
		{
			break;
		}
	}
	if (ReadParams->bDebug)
	{
		printf ("*****Test shutting down successfully\n");
	}
	WriteParams->Running = FALSE;
	ReadParams->Running = FALSE;
	Sleep(10);
}

/*! FIFOc2sThreadProc
 *
 * \brief FIFO C2S Thread routine
 * \param params
 * \return None
 */
VOID 
FIFOc2sThreadProc(
	PTHREAD_PARAMS		params
)
{
	PFIFO_C2S_STRUCT	pFIFOc2s;
	DWORD				ReqSize;
	DWORD				LastErrorStatus;
	PUINT8				Buffer;
	UINT32				Offset;
	UINT32				Length;
	DWORD				bytesReturned = 0;
	UINT32				ReleaseElementCount = 0;
	UINT32				TotalTransferCount = 0;
	UINT32				t;

	if (params->FIFOBuffer == NULL)
	{
		printf("FIFOc2sThreadProc FIFOBuffer has not been setup, exiting\n");
		return;
	}

	ReqSize = sizeof(FIFO_C2S_STRUCT) + (MAX_NUMBER_FIFO_ENTRIES * sizeof(FIFOC2S_ENTRY_STRUCT));
	pFIFOc2s = (PFIFO_C2S_STRUCT)malloc(ReqSize);
	if (pFIFOc2s == NULL)
	{
		printf("FIFOc2sThreadProc FIFO request could not be allocated, exiting\n");
		return;
	}

	do 
	{
		// See if we are still supposed to run, another thread may want to shut us down.
		if (!params->Running)
		{
			break;
		}
		if (params->bDebug)
		{
			//printf ("Thread 0x%lx, Set to receive %ld packets.\n", params->hThreadHandle, params->Iterations);
		}

		// (re)set the structure calling parameters
		pFIFOc2s->AvailNumEntries	= MAX_NUMBER_FIFO_ENTRIES;
		pFIFOc2s->RetNumEntries		= 0;
		pFIFOc2s->ChannelNum		= (UINT16)params->ReadDMAChannel;
		pFIFOc2s->ReleaseCount		= ReleaseElementCount;

		LastErrorStatus = 0;
		// Send the FIFO Receives structure down to the driver to fill out
        if (ioctl (params->hReadDevice, FIFO_C2S_IOCTL, pFIFOc2s) != 0)
        {
            printf("ioctl @ 1381: FIFO C2S NumEntries = %i\n", pFIFOc2s->AvailNumEntries);
    		// ioctl failed
    		if (params->Running)
			{
				printf("FIFOReadThreadProc IOCTL call failed. errno = %d\n", errno);
				break;
			}
            else
        	{
				params->Running = FALSE;
				break;
			}
	    }
        else
        {
            printf("ioctl @ 1396: FIFO C2S AvailNumEntries = %i\n", pFIFOc2s->AvailNumEntries);
            break;
        }
        
		if (params->bDebug)
		{
			printf ("FIFO C2S returned %d transfers for thread 0x%lx ...\n", pFIFOc2s->RetNumEntries, params->hThreadHandle);
			if (pFIFOc2s->ChannelStatus)
			{
				if (params->bDebug)
				{
					printf ("FIFO C2S returned DMA Channel error: 0x%x.\n", pFIFOc2s->ChannelStatus);
				}
				if (params->bStopOnError)
				{
					// Exit the loop and cleanup
					params->Running = FALSE;
					break;
				}
			}
		}

		if (pFIFOc2s->ChannelStatus == STAT_DMA_OVERRUN_ERROR)
		{
			params->DMAOverrunCount++;
		}
		if (pFIFOc2s->ChannelStatus == STAT_ERROR_MALFORMED)
		{
			params->ElementErrorCount++;
		}
		// Special Statistics
		if (pFIFOc2s->RetNumEntries == 0)
			params->ZeroPacketCount++;
		else if (pFIFOc2s->RetNumEntries == pFIFOc2s->AvailNumEntries)
			params->FullPacketCount++;
		else
			params->PartialPacketCount++;

		ReleaseElementCount = 0;
		TotalTransferCount += pFIFOc2s->RetNumEntries;
		for (t = 0; t < pFIFOc2s->RetNumEntries; t++)
		{
			Offset = pFIFOc2s->Transfers[t].Offset;
			Length = pFIFOc2s->Transfers[t].Length;

			// Keep a count of the total number of elements to release
			ReleaseElementCount += pFIFOc2s->Transfers[t].NumberElements;

			if (params->bDebug)
			{
				printf("FIFO C2S entry %d (Offset 0x%x) of size %d on thread 0x%lx\n", 
					t, Offset, Length, params->hThreadHandle);
			}
			// Make sure the Buffer pointer and length are valid
			if ((Offset < params->AllocSize) && 
				(Length != 0) && 
				(pFIFOc2s->Transfers[t].Status == 0))
			{
				params->TransferCount++;
				params->TotalByteCountTransferred += (UINT64)Length;
				Buffer = params->FIFOBuffer + Offset;
				if (params->bDebug)
				{
					printf("FIFOBuffer 0x%p, Buffer 0x%p, Offset 0x%x\n", 
						params->FIFOBuffer, Buffer, Offset);
				}
				if (!ValidateDataBuffer(params, Buffer, Length))
				{	
					if (params->bDebug)
					{
						printf("FIFO C2S Transfer entry %d (Buffer 0x%p) of size %d on thread 0x%lx\n", 
								t, Buffer, Length, params->hThreadHandle);
					}
					if (params->bStopOnError)
					{
						// Exit the loop and cleanup
						params->Running = FALSE;
						break;
					}
				}
			}
			else
			{
				if (!params->bQuiet)
				{
					printf("Invalid Transfer recieved, Offset=0x%x, Length=%d, Status=0x%x\n", 
							Offset, Length, pFIFOc2s->Transfers[t].Status);
				}
				params->iPass = ERROR_TRANSFER_COUNT_MISMATCH;
				if (params->bStopOnError)
				{
					// Exit the loop and cleanup
					params->Running = FALSE;
					break;
				}
			}
		} // for loop
		// Check to see if we are at or past out trasfer count
		if (TotalTransferCount >= params->Iterations)
		{
			//printf ("Reached Transfer count, stopping thread.\n");
			break;
		}
		params->Iterations--;
	} while ((LastErrorStatus == 0) &&
			(params->Iterations != 0));

	// Free the transfer structure allocated above.
	free(pFIFOc2s);

	if (params->bDebug)
	{
		printf ("*****Thread 0x%lx, shutting down successfully\n", params->hThreadHandle);
	}
	params->Running = FALSE;
	Sleep(10);
}


/*! FIFOs2cThreadProc
 *
 * \brief FIFO S2C Thread routine
 * \param params
 * \return None
 */
VOID 
FIFOs2cThreadProc(
	PTHREAD_PARAMS		params
)
{
	PFIFO_S2C_STRUCT	pFIFOs2c;
	DWORD				ReqSize;
	DWORD				LastErrorStatus;
	PUINT8				Buffer;
	UINT32				Offset = 0;
	UINT32				Length = params->MaxTransferSize;
	DWORD				bytesReturned = 0;
	UINT32				ReleaseElementCount = 0;
	UINT32				TotalTransferCount = 0;
	UINT32				t;

	if (params->FIFOBuffer == NULL)
	{
		printf("FIFOs2cThreadProc FIFOBuffer has not been setup, exiting\n");
		return;
	}

	ReqSize = sizeof(FIFO_S2C_STRUCT) + (MAX_NUMBER_FIFO_ENTRIES * sizeof(FIFOS2C_ENTRY_STRUCT));
	pFIFOs2c = (PFIFO_S2C_STRUCT)malloc(ReqSize);
	if (pFIFOs2c == NULL)
	{
		printf("FIFOs2cThreadProc FIFO request could not be allocated, exiting\n");
		return;
	}

	params->ZeroPacketCount = 0;
	params->FullPacketCount = 0;
	params->PartialPacketCount = 0;
	pFIFOs2c->NumEntries = MAX_NUMBER_FIFOS2C_ENTRIES;

	do 
	{
		// See if we are still supposed to run, another thread may want to shut us down.
		if (!params->Running)
		{
			break;
		}
		if (params->bDebug)
		{
			//printf ("Thread 0x%lx, Set to receive %ld packets.\n", params->hThreadHandle, params->Iterations);
		}

		// (re)set the structure calling parameters
		pFIFOs2c->NumEntries	= MAX_NUMBER_FIFOS2C_ENTRIES;
		pFIFOs2c->ChannelNum	= (UINT16)params->WriteDMAChannel;

		for (t = 0; t < pFIFOs2c->NumEntries; t++)
		{
			pFIFOs2c->Transfers[t].Offset = Offset;
			pFIFOs2c->Transfers[t].Length = Length;

			Offset += Length;
			if (Offset > params->AllocSize) 
			{
				Offset = 0;
			}
		} // for loop

		LastErrorStatus = 0;
		// Send the FIFO Receives structure down to the driver to fill out
        if (ioctl (params->hWriteDevice, FIFO_S2C_IOCTL, pFIFOs2c) != 0)
        {
    		// ioctl failed
    		if (params->Running)
			{
				printf("FIFOs2cThreadProc IOCTL call failed. errno = %d\n", errno);
				params->Running = FALSE;
                LastErrorStatus = -EIO;
				break;
			}
	    }
		else
		{
			// Special Statistics
			if (pFIFOs2c->NumEntriesSent == 0)
				params->ZeroPacketCount++;
			else if (pFIFOs2c->NumEntries == pFIFOs2c->NumEntriesSent)
				params->FullPacketCount++;
			else
				params->PartialPacketCount++;

			for (t = 0; t < pFIFOs2c->NumEntriesSent; t++)
			{
				params->TotalByteCountTransferred += (UINT64)pFIFOs2c->Transfers[t].Length;
				params->TransferCount++;
			} // End of 'for' loop
		}

		// Check to see if we are at or past out trasfer count
		if (TotalTransferCount >= params->Iterations)
		{
			//printf ("Reached Transfer count, stopping thread.\n");
			break;
		}
        params->Iterations--;
	} while ((LastErrorStatus == 0) && (params->Iterations != 0));

	// Free the transfer structure allocated above.
	free(pFIFOs2c);

	if (params->bDebug)
	{
		printf ("*****Thread 0x%lx, shutting down successfully\n", params->hThreadHandle);
	}
	params->Running = FALSE;
	Sleep(10);
}


/*! DrainFIFOc2s
 *
 * \brief 
 * \param params
 * \return None
 */
VOID 
DrainFIFOc2s(
	PTHREAD_PARAMS		params
)
{
	PFIFO_C2S_STRUCT	pFIFOc2s;
	DWORD				ReqSize;
	DWORD				bytesReturned		= 0;

	if (params->FIFOBuffer == NULL)
	{
		printf("DrainFIFOc2s FIFOBuffer has not been setup, exiting\n");
		return;
	}

	ReqSize = sizeof(FIFO_C2S_STRUCT) + (1 * sizeof(FIFOC2S_ENTRY_STRUCT));
	pFIFOc2s = (PFIFO_C2S_STRUCT)malloc(ReqSize);
	if (pFIFOc2s == NULL)
	{
		printf("DrainFIFOc2s FIFO request could not be allocated, exiting\n");
		return;
	}

	do {
		// (re)set the structure calling parameters
		pFIFOc2s->AvailNumEntries	= 1;
		pFIFOc2s->RetNumEntries		= 0;
		pFIFOc2s->ChannelNum		= (UINT16)params->ReadDMAChannel;
		pFIFOc2s->ReleaseCount		= 0;

		// Send the FIFO Receives structure down to the driver to fill out
        if (ioctl (params->hReadDevice, FIFO_C2S_IOCTL, pFIFOc2s) != 0)
        {
            // ioctl failed
            break;
        }
	} while (pFIFOc2s->RetNumEntries != 0);
	// Free the transfer structure allocated above.
	free(pFIFOc2s);
}

/*! DrainFIFOs2c
 *
 * \brief Drain FIFO S2C
 * \param params
 * \return None
 */
VOID 
DrainFIFOs2c(
	PTHREAD_PARAMS		params
)
{
	PFIFO_S2C_STRUCT	pFIFOs2c;
	DWORD				ReqSize;
	DWORD				bytesReturned		= 0;
    UINT32				t;

	if (params->FIFOBuffer == NULL)
	{
		printf("DrainFIFOs2c FIFOBuffer has not been setup, exiting\n");
		return;
	}

	ReqSize = sizeof(FIFO_S2C_STRUCT) + (MAX_NUMBER_FIFOS2C_ENTRIES * sizeof(FIFOS2C_ENTRY_STRUCT));
	pFIFOs2c = (PFIFO_S2C_STRUCT)malloc(ReqSize);
	if (pFIFOs2c == NULL)
	{
		printf("DrainFIFOs2c FIFO request could not be allocated, exiting\n");
		return;
	}

	do {
		// (re)set the structure calling parameters
		pFIFOs2c->NumEntriesSent = 0;
        pFIFOs2c->NumEntries	= MAX_NUMBER_FIFOS2C_ENTRIES;
        pFIFOs2c->ChannelNum	= (UINT16)params->WriteDMAChannel;

        for (t = 0; t < pFIFOs2c->NumEntries; t++)
        {
            pFIFOs2c->Transfers[t].Offset = 0;
            pFIFOs2c->Transfers[t].Length = 256;
        }
		// Send the FIFO S2C structure down to the driver to fill out
        if (ioctl (params->hWriteDevice, FIFO_S2C_IOCTL, pFIFOs2c) != 0)
        {
            // ioctl failed
			printf("DrainFIFOs2c ioctl called failed, exiting\n");
            break;
		}
	} while (pFIFOs2c->NumEntriesSent != 0);
	// Free the transfer structure allocated above.
	free(pFIFOs2c);
}

#endif // Windows vs. Linux

