// -------------------------------------------------------------------------
// 
// PRODUCT:			Expresso Multi-Channel DMA Driver
// MODULE NAME:		WinUtils.cpp
// 
// MODULE DESCRIPTION: 
// 
// Contains Windows specific utility functions for the CLI application.
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

//------------------------------------------
// Global variables
//------------------------------------------

UINT64 g_idleTime;
UINT64 g_kernelTime;
UINT64 g_userTime;

GUID    GuidTable[] = {
    PUBLIC_GUID_EXPRESSO_DRIVER_INTERFACE,
};

/*! OpenDMAChannel
 *
 * \brief Returns the Handle to the request board and DMA Channel number
 * \param BoardNumber
 * \param DMAChannel
 * \return ConnectDriver(BoardNumber, &ChannelName[0])
 */
HANDLE 
OpenDMAChannel(
	UINT32		BoardNumber,
	INT32		DMAChannel
)
{
	CHAR		ChannelName[MAX_CHANNEL_NAME_SIZE];

	sprintf_s(&ChannelName[0], MAX_CHANNEL_NAME_SIZE, "DMA%d", DMAChannel);
	return ConnectDriver(BoardNumber, &ChannelName[0]);
}

/*! ConnectToBoard
 *
 * \brief This connects to the board you are interested in talking to.
 * \param BoardNumber
 * \param DMAChannelName
 * \return hDevice
 */
HANDLE 
ConnectDriver(
	UINT32			BoardNumber,
	PCHAR			DMAChannelName
)
{
	HDEVINFO		hDevInfo;
	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData;
	HANDLE			hDevice			= INVALID_HANDLE_VALUE;
	UINT32			count			= 0;
	DWORD			RequiredSize	= 0;
	BOOL			BoolStatus		= TRUE;
	wchar_t 		CompleteDeviceName[MAX_DEVICE_PATH_LENGTH];
	wchar_t			DeviceName[MAX_DEVICE_NAME_EXTENSION];
	size_t			ConvertedCount;

	// Lookup the GUID and see if it is present, store the info in hDevInfo
	hDevInfo = SetupDiGetClassDevs(
							&GuidTable[0], 
							NULL, 
							NULL,
							DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		printf("SetupDiGetClassDevs returned INVALID_HANDLE_VALUE, error = 0x%x\n", GetLastError());
	}

	// Count the number of devices present
	DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	while (SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GuidTable[0], 
							count++, &DeviceInterfaceData) == TRUE);

	// last one failed, find out why
	if (GetLastError() != ERROR_NO_MORE_ITEMS)
	{
		printf("SetupDiEnumDeviceInterfaces returned FALSE, index= %d, error = %d\n\tShould be ERROR_NO_MORE_ITEMS (%d)\n", 
			count-1, GetLastError(), ERROR_NO_MORE_ITEMS);
	}
	// Always counts one too many
	count--;

	// Check to see if there are any boards present
	if (count == 0)
	{
		//printf ("Boards or requested DMA Channel is not present.\n");
		return INVALID_HANDLE_VALUE;
	} else if (count <= BoardNumber)
		// check to see if the BoardNumber if valid
	{
		printf ("Invalid board number.\n");
		return INVALID_HANDLE_VALUE;
	}

	// Get information for device
    BoolStatus = SetupDiEnumDeviceInterfaces(
									hDevInfo,  
									NULL, 
									&GuidTable[0],
									BoardNumber, 
									&DeviceInterfaceData);
	if (BoolStatus == FALSE)
	{
		printf("SetupDiEnumDeviceInterfaces failed for board, BoardNumber = %d\n  Error: %d\n", 
			BoardNumber, GetLastError());
		return INVALID_HANDLE_VALUE;
	}

	// Get the Device Interface Detailed Data
    BoolStatus = SetupDiGetDeviceInterfaceDetail(
											hDevInfo, 
											&DeviceInterfaceData, 
											NULL, 
											0, 
											&RequiredSize, 
											NULL);
	// this should fail (returning false) and setting error to ERROR_INSUFFICIENT_BUFFER
	if ((BoolStatus == TRUE) || (GetLastError() != ERROR_INSUFFICIENT_BUFFER))
	{
		printf("SetupDiGetDeviceInterfaceDetail failed for board, BoardNumber = %d\n  Error: %d\n", 
			BoardNumber, GetLastError());
		return INVALID_HANDLE_VALUE;
	}

	// allocate the correct size
	pDeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc(RequiredSize);
	if (pDeviceInterfaceDetailData == NULL)
	{
		printf("Insufficient memory, pDeviceInterfaceDetailData\n");
		return INVALID_HANDLE_VALUE;
	}
	// set the size to the fixed data size (not the full size)
	pDeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	// get the data
    BoolStatus = SetupDiGetDeviceInterfaceDetail(
									hDevInfo,					
									&DeviceInterfaceData, 
									pDeviceInterfaceDetailData,
									RequiredSize, 
									NULL, 
									NULL);		// Do not need DeviceInfoData at this time				
	if (BoolStatus == FALSE)
	{
		printf("SetupDiGetDeviceInterfaceDetail failed for board, BoardNumber = %d\n  Error: %d\n", 
			BoardNumber, GetLastError());
		free (pDeviceInterfaceDetailData);
		return INVALID_HANDLE_VALUE;
	}

    wcscpy_s(CompleteDeviceName, MAX_DEVICE_PATH_LENGTH, pDeviceInterfaceDetailData->DevicePath);
	if (DMAChannelName != NULL)
	{
		// Append the DMA Channel name to the GUID Path
		wcscat_s(CompleteDeviceName, MAX_DEVICE_PATH_LENGTH, (const wchar_t *)"\\");
		ConvertedCount = strlen(DMAChannelName) + 1;
		mbstowcs_s(&ConvertedCount, DeviceName, ConvertedCount, DMAChannelName, _TRUNCATE);
		wcscat_s(CompleteDeviceName, MAX_DEVICE_PATH_LENGTH, DeviceName);
	}

	// Now connect to the card
	hDevice = CreateFile(
					CompleteDeviceName,
					GENERIC_WRITE | GENERIC_READ,
					FILE_SHARE_READ | FILE_SHARE_WRITE, 
					NULL, OPEN_EXISTING,
					FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
					INVALID_HANDLE_VALUE);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("CreaFile failed for board, BoardNumber = %d\n  Error: %d\n", 
			BoardNumber, GetLastError());
		//DisplayErrorInfo(NULL, NULL, GetLastError());
		free (pDeviceInterfaceDetailData);
		return INVALID_HANDLE_VALUE;
	}
	// Free up device detail
	if (pDeviceInterfaceDetailData != NULL)
	{
		free (pDeviceInterfaceDetailData);
	}
	return hDevice;
}

/*! SetupChannelMode
 *
 * \brief Configures the DMA Channel for FIFO Mode
 * \param pcfgParams
 * \param Channel
 * \param Buffer
 * \param Length
 * \param Direction
 * \return Status
 */
INT32 
SetupChannelMode(
	PCFG_PARAMS				pcfgParams,
	HANDLE					Channel,
	PVOID					Buffer,
	DWORD					Length,
	BOOLEAN					S2CDirection
)
{
	SETUP_CHANNEL_STRUCT	SetupChannel;
    OVERLAPPED				os;			// OVERLAPPED structure for the operation
	DWORD					bytesReturned	= 0;
	UINT32					Status			= 0;

	SetupChannel.AllocationMode = DMA_MODE_FIFO;
	SetupChannel.ChannelNum		= pcfgParams->ReadDMAChannel;
	SetupChannel.Direction		= 0;	// C2S by default
	if (S2CDirection)
	{
		SetupChannel.ChannelNum		= pcfgParams->WriteDMAChannel;
		SetupChannel.Direction	= 1;	// S2C
	}
	os.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Send CONFIGURE_CHANNEL_IOCTL
	if (!DeviceIoControl(
					Channel, 
					SETUP_CHANNEL_IOCTL,
					(LPVOID)&SetupChannel, 
					sizeof(SETUP_CHANNEL_STRUCT),
					Buffer, 
					Length,
					&bytesReturned, 
					&os))
	{
		Status = GetLastError();
		if (Status == ERROR_IO_PENDING)
		{
			// Wait here (forever) for the Overlapped I/O to complete
			if (!GetOverlappedResult(Channel, &os, &bytesReturned, TRUE))
			{
				Status = GetLastError();
				printf("SetupChannelMode IOCTL call failed. Error = %d\n", Status);
			}
			else
			{
				Status = 0;
			}
		}
		else
		{
			// ioctl failed
			Status = GetLastError();
			printf("SetupChannelMode IOCTL call failed. Error = %d\n", Status);
		}
	}
	// Check the status
	if (Status != 0)
	{
		printf("SetupChannelMode IOCTL error:\n");
		pcfgParams->iPass = DisplayErrorInfo(pcfgParams->bQuiet, Status);
	}
	CloseHandle(os.hEvent);
	return Status;
}

/*!	ShutdownChannelMode
 *
 * \brief Shuts down the DMA Channel 
 * \param pcfgParams
 * \param Channel
 * \param S2CDirection
 * \return Status
 */
INT32 
ShutdownChannelMode(
	PCFG_PARAMS				pcfgParams,
	HANDLE					Channel,
	BOOLEAN					S2CDirection
)
{
	SHUTDOWN_CHANNEL_STRUCT	ShutdownChannel;
    OVERLAPPED				os;			// OVERLAPPED structure for the operation
	DWORD					bytesReturned	= 0;
	UINT32					Status			= 0;

	ShutdownChannel.AllocationMode = DMA_MODE_FIFO;
	ShutdownChannel.ChannelNum = pcfgParams->ReadDMAChannel;
	if (S2CDirection)
	{
		ShutdownChannel.ChannelNum	= pcfgParams->WriteDMAChannel;
	}
	os.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Send Shutdown FIFO Mode IOCTL
	if (!DeviceIoControl(
					Channel, 
					SHUTDOWN_CHANNEL_IOCTL,
					(LPVOID)&ShutdownChannel, 
					sizeof(SHUTDOWN_CHANNEL_STRUCT),
					NULL, 
					0,
					&bytesReturned, 
					&os))
	{
		Status = GetLastError();
		if (Status == ERROR_IO_PENDING)
		{
			// Wait here (forever) for the Overlapped I/O to complete
			if (!GetOverlappedResult(Channel, &os, &bytesReturned, TRUE))
			{
				Status = GetLastError();
				printf("ShutdownChannelMode IOCTL call failed. Error = %d\n", Status);
			}
			else
			{
				Status = 0;
			}
		}
		else
		{
			// IOCtl failed
			Status = GetLastError();
			printf("ShutdownChannelMode IOCTL call failed. Error = %d\n", Status);
		}
	}
	// Check the status
	if (Status != 0)
	{
		printf("ShutdownChannelMode IOCTL error:\n");
		pcfgParams->iPass = DisplayErrorInfo(pcfgParams->bQuiet, Status);
	}
	CloseHandle(os.hEvent);
	return Status;
}

/*!	StartThread
 *
 * \brief Start a thread of execution
 * \param params
 * \param ThreadProc
 * \return Status
 */ 
INT32	
StartThread(
	PTHREAD_PARAMS				params,
	LPTHREAD_START_ROUTINE		ThreadProc
)
{
	// Create a thread.	
	params->hThreadHandle = CreateThread(
						(LPSECURITY_ATTRIBUTES)NULL,		// No security.
						(DWORD)0,							// Same stack size. 
						ThreadProc,							// Thread procedure. 
						(LPVOID)params,						// Pointer the config parameters. 
						(DWORD)0,							// Start immediately. 
						NULL);								// Thread ID. 

	if (params->hThreadHandle == NULL)
	{
		fprintf(stderr, "CreateThread Failed, returned %d", GetLastError());
		return -2;
	}

	// Boost the priority of this thread so we don't drop DMAs.
	//	if (!SetThreadPriority(params->hThreadHandle, THREAD_PRIORITY_ABOVE_NORMAL)) //THREAD_PRIORITY_HIGHEST))
	//	{
	//		fprintf(stderr, "SetThreadPriority failed");
	//		return -3;
	//	}
	return 0;
}

/*!	GetChannelStats
 *
 * \brief Keeps track of each pass info and optionally display it
 * \param Channel
 * \param pPassStats
 * \return Status
 */
INT32
GetChannelStats(
	HANDLE				Channel,
	PPASS_STATS			pPassStats
)
{
	CHANNEL_STAT_STRUCT	ChanStats;
    OVERLAPPED			os;			// OVERLAPPED structure for the operation
	DWORD				bytesReturned	= 0;
	DWORD				Status			= 0;

	pPassStats->TransfersPerSecond	= 0;
	pPassStats->IntsPerSecond		= 0;
	pPassStats->DPCsPerSecond		= 0;

	os.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Send GET_CHANNEL_STATS_IOCTL
	if (!DeviceIoControl(
					Channel, 
					GET_CHANNEL_STATS_IOCTL,
					NULL, 
					0,
					(LPVOID)&ChanStats, 
					sizeof(CHANNEL_STAT_STRUCT),
					&bytesReturned, 
					&os))
	{
		Status = GetLastError();
		if (Status == ERROR_IO_PENDING)
		{
			// Wait here (forever) for the Overlapped I/O to complete
			if (!GetOverlappedResult(Channel, &os, &bytesReturned, TRUE))
			{
				Status = GetLastError();
				printf("GetChannelStats IOCTL wait call failed. Error = 0x%x\n", Status);
			}
			else
			{
				Status = 0;
			}
		}
		else
		{
			// IOCtl failed
			Status = GetLastError();
			printf("GetChannelStats IOCTL call failed. Error = %d\n", Status);
		}
	}
	// Check returned structure size
	if ((bytesReturned != sizeof(CHANNEL_STAT_STRUCT)) && 
		(Status == STATUS_SUCCESSFUL))
	{
		// IOCtl failed
		printf("GetChannelStats IOCTL returned invalid size (%d)\n", bytesReturned);
		Status = STATUS_INCOMPLETE;
	}

	// Check the status
	if (Status != 0)
	{
		printf("GetChannelStats IOCTL error:\n");
		DisplayErrorInfo(FALSE, Status);
	}
	else
	{
		pPassStats->TransfersPerSecond	= ChanStats.RequestCount;
		pPassStats->IntsPerSecond		= ChanStats.IntsPerSecond;
		pPassStats->DPCsPerSecond		= ChanStats.DPCsPerSecond;
	}
	CloseHandle(os.hEvent);
	return Status;
}

/*! GetCPUUtil 
 * 
 * \brief Retieves the CPU Utilization since the last time this routine was called
 * \note  It is necessary to call once to establish a baseline before collecting stats.
 * \return CPU Utilization since last call to this routine
 */
INT32
GetCPUUtil(
)
{
	INT32	cpu = 0;

    UINT64	idleTime;
	UINT64	kernelTime;
	UINT64	userTime;
	UINT64	idl;
	UINT64	ker;
	UINT64	usr;
	UINT64	sys;

	GetSystemTimes((LPFILETIME)&idleTime, (LPFILETIME)&kernelTime, (LPFILETIME)&userTime);

    usr = userTime   - g_userTime;
    ker = kernelTime - g_kernelTime;
    idl = idleTime   - g_idleTime;
	sys = (usr + ker);

    if( sys > 0 )
	{
		cpu = INT32( (sys - idl) *100 / sys ); // System Idle take 100 % of cpu :-((
	}

	g_idleTime		= idleTime;
    g_kernelTime	= kernelTime;
    g_userTime		= userTime;
	return cpu;
}


/*! SetupOSSpecifics
 *
 * \brief Sets up any OS Specific functions, i.e Open /proc/stat in Linux
 * \param pcfgParams  
 * \return 0     
 */
INT32 
SetupOSSpecifics(
	PCFG_PARAMS	pcfgParams
)
{
	UNREFERENCED_PARAMETER(pcfgParams);

	return 0;
}

 /*! ShutdownOSSpecifics
 *
 * \brief Shutsdown any OS Specific functions, i.e Close /proc/stat in Linux
 * \param pcfgParams
 * \return 0
 */
INT32	
ShutdownOSSpecifics(
	PCFG_PARAMS	pcfgParams
)
{
	UNREFERENCED_PARAMETER(pcfgParams);

	return 0;
}

#include "../Include/ExpressoHw.h"

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
    OVERLAPPED						os;			// OVERLAPPED structure for the operation
	DWORD							bytesReturned	= DUMP_BUFFER_SIZE;
	DWORD							Status			= 0;
	INT32							i;

	pChanRawInfo = (PUINT8)malloc(DUMP_BUFFER_SIZE);
	if (pChanRawInfo == NULL)
	{
		printf("DumpChannelInfo malloc failed.\n");
		return -1;
	}
	os.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Send DUMP_CHANNEL_STRUCTS_IOCTL
	if (!DeviceIoControl(
					Channel, 
					DUMP_CHANNEL_STRUCTS_IOCTL,
					NULL, 
					0,
					(LPVOID)pChanRawInfo, 
					bytesReturned,
					&bytesReturned, 
					&os))
	{
		Status = GetLastError();
		if (Status == ERROR_IO_PENDING)
		{
			// Wait here (forever) for the Overlapped I/O to complete
			if (!GetOverlappedResult(Channel, &os, &bytesReturned, TRUE))
			{
				Status = GetLastError();
				printf("DumpChannelInfo IOCTL wait call failed. Error = 0x%x\n", Status);
			}
			else
			{
				Status = 0;
			}
		}
		else
		{
			// ioctl failed
			Status = GetLastError();
			printf("GetChannelStats IOCTL call failed. Error = %d\n", Status);
		}
	}
	// check returned structure size
	if ((bytesReturned < 512) && 
		(Status == STATUS_SUCCESSFUL))
	{
		// ioctl failed
		printf("GetChannelStats IOCTL returned invalid size (%d)\n", bytesReturned);
		Status = STATUS_INCOMPLETE;
	}

	// Check the status
	if (Status != 0)
	{
		printf("GetChannelStats IOCTL error:\n");
		DisplayErrorInfo(FALSE, Status);
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
	}
	CloseHandle(os.hEvent);
	free(pChanRawInfo);
	return Status;
}
#endif // Debug

