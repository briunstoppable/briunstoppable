// -------------------------------------------------------------------------
// 
// PRODUCT:			Expresso Multi-Channel DMA Driver
// MODULE NAME:		TestCmds.cpp
// 
// MODULE DESCRIPTION: 
// 
// Contains the test command functions called from main.
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
//#include "../include/ExpressoIoctl.h"

// --------------------------------------------------------------------------------------
// Defines
// --------------------------------------------------------------------------------------

#define MAX_CMD_LENGTH 20
#define MAX_BUFFER_SIZE 1024

#define	PCI_EXPRESS_CAPABILITY_ID	0x10
#define MSI_MESSAGE_ID				0x05
#define	MSIX_MESSAGE_ID				0x11

#define	PCI_CONFIG_HEADER_TYPE_0_LENGTH	64

// --------------------------------------------------------------------------------------
// Internal prototypes
// --------------------------------------------------------------------------------------

/*! displayBoardCfg
 * 
 * \brief Function to prints out the Board configuration.
 * \param pBoardConfig
 * \param verbose
 * \return None
 */
VOID 
displayBoardCfg(
	PBOARD_CONFIG_STRUCT		pBoardConfig,
	BOOLEAN						verbose
)
{
	INT32						temp;
    PPCI_DEVICE_SPECIFIC_HEADER pDevSpecific;
	PUINT8						pPCIConfSpace;
	UINT8						offset;
    UINT8						MessageID;
	PPCI_MSI_CAPABILITY			pPciMSICapabilities;
	PPCI_MSIX_CAPABILITY		pPciMSIXCapabilities;

	UNREFERENCED_PARAMETER(verbose);

	printf("PCI Express Configuration Information:\n");

	printf("    VendorId =          0x%04x\n", pBoardConfig->PciConfig.VendorId);
	printf("    DeviceId =          0x%04x\n", pBoardConfig->PciConfig.DeviceId);
    printf("    Command =	        0x%04x\n", pBoardConfig->PciConfig.Command);
    printf("    Status =            0x%04x\n", pBoardConfig->PciConfig.Status);
	printf("    RevisionId =        0x%02x\n", pBoardConfig->PciConfig.RevisionId);
	printf("    Interface =         0x%02x\n", pBoardConfig->PciConfig.Interface);
	printf("    SubClass =          0x%02x\n", pBoardConfig->PciConfig.SubClass);
	printf("    BaseClass =         0x%02x\n", pBoardConfig->PciConfig.BaseClass);
    printf("    CacheLineSize =     0x%02x\n", pBoardConfig->PciConfig.CacheLineSize);
    printf("    LatencyTimer =      0x%02x\n", pBoardConfig->PciConfig.LatencyTimer);
    printf("    HeaderType =        0x%02x\n", pBoardConfig->PciConfig.HeaderType);
    printf("    BIST =              0x%02x\n", pBoardConfig->PciConfig.BIST);
	printf("    BarCfg[0] =         0x%08x\n", pBoardConfig->PciConfig.BarCfg[0]);
	printf("    BarCfg[1] =         0x%08x\n", pBoardConfig->PciConfig.BarCfg[1]);
	printf("    BarCfg[2] =         0x%08x\n", pBoardConfig->PciConfig.BarCfg[2]);
	printf("    BarCfg[3] =         0x%08x\n", pBoardConfig->PciConfig.BarCfg[3]);
	printf("    BarCfg[4] =         0x%08x\n", pBoardConfig->PciConfig.BarCfg[4]);
	printf("    BarCfg[5] =         0x%08x\n", pBoardConfig->PciConfig.BarCfg[5]);
    printf("    CardBusCISPtr =     0x%08x\n", pBoardConfig->PciConfig.CardBusCISPtr);
	printf("    SubsystemVendorId = 0x%04x\n", pBoardConfig->PciConfig.SubsystemVendorId);
	printf("    SubsystemId =       0x%04x\n", pBoardConfig->PciConfig.SubsystemId);
	printf("    ExpRomCfg =         0x%08x\n", pBoardConfig->PciConfig.ExpRomCfg);
	printf("    CapabilitiesPtr =   0x%02x\n", pBoardConfig->PciConfig.CapabilitiesPtr);
    printf("    InterruptLine =     0x%02x\n", pBoardConfig->PciConfig.InterruptLine);
    printf("    InterruptPin =      0x%02x\n", pBoardConfig->PciConfig.InterruptPin);
    printf("    MinimumGrant =      0x%02x\n", pBoardConfig->PciConfig.MinimumGrant);
    printf("    MaximumLatency =    0x%02x\n", pBoardConfig->PciConfig.MaximumLatency);

   if (pBoardConfig->PciConfig.CapabilitiesPtr != 0)
   {
		pPCIConfSpace = (PUINT8)&pBoardConfig->PciConfig;
		offset = pBoardConfig->PciConfig.CapabilitiesPtr;
		do
		{
			MessageID = pPCIConfSpace[offset];

			// Did we find the PCI Express Capability entry?
			if (MessageID == PCI_EXPRESS_CAPABILITY_ID)
			{
				pDevSpecific = (PPCI_DEVICE_SPECIFIC_HEADER)&pPCIConfSpace[offset];
				printf("  PCI Express Capability:\n");
				printf("    PCIeDeviceCap =     0x%08x\n", pDevSpecific->PCIeDeviceCap);
				printf("    PCIeDeviceControl = 0x%04x\n", pDevSpecific->PCIeDeviceControl);
				// decode Device Control - Max Payload Size
				temp = (pDevSpecific->PCIeDeviceControl>>5) & 0x7;
				printf("    Max Payload Size =  ");
				switch (temp)
				{
					case 0: //MaxPayload128Bytes:
						printf("128 bytes\n");
						break;
					case 1: //MaxPayload256Bytes:
						printf("256 bytes\n");
						break;
					case 2: //MaxPayload512Bytes:
						printf("512 bytes\n");
						break;
					case 3: //MaxPayload1024Bytes:
						printf("1024 bytes\n");
						break;
					case 4: //MaxPayload2048Bytes:
						printf("2048 bytes\n");
						break;
					case 5: //MaxPayload4096Bytes:
						printf("4096 bytes\n");
						break;
					default:
						printf("Invalid (0x%x)\n", temp);
						break;
				}
				// decode Device Control - Max Read Request Size
				temp = (pDevSpecific->PCIeDeviceControl>>12) & 0x7;
				printf("    Max Read Req Size = ");
				switch (temp)
				{
					case 0: //MaxPayload128Bytes:
						printf("128 bytes\n");
						break;
					case 1: //MaxPayload256Bytes:
						printf("256 bytes\n");
						break;
					case 2: //MaxPayload512Bytes:
						printf("512 bytes\n");
						break;
					case 3: //MaxPayload1024Bytes:
						printf("1024 bytes\n");
						break;
					case 4: //MaxPayload2048Bytes:
						printf("2048 bytes\n");
						break;
					case 5: //MaxPayload4096Bytes:
						printf("4096 bytes\n");
						break;
					default:
						printf("Invalid (0x%x)\n", temp);
						break;
				}
				printf("    PCIeDeviceStatus =  0x%04x\n", pDevSpecific->PCIeDeviceStatus);
				printf("    PCIeLinkCap =       0x%08x\n", pDevSpecific->PCIeLinkCap);
				printf("    PCIeLinkControl =   0x%04x\n", pDevSpecific->PCIeLinkControl);
				printf("    PCIeLinkStatus =    0x%04x\n", pDevSpecific->PCIeLinkStatus);
				// decode Link Status - Link Speed
				temp = pDevSpecific->PCIeLinkStatus & 0xf;
				printf("    Link Speed =        ");
				switch (temp)
				{
					case 1:
						printf("2.5 gigabits/sec\n");
						break;
					case 2:
						printf("5.0 gigabits/sec\n");
						break;
					case 3:
						printf("8.0 gigabits/sec\n");
						break;
					default:
						printf("Invalid (0x%x)\n", temp);
						break;
				}
				// decode Link Status - Link Width
				temp = (pDevSpecific->PCIeLinkStatus>>4) & 0x3f;
				printf("    Link Width =        ");
				switch (temp)
				{
					case 1:
					case 2:
					case 4:
					case 8:
					case 12:
					case 16:
					case 32:
						printf("%d lanes\n", temp);
						break;
					default:
						printf("Invalid (0x%x)\n", temp);
						break;
				}
			}
			if (MessageID == MSI_MESSAGE_ID)
			{
				pPciMSICapabilities = (PPCI_MSI_CAPABILITY)&pPCIConfSpace[offset];

				printf("  MSI Capability:\n");
				// This is the MSI Config info, offset is pointing to the Message Control word
				printf("    MSI Control: 0x%x\n", pPciMSICapabilities->MessageControl);
			}

			// Did we find the MSIX Message entry?
			if (MessageID == MSIX_MESSAGE_ID)
			{
				pPciMSIXCapabilities = (PPCI_MSIX_CAPABILITY)&pPCIConfSpace[offset];

				printf("  MSI-X Capability:\n");
				printf("    MSIX Control: 0x%x, Table BIR:0x%x, PBA BIR:0x%x\n", 
					pPciMSIXCapabilities->MessageControl, pPciMSIXCapabilities->TableOffsetBIR, 
					pPciMSIXCapabilities->PBAOffsetBIR);
		}

		offset++;
		offset = pPCIConfSpace[offset];
		} while ((offset != 0x00) && (offset < sizeof(PCI_CONFIG_HEADER)));
	}
	
	printf("\nDriver/Board Configuration Information:\n");
	// print out the driver config data
	printf("    Driver Version =    %d.%d.%d.%d\n",
		pBoardConfig->DriverVersionMajor, pBoardConfig->DriverVersionMinor, 
		pBoardConfig->DriverVersionSubMinor, pBoardConfig->DriverVersionBuildNumber);
	printf("    Number of DMA channels  = %d\n", pBoardConfig->NumDmaChannels);

}

/*! getBoardCfg
 *
 * \brief Function to handle the "getBoardCfg" command.
 * \param pcfgParams
 * \return status
 */
INT32 
getBoardCfg(
	PCFG_PARAMS			pcfgParams 
)
{
	BOARD_CONFIG_STRUCT	boardConfig;
	DWORD				bytesReturned	= 0;
	UINT32				Status 			= 0;

#ifdef WIN32
    OVERLAPPED			os;			// OVERLAPPED structure for the operation

	os.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Send GET_BOARD_CONFIG_IOCTL
	if (!DeviceIoControl( pcfgParams->hDrvDevice, GET_BOARD_CONFIG_IOCTL,
			 NULL, 0,
			(LPVOID)&boardConfig, sizeof(BOARD_CONFIG_STRUCT),
			&bytesReturned, &os))
	{
		Status = GetLastError();
		if (Status == ERROR_IO_PENDING)
		{
			// Wait here (forever) for the Overlapped I/O to complete
			if (!GetOverlappedResult(pcfgParams->hDrvDevice, &os, &bytesReturned, TRUE))
			{
				Status = GetLastError();
				printf("GetBoardCfg IOCTL call failed. Error = %d\n", Status);
			}
			else
			{
				Status = 0;
			}
		}
		else
		{
			// IOCTL Failed
			Status = GetLastError();
			printf("GetBoardCfg IOCTL call failed. Error = %d\n", Status);
		}
	}
    CloseHandle(os.hEvent);

    // Check returned structure size
	if ((bytesReturned != sizeof(BOARD_CONFIG_STRUCT)) && 
		(Status == STATUS_SUCCESSFUL))
	{
		// IOCTL Failed
		printf("GetBoardCfg IOCTL returned invalid size (%d)\n", bytesReturned);
		Status = STATUS_INCOMPLETE;
	}

#else // Linux

	// Send GET_BOARD_CONFIG_IOCTL
	Status = ioctl (pcfgParams->hDrvDevice, GET_BOARD_CONFIG_IOCTL, &boardConfig);
	if (Status != 0)
	{
		// IOCTL Failed
		printf("GetBoardCfg IOCTL call failed.  Status=0x%x, Error = 0x%x\n", Status, errno);
		Status = errno;
	}

#endif // Windows vs. Linux
	// Check the status
	if (Status != 0)
	{
		pcfgParams->iPass = DisplayErrorInfo(pcfgParams->bQuiet, Status);
	}
	else
	{
		displayBoardCfg(&boardConfig, pcfgParams->bVerbose);
	}
	return Status;
}

/*! writePCI
 *
 * \brief Function to handle the "WritePCI" command.
 *  Calls the DmaDriver DLL to do the work.
 * \param pcfgParams 
 * \return Status
 */
INT32 
writePCI(
	PCFG_PARAMS			pcfgParams
)
{
#ifdef WIN32
    OVERLAPPED				os;			// OVERLAPPED structure for the operation
#endif // Windows vs. Linux
	RW_PCI_CONFIG_STRUCT	PCIConfig;
	UINT32					status				= 0;
	PUINT8					Buffer;
	UINT32					BufferSize;
	INT32					UserDataLen			= 0;
	INT32					UserDataFragments	= 0;
	DWORD					bytesReturned		= 0;

	PCIConfig.Offset = 0;
	PCIConfig.Length = DEFAULT_MEM_LENGTH;

	// Get a parameters
	if (pcfgParams->CmdArgc > 1)
	{
		PCIConfig.Offset = (UINT32) __strtoul(pcfgParams->CmdArgv[1]);
	}
	if (pcfgParams->CmdArgc > 2)
	{
		PCIConfig.Length = (INT32) __strtoul(pcfgParams->CmdArgv[2]);
	}

	// Allocate the buffer
	BufferSize = (PCIConfig.Length + 3) & 0xFFFFFFFC;
	Buffer = (PUINT8) malloc((size_t)BufferSize); 
	
	// Print debug messages
	if (pcfgParams->bVerbose == TRUE)
	{
		printf("Offset = %d\n", PCIConfig.Offset);
		printf("Length = %d\n", PCIConfig.Length);
		printf("BufferSize = %d\n", BufferSize);
	}

	if (Buffer == NULL)
	{
		printf("Buffer malloc failed, Length = 0x%x\n", PCIConfig.Length);
		pcfgParams->iPass = ERROR_MEMORY_ALLOC_FAILED;
		return -1;
	}

	// Get the data from the command line.
	if (pcfgParams->CmdArgc > 3)
	{
		if (strlen(pcfgParams->CmdArgv[3]))
		{
			UserDataLen = atohx((PCHAR)Buffer, pcfgParams->CmdArgv[3]);
			reverse_string((PCHAR)Buffer);
			if (UserDataLen)
			{
				UserDataFragments = (PCIConfig.Length - UserDataLen) / UserDataLen;
			}
		}
		if (UserDataFragments)
		{
			INT32 idx = UserDataLen;
			for (INT32 t = 0; t < UserDataFragments; t++)
			{
				memcpy((PVOID)&Buffer[idx], 
						(PVOID)&Buffer[0], 
						(size_t)UserDataLen);
				idx += UserDataLen;
			}
		}
	}
	else
	{
		printf("Missing data parameter to write to PCI Configuration.\n");
		pcfgParams->iPass = ERROR_BAD_PARAMETER;
		free (Buffer);
		return -1;
	}
#ifdef WIN32
	os.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// Send PCI Write IOCTL
	if (!DeviceIoControl(
					pcfgParams->hDrvDevice, 
					WRITE_PCI_CONFIG_IOCTL,
					(LPVOID)&PCIConfig, 
					sizeof(RW_PCI_CONFIG_STRUCT),
					(LPVOID)Buffer, 
					(DWORD)PCIConfig.Length,
					&bytesReturned, 
					&os))
	{
		status = GetLastError();
		if (status == ERROR_IO_PENDING)
		{
			// Wait here (forever) for the Overlapped I/O to complete
			if (!GetOverlappedResult(pcfgParams->hDrvDevice, &os, &bytesReturned, TRUE))
			{
				status = GetLastError();
				printf("WritePCI IOCTL call failed. Error = %d\n", status);
			}
			else
			{
				status = 0;
			}
		}
	}
    CloseHandle(os.hEvent);
#else // Linux
    // Send PCI Write IOCTL
    PCIConfig.Buffer = (PUINT8)Buffer;
	status = ioctl(pcfgParams->hDrvDevice, 
					WRITE_PCI_CONFIG_IOCTL, 
					&PCIConfig);
	if (status != 0)
	{
		// ioctl failed
		printf("WritePCI IOCTL call failed.  Status=0x%x, Error = 0x%x\n", status, errno);
		status = errno;
	}
#endif // Windows vs. Linux
	if (pcfgParams->bVerbose == TRUE)
	{
		printf("PCI Write Configuration complete.\n");
	}

	// Free Buffer
	free (Buffer);
	return status;
}

/*! readPCI
 *
 * \brief Function to handle the "ReadPCI" command.
 *	Calls the DmaDriver DLL to do the work.
 * \param pcfgParams
 * \return status
 */
INT32 
readPCI(
	PCFG_PARAMS				pcfgParams
)
{
#ifdef WIN32
    OVERLAPPED				os;			// OVERLAPPED structure for the operation
#endif // Windows vs. Linux
	RW_PCI_CONFIG_STRUCT	PCIConfig;
	UINT32					status			= 0;
	PUINT32					Buffer;
	UINT32					BufferSize;
	DWORD					bytesReturned	= 0;

	PCIConfig.Offset = 0;
	PCIConfig.Length = DEFAULT_MEM_LENGTH;

	// get a parameters
	if (pcfgParams->CmdArgc > 1)
	{
		PCIConfig.Offset = (UINT32) __strtoul(pcfgParams->CmdArgv[1]);
	}
	if (pcfgParams->CmdArgc > 2)
	{
		PCIConfig.Length = (UINT32)__strtoul(pcfgParams->CmdArgv[2]);
	}

	// Allocate the buffer
	BufferSize	= (PCIConfig.Length + 3) & 0xFFFFFFFC;
	Buffer		= (PUINT32) malloc((size_t)BufferSize); 

	// print debug messages
	if (pcfgParams->bVerbose == TRUE)
	{
		printf("Offset = %d\n", PCIConfig.Offset);
		printf("Length = %d\n", PCIConfig.Length);
		printf("BufferSize = %d\n", BufferSize);
	}

	if (Buffer == NULL)
	{
		printf("Buffer malloc failed, Length = 0x%x\n", BufferSize);
		pcfgParams->iPass = ERROR_MEMORY_ALLOC_FAILED;
		return -1;
	}

	// Fill the buffer with data to detect 
	for (INT32 i = 0; (i < (INT32)(BufferSize / sizeof(UINT32))); i++)
	{
		Buffer[i] = 0xeeeeeeee;
	}

#ifdef WIN32
	os.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// Send PCI Read IOCTL
	if (!DeviceIoControl(
					pcfgParams->hDrvDevice, 
					READ_PCI_CONFIG_IOCTL,
					(LPVOID) &PCIConfig, 
					sizeof(RW_PCI_CONFIG_STRUCT),
					(LPVOID) Buffer, 
					(DWORD)PCIConfig.Length,
					&bytesReturned, 
					&os))
	{
		status = GetLastError();
		if (status == ERROR_IO_PENDING)
		{
			// Wait here (forever) for the Overlapped I/O to complete
			if (!GetOverlappedResult(pcfgParams->hDrvDevice, &os, &bytesReturned, TRUE))
			{
				status = GetLastError();
				printf("ReadPCI IOCTL call failed. Error = %d\n", status);
			}
			else
			{
				status = 0;
			}
		}
	}
    CloseHandle(os.hEvent);
#else // Linux
    // Send PCI Read IOCTL
    PCIConfig.Buffer = (PUINT8)Buffer;
    status = ioctl (pcfgParams->hDrvDevice, READ_PCI_CONFIG_IOCTL, &PCIConfig);
    if (status != 0)
    {
        // ioctl failed
        printf("ReadPCI IOCTL call failed.  Status=0x%x, Error = 0x%x\n", status, errno);
        status = errno;
    }
#endif // Windows vs. Linux

	// Check the status
	if (status == 0)
	{
		// print debug messages
		if (pcfgParams->bVerbose == TRUE)
		{
			printf("PCI Read Configuration complete, buffer = \n");
		}
		DisplayFormattedBytes(
						pcfgParams->bQuiet, 
						PCIConfig.Offset, 
						(PUINT8)Buffer, 
						(INT32)PCIConfig.Length);
	}

	// Free Buffer
	free (Buffer);
	return status;
}

/*! WaitUserInterrupt
 * 
 * \brief Function to wait for a User Interrupt
 * \param pcfgParams
 * \return Status
 */
INT32 
waitUserInterrupt(
	PCFG_PARAMS			pcfgParams 
)
{
	DWORD				bytesReturned = 0;
	UINT32				Status = 0;

#ifdef WIN32
    OVERLAPPED			os;			// OVERLAPPED structure for the operation

	os.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Send USER_INTERRUPT_WAIT_IOCTL
	if (!DeviceIoControl(pcfgParams->hDrvDevice, USER_INTERRUPT_WAIT_IOCTL, NULL, 0, NULL, 0, &bytesReturned, &os))
	{
		Status = GetLastError();
		if (Status == ERROR_IO_PENDING)
		{
			// Wait here (forever) for the Overlapped I/O to complete
			if (!GetOverlappedResult(pcfgParams->hDrvDevice, &os, &bytesReturned, TRUE))
			{
				Status = GetLastError();
				// This case is the driver handle is already closed or
				// The firmware does not support this function
				if (!((Status == 6) || (Status == 995)))
				{
					if (pcfgParams->bVerbose == TRUE)
					{
						printf("User Interrupt Wait IOCTL GetOverlappedResult call failed. Error = %d\n", Status);
					}
				}
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
			// This case is the driver handle is already closed or
			// The firmware does not support this function
			if (!((Status == 6) || (Status == 995)))
			{
				if (pcfgParams->bVerbose == TRUE)
				{
					printf("User Interrupt Wait IOCTL call failed. Error = %d\n", Status);
				}
			}
		}
	}
    CloseHandle(os.hEvent);

#else // Linux

#if 0 // Not implemented
	// Send USER_INTERRUPT_WAIT_IOCTL
	Status = ioctl (
				pcfgParams->hDrvDevice, 
				USER_INTERRUPT_WAIT_IOCTL, 
				NULL);
	if (Status != 0)
	{
		// ioctl failed
		printf("User Interrupt Wait IOCTL call failed.  Status=0x%x, Error = 0x%x\n", Status, errno);
		Status = errno;
	}
#else
	Status = -1;
#endif // Not Implemented for Linux.

#endif // Windows vs. Linux
	return Status;
}

// -------------------------------------------------------------------------
//	Read, Write Memory Testing Functions
//  These functions are called by the main() function
// -------------------------------------------------------------------------
//	These functions finish setting up the test by retrieving any additonal 
//	parameter information associated with the command. 
// -------------------------------------------------------------------------

/*! writeMem
 *
 * \brief Function to handle the "WriteMem" command.
 * \param pcfgParams
 * \return Status
 */
INT32 
writeMem(
	PCFG_PARAMS			pcfgParams
)
{
#ifdef WIN32
    OVERLAPPED			os;			// OVERLAPPED structure for the operation
#endif // Windows vs. Linux
	MEM_COPY_STRUCT		MemWrite;
	UINT32				Status				= 0;
	PUINT8				Buffer;
	UINT64				BufferSize;
	UINT64				UserDataLen			= 0;
	UINT64				UserDataFragments	= 0;
	DWORD				bytesReturned		= 0;

	MemWrite.BarNum		= 0;
	MemWrite.CardOffset = 0;
	MemWrite.Length		= DEFAULT_MEM_LENGTH;

	// get a parameters
	if (pcfgParams->CmdArgc > 1)
	{
		MemWrite.BarNum = (UINT32) __strtoul(pcfgParams->CmdArgv[1]);
	}
	if (pcfgParams->CmdArgc > 2)
	{
		MemWrite.CardOffset = (UINT64) __strtoull(pcfgParams->CmdArgv[2]);
	}
	if (pcfgParams->CmdArgc > 3)
	{
		MemWrite.Length = (UINT64) __strtoull(pcfgParams->CmdArgv[3]);
	}

	// Allocate the buffer
	BufferSize = (MemWrite.Length + 3) & 0xFFFFFFFC;
	Buffer = (PUINT8) malloc((size_t)BufferSize); 
	
	// print debug messages
	if (pcfgParams->bVerbose == TRUE)
	{
		printf("BarNum = %d\n", MemWrite.BarNum);
		printf("CardOffset = %lld\n", MemWrite.CardOffset);
		printf("Length = %lld\n", MemWrite.Length);
		printf("BufferSize = %lld\n", BufferSize);
	}

	if (Buffer == NULL)
	{
		printf("Buffer malloc failed, Length = 0x%llx\n", MemWrite.Length);
		pcfgParams->iPass = ERROR_MEMORY_ALLOC_FAILED;
		return -1;
	}

	/*! 
	 * Get the data pattern from the command line if available. 
	 * If not just fill with the pattern of 0x01 02 03 04...
	 */
	if (pcfgParams->CmdArgc > 4)
	{
		if (strlen(pcfgParams->CmdArgv[4]))
		{
			UserDataLen = atohx((PCHAR)Buffer, pcfgParams->CmdArgv[4]);
			reverse_string((PCHAR)Buffer);
			if (UserDataLen)
			{
				UserDataFragments = (MemWrite.Length  - UserDataLen) / UserDataLen;
			}
		}
		if (UserDataFragments)
		{
			INT32 idx = (INT32)UserDataLen;
			for (INT32 t = 0; t < UserDataFragments; t++)
			{
				memcpy(
					(PVOID)&Buffer[idx], 
					(PVOID)&Buffer[0], 
					(size_t)UserDataLen);
				idx += (INT32)UserDataLen;
			}
		}
	}
	else
	{
		for (INT32 t = 0; t < MemWrite.Length; t++)
		{
			Buffer[t] = (UINT8)(t & 0xFF);
		}
	}

#ifdef WIN32
	os.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// Send Mem Write IOCTL
	if (!DeviceIoControl(
					pcfgParams->hDrvDevice, 
					WRITE_MEM_ACCESS_IOCTL,
					(LPVOID) &MemWrite, 
					sizeof(MEM_COPY_STRUCT),
					(LPVOID) Buffer, 
					(DWORD) MemWrite.Length,
					&bytesReturned, 
					&os))
	{
		Status = GetLastError();
		if (Status == ERROR_IO_PENDING)
		{
			// Wait here (forever) for the Overlapped I/O to complete
			if (!GetOverlappedResult(pcfgParams->hDrvDevice, &os, &bytesReturned, TRUE))
			{
				Status = GetLastError();
				printf("WriteMem IOCTL call failed. Error = %d\n", Status);
			}
			else
			{
				Status = 0;
			}
		}
	}
    CloseHandle(os.hEvent);
#else // Linux
    // Send Mem Write IOCTL
    MemWrite.Buffer = (PUINT8)Buffer;
	Status = ioctl(
				pcfgParams->hDrvDevice, 
				WRITE_MEM_ACCESS_IOCTL, 
				&MemWrite);
	if (Status != 0)
	{
		// ioctl failed
		printf("WriteMem IOCTL call failed.  Status=0x%x, Error = 0x%x\n", Status, errno);
		Status = errno;
	}
#endif // Windows vs. Linux

	// Check the status
	if (Status != 0)
	{
		pcfgParams->iPass = DisplayErrorInfo(pcfgParams->bQuiet, Status);
	}
	else
	{
		if (pcfgParams->bVerbose == TRUE)
		{
			printf("Write complete.\n");
		}
	}
	// Free Buffer
	free (Buffer);
	return Status;
}

/*! readMem
 *
 * \brief Function to handle the "ReadMem" command.
 * \param pcfgParams
 * \return Status
 */
INT32 
readMem(
	PCFG_PARAMS			pcfgParams
)
{
	MEM_COPY_STRUCT		MemRead;
    OVERLAPPED			os;			// OVERLAPPED structure for the operation
	UINT32				Status = 0;
	PUINT32				Buffer;
	UINT64				BufferSize;
	DWORD				bytesReturned = 0;

	MemRead.BarNum		= 0;
	MemRead.CardOffset	= 0;
	MemRead.Length		= DEFAULT_MEM_LENGTH;

	// get a parameters
	if (pcfgParams->CmdArgc > 1)
	{
		MemRead.BarNum = (UINT32) __strtoul(pcfgParams->CmdArgv[1]);
	}
	if (pcfgParams->CmdArgc > 2)
	{
		MemRead.CardOffset = (UINT64) __strtoull(pcfgParams->CmdArgv[2]);
	}
	if (pcfgParams->CmdArgc > 3)
	{
		MemRead.Length = (UINT64) __strtoull(pcfgParams->CmdArgv[3]);
	}

	// Allocate the buffer
	BufferSize = (MemRead.Length + 3) & 0xFFFFFFFC;
	Buffer = (PUINT32) malloc((size_t)BufferSize); 

	// print debug messages
	if (pcfgParams->bVerbose == TRUE)
	{
		printf("BarNum = %d\n", MemRead.BarNum);
		printf("CardOffset = %lld\n", MemRead.CardOffset);
		printf("Length = %lld\n", MemRead.Length);
		printf("BufferSize = %lld\n", BufferSize);
	}

	if (Buffer == NULL)
	{
		printf("Buffer malloc failed, Length = 0x%llx\n", BufferSize);
		pcfgParams->iPass = ERROR_MEMORY_ALLOC_FAILED;
		return -1;
	}

	// Fill the buffer with data to detect 
	for (INT32 i = 0; (i < (INT32)(BufferSize / sizeof(UINT32))); i++)
	{
		Buffer[i] = 0xeeeeeeee;
	}

#ifdef WIN32
	os.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	// Send Mem Read IOCTL
	if (!DeviceIoControl(pcfgParams->hDrvDevice, READ_MEM_ACCESS_IOCTL,
			(LPVOID) &MemRead, sizeof(MEM_COPY_STRUCT),
			(LPVOID) Buffer, (DWORD)MemRead.Length,
			&bytesReturned, &os))
	{
		Status = GetLastError();
		if (Status == ERROR_IO_PENDING)
		{
			// Wait here (forever) for the Overlapped I/O to complete
			if (!GetOverlappedResult(pcfgParams->hDrvDevice, &os, &bytesReturned, TRUE))
			{
				Status = GetLastError();
				printf("ReadMem IOCTL call failed. Error = %d\n", Status);
			}
			else
			{
				Status = 0;
			}
		}
	}
    CloseHandle(os.hEvent);
#else // Linux
    // Send Mem Read IOCTL
    MemRead.Buffer = (PUINT8)Buffer;
    Status = ioctl (pcfgParams->hDrvDevice, READ_MEM_ACCESS_IOCTL, &MemRead);
    if (Status != 0)
    {
        // ioctl failed
        printf("ReadMem IOCTL call failed.  Status=0x%x, Error = 0x%x\n", Status, errno);
        Status = errno;
    }
#endif // Windows vs. Linux

	// Check the status
	if (Status != 0)
	{
		pcfgParams->iPass = DisplayErrorInfo(pcfgParams->bQuiet, Status);
	}
	else
	{
		// print debug messages
		if (pcfgParams->bVerbose == TRUE)
		{
			printf("Read complete, buffer = \n");
		}
		DisplayFormattedBytes(
			pcfgParams->bQuiet, 
			MemRead.CardOffset, 
			(PUINT8)Buffer, 
			(INT32)MemRead.Length
		);
	}
	// Free Buffer
	free (Buffer);
	return Status;
}

// --------------------------------------------------------------------------------------
//	Read, Write DMA Testing Functions - These functions are called by the main() function
// --------------------------------------------------------------------------------------
//	These functions finish setting up the test by retrieving any additonal parameter
//	information associated with the command. These functions also allocate buffer,
//	setup thread paramater structures, start the threads, display progress, shutdown
//	and cleanup after the test is completed.
//  --------------------------------------------------------------------------------------

/*! WriteDMA
 *
 * \brief
 * \param pcfgParams
 * \return Status
 */
INT32 
WriteDMA(
	PCFG_PARAMS			pcfgParams
)
{
	THREAD_PARAMS		params[MAX_THREADS];
	HANDLE				hWriteDMAChannel;
	UINT64				BufferSize;
	PASS_STATS			PassStats;
	SUMMARY_STATS		SummaryStats;
	UINT32				NumberTransfers;
	UINT32				i;
	BOOLEAN				Running;

	NumberTransfers = pcfgParams->Iterations;
	pcfgParams->CardAddress		= 0;
	pcfgParams->BufferSize		= DEFAULT_MAX_TRANSFER_SIZE;
	pcfgParams->MaxTransferSize = DEFAULT_MAX_TRANSFER_SIZE;

	// get the parameters from the command line, if any
	if (pcfgParams->CmdArgc > 1)
	{
		pcfgParams->CardAddress = (UINT32) __strtoul(pcfgParams->CmdArgv[1]);
	}
	if (pcfgParams->CmdArgc > 2)
	{
		BufferSize = (UINT64) __strtoull(pcfgParams->CmdArgv[2]);
		if (BufferSize > 0x00000000ffffffff)
		{
			printf("Max Write Size (%lld) is too large\n", BufferSize);
			return -2;
		}
		pcfgParams->BufferSize = (UINT32)BufferSize;
		pcfgParams->MaxTransferSize = pcfgParams->BufferSize;
	}

	// Make sure there we do at least one transfer for each thread
	if (NumberTransfers < (UINT32)pcfgParams->NumberThreads)
	{
		NumberTransfers = pcfgParams->NumberThreads;
	}
	else
	{
		NumberTransfers = NumberTransfers / pcfgParams->NumberThreads;
	}

	// print debug messages
	if (pcfgParams->bVerbose == TRUE)
	{
		printf("Number Transfers = %d\n", NumberTransfers);
		printf("MaxTransferSize = %d\n", pcfgParams->MaxTransferSize);
		printf("Buffer Size = %d\n", pcfgParams->BufferSize);
		printf("Threads = %d\n", pcfgParams->NumberThreads);
	}

	InitSummaryStats(&SummaryStats);
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		InitThreadParams(pcfgParams, &params[i]);
		params[i].Iterations		= NumberTransfers;
		params[i].WriteDirection	= TRUE;
	}

	// Now that we have parsed all the parameters and options, now we try
	// establish contact with the driver...
	// Open the specific DMA Channel number
	hWriteDMAChannel = OpenDMAChannel(pcfgParams->cardNum, pcfgParams->WriteDMAChannel);
	if (hWriteDMAChannel == INVALID_HANDLE_VALUE)
	{
		printf("Error: Driver for board number %d, DMA Channel %d not found, WriteDMA command failed\n", 
					pcfgParams->cardNum, pcfgParams->WriteDMAChannel);
		pcfgParams->iPass = ERROR_NO_BOARD_FOUND;
		goto ShutdownWriteDMA;
	}
	// Copy the driver handle to all the thread stores
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		params[i].hWriteDevice		= hWriteDMAChannel;
	}
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		if (BufferAllocAndSet(pcfgParams, &params[i]))
		{
			goto ShutdownWriteDMA;
		}
	}

	// Now we have the buffers we need start the test threads
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		StartThread(&params[i], (LPTHREAD_START_ROUTINE)ReadWriteThreadProc);
	}

	// Clear the stats by doing a read first.
	GetChannelStats(hWriteDMAChannel, &PassStats);
	InitPassStats(&PassStats);

	// Loop forever until all threads are finished
	Running = TRUE;
	while (Running)
	{
		// Wait 1 second
		Sleep(1000);
		Running = FALSE;
		GetChannelStats(hWriteDMAChannel, &PassStats);
        PassStats.BytesTransfered = 0;
		for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
		{
			PassStats.BytesTransfered += params[i].TotalByteCountTransferred;
			params[i].TotalByteCountTransferred = 0;
		}
		// Display the results of this pass
		DisplayPassInfo(pcfgParams, &PassStats, &SummaryStats);
		Running = TestRunControl(pcfgParams, &params[0]);
	}
	// Display the summaries from this test run
	DisplaySummaryInfo(pcfgParams, &SummaryStats);

ShutdownWriteDMA:
	// Shutdown the test threads if not already done so.
	ShutdownTestThreads(pcfgParams, &params[0]);
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		// Free the buffer that was allocated for each thread
		FreeThreadParams(&params[i]);
	}

	// Close our interface with the driver.
	CloseHandle(hWriteDMAChannel);

	if (pcfgParams->bVerbose == TRUE)
	{
		printf("\nWrite DMA completed.\n");
	}
	return 0;
}

/*! ReadDMA
 *
 * \brief
 * \param pcfgParams
 * \return Status
 */
INT32 
ReadDMA(
	PCFG_PARAMS			pcfgParams
)
{
	THREAD_PARAMS		params[MAX_THREADS];
	HANDLE				hReadDMAChannel;
	PASS_STATS			PassStats;
	SUMMARY_STATS		SummaryStats;
	UINT64				BufferSize;
	UINT32				NumberTransfers;
	UINT32				i;
	BOOLEAN				Running;

	// get a parameters from the command line
	NumberTransfers	= pcfgParams->Iterations;

	// We are here because the Application needs to provide the Receive Bufffer
	pcfgParams->CardAddress		= 0;
	pcfgParams->BufferSize		= DEFAULT_MAX_TRANSFER_SIZE;
	pcfgParams->MaxTransferSize = DEFAULT_MAX_TRANSFER_SIZE;
	
	if (pcfgParams->CmdArgc > 1)
	{
		pcfgParams->CardAddress = (UINT32)__strtoul(pcfgParams->CmdArgv[1]);
	}
	if (pcfgParams->CmdArgc > 2)
	{
		BufferSize = (UINT64) __strtoull(pcfgParams->CmdArgv[2]);
		if (BufferSize > 0x00000000ffffffff)
		{
			printf("Max Write Size (%lld) is too large\n", BufferSize);
			return -2;
		}
		pcfgParams->BufferSize		= (UINT32)BufferSize;
		pcfgParams->MaxTransferSize = pcfgParams->BufferSize;
	}

	// Make sure there we do at least one transfer for each thread
	if (NumberTransfers < (UINT32)pcfgParams->NumberThreads)
	{
		NumberTransfers = pcfgParams->NumberThreads;
	}
	else
	{
		NumberTransfers = NumberTransfers / pcfgParams->NumberThreads;
	}

	// print debug messages
	if (pcfgParams->bVerbose == TRUE)
	{
		printf("Number Transfers = %d\n", NumberTransfers);
		printf("MaxTransferSize = %d\n", pcfgParams->MaxTransferSize);
		printf("Buffer Size = %d\n", pcfgParams->BufferSize);
		printf("Threads = %d\n", pcfgParams->NumberThreads);
	}

	InitSummaryStats(&SummaryStats);
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		InitThreadParams(pcfgParams, &params[i]);
		params[i].Iterations		= NumberTransfers;
		params[i].WriteDirection	= FALSE;
	}

	/*	Now that we have parsed all the parameters and options, now we try
	 *	establish contact with the driver...
	 *	Open the specific DMA Channel number
	 */
	hReadDMAChannel = OpenDMAChannel(pcfgParams->cardNum, pcfgParams->ReadDMAChannel);
	if (hReadDMAChannel == INVALID_HANDLE_VALUE)
	{
		printf("Error: Driver for board number %d, DMA Channel %d not found, ReadDMA command failed\n", 
					pcfgParams->cardNum, pcfgParams->ReadDMAChannel);
		pcfgParams->iPass = ERROR_NO_BOARD_FOUND;
		goto ShutdownReadDMA;
	}

	// Copy the driver handle to all the thread stores
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		params[i].hReadDevice = hReadDMAChannel;
	}
	//Allocate all the per thread buffers
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		if (BufferAllocAndSet(pcfgParams, &params[i]))
		{
			goto ShutdownReadDMA;
		}
	}

	// Now we have the buffers we need start the test threads
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		StartThread(&params[i], (LPTHREAD_START_ROUTINE)ReadWriteThreadProc);
	}
	// Clear the stats by doing a read first.
	GetChannelStats(hReadDMAChannel, &PassStats);
	InitPassStats(&PassStats);

	// Loop forever until all threads are finished
	Running = TRUE;
	while (Running)
	{
		// Wait 1 second
		Sleep(1000);
		Running = FALSE;
		GetChannelStats(hReadDMAChannel, &PassStats);
        PassStats.BytesTransfered = 0;
		for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
		{
			PassStats.BytesTransfered += params[i].TotalByteCountTransferred;
			params[i].TotalByteCountTransferred = 0;
		}
		// Display the results of this pass
		DisplayPassInfo(pcfgParams, &PassStats, &SummaryStats);
		Running = TestRunControl(pcfgParams, &params[0]);
	}

	// Display the summaries frm this test run
	DisplaySummaryInfo(pcfgParams, &SummaryStats);

ShutdownReadDMA:
	// Shutdown the test threads if not already done so.
	ShutdownTestThreads(pcfgParams, &params[0]);
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		// Free the buffer that was allocated for each thread
		FreeThreadParams(&params[i]);
	}

	// Close our interface with the driver.
	CloseHandle(hReadDMAChannel);

	if (pcfgParams->bVerbose == TRUE)
	{
		// print debug messages
		printf("\nRead DMA Complete\n");
	}
	return 0;
}

/*! WriteReadDMA 
 *
 * \brief Write then Read loopback stress test
 * \param pcfgParams
 * \return Status
 */
INT32 
WriteReadDMA(
	PCFG_PARAMS		pcfgParams
)
{
	THREAD_PARAMS	params[NUM_STRESS_PARAMS];
	HANDLE			hWriteDMAChannel = 0;
	HANDLE			hReadDMAChannel = 0;
	PASS_STATS		WritePassStats;
	PASS_STATS		PassStats;
	SUMMARY_STATS	SummaryStats;
	UINT64			BufferSize;
	UINT32			NumberTransfers;
	UINT32			i;
	BOOLEAN			Running;

	// Hard set the loopback to a single thread. It is currently not 
	// designed to handle more than one.
	pcfgParams->NumberThreads = 1;

	// get a parameters from the command line
	pcfgParams->MaxTransferSize = DEFAULT_DMA_LENGTH;
	pcfgParams->BufferSize		= DEFAULT_BUFFER_SIZE;
	pcfgParams->CardAddress		= 0;
	
	if (pcfgParams->CmdArgc > 1)
	{
		pcfgParams->CardAddress	= (UINT64) __strtoull(pcfgParams->CmdArgv[1]);
	}
	else 
	{
		if (pcfgParams->bRandomize)
		{
			pcfgParams->CardAddress = (UINT64) DEFAULT_ADDR_DMA_MAX;
		}
	}
	if (pcfgParams->CmdArgc > 2)
	{
		BufferSize = (UINT64)__strtoull(pcfgParams->CmdArgv[2]);
		if (BufferSize > 0x00000000ffffffff)
		{
			printf("Max transfer Size (%lld) is too large\n", BufferSize);
			return -2;
		}
		pcfgParams->BufferSize		= (UINT32)BufferSize;
		pcfgParams->MaxTransferSize = pcfgParams->BufferSize;
	}

#if 0 // Paramter validation feature disabled
	if (pcfgParams->MaxTransferSize > pcfgParams->MaxDMAAddress)
	{
		if (pcfgParams->bVerbose == TRUE)
		{
			printf("Max transfer size is larger than the DMA Memory, setting to DMA Memory size.\n");
		}
		pcfgParams->MaxTransferSize = pcfgParams->MaxDMAAddress;
		pcfgParams->BufferSize		= pcfgParams->MaxDMAAddress;
	}
	if (!pcfgParams->bWriteFIFO)
	{
		if (pcfgParams->CardAddress > pcfgParams->MaxDMAAddress)
		{
			if (pcfgParams->bVerbose == TRUE)
			{
				printf("Write Card Offset is larger than the DMA Memory, setting Card Offset to 0.\n");
			}
			pcfgParams->CardAddress = 0;
		}
	}
	if (!pcfgParams->bReadFIFO)
	{
		if (pcfgParams->CardAddress > pcfgParams->MaxDMAAddress)
		{
			if (pcfgParams->bVerbose == TRUE)
			{
				printf("Read Card Offset is larger than the DMA Memory, setting Card Offset to 0.\n");
			}
			pcfgParams->CardAddress = 0;
		}
	}
#endif // Feature disabled

	// Make sure there we do at least one transfer for each thread
	NumberTransfers	= pcfgParams->Iterations;
	if (NumberTransfers < (UINT32)pcfgParams->NumberThreads)
	{
		NumberTransfers = pcfgParams->NumberThreads;
	}

	InitThreadParams(pcfgParams, &params[READ_STRESS]);
	params[READ_STRESS].Iterations		= NumberTransfers;
	params[READ_STRESS].WriteDirection	= FALSE;
	InitThreadParams(pcfgParams, &params[WRITE_STRESS]);
	params[WRITE_STRESS].Iterations		= NumberTransfers;
	params[WRITE_STRESS].WriteDirection	= TRUE;

	// Now that we have parsed all the parameters and options, now we try
	// establish contact with the driver...
	// Open the specific DMA Channel number
	hWriteDMAChannel = OpenDMAChannel(pcfgParams->cardNum, pcfgParams->WriteDMAChannel);
	if (hWriteDMAChannel == INVALID_HANDLE_VALUE)
	{
		printf("Error: Driver for board number %d, Write DMA Channel %d not found, WriteReadDMA command failed\n", 
					pcfgParams->cardNum, pcfgParams->WriteDMAChannel);
		pcfgParams->iPass = ERROR_NO_BOARD_FOUND;
		goto ShutdownWriteReadDMA;
	}
	hReadDMAChannel = OpenDMAChannel(pcfgParams->cardNum, pcfgParams->ReadDMAChannel);
	if (hReadDMAChannel == INVALID_HANDLE_VALUE)
	{
		printf("Error: Driver for board number %d, Read DMA Channel %d not found, WriteReadDMA command failed\n", 
					pcfgParams->cardNum, pcfgParams->ReadDMAChannel);
		pcfgParams->iPass = ERROR_NO_BOARD_FOUND;
		goto ShutdownWriteReadDMA;
	}
	params[READ_STRESS].hReadDevice			= hReadDMAChannel;
	params[WRITE_STRESS].hWriteDevice		= hWriteDMAChannel;

	// Allocate and setup the Receive Buffer
	params[READ_STRESS].bPatternSpecified	= TRUE;
	params[READ_STRESS].DataPatternType		= PATTERN_INCREMENTING;
	if (BufferAllocAndSet(pcfgParams, &params[READ_STRESS]))
	{
		goto ShutdownWriteReadDMA;
	}

	// Make the send size the Max Transfer Size for allocation.
	pcfgParams->BufferSize = pcfgParams->MaxTransferSize;
	// Allocate and setup the Send Buffer
	if (BufferAllocAndSet(pcfgParams, &params[WRITE_STRESS]))
	{
		goto ShutdownWriteReadDMA;
	}

	// Startup the Send thread(s)
	if (StartThread(&params[0], (LPTHREAD_START_ROUTINE)WRLoopbackThreadProc) != 0)
	{
		goto ShutdownWriteReadDMA;
	}

	// print debug messages
	if (pcfgParams->bVerbose == TRUE)
	{
		printf("Number Transfers = %d\n", NumberTransfers);
		printf("MaxTransferSize = %d\n", pcfgParams->MaxTransferSize);
		printf("Send Buffer Allocated Size per thread = %d\n", pcfgParams->MaxTransferSize);
	}

	InitSummaryStats(&SummaryStats);
	// Clear the stats by doing a read first.
	GetChannelStats(hWriteDMAChannel, &WritePassStats);
	GetChannelStats(hReadDMAChannel, &PassStats);
	InitPassStats(&WritePassStats);
	InitPassStats(&PassStats);

	// Loop forever until all threads are finished
	Running = TRUE;
	while (Running)
	{
		// Wait 1 second
		Sleep(1000);
		Running = FALSE;
		GetChannelStats(hWriteDMAChannel, &WritePassStats);
		GetChannelStats(hReadDMAChannel, &PassStats);
		PassStats.DPCsPerSecond	+= WritePassStats.DPCsPerSecond;
		PassStats.IntsPerSecond	+= WritePassStats.IntsPerSecond;
		PassStats.TransfersPerSecond += WritePassStats.TransfersPerSecond;
		PassStats.BytesTransfered = 0;
		for (i = 0; i < NUM_STRESS_PARAMS; i++)
		{
			PassStats.BytesTransfered += params[i].TotalByteCountTransferred;
			params[i].TotalByteCountTransferred = 0;
		}
		// Display the results of this pass
		DisplayPassInfo(pcfgParams, &PassStats, &SummaryStats);
		Running = TestRunControl(pcfgParams, &params[0]);
	}


	// Display the summaries from this test run
	DisplaySummaryInfo(pcfgParams, &SummaryStats);

ShutdownWriteReadDMA:
	// Shutdown the test threads if not already done so.
	ShutdownTestThreads(pcfgParams, &params[0]);

	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		// Free the buffer that was allocated for each thread
		FreeThreadParams(&params[i]);
	}

	// Close our interface with the driver.
	CloseHandle(hWriteDMAChannel);
	CloseHandle(hReadDMAChannel);

	if (pcfgParams->bVerbose == TRUE)
	{
		// print debug messages
		printf("Write/Read Stress Complete\n");
	}
	return 0;
}

/*! FIFOC2S
 *
 * \brief
 * \param pcfgParams
 * \return Status
 */
INT32 
FIFOC2S(
	PCFG_PARAMS			pcfgParams
)
{
	THREAD_PARAMS		params[MAX_THREADS];
	HANDLE				hReadDMAChannel;
	PASS_STATS			PassStats;
	SUMMARY_STATS		SummaryStats;
	GENENRATOR_CHECKER_PARAMS	GenChkParams;
	TRAFFIC_CTRL		TrafficCtrl;
	UINT64				BufferSize;
	UINT32				NumberTransfers;

	UINT32				FullPacketCount = 0;
	UINT32				PartialPacketCount = 0;
	UINT32				ZeroPacketCount = 0;

	UINT32				i;
	INT32				LoopCount;
	BOOLEAN				Running;

	// get a parameters from the command line
	NumberTransfers	= pcfgParams->Iterations;

	// We are here because the Application needs to provide the Receive Bufffer
	pcfgParams->CardAddress		= 0;
	pcfgParams->BufferSize		= DEFAULT_MAX_TRANSFER_SIZE;
	pcfgParams->MaxTransferSize = DEFAULT_STRESS_DMA_LENGTH;
	
	if (pcfgParams->CmdArgc > 1)
	{
		pcfgParams->MaxTransferSize = (UINT32) __strtoul(pcfgParams->CmdArgv[1]);
	}
	if (pcfgParams->CmdArgc > 2)
	{
		BufferSize = (UINT64) __strtoull(pcfgParams->CmdArgv[2]);
		if (BufferSize > 0x00000000ffffffff)
		{
			printf("Max Write Size (%lld) is too large\n", BufferSize);
			return -2;
		}
		pcfgParams->BufferSize = (UINT32)BufferSize;
	}

	if ((pcfgParams->MaxTransferSize * 16) > pcfgParams->BufferSize)
	{
		pcfgParams->BufferSize = pcfgParams->MaxTransferSize * 16;
	}
	if (pcfgParams->bPatternSpecified)
	{
		if (pcfgParams->DataPatternType == PATTERN_RANDOM)
		{
			pcfgParams->bPatternSpecified = FALSE;
			printf("Random pattern validation is not currently supported in FIFO mode.\n");
		}
	}

	// Make sure there we do at least one transfer for each thread
	if (NumberTransfers < (UINT32)pcfgParams->NumberThreads)
	{
		NumberTransfers = pcfgParams->NumberThreads;
	}
	else
	{
		NumberTransfers = NumberTransfers / pcfgParams->NumberThreads;
	}

	printf("\nThis test will run for %d transfers or %d seconds, which ever comes first.\n\n", pcfgParams->Iterations, pcfgParams->Seconds);

	// print debug messages
	if (pcfgParams->bVerbose == TRUE)
	{
		printf("Number Transfers = %d\n", NumberTransfers);
		printf("MaxTransferSize = %d\n", pcfgParams->MaxTransferSize);
		printf("Buffer Size = %d\n", pcfgParams->BufferSize);
		printf("Threads = %d\n", pcfgParams->NumberThreads);
	}

	InitSummaryStats(&SummaryStats);
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		InitThreadParams(pcfgParams, &params[i]);
		params[i].Iterations		= NumberTransfers;
		params[i].WriteDirection	= FALSE;
	}

	// Now that we have parsed all the parameters and options, now we try
	// establish contact with the driver...
	// Open the specific DMA Channel number
	hReadDMAChannel = OpenDMAChannel(pcfgParams->cardNum, pcfgParams->ReadDMAChannel);
	if (hReadDMAChannel == INVALID_HANDLE_VALUE)
	{
		printf("Error: Driver for board number %d, DMA Channel %d not found, FIFOC2S command failed\n", 
					pcfgParams->cardNum, pcfgParams->ReadDMAChannel);
		pcfgParams->iPass = ERROR_NO_BOARD_FOUND;
		goto ShutdownFIFOC2S;
	}
	//Allocate all the per thread buffers
	if (BufferAllocAndSet(pcfgParams, &params[0]))
	{
		goto ShutdownFIFOC2S;
	}
	// Copy the driver handle to all the thread stores
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		params[i].hReadDevice		= hReadDMAChannel;
		params[i].FIFOBuffer		= params[0].Buffer;
		params[i].AllocSize			= params[0].AllocSize;
	}
	// Setup the FIFO Mode
	if (SetupChannelMode(pcfgParams, hReadDMAChannel, params[0].Buffer, (DWORD)params[0].AllocSize, FALSE))
	{
        printf("Error: SetupChannelMode failed\n");
        pcfgParams->iPass = errno;
		goto ShutdownFIFOC2S;
	}

	// Setup the parameters for the Traffic Generator/Checker
	GenChkParams.DataSeed = 0x03020100;
	GenChkParams.ErrorCount = 0;
	GenChkParams.NumTransfers = pcfgParams->Iterations;
	GenChkParams.TransferLength[0] = pcfgParams->MaxTransferSize - 1;
	GenChkParams.TransferLength[1] = pcfgParams->MaxTransferSize - 1;  // (pcfgParams->MaxTransferSize / 2) - 1;
	GenChkParams.TransferLength[2] = pcfgParams->MaxTransferSize - 1;  // (pcfgParams->MaxTransferSize / 4) - 1;
	GenChkParams.TransferLength[3] = pcfgParams->MaxTransferSize - 1;  // (pcfgParams->MaxTransferSize / 8) - 1;
	TrafficCtrl.DMAChannel	= pcfgParams->ReadDMAChannel;
	TrafficCtrl.Generator	= TRUE;
	TrafficCtrl.Start		= TRUE;

	// Setup the traffic Generator so we get DMA transfers
	if (SetupTrafficGenChk(pcfgParams, &TrafficCtrl,  &GenChkParams))
	{
        printf("Error: SetupTrafficGenChk failed\n");
        pcfgParams->iPass = errno;
		goto ShutdownFIFOC2S;
	}
	// Now we have the buffers we need start the test threads
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		StartThread(&params[i], (LPTHREAD_START_ROUTINE)FIFOc2sThreadProc);
	}
	// Clear the stats by doing a read first.
	GetChannelStats(hReadDMAChannel, &PassStats);
	InitPassStats(&PassStats);

	// Loop forever until all threads are finished
	Running = TRUE;
	LoopCount = 2;
	while (Running)
	{
		// Wait 1 second
		Sleep(1000);
		Running = FALSE;
		GetChannelStats(hReadDMAChannel, &PassStats);
        PassStats.BytesTransfered = 0;
		for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
		{
			PassStats.BytesTransfered += params[i].TotalByteCountTransferred;
			params[i].TotalByteCountTransferred = 0;
		}
		// Display the results of this pass
		DisplayPassInfo(pcfgParams, &PassStats, &SummaryStats);
		Running = TestRunControl(pcfgParams, &params[0]);
		if (PassStats.BytesTransfered != 0)
		{
			LoopCount = 2;
		}
		else
		{
			if (--LoopCount == 0)
			{
#if WIN32
#if _DEBUG
				DumpTrafficGenChk(pcfgParams, &TrafficCtrl);
				DumpChannelInfo(hReadDMAChannel);
#endif // Debug
#endif // Windows Debug
				Running = FALSE;
			}
		}
	}
ShutdownFIFOC2S:
	// Shutdown the test threads if not already done so.
	ShutdownTestThreads(pcfgParams, &params[0]);
	// After shutting down the threads, wait before shutting down the packet generator 
	Sleep(100);

	memset(&GenChkParams, 0, sizeof(GENENRATOR_CHECKER_PARAMS));
	TrafficCtrl.Start = FALSE;
	// Shutdown the traffic generator
	SetupTrafficGenChk(pcfgParams, &TrafficCtrl, &GenChkParams);
	// After shutting down the traffic generator, let the thread(s) drain 
	Sleep(100);
	// Drain off any remaining packets left by the Packet Generator.
	DrainFIFOc2s(&params[0]);
	// Shutdown the test threads if not already done so.
//	ShutdownTestThreads(pcfgParams, &params[0]);
	// Display the summaries from this test run
	DisplaySummaryInfo(pcfgParams, &SummaryStats);

	// Shutdown the channel to release our buffer
	ShutdownChannelMode(pcfgParams, hReadDMAChannel, FALSE);

	if (pcfgParams->bVerbose == TRUE)
	{
		for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
		{
			ZeroPacketCount += params[i].ZeroPacketCount;
			PartialPacketCount += params[i].PartialPacketCount;
			FullPacketCount += params[i].FullPacketCount;
		}
		printf("Packet Stats: Full %d, Partial %d, Empty %u\n", 
				FullPacketCount, PartialPacketCount, ZeroPacketCount);
	}

	// Free the buffer pool
	FreeThreadParams(&params[0]);

	// Close our interface with the driver.
	CloseHandle(hReadDMAChannel);

	if (pcfgParams->bVerbose == TRUE)
	{
		// print debug messages
		printf("\nFIFO Recv Complete\n");
	}
	return 0;
}


/*! FIFOS2C
 *
 * \brief
 * \param pcfgParams
 * \return Status
 */
INT32 
FIFOS2C(
	PCFG_PARAMS			pcfgParams
)
{
	THREAD_PARAMS		params[MAX_THREADS];
	HANDLE				hWriteDMAChannel;
	PASS_STATS			PassStats;
	SUMMARY_STATS		SummaryStats;
	GENENRATOR_CHECKER_PARAMS	GenChkParams;
	TRAFFIC_CTRL		TrafficCtrl;
	UINT64				BufferSize;
	UINT32				NumberTransfers;
	UINT32				FullPacketCount = 0;
	UINT32				PartialPacketCount = 0;
	UINT32				ZeroPacketCount = 0;
	UINT32				i;
//	INT32				LoopCount;
	BOOLEAN				Running;

	// get a parameters from the command line
	NumberTransfers	= pcfgParams->Iterations;

	// We are here because the Application needs to provide the Receive Bufffer
	pcfgParams->CardAddress		= 0;
	pcfgParams->BufferSize		= DEFAULT_MAX_FIFO_TRANSFER_SIZE;
	pcfgParams->MaxTransferSize = DEFAULT_STRESS_DMA_LENGTH;  // DEFAULT_FIFO_DMA_LENGTH;
	
	if (pcfgParams->CmdArgc > 1)
	{
		pcfgParams->MaxTransferSize = (UINT32) __strtoul(pcfgParams->CmdArgv[1]);
	}
	if (pcfgParams->CmdArgc > 2)
	{
		BufferSize = (UINT64) __strtoull(pcfgParams->CmdArgv[2]);
		if (BufferSize > 0x00000000ffffffff)
		{
			printf("Max Write Size (%lld) is too large\n", BufferSize);
			return -2;
		}
		pcfgParams->BufferSize = (UINT32)BufferSize;
	}

	if ((pcfgParams->MaxTransferSize * 16) > pcfgParams->BufferSize)
	{
		pcfgParams->BufferSize = pcfgParams->MaxTransferSize * 16;
	}
	if (pcfgParams->bPatternSpecified)
	{
		if (pcfgParams->DataPatternType == PATTERN_RANDOM)
		{
			pcfgParams->bPatternSpecified = FALSE;
			printf("Random pattern validation is not currently supported in FIFO mode.\n");
		}
	}

	// Make sure there we do at least one transfer for each thread
	if (NumberTransfers < (UINT32)pcfgParams->NumberThreads)
	{
		NumberTransfers = pcfgParams->NumberThreads;
	}
	else
	{
		NumberTransfers = NumberTransfers / pcfgParams->NumberThreads;
	}

	printf("\nThis test will run for %d transfers or %d seconds, which ever comes first.\n\n", pcfgParams->Iterations, pcfgParams->Seconds);

	// print debug messages
	if (pcfgParams->bVerbose == TRUE)
	{
		printf("Number Transfers = %d\n", NumberTransfers);
		printf("MaxTransferSize = %d\n", pcfgParams->MaxTransferSize);
		printf("Buffer Size = %d\n", pcfgParams->BufferSize);
		printf("Threads = %d\n", pcfgParams->NumberThreads);
	}

	InitSummaryStats(&SummaryStats);
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		InitThreadParams(pcfgParams, &params[i]);
		params[i].Iterations		= NumberTransfers;
		params[i].WriteDirection	= TRUE;
	}

	// Now that we have parsed all the parameters and options, now we try
	// establish contact with the driver...
	// Open the specific DMA Channel number
	hWriteDMAChannel = OpenDMAChannel(pcfgParams->cardNum, pcfgParams->WriteDMAChannel);
	if (hWriteDMAChannel == INVALID_HANDLE_VALUE)
	{
		printf("Error: Driver for board number %d, DMA Channel %d not found, FIFOS2C command failed\n", 
					pcfgParams->cardNum, pcfgParams->WriteDMAChannel);
		pcfgParams->iPass = ERROR_NO_BOARD_FOUND;
		goto ShutdownFIFOS2C;
	}
	//Allocate all the per thread buffers
	if (BufferAllocAndSet(pcfgParams, &params[0]))
	{
		goto ShutdownFIFOS2C;
	}
	// Copy the driver handle to all the thread stores
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		params[i].hWriteDevice		= hWriteDMAChannel;
		params[i].FIFOBuffer		= params[0].Buffer;
		params[i].AllocSize			= params[0].AllocSize;
	}
	// Setup the FIFO Mode
	if (SetupChannelMode(pcfgParams, hWriteDMAChannel, params[0].Buffer, (DWORD)params[0].AllocSize, TRUE))
	{
        printf("Error: SetupChannelMode failed\n");
        pcfgParams->iPass = errno;
		goto ShutdownFIFOS2C;
	}

	// Setup the parameters for the Traffic Generator/Checker
	GenChkParams.DataSeed = 0x03020100;
	GenChkParams.ErrorCount = 0;
	GenChkParams.NumTransfers = 0; //pcfgParams->Iterations;
	GenChkParams.TransferLength[0] = pcfgParams->MaxTransferSize - 1;
	GenChkParams.TransferLength[1] = pcfgParams->MaxTransferSize - 1;  // (pcfgParams->MaxTransferSize / 2) - 1;
	GenChkParams.TransferLength[2] = pcfgParams->MaxTransferSize - 1;  // (pcfgParams->MaxTransferSize / 4) - 1;
	GenChkParams.TransferLength[3] = pcfgParams->MaxTransferSize - 1;  // (pcfgParams->MaxTransferSize / 8) - 1;
	TrafficCtrl.DMAChannel	= pcfgParams->WriteDMAChannel;
	TrafficCtrl.Generator	= FALSE; 
	TrafficCtrl.Start		= TRUE;

	// Setup the traffic Generator so we get DMA transfers
	if (SetupTrafficGenChk(pcfgParams, &TrafficCtrl,  &GenChkParams))
	{
        printf("Error: SetupTrafficGenChk failed\n");
        pcfgParams->iPass = errno;
		goto ShutdownFIFOS2C;
	}
#if _DEBUG
	DumpTrafficGenChk(pcfgParams, &TrafficCtrl);
	DumpChannelInfo(hWriteDMAChannel);
#endif // Debug

	// Now we have the buffers we need start the test threads
	for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
	{
		StartThread(&params[i], (LPTHREAD_START_ROUTINE)FIFOs2cThreadProc);
	}
	// Clear the stats by doing a read first.
	GetChannelStats(hWriteDMAChannel, &PassStats);
	InitPassStats(&PassStats);

	// Loop forever until all threads are finished
	Running = TRUE;
//	LoopCount = 2;
	while (Running)
	{
		// Wait 1 second
		Sleep(1000);
		Running = FALSE;
		GetChannelStats(hWriteDMAChannel, &PassStats);
        PassStats.BytesTransfered = 0;
		for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
		{
			PassStats.BytesTransfered += params[i].TotalByteCountTransferred;
			params[i].TotalByteCountTransferred = 0;
		}
		// Display the results of this pass
		DisplayPassInfo(pcfgParams, &PassStats, &SummaryStats);
		Running = TestRunControl(pcfgParams, &params[0]);
//		if (PassStats.BytesTransfered != 0)
//		{
//			LoopCount = 2;
//		}
//		else
//		{
//			if (--LoopCount == 0)
//			{
//				Running = FALSE;
//			}
//		}
	}
ShutdownFIFOS2C:
	// Shutdown the test threads if not already done so.
	ShutdownTestThreads(pcfgParams, &params[0]);
	// After shutting down the thread(s) wait and then shut down the packet checker
	Sleep(100);

#if _DEBUG
	Sleep(100);
	DumpTrafficGenChk(pcfgParams, &TrafficCtrl);
	DumpChannelInfo(hWriteDMAChannel);
#endif // Debug

	memset(&GenChkParams, 0, sizeof(GENENRATOR_CHECKER_PARAMS));
	TrafficCtrl.Start = FALSE;
	// Shutdown the traffic generator
	SetupTrafficGenChk(pcfgParams, &TrafficCtrl, &GenChkParams);

	// After shutting down the traffic generator, let the thread(s) drain 
	Sleep(100);
	// Drain off any remaining packets left by the Packet Checker.
	DrainFIFOs2c(&params[0]);

	// Shutdown the test threads if not already done so.
//	ShutdownTestThreads(pcfgParams, &params[0]);
	// Display the summaries from this test run
	DisplaySummaryInfo(pcfgParams, &SummaryStats);
	 
	// Shutdown the channel to release our buffer
	ShutdownChannelMode(pcfgParams, hWriteDMAChannel, TRUE);

	if (pcfgParams->bVerbose == TRUE)
	{
		for (i = 0; i < (UINT32)pcfgParams->NumberThreads; i++)
		{
			ZeroPacketCount += params[i].ZeroPacketCount;
			PartialPacketCount += params[i].PartialPacketCount;
			FullPacketCount += params[i].FullPacketCount;
		}
		printf("Packet Stats: Full %d, Partial %d, Empty %d\n", 
				FullPacketCount, PartialPacketCount, ZeroPacketCount);
	}

	// Free the buffer pool
	FreeThreadParams(&params[0]);

	// Close our interface with the driver.
	CloseHandle(hWriteDMAChannel);

	if (pcfgParams->bVerbose == TRUE)
	{
		// print debug messages
		printf("\nFIFO Write (S2C) Complete\n");
	}
	return 0;
}
