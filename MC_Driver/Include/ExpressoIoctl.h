// -------------------------------------------------------------------------
// 
// PRODUCT:			Expresso Multi-Channel DMA Driver
// MODULE NAME:		ExpressoIOCtl.h
// 
// MODULE DESCRIPTION: 
// 
// Contains the Driver IOCtl defines, typedefs, structures and function prototypes.
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

//
// This header defines all the User defined IOCTL codes and data structures for 
// the PCI Driver.  The data structures sent to/from the driver are also defined.
// (Any user defined types that are shared by the driver and app must be defined in this
// function or by the system)
//
// Define control codes for the Expresso Driver
//
// IOCTL  Description             	Data to Driver       	Data from Driver
// -----  -----------             	-----------------     	---------------------
//  900   Get Board Config        	None                  	BOARD_CONFIG_STRUCT
//  902   Memory Read             	MEM_COPY_STRUCT      		data
//  903   Memory Write            	MEM_COPY_STRUCT      		data
//  905	  Configure Channel			CHANNEL_CONFIG_STRUCT	CHANNEL_CONFIG_STRUCT
//  908   Get DMA Performance     	EngineNum (ULONG)  		DMA_STAT_STRUCT
//

#ifndef __ExpressoIOCtl__h_
#define __ExpressoIOCtl__h_

#define MAX_BARS                    6       // TYPE-0 configuration header values.
#define PCI_STD_CONFIG_HEADER_SIZE  64      // 64 Bytes is the standard header size

// Vendor and Device IDs
#define NWLOGIC_VENDOR_ID			0x19AA
#define NWLOGIC_MC_VERSION_1_DEV_ID 0xE008
#define NWLOGIC_MC_VERSION_2_DEV_ID 0xE00C

#define	PCI_EXPRESS_CAPABILITY_ID	0x10
#define MSI_MESSAGE_ID				0x05
#define	MSIX_MESSAGE_ID				0x11

#ifndef WIN32   // Linux version ---------------------------------------------

#ifndef PACKED
#    define PACKED					__attribute__((packed))
#endif /* PACKED */

#define PROCFS_FB_NAME				"expresso"
#define PROCFS_DMA_NAME				"DMA"
#define PROCFS_FB_PATH				"/proc/EXPO/" PROCFS_FB_NAME

#define PROCFS_PATH					"/proc/EXPO"

#define NWLogic_VendorID            0x19AA
#define NWLogic_PciExp_E008         0xE008
#define NWLogic_PciExp_E00C         0xE00C

/*!
** Number of descriptors to preallocate for each DMA engine. This
** will limit the maximum amount that can be transfered. Theoretically
** the maximum will be this number times the maximum amount of a 
** single DMA descriptor transfer.
*/
#define NUM_DESCRIPTORS_PER_ENGINE          8192

// General IOCTL
#define GET_BOARD_CONFIG_IOCTL              0x900
#define READ_MEM_ACCESS_IOCTL        	    0x902
#define WRITE_MEM_ACCESS_IOCTL       	    0x903
#define CONFIGURE_CHANNEL_IOCTL			    0x905
#define GET_CHANNEL_STATS_IOCTL			    0x908
#define	WRITE_PCI_CONFIG_IOCTL				0x909
#define READ_PCI_CONFIG_IOCTL				0x90A

#define DUMP_CHANNEL_STRUCTS_IOCTL		    0x912

// FIFO Mode DMA IOCTLs
#define SETUP_CHANNEL_IOCTL      	        0x920
#define SHUTDOWN_CHANNEL_IOCTL              0x921

// FIFO Mode IOCTLs
#define FIFO_C2S_IOCTL        			    0x926
#define FIFO_S2C_IOCTL        			    0x927

#define STATUS_INVALID_PARAMETER			1
#define STATUS_INVALID_DEVICE_STATE			2
#define STATUS_INSUFFICIENT_RESOURCES		3

#else   // Windows version  ------------------------------------------------------------

// Use this GUID for application access to the driver.
#define PUBLIC_GUID_EXPRESSO_DRIVER_INTERFACE \
{0xfe8ce7e3, 0x1063, 0x4947, { 0xa2, 0xf9, 0x78, 0xae, 0x52, 0x10, 0x44, 0xab}}

// This GUID should be identical to the public version except the format is different.
// It is used internally in the driver.
// {FE8CE7E3-1063-4947-A2F9-78AE521044AB}
DEFINE_GUID(GUID_EXPRESSO_DRIVER_INTERFACE, 
0xfe8ce7e3, 0x1063, 0x4947, 0xa2, 0xf9, 0x78, 0xae, 0x52, 0x10, 0x44, 0xab);

// General IOCTL
#define GET_BOARD_CONFIG_IOCTL          CTL_CODE(FILE_DEVICE_UNKNOWN, 0x900, METHOD_BUFFERED,   FILE_ANY_ACCESS)
#define READ_MEM_ACCESS_IOCTL        	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x902, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define WRITE_MEM_ACCESS_IOCTL       	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x903, METHOD_IN_DIRECT,  FILE_ANY_ACCESS)
#define GET_CHANNEL_STATS_IOCTL			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x908, METHOD_BUFFERED,   FILE_ANY_ACCESS)
#define WRITE_PCI_CONFIG_IOCTL			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x909, METHOD_IN_DIRECT,  FILE_ANY_ACCESS)
#define READ_PCI_CONFIG_IOCTL        	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x90A, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define USER_INTERRUPT_WAIT_IOCTL		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x910, METHOD_BUFFERED,   FILE_ANY_ACCESS)

#define DUMP_CHANNEL_STRUCTS_IOCTL		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x912, METHOD_BUFFERED,   FILE_ANY_ACCESS)

// FIFO Mode DMA IOCTLs
#define SETUP_CHANNEL_IOCTL      	    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x920, METHOD_IN_DIRECT,  FILE_ANY_ACCESS)
#define SHUTDOWN_CHANNEL_IOCTL          CTL_CODE(FILE_DEVICE_UNKNOWN, 0x921, METHOD_IN_DIRECT,  FILE_ANY_ACCESS)

// FIFO Mode IOCTLs
#define FIFO_C2S_IOCTL		   			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x926, METHOD_OUT_DIRECT,  FILE_ANY_ACCESS)
#define FIFO_S2C_IOCTL		   			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x927, METHOD_OUT_DIRECT,  FILE_ANY_ACCESS)


//#pragma pack(push,1)
#pragma pack(1)

#ifndef PACKED
#define PACKED                      
#endif /* PACKED */

#endif  // WINDOWS version ----------------------------------------------------------

#define PCI_DEVICE_SPECIFIC_SIZE 192    // Device specific bytes [64..256].

typedef struct _PCI_CONFIG_HEADER
{
    UINT16		VendorId;           // VendorID[15:0]
	UINT16		DeviceId;           // DeviceID[15:
	UINT16		Command;            // Command[15:0]
	UINT16		Status;             // Status[15:0]
    UINT8		RevisionId;         // RevisionId[7:0]
	UINT8		Interface;          // Interface[7:0]
	UINT8		SubClass;           // SubClass[7:0]
	UINT8		BaseClass;          // BaseClass[7:0]
	UINT8		CacheLineSize;
	UINT8		LatencyTimer;
	UINT8		HeaderType;
	UINT8		BIST;
	UINT32		BarCfg[MAX_BARS];   // BAR[5:0] Configuration
	UINT32		CardBusCISPtr;
    UINT16		SubsystemVendorId;  // SubsystemVendorId[15:0]
	UINT16		SubsystemId;        // SubsystemId[15:0]
	UINT32		ExpRomCfg;          // Expansion ROM Configuration
	UINT8		CapabilitiesPtr;
    UINT8		Reserved1[3];
	UINT32		Reserved2[1];
	UINT8		InterruptLine;
	UINT8		InterruptPin;
	UINT8		MinimumGrant;
	UINT8		MaximumLatency;
	UINT8		DeviceSpecific[PCI_DEVICE_SPECIFIC_SIZE];
} PACKED PCI_CONFIG_HEADER, *PPCI_CONFIG_HEADER;

typedef struct _PCI_DEVICE_SPECIFIC_HEADER
{
	UINT8		PCIeCapabilityID;
	UINT8		PCIeNextCapabilityPtr;
    UINT16		PCIeCapability;
    UINT32		PCIeDeviceCap;      // PCIe Capability: Device Capabilities
    UINT16		PCIeDeviceControl;  // PCIe Capability: Device Control
    UINT16		PCIeDeviceStatus;   // PCIe Capability: Device Status
    UINT32		PCIeLinkCap;        // PCIe Capability: Link Capabilities
    UINT16		PCIeLinkControl;    // PCIe Capability: Link Control
    UINT16		PCIeLinkStatus;     // PCIe Capability: Link Status
} PACKED PCI_DEVICE_SPECIFIC_HEADER, *PPCI_DEVICE_SPECIFIC_HEADER;

/* The PCI Config space capabilites for MSI */
typedef struct _PCI_MSI_CAPABILITY  {
    UINT8		PCIeCapabilityID;
	UINT8		PCIeNextCapabilityPtr;
	UINT16		MessageControl;
} PACKED PCI_MSI_CAPABILITY, *PPCI_MSI_CAPABILITY;

/* The PCI Config space capabilites for MSIX */
typedef struct _PCI_MSIX_CAPABILITY  {
	UINT8		PCIeCapabilityID;
	UINT8		PCIeNextCapabilityPtr;
	UINT16		MessageControl;
	UINT32		TableOffsetBIR;
	UINT32		PBAOffsetBIR;
} PACKED PCI_MSIX_CAPABILITY, *PPCI_MSIX_CAPABILITY;

// BOARD_CONFIG_STRUCT
//
//  Board Configuration Definitions
typedef struct _BOARD_CONFIG_STRUCT
{
    PCI_CONFIG_HEADER   PciConfig;
	UINT32	    NumDmaChannels;		// Number of DMA Channels
	UINT8		DriverVersionMajor;	// Driver Major Revision number
	UINT8		DriverVersionMinor;	// Driver Minor Revision number
	UINT8		DriverVersionSubMinor; // Driver Sub Minor Revision number 
	UINT8		DriverVersionBuildNumber; // Driver Build number
} BOARD_CONFIG_STRUCT, *PBOARD_CONFIG_STRUCT;


// Board Config Struc defines
#define CARD_IRQ_MSI_ENABLED                0x0008
#define CARD_IRQ_MSIX_ENABLED               0x0040
#define CARD_MAX_PAYLOAD_SIZE_MASK          0x0700
#define CARD_MAX_READ_REQUEST_SIZE_MASK     0x7000

// BarCfg defines
#define BAR_CFG_BAR_SIZE_OFFSET     	    (24)
#define BAR_CFG_BAR_PRESENT				    0x0001
#define BAR_CFG_BAR_TYPE_MEMORY			    0x0000
#define BAR_CFG_BAR_TYPE_IO		 		    0x0002
#define BAR_CFG_MEMORY_PREFETCHABLE		    0x0004
#define BAR_CFG_MEMORY_64BIT_CAPABLE	    0x0008

// ExpRomCfg defines
#define EXP_ROM_BAR_SIZE_OFFSET	            (24)
#define EXP_ROM_PRESENT			            0x0001
#define EXP_ROM_ENABLED			            0x0002

// Return status defines
#define STATUS_SUCCESSFUL		    	    0x00
#define STATUS_INCOMPLETE			        0x01
#define STATUS_INVALID_BARNUM		        0x02
#define STATUS_INVALID_CARDOFFSET	        0x04
#define STATUS_OVERFLOW				        0x08
#define STATUS_INVALID_BOARDNUM		        0x10			// Board number does not exist
#define STATUS_INVALID_MODE					0x20			// Mode not supported by hardware
#define STATUS_BAD_PARAMETER				0x40			// Passed an invalid parameter

// Return DMA status defines
#define STATUS_DMA_SUCCESSFUL				0x00
#define STATUS_DMA_INCOMPLETE				0x01
#define STATUS_DMA_INVALID_ENGINE			0x02
#define STATUS_DMA_INVALID_ENGINE_DIRECTION	0x04
#define STATUS_DMA_BUFFER_MAP_ERROR			0x08
#define STATUS_DMA_ENGINE_TIMEOUT			0x10
#define STATUS_DMA_ABORT_DESC_ALIGN_ERROR	0x20
#define STATUS_DMA_ABORT_BY_DRIVER			0x40
#define STATUS_DMA_ABORT_BY_HARDWARE		0x80

#ifndef MAX_NUM_DMA_CHANNELS
#define MAX_NUM_DMA_CHANNELS	            1024
#endif // MAX_NUM_DMA_CHANNELS


// CHANNEL_STAT_STRUCT
//
// Channel Status Structure - Status Information from a DMA Channel IOCTL transaction
typedef struct _CHANNEL_STAT_STRUCT
{
#ifndef WIN32
    UINT32		ChannelNum;         // DMA Channel number to use
#endif  // Linux Only
	UINT32		RequestCount;		// The number of Transfers requested.
	UINT32		IntsPerSecond;      // Number of interrupts since last request
	UINT32		DPCsPerSecond;      // Number of DPCs/Tasklets since last request
	UINT32		DMAErrors;			// Number of DMA Errors since last request
} CHANNEL_STAT_STRUCT, *PCHANNEL_STAT_STRUCT;

// MEM_COPY_STRUCT
//
// Memory Copy Structure - Information for performing a Memory Transfer
typedef struct _MEM_COPY_STRUCT
{
	UINT32		BarNum;             // Base Addres Register (BAR) to access
	UINT64		CardOffset;         // Byte starting offset in BAR
	UINT64		Length;             // Transaction length in bytes
#ifndef WIN32
    PUINT8		Buffer;         // Buffer Pointer
#endif  // Linux Only
} MEM_COPY_STRUCT, *PMEM_COPY_STRUCT;

// RW_PCI_CONFIG_STRUCT
//
// PCI Config Read/Write Structure - Information for reading or writing CI onfiguration space
typedef struct _RW_PCI_CONFIG_STRUCT
{
	UINT32		Offset;             // Byte starting offset in application buffer
	UINT32		Length;             // Transaction length in bytes
#ifndef WIN32
    PUINT8		Buffer;         // Buffer Pointer
#endif  // Linux Only
} RW_PCI_CONFIG_STRUCT, *PRW_PCI_CONFIG_STRUCT;

typedef enum _eDMA_MODES {
	DMA_MODE_NOT_SET,
	DMA_MODE_READ_WRITE,
	DMA_MODE_FIFO,
	DMA_MODE_STREAMING,
	DMA_MODE_ADDRESSABLE
} eDMA_MODES;

/* DMA_MODE_NOT_SET - DMA Mode not set, uses default DMA Channel Read/Write behavior */

// 
// SETUP_CHANNEL_STRUCT
//
// Setup the DMA Channel Mode Structure - Information for performing the setting up the various operating modes
typedef struct _SETUP_CHANNEL_STRUCT
{
	UINT32		ChannelNum;         // DMA Channel number to use
	UINT32		AllocationMode;     // Allocation type Flags
	UINT32		Direction;			// Direction of FIFO, C2S (0) or S2C (1)
	UINT32		RESERVED2;			// Reserved for future use
	UINT32		RESERVED3;			// Reserved for future use
	UINT32		RESERVED4;			// Reserved for future use
	UINT32		Length;				// Length of Application buffer or size to Allocate (if FIFO)
#ifndef WIN32 // Linux variant
    PVOID		BufferAddress;      // Buffer Address for data transfer
#endif // Linux -
} SETUP_CHANNEL_STRUCT, *PSETUP_CHANNEL_STRUCT;


// SHUTDOWN_CHANNEL_STRUCT
//
// Shutdown Channel Structure - Information for shutting down the DMA Channel
typedef struct _SHUTDOWN_CHANNEL_STRUCT
{
	UINT32		ChannelNum;         // DMA Channel to use
	UINT32		AllocationMode;     // Allocation type Flags
} SHUTDOWN_CHANNEL_STRUCT, *PSHUTDOWN_CHANNEL_STRUCT;


// Maximum number of transfer a single IOCtl call can retrieve
#define	MIN_NUMBER_FIFO_ENTRIES		1
#define	MAX_NUMBER_FIFO_ENTRIES		256

// FIFOC2S_ENTRY_STRUCT
//
//  FIFO Entry Structure - Per transfer structure to be included into
//		The FIFO_C2S_STRUCT
typedef struct _FIFOC2S_ENTRY_STRUCT
{
	UINT32		Offset;				// Offset into buffer pool for the receive
	UINT32		Length;        		// Length of transfer
	UINT32		Status;        		// Transfer Status
	UINT16		UserHandle;			// Contents of UserHandle
	UINT16		UserID;				// Contents of User ID
	UINT32		NumberElements;		// Number of elements used for this transfer
} FIFOC2S_ENTRY_STRUCT, *PFIFOC2S_ENTRY_STRUCT;

// Channel Status definitions
#define	STAT_DMA_OVERRUN_ERROR			0x8000		// 
#define	STAT_ERROR_MALFORMED			0x4000		//
#define	STAT_DMA_SOURCE_ERROR			0x0001		// Source Error Detected during DMA
#define	STAT_DMA_DESTINATION_ERROR		0x0002		// Destination Error Detected during DMA
#define	STAT_DMA_INTERNAL_ERROR			0x0004		// Internal Error Detected during DMA
#define STAT_WINDOWS_TRUNCATED_REQUEST	0x0800	// Windows truncated the size of teh request.

// FIFO_C2S_STRUCT
//
//  FIFO C2S Structure - Superstructure that contains multiple transfers
//	C2S indications
typedef struct _FIFO_C2S_STRUCT
{
	UINT16		ChannelNum;         // DMA Channel to use
	UINT16		AvailNumEntries;	// Number of transfer entries available
	UINT16		RetNumEntries;		// Returned Number of transfer entries
	UINT16		ChannelStatus;		// DMA Channel status
	UINT32		ReleaseCount;		// Number of DMA Elements to release.
	FIFOC2S_ENTRY_STRUCT	Transfers[1];	// Transfer Entries
} FIFO_C2S_STRUCT, *PFIFO_C2S_STRUCT;

#define	MIN_NUMBER_FIFOS2C_ENTRIES		1
#define	MAX_NUMBER_FIFOS2C_ENTRIES		16

// FIFOS2C_ENTRY_STRUCT
//
//  FIFO Entry Structure - Per transfer structure to be included into
//		The FIFO_S2C_STRUCT
typedef struct _FIFOS2C_ENTRY_STRUCT
{
	UINT32		Offset;				// Offset into buffer pool for the receive
	UINT32		Length;        		// Length of transfer
} FIFOS2C_ENTRY_STRUCT, *PFIFOS2C_ENTRY_STRUCT;

// FIFO_S2C_STRUCT
//
//  FIFO S2C Structure - Superstructure that contains multiple transfers
//	S2C indications
typedef struct _FIFO_S2C_STRUCT
{
	UINT16		ChannelNum;				// DMA Channel to use
	UINT16		NumEntries;				// Number of transfer entries available
	UINT16		NumEntriesSent;			// Actual number of entries sent
	UINT32		NumAvailableElements;	// Number of available DMA Elements after this function
	UINT32		LastTransactionStatus;	// Cumulative Status of the previous S2C Transaction
	UINT32		TransactionStatus;		// Status of the current S2C Transaction (NT STATUS value)
	FIFOS2C_ENTRY_STRUCT	Transfers[1];	// Transfer Entries
} FIFO_S2C_STRUCT, *PFIFO_S2C_STRUCT;

#ifdef WIN32   // Windows version
#pragma pack()
//#pragma pack(pop)
#endif // Windows version

#endif /* __ExpressoIOCtl__h_ */
