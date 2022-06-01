// -------------------------------------------------------------------------
// 
// PRODUCT:			Expresso Multi-Channel DMA Driver
// MODULE NAME:		ExpressoHw.h
// 
// MODULE DESCRIPTION: 
// 
// Contains the Hardware specific defines, typedefs, structures.
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

#ifndef _EXPRESSO_HW_H_
#define _EXPRESSO_HW_H_

#pragma pack(push,1)

#ifndef WIN32   // Linux version ---------------------------------------------

#ifndef PACKED
#    define PACKED                      __attribute__((packed))
#endif	// PACKED

#else	// Windows
#ifndef PACKED
#    define PACKED 
#endif	// PACKED

#endif	// Windows vs. Linux

//----------------------------------------------------------------
// DMA Channel and register Defines
//----------------------------------------------------------------
#define MAX_NUM_DMA_CHANNELS		1024

#define DMA_REG_BAR					0
#define MAX_NUMBER_OF_BARS 			6
#define	MAX_NUM_MSIX_VECTORS		1024

//----------------------------------------------------------------
// DMA Descriptor Defines
//----------------------------------------------------------------
//! DMA descriptors must be on a 64 byte boundary
#define DEFAULT_NUM_SRC_SG_ELEMENTS	8192
#define DEFAULT_NUM_DST_SG_ELEMENTS	8192 
#define DEFAULT_NUM_STA_ELEMENTS	8192 

#define ELEMENT_ALIGN_LEN     		64
#define ELEMENT_ALIGN_REQUIREMENT	(FILE_64_BYTE_ALIGNMENT)
// Maximum number of bytes for number of descriptors (-1 for alignment)
#define DMA_MAX_TRANSFER_LENGTH ((DEFAULT_NUM_SRC_SG_ELEMENTS - 1) * PAGE_SIZE)

// Make sure this is the larger of either the  DEFAULT_NUM_SRC_SG_ELEMENTS or
//  DEFAULT_NUM_DST_SG_ELEMENTS	.
#define MAXIMUM_NUM_ELEMENTS		DEFAULT_NUM_DST_SG_ELEMENTS + 1
#define MINIMUM_NUM_ELEMENTS		64

#define MAX_AXI_TRANSFER_SIZE       0x1000000

//----------------------------------------------------------------
// MSI-X Capability Defines
//----------------------------------------------------------------
#define	MSIX_MESSAGE_ID				0x11
#define	TABLE_BIR_MASK				0x07

#define	MSG_CTRL_MSIX_ENABLE		0x8000
#define	MSG_CTRL_FUNC_MASK_VECTORS	0x4000
#define MSG_CTRL_TABLE_SIZE_MASK	0x03FF

#define MSIX_TABLE_OFFSET_MASK		0xFFFFFFF8
#define MSIX_TABLE_BIR_MASK			0x00000007

#define PBA_OFFSET_MASK				0xFFFFFFF8
#define PBA_BIR_MASK				0x00000007

#ifndef _WIN32	// Linux Version

#define SOURCE_ELEMENT_ALIGN_VIRT_ADDR(x) ((PSOURCE_SG_ELEMENT_STRUCT)((((UINT32)(x)) + (ELEMENT_ALIGN_LEN - 1)) & ~(ELEMENT_ALIGN_LEN - 1)))
#define DESTINATION_ELEMENT_ALIGN_VIRT_ADDR(x) ((PDESTINATION_SG_ELEMENT_STRUCT)((((UINT32)(x)) + (ELEMENT_ALIGN_LEN - 1)) & ~(ELEMENT_ALIGN_LEN - 1)))
#define STATUS_ELEMENT_ALIGN_VIRT_ADDR(x) ((PSTATUS_SG_ELEMENT_STRUCT)((((UINT32)(x)) + (ELEMENT_ALIGN_LEN - 1)) & ~(ELEMENT_ALIGN_LEN - 1)))
#define ELEMENT_ALIGN_PHYS_ADDR(x) (((x) + (ELEMENT_ALIGN_LEN - 1)) & ~(ELEMENT_ALIGN_LEN - 1))

#endif			// Linux Version

#define	DESC_FLAGS_HW_OWNED			0x00000001
#define	DESC_FLAGS_SW_OWNED			0x00000002
#define	DESC_FLAGS_SW_FREED			0x00000004

//----------------------------------------------------------------
// Defines for accessing Byte Count and flags files in the Queue Elements
//----------------------------------------------------------------
#define	BYTE_COUNT_MASK				(24 << 0)	// Bit 0 - 23: Byte Count to transfer
#define	DMA_LOCATION				(1 << 24)	// Bit 24: 1 = AXI, 0 = PCI
#define	SRC_EOP						(1 << 25)	// Bit 25: End Of Packet Flag
#define	SRC_INTERRUPT				(1 << 26)	// Bit 26: 1 = Interrupt, 0 = No interrupts
#define	DMA_ELEM_ATTRIBUTES			(4 << 28)	// bits 28 - 31: DMA Read/Write Attributes		

#ifdef WIN32	// Windows Version
#pragma warning(disable:4214)  // Bit fields other than int warning
#endif			// Windows Version

/*!	\struct SOURCE_SG_ELEMENT_STRUCT
 *	\brief Source Scatter-Gather Queue Element structure
 */
typedef struct _SOURCE_SG_ELEMENT_STRUCT
{
	// -----------------------------------------------------------------	
	// Hardware specific entries - Do not change or reorder
	// -----------------------------------------------------------------
	UINT64 	SRC_ADDRESS;						// Source Address Pointer - 64 Bits
	union {
		UINT32 DWORD;  							// Source Queue Pointer - 64 Bits
		struct {
			UINT32			BYTE_COUNT	:24;	// Bit 0 - 23: Byte Count to transfer
			UINT32			LOCATION	:1;		// Bit 24: 1 = AXI, 0 = PCI
			UINT32			EOP			:1;		// Bit 25: End Of Packet Flag
			UINT32			INTERRUPT	:1;		// Bit 26: 1 = Interrupt, 0 = No interrupts
			UINT32						:1;		// Bit 27: Reserved
			UINT32			ATTRIBUTES	:4;		// bits 28 - 31: DMA Read Attributes		
		} FIELDS;
	} BYTE_COUNT_FLAGS;
#ifdef VERSION_2
	UINT16		USER_HANDLE;
	UINT16		USER_ID;
#else // Version 1
	UINT32		RESERVED;						// Reserved 32 bits
#endif // Version 1 vs. Version 2
} PACKED SOURCE_SG_ELEMENT_STRUCT, *PSOURCE_SG_ELEMENT_STRUCT;

/*!	\struct DESTINATION_SG_ELEMENT_STRUCT
 *	\brief Destination Scatter-Gather Queue Element structure
 */
 typedef struct _DESTINATION_SG_ELEMENT_STRUCT
{
	// -----------------------------------------------------------------	
	// Hardware specific entries - Do not change or reorder
	// -----------------------------------------------------------------
	UINT64		DEST_ADDRESS;					// Destination Address Pointer - 64 Bits
	union {
		UINT32 DWORD;  							// Source Queue Pointer - 64 Bits
		struct {
			UINT32		BYTE_COUNT	:24;		// Bit 0 - 23: Byte Count to transfer
			UINT32		LOCATION	:1;			// Bit 24: 1 = AXI, 0 = PCI
			UINT32		PACK_NEXT	:1;			// Bit 25: Set to 1 to start next transfer in next SGL
			UINT32					:2;			// Bit 26 - 27: Reserved
			UINT32		ATTRIBUTES	:4;			// bits 28 - 31: DMA Write Attributes		
		} FIELDS;
	} BYTE_COUNT_FLAGS;
#ifdef VERSION_2
	UINT16		USER_HANDLE;
	UINT16		RESERVED;
#else	// Version 1
	UINT32		RESERVED;						// Reserved 32 bits
#endif	// Version 1 vs. Version 2
} PACKED DESTINATION_SG_ELEMENT_STRUCT, *PDESTINATION_SG_ELEMENT_STRUCT;

//----------------------------------------------------------------
// Defines for accessing Status Flags in the Status Queue Element
//----------------------------------------------------------------
#define	DMA_COMPLETED				(1 << 0)	// Bit 0: DMA Completed
#define	DMA_SOURCE_ERROR			(1 << 1)	// Bit 1: Source Error Detected during DMA
#define	DMA_DESTINATION_ERROR 		(1 << 2)	// Bit 2: Destination Error Detected during DMA
#define	DMA_INTERNAL_ERROR			(1 << 3)	// Bit 3: Internal Error Detected during DMA

/*!	\struct STATUS_ELEMENT_STRUCT
 *	\brief STATUS Queue Element structure
 */
typedef struct _STATUS_ELEMENT_STRUCT
{
	// -----------------------------------------------------------------	
	// Hardware specific entries - Do not change or reorder
	// -----------------------------------------------------------------
	union {
		UINT32 	DWORD;								// Status Flags - 32 Bits
		struct {
			UINT32		COMPLETED			:1;		// Bit 0: DMA Completed
			UINT32		SOURCE_ERROR		:1;		// Bit 1: Source Error Detected during DMA
			UINT32		DESTINATION_ERROR	:1;		// Bit 2: Destination Error Detected during DMA
			UINT32		INTERNAL_ERROR		:1;		// Bit 3: Internal Error Detected during DMA
#ifdef VERSION_2
			UINT32		COMP_BYTE_COUNT		:27;	// Bits 4 - 30: Completed Byte Count
			UINT32		UPPER_STAT_NZ		:1;		// Bits 31: If set upper 32 bit contain valid status
#else // Version 1
			UINT32							:28;	// Bits 4 - 31: Reserved
#endif // Version 1 vs. Version 2
		} FIELDS;
	} STATUS;
} PACKED STATUS_ELEMENT_STRUCT, *PSTATUS_ELEMENT_STRUCT;


#ifdef VERSION_2
/*!	\struct STATUS_ELEMENT64_STRUCT
 *	\brief STATUS Queue Element structure
 */
typedef struct _STATUS_ELEMENT64_STRUCT
{
	// -----------------------------------------------------------------	
	// Hardware specific entries - Do not change or reorder
	// -----------------------------------------------------------------
	union {
		UINT32 	DWORD;						// Status Flags - 32 Bits
		struct {
			UINT32		COMPLETED:1;			// Bit 0: DMA Completed
			UINT32		SOURCE_ERROR:1;			// Bit 1: Source Error Detected during DMA
			UINT32		DESTINATION_ERROR:1;	// Bit 2: Destination Error Detected during DMA
			UINT32		INTERNAL_ERROR:1;		// Bit 3: Internal Error Detected during DMA
			UINT32		COMP_BYTE_COUNT:27;		// Bits 4 - 30: Completed Byte Count
			UINT32		UPPER_STAT_NZ:1;		// Bits 31: If set upper 32 bit contain valid status
		} FIELDS;
	} STATUS;
	UINT16		USER_HANDLE;
	UINT16		USER_ID;
} PACKED STATUS_ELEMENT64_STRUCT, *PSTATUS_ELEMENT64_STRUCT;
#endif // Version 1 vs. Version 2

//----------------------------------------------------------------
// Defines for accessing the x_QUEUE_STRUCTs Adress Pointer QWORD
//----------------------------------------------------------------
#define	AXI_QUEUE_LOCATION			(1 << 0)   	// Bit 0: Queue Location, 0 = PCIe, 1 = AXI 
#define	FIFO_ADDR_MODE				(1 << 1)   	// Bit 1: 1 = Address is non-incrementing FIFO
#define	DMA_QUEUE_ATTRIBUTES		(4 << 2)   	// Bits 2 - 5: Attribute for DMA Source Queue
#define	QUEUE_ADDRESS_PTR			(58 << 6)	// Bits 6 - 63: Queue Starting Address

/*! \struct SOURCE_SG_QUEUE_STRUCT
 *	\brief Source Scatter-Gather Queue Management structure
 */
typedef struct _SOURCE_SG_QUEUE_STRUCT
{
	// -----------------------------------------------------------------
	// Hardware specific entries - Do not change or reorder
	// -----------------------------------------------------------------
	union {
		UINT64 QWORD;  							// Source Queue Pointer - 64 Bits
		struct {
			UINT32		DWORD_LO;				// Source Queue Pointer Lo
			UINT32		DWORD_HI;				// Source Queue Pointer High
		} DWORD;
		struct {
			UINT64		QUEUE_LOCATION	:1;		// Bit 0: Queue Location, 0 = PCIe, 1 = AXI 
#ifdef VERSION_2
			UINT64		QUEUE_ENABLE	:1;		// Bit 1: Queue Enable - set to 1 to enable
#else
			UINT64						:1;		// Bit 1: Reserved
#endif
			UINT64		ATTRIBUTE		:4;		// Bits 2 - 5: Attribute for DMA Source Queue
			UINT64		ADDRESS			:58;	// Bits 6 - 63: Queue Starting Address
		} FIELDS;
	} SRC_Q_PTR;
	UINT32	SRC_Q_SIZE;							// Source Queue Size
	UINT32	SRC_Q_LIMIT;						// Source Queue Size Limit
} PACKED SOURCE_SG_QUEUE_STRUCT, *PSOURCE_SG_QUEUE_STRUCT;

/*!	\struct DESTINATION_SG_QUEUE_STRUCT
 *	\brief Destination Scatter-Gather Queue Management structure
 */
typedef struct _DESTINATION_SG_QUEUE_STRUCT
{
	// -----------------------------------------------------------------
	// Hardware specific entries - Do not change or reorder
	// -----------------------------------------------------------------
	union {
		UINT64 QWORD;  							// Destination Queue Pointer - 64 Bits
		struct {
			UINT32		DWORD_LO;				// Destination Queue Pointer Lo
			UINT32		DWORD_HI;				// Destination Queue Pointer High
		} DWORD;
		struct {
			UINT64		QUEUE_LOCATION	:1;		// Bit 0: Queue Location, 0 = PCIe, 1 = AXI 
#ifdef VERSION_2
			UINT64		QUEUE_ENABLE	:1;		// Bit 1: Queue Enable - set to 1 to enable
#else
			UINT64						:1;		// Bit 1: Reserved
#endif
			UINT64		ATTRIBUTE		:4;		// Bits 2 - 5: Attribute for DMA Destination Queue
			UINT64		ADDRESS			:58;	// Bits 6 - 63: Queue Starting Address
		} FIELDS;
	} DST_Q_PTR;
	UINT32		DST_Q_SIZE;						// Destination Queue Size
	UINT32		DST_Q_LIMIT;					// Destination Queue Size Limit
} PACKED DESTINATION_SG_QUEUE_STRUCT, *PDESTINATION_SG_QUEUE_STRUCT;

/*! \struct COMPLETION_STATUS_QUEUE_STRUCT
 *	\brief Completion Status Queue Management structure
 */
typedef struct _COMPLETION_STATUS_QUEUE_STRUCT
{
	// -----------------------------------------------------------------
	// Hardware specific entries - Do not change or reorder
	// -----------------------------------------------------------------
	union {
		UINT64 QWORD;
		struct {
			UINT32		DWORD_LO;				// Status Queue Pointer Lo
			UINT32		DWORD_HI;				// Status Queue Pointer High
		} DWORD;
		struct {
			UINT64		QUEUE_LOCATION	:1;		// Bit 0: Queue Location, 0 = PCIe, 1 = AXI 
#ifdef VERSION_2
			UINT64		QUEUE_ENABLE	:1;		// Bit 1: Queue Enable - set to 1 to enable
#else
			UINT64						:1;		// Bit 1: Reserved
#endif
			UINT64		ATTRIBUTE		:4;		// Bits 2 - 5: Attribute for DMA Status Queue
			UINT64		ADDRESS			:58;	// Bits 6 - 63: Queue Starting Address
		} FIELDS;
	} STA_Q_PTR;
	UINT32		STA_Q_SIZE;						// Source Queue Size
	UINT32		STA_Q_LIMIT;					// Source Queue Size Limit
} PACKED COMPLETION_STATUS_QUEUE_STRUCT, *PCOMPLETION_STATUS_QUEUE_STRUCT;

/*! \struct HARDWARE_QUEUE_STRUCT
 *	\brief Hardware Queue Management structure
 *	\note Software is not expected to do anything with the registers except initialize to 0.
 */
typedef struct _HARDWARE_QUEUE_STRUCT
{
	// -----------------------------------------------------------------
	// Hardware specific entries - Do not change or reorder
	// -----------------------------------------------------------------
	UINT32		SRC_Q_NEXT;						// Next Source Queue Element to fetch - SW only initializes to 0.
	UINT32		DST_Q_NEXT;						// Next Destination Queue Element to fetch - SW only initializes to 0.
	UINT32		STA_Q_NEXT;						// Next Status Queue Element to fetch - SW only initializes to 0.
#ifdef VERSION_2
	UINT32		STAD_Q_NEXT;					// Next Destination Status Queue Element to fetch - SW only initializes to 0.
#endif // VERSION_2
} PACKED HARDWARE_QUEUE_STRUCT, *PHARDWARE_QUEUE_STRUCT;

#ifdef VERSION_2

/*! \struct SCRATCHPAD_STRUCT
 *	\brief ScratchPad Registers structure
 */
typedef struct _SCRATCHPAD_STRUCT
{
	// -----------------------------------------------------------------
	// Hardware specific entries - Do not change or reorder
	// -----------------------------------------------------------------
	UINT32		SCRATCH0;						// For Use by software, DMA does not use or alter
	UINT32		SCRATCH1;						// For Use by software, DMA does not use or alter
	UINT32		SCRATCH2;						// For Use by software, DMA does not use or alter
	UINT32		SCRATCH3;						// For Use by software, DMA does not use or alter
} PACKED SCRATCHPAD_STRUCT, *PSCRATCHPAD_STRUCT;

/*! \struct PCIE_INTERRUPT_CTRL_STRUCT
 *	\brief PCIe Interrupt Control structure
 */
typedef struct _PCIE_INTERRUPT_CTRL_STRUCT
{
	// -----------------------------------------------------------------
	// Hardware specific entries - Do not change or reorder
	// -----------------------------------------------------------------
	union {
		UINT32		DWORD;							// PCIe Interrupt Control
		struct {
			UINT32		PCIE_INT_ENABLE		:1;		// Bit 0: 1 = Enable PCI Express Interrupt
			UINT32		DMA_ERROR_ENABLE	:1;		// Bit 1: 1 = Enable, 0 = Disable PCIe Error interrupts
			UINT32		DMA_SGL_ENABLE		:1;		// Bit 2: 1 = Enable, 0 = Disable DMA SGL Interrupts
			UINT32							:13;	// Bit 3 - 15: Reserved
			UINT32		INT_COALEASE_COUNT	:8;		// Bits 16 - 23: Interrupt Coalesce count
			UINT32							:8;		// Bit 24 - 31: Reserved
		} FIELDS;
	} CONTROL;										// Offset 0x60
	union {
		UINT32		DWORD;							// PCI Express Interrupt Status
		struct {
			UINT32							:1;		// Bit 0: Reserved
			UINT32		PCIE_DMA_ERROR		:1;		// Bit 1: 1 = DMA Error was detected, write 1 to clear
			UINT32		PCIE_SGL_INT		:1;		// Bit 2: 1 = a SGL Interrupt occurred, write 1 to clear
			UINT32		PCIE_SW_INT_STATUS	:1;		// Bit 3: 1 = a software Interrupt ocurred, write 1 to clear
			UINT32 							:28;	// Bits 4 - 31: Reserved
		} FIELDS;
	} STATUS;										// Offset 0x64
} PACKED PCIE_INTERRUPT_CTRL_STRUCT, *PPCIE_INTERRUPT_CTRL_STRUCT;

/*! \struct AXI_INTERRUPT_CTRL_STRUCT
 *	\brief AXI Interrupt Control structure
 */
typedef struct _AXI_INTERRUPT_CTRL_STRUCT
{
	// -----------------------------------------------------------------
	// Hardware specific entries - Do not change or reorder
	// -----------------------------------------------------------------
	union {
		UINT32		DWORD;							// AXI Interrupt Control
		struct {
			UINT32		AXI_INT_ENABLE		:1;		// Bit 0: 1 = Enable AXI Interrupt
			UINT32		DMA_ERROR_ENABLE	:1;		// Bit 1: 1 = Enable, 0 = Disable AXI Error interrupts
			UINT32		DMA_SGL_ENABLE		:1;		// Bit 2: 1 = Enable, 0 = Disable DMA SGL Interrupts
			UINT32							:13;	// Bit 3 - 15: Reserved
			UINT32		INT_COALEASE_COUNT	:8;		// Bits 16 - 23: Interrupt Coalesce count
			UINT32							:8;		// Bit 24 - 31: Reserved
		} FIELDS;
	} CONTROL;										// Offset 0x68
	union {
		UINT32		DWORD;							// AXI Interrupt Status
		struct {
			UINT32							:1;		// Bit 0: Reserved
			UINT32	AXI_DMA_ERROR			:1;		// Bit 1: 1 = DMA Error was detected, write 1 to clear
			UINT32	AXI_SGL_INT				:1;		// Bit 2: 1 = a SGL Interrupt occurred, write 1 to clear
			UINT32	AXI_SW_INT_STATUS		:1;		// Bit 3: 1 = a software Interrupt ocurred, write 1 to clear
			UINT32							:28;	// Bits 4 - 31: Reserved
		} FIELDS;
	} STATUS;										// Offset 0x6C
} PACKED AXI_INTERRUPT_CTRL_STRUCT, *PAXI_INTERRUPT_CTRL_STRUCT;

#define	CTRL_INT_ENABLE				(1 << 0)		// Bit 0:  1 = Enable Interrupts
#define	CTRL_INT_DMA_ERROR_ENABLE	(1 << 1)		// Bit 1:  1 = Enable DMA Error Interrupts
#define	CTRL_INT_SGL_ENABLE			(1 << 2)		// Bit 2:  1 = Enable SGL Interrupts
#define CTRL_INT_COALEASCE_COUNT	(8 << 16)		// Bit 16- 23: SGL Interrupt Coaleasce count

#define	STAT_INT_DMA_ERROR			(1 << 1)		// Bit 1:  1 = PCI Express DMA Error Interrupt ocurred, write 1 to clear
#define STAT_INT_SGL				(1 << 2)		// Bit 2:  1 = DMA SGL Interrupt, write 1 to clear
#define	STAT_INT_SW					(1 << 3)		// Bit 3:  1 = Software Interrupt occurred, write 1 to clear
#define	STAT_INT_MASK				(STAT_INT_DMA_ERROR | STAT_INT_SGL | STAT_INT_SW)

/*! \struct DMA_CTRL_STATUS_STRUCT
 *	\brief DMA Control and Status structure
 */
typedef struct _DMA_CTRL_STATUS_STRUCT
{
	// -----------------------------------------------------------------
	// Hardware specific entries - Do not change or reorder
	// -----------------------------------------------------------------
	union {
		UINT32		DWORD;							// Reserved - PCI_INTERRUPT_ASSERT
		struct {
			UINT32							:3;		// Bit 0 - 2: Reserved
			UINT32		ASSERT_PCIE_INT		:1;		// Bit 3: 1 = Generate a PCI Express Interrupt
			UINT32							:28;	// Bits 4 - 31: Reserved
		} FIELDS;
	} PCIE_INT_ASSERT;								// Offset 0x70
	union {
		UINT32		DWORD;							// Reserved - AXI_INTERRUPT_ASSERT
		struct {
			UINT32							:3;		// Bit 0 - 2: Reserved
			UINT32		ASSERT_AXI_INT		:1;		// Bit 3: 1 = Generate an AXI Interrupt
			UINT32							:28;	// Bits 4 - 31: Reserved
		} FIELDS;
	} AXI_INT_ASSERT;								// Offset 0x74
	union {
		UINT32		DWORD;							// DMA Control DWord
		struct {
			UINT32		DMA_ENABLE			:1;		// Bit 0: 1 = Enable, 0 = Disable
			UINT32		DMA_RESET			:1;		// Bit 1: 1 = Reset the DMA Channel
			UINT32		DMA_STAT_SIZE		:1;		// Bit 2: 1 = 64 bit Status Queues size, 0 = 32 bit
			UINT32							:29;	// Bit 3 - 31: Reserved
		} FIELDS;
	} DMA_CONTROL;									// Offset 0x78
	union {
		UINT32			DWORD;						// DMA Status DWord
		struct {
			UINT32		DMA_RUNNING			:1;		// Bit 0: 1 = DMA Busy, 0 = Idle
			UINT32							:3;		// Bits 1 - 3 reserved
			UINT32		DMA_CHANNEL_NUMBER	:10;	// Bits 4 - 13: DMA Channel Number
			UINT32							:1;		// Bit 14: Reserved
			UINT32		DMA_PRESENT			:1;		// Bit 15: DMA Channel Present
			UINT32							:16;	// Bit 16 - 31: Reserved
		} FIELDS;
	} DMA_STATUS;									// Offset 0x7C
} PACKED DMA_CTRL_STATUS_STRUCT, *PDMA_CTRL_STATUS_STRUCT;

//----------------------------------------------------------------
// Defines for accessing the Control / Status as a single DWORD
//----------------------------------------------------------------
#define	CTRL_DMA_ENABLE				(1 << 0)		// Bit 0: 1 = Enable, 0 = Disable
#define	CTRL_DMA_RESET				(1 << 1)		// Bit 1: 1 = Reset DMA Channel
#define CTRLDMA_STAT_SIZE_64		(1 << 2)		// Bit 2: 1 = Status size is 64 bit.

#define	STAT_DMA_RUNNING			(1 << 0)		// Bit 1: 1 = DMA Busy, 0 = Idle
#define STAT_DMA_CHANNEL_NUMBER		(10 << 4)		// Bit 4- 13: DMA Channel Number
#define STAT_DMA_CHANNEL_PRESENT	(1 << 15)		// Bit 15: 1 = DMA Channel Present

#else // Version 1 --------------------------------------------

//----------------------------------------------------------------
// Defines for accessing the Control / Status as a single DWORD
//----------------------------------------------------------------
#define	CTRL_DMA_ENABLE				(1 << 0)		// Bit 0: 1 = Enable, 0 = Disable
#define	STAT_DMA_RUNNING			(1 << 1)		// Bit 1: 1 = DMA Busy, 0 = Idle
#define	CTRL_DMA_RESET				(1 << 2)		// Bit 2: 1 = Reset DMA Channel
#define STAT_DMA_CHANNEL_NUMBER		(10 << 4)		// Bit 4- 13: DMA Channel Number
#define DMA_CHANNEL_PRESENT			(1 << 15)		// Bit 15: 1 = DMA Channel Present

#define	PCIE_CTRL_INT_ENABLE		(1 << 16)		// Bit 16 1 = Enable PCI Express Interrupt
#define	PCIE_STAT_INT_OCURRED		(1 << 17)		// Bit 17: 1 = PCI Express Interrupt ocurred, write 1 to clear
#define PCIE_STAT_DMA_ERROR			(1 << 18)		// Bit 18: 1 = DMA Error was detected, write 1 to clear

#define	AXI_CTRL_INT_ENABLE			(1 << 24)		// Bit 24: 1 = Enable AXI Interrupt
#define	AXI_STAT_INT_OCURRED		(1 << 25)		// Bit 25: 1 = AXI Interrupt ocurred, write 1 to clear
#define	AXI_STAT_DMA_ERROR			(1 << 26)		// Bit 26: 1 = DMA Error was detected, write 1 to clear

/*! \struct DMA_CTRL_STATUS_STRUCT
 *  \brief DMA Control and Status structure
 */
typedef struct _DMA_CTRL_STATUS_STRUCT
{
	// -----------------------------------------------------------------
	// Hardware specific entries - Do not change or reorder
	// -----------------------------------------------------------------
	union {
		UINT16		WORD;							// DMA Control Word
		struct {
			UINT16		DMA_ENABLE			:1;		// Bit 0: 1 = Enable, 0 = Disable
			UINT16		DMA_RUNNING			:1;		// Bit 1: 1 = DMA Busy, 0 = Idle
			UINT16		DMA_RESET			:1;		// Bit 2: 1 = Reset the DMA Channel
			UINT16							:1;		// Bit 3: Reserved
			UINT16		DMA_CHANNEL_NUMBER	:10;	// Bits 4 - 13: DMA Channel Number
			UINT16							:1;		// Bit 14: Reserved
			UINT16		DMA_PRESENT			:1;		// Bit 15: DMA Channel Present
		} FIELDS;
	} DMA_CONTROL;
	union {
		UINT8		BYTE;							// PCI Express Status Byte
		struct {
			UINT8		PCIE_INT_ENABLE		:1;		// Bit 0: 1 = Enable PCI Express Interrupt
			UINT8		PCIE_INT_STATUS		:1;		// Bit 1: 1 = PCI Express Interrupt ocurred, write 1 to clear
			UINT8		PCIE_DMA_ERROR		:1;		// Bit 2: 1 = DMA Error was detected, write 1 to clear
			UINT8							:5;		// Bits 3 - 7: Reserved
		} FIELDS;
	} PCIE_STATUS;
	union {
		UINT8		BYTE;							// AXI Status Byte
		struct {
			UINT8		AXI_INT_ENABLE		:1;		// Bit 0: 1 = Enable AXI Interrupt
			UINT8		AXI_INT_STATUS		:1;		// Bit 1: 1 = AXI Interrupt ocurred, write 1 to clear
			UINT8		AXI_DMA_ERROR		:1;		// Bit 2: 1 = DMA Error was detected, write 1 to clear
			UINT8							:5;		// Bits 3 - 7: Reserved
		} FIELDS;
	} AXI_STATUS;
} PACKED DMA_CTRL_STATUS_STRUCT, *PDMA_CTRL_STATUS_STRUCT;


/*! \struct BAR1_MEMORY_MAP_STRUCT
 *	\brief BAR 1 Memory Map - Currently only for User Interrupts
 */
typedef struct _BAR1_MEMORY_MAP_STRUCT
{
	union {
		UINT8		USER_INT_BYTE;					// User Interrupt Status Byte
		struct {
			UINT8		USER_INT_ACTIVE		:1;		// Bit 0: 1 = User Interrupt Active
			UINT8							:7;		// Bits 1 - 7: Reserved
		} FIELDS;
	} USER_INTERRUPT;
} PACKED BAR1_MEMORY_MAP_STRUCT, *PBAR1_MEMORY_MAP_STRUCT;

#endif // VERSION_2 vs Version 1 

#ifdef WIN32	// Windows Version
#pragma warning(default:4214)
#endif	// Windows Version

#ifdef VERSION_2

/*!	\struct DMA_CHANNEL_STRUCT
 *	\brief DMA Channel Structure - One structure for each DMA Channel
 */
typedef struct _DMA_CHANNEL_STRUCT
{
	SOURCE_SG_QUEUE_STRUCT			SOURCE;
	DESTINATION_SG_QUEUE_STRUCT		DEST;
	COMPLETION_STATUS_QUEUE_STRUCT	STATUS;
	COMPLETION_STATUS_QUEUE_STRUCT	STATD;
	HARDWARE_QUEUE_STRUCT			HDWR;
	SCRATCHPAD_STRUCT				SCRATCH;
	PCIE_INTERRUPT_CTRL_STRUCT		PCIE_INT;
	AXI_INTERRUPT_CTRL_STRUCT		AXI_INT;
	DMA_CTRL_STATUS_STRUCT			DMA_CTRL;
} PACKED DMA_CHANNEL_STRUCT, *PDMA_CHANNEL_STRUCT;
#else // Version 1 

/*!	\struct DMA_CHANNEL_STRUCT
 *	\brief DMA Channel Structure - One structure for each DMA Channel
 */
typedef struct _DMA_CHANNEL_STRUCT
{
	SOURCE_SG_QUEUE_STRUCT			SOURCE;
	DESTINATION_SG_QUEUE_STRUCT		DEST;
	COMPLETION_STATUS_QUEUE_STRUCT	STATUS;
	HARDWARE_QUEUE_STRUCT			HDWR;
	DMA_CTRL_STATUS_STRUCT			DMA_CTRL;
} PACKED DMA_CHANNEL_STRUCT, *PDMA_CHANNEL_STRUCT;
#endif

#ifndef WIN32   // Linux version ---------------------------------------------

static const UINT32	BAR_TYPE_MASK       = 0x1;
static const UINT32	BAR_TYPE_MEM        = 0x0;
static const UINT32	BAR_TYPE_IO         = 0x1;
static const UINT32	BAR_MEM_ADDR_MASK   = 0x6;
static const UINT32	BAR_MEM_ADDR_32     = 0x0;
static const UINT32	BAR_MEM_ADDR_1M     = 0x2;
static const UINT32	BAR_MEM_ADDR_64     = 0x4;
static const UINT32	BAR_MEM_CACHE_MASK  = 0x8;
static const UINT32	BAR_MEM_CACHABLE    = 0x8;

#define BAR_MEM_MASK        (~0x0F)
#define BAR_IO_MASK         (~0x03)
#define IS_IO_BAR(x)            (((x) & BAR_TYPE_MASK)      == BAR_TYPE_IO)
#define IS_MEMORY_BAR(x)        (((x) & BAR_TYPE_MASK)      == BAR_TYPE_MEM)
#define IS_MEMORY_32BIT(x)      (((x) & BAR_MEM_ADDR_MASK)  == BAR_MEM_ADDR_32)
#define IS_MEMORY_64BIT(x)      (((x) & BAR_MEM_ADDR_MASK)  == BAR_MEM_ADDR_64)
#define IS_MEMORY_BELOW1M(x)    (((x) & BAR_MEM_ADDR_MASK)  == BAR_MEM_ADDR_1M)
#define IS_MEMORY_CACHABLE(x)   (((x) & BAR_MEM_CACHE_MASK) == BAR_MEM_CACHABLE)

//----------------------------------------------------------------------
// Bar Types
//----------------------------------------------------------------------
#define REGISTER_MEM_PCI_BAR_TYPE  0
#define RAM_MEM_PCI_BAR_TYPE       1
#define REGISTER_IO_PCI_BAR_TYPE   2
#define DISABLED_PCI_BAR_TYPE      3

#define TRANSFER_SIZE_8_BIT        0x01    // 8 Bit transfer
#define TRANSFER_SIZE_16_BIT       0x02    // 16 Bit transfer
#define TRANSFER_SIZE_32_BIT       0x04    // 32 Bit transfer
#define TRANSFER_SIZE_64_BIT       0x08    // 64 Bit transfer (not currently supported)
#define TRANSFER_SIZE_32_BIT_DMA   0x10    // 32 Bit transfer
#define TRANSFER_SIZE_64_BIT_DMA   0x20    // 64 Bit transfer

#endif  // Linux version -----------------------------------------------

/*! \struct BAR0_REGISTER_MAP_STRUCT 
 *	\brief Register Structure located at BAR0
 */
typedef struct _BAR0_REGISTER_MAP_STRUCT
{
	DMA_CHANNEL_STRUCT		DMA_CHANNEL[MAX_NUM_DMA_CHANNELS];
} PACKED BAR0_REGISTER_MAP_STRUCT, *PBAR0_REGISTER_MAP_STRUCT;


/*! \struct DUMP_CHANNEL_STRUCT
 *	\brief Channel Dump structure. 
 */
typedef struct _DUMP_CHANNEL_STRUCT
{
	DMA_CHANNEL_STRUCT		DmaChannel;
	INT32					NumberOfDstElements;
	INT32					NumberOfUsedDstElements;
	INT32					NumberOfStatElements;
	INT32					LastElemProcessed;
} PACKED DUMP_CHANNEL_STRUCT, *PDUMP_CHANNEL_STRUCT;

#pragma pack(pop)

#endif // _EXPRESSO_HW_H_
