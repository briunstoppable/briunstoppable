// -------------------------------------------------------------------------
// 
// PRODUCT:			Expresso Multi-Channel DMA Driver
// MODULE NAME:		StrDispUtils.cpp
// 
// MODULE DESCRIPTION: 
// 
// Contains String and Display utility functions for the CLI application.
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

UINT64
strtoull_l(
	const char *__restrict		nptr, 
	char **	__restrict			endptr, 
	INT32						base
);

/*!
 * Function:
 *   InitPassStats
 *
 * Description:
 *   Prepares the per pass stats and CPU Utilization
 */
VOID 
InitPassStats(
	PPASS_STATS pPassStats
)
{
	pPassStats->BytesTransfered		= 0;
	pPassStats->CPUUtil				= 0;
	pPassStats->TransfersPerSecond	= 0;
	pPassStats->IntsPerSecond		= 0;
	pPassStats->DPCsPerSecond		= 0;
}

/*!
 * Function:
 *   InitSummaryStats
 *
 * Description:
 *   Prepares the summary stats and CPU Utilization
 */
VOID	
InitSummaryStats(
	PSUMMARY_STATS	pSummaryStats
)
{
	pSummaryStats->Samples					= 0;
	pSummaryStats->BytesTransferedTotal		= 0;
	pSummaryStats->IntsPerSecondTotal		= 0;
	pSummaryStats->DPCsPerSecondTotal		= 0;
	pSummaryStats->CPUUtilTotal				= 0;
	pSummaryStats->TransfersPerSecondTotal	= 0;
	GetCPUUtil();
}

/*!
 * Function:
 *   DisplayPassInfo
 *
 * Description:
 *   Keeps track of each pass info and optionally display it
 */
VOID	
DisplayPassInfo(
   	PCFG_PARAMS		pcfgParams,
	PPASS_STATS		pPassStats,
	PSUMMARY_STATS	pSummaryStats
)
{
	FLOAT			MBTransferred = 0;
    // Convert the result to megabytes.
    if (pPassStats->BytesTransfered != 0)
    {
        MBTransferred = (FLOAT)(pPassStats->BytesTransfered / 1000000.0);
    }

	// Keep track of the totals for the final summary
	pSummaryStats->Samples++;
	pSummaryStats->BytesTransferedTotal += pPassStats->BytesTransfered;
	pSummaryStats->TransfersPerSecondTotal += pPassStats->TransfersPerSecond;
	pSummaryStats->IntsPerSecondTotal += pPassStats->IntsPerSecond;
	pSummaryStats->DPCsPerSecondTotal += pPassStats->DPCsPerSecond;
	pPassStats->CPUUtil = GetCPUUtil();
	pSummaryStats->CPUUtilTotal += pPassStats->CPUUtil;

	if (!pcfgParams->bQuiet)
	{
#ifdef WIN32	// Windows version
		printf("Pass %lld: MegaBytes/Sec: %6.3f, ", pSummaryStats->Samples, MBTransferred);
		printf("\t CPU Util: %d\n", pPassStats->CPUUtil);
		if (pcfgParams->bVerbose == TRUE)
		{
			printf("Reqs/Sec: %lld, ", pPassStats->TransfersPerSecond);
			printf("\tInts/Sec: %lld, ", pPassStats->IntsPerSecond);
			printf("\tDPCs/Sec: %lld\n", pPassStats->DPCsPerSecond);
		}
#else // Linux version
        printf("Pass %lld: MegaBytes/Sec: %6.3f, ", pSummaryStats->Samples, MBTransferred);
        printf("\t CPU Util: %d\n", pPassStats->CPUUtil);
		if (pcfgParams->bVerbose == TRUE)
		{
            printf("Reqs/Sec: %qd, ", pPassStats->TransfersPerSecond);
			printf("\tInts/Sec: %qd, ", pPassStats->IntsPerSecond);
			printf("\tTsklts/Sec: %qd\n", pPassStats->DPCsPerSecond);
		}
#endif	// Windows vs. Linux
	}
	if (pPassStats->BytesTransfered == 0)
	{
		pcfgParams->iPass = ERROR_NO_DATA_TRANSFER;
	}
}

/*! DisplaySummaryInfo
 * 
 * Function:
 *		DisplaySummaryInfo
 *
 * Description:
 * \brief Display the summary from the test passes
 */
VOID
DisplaySummaryInfo(
   	PCFG_PARAMS		pcfgParams,
	PSUMMARY_STATS	pSummaryStats
)
{
	FLOAT			MBTransferred  = 0;

	if (!pcfgParams->bQuiet)
	{
	    // Convert the result to megabytes.
		if (pSummaryStats->BytesTransferedTotal != 0)
		{
			MBTransferred = (FLOAT)((pSummaryStats->BytesTransferedTotal / pSummaryStats->Samples) / 1000000.0);
		}
		if (pSummaryStats->Samples)
		{
#ifdef WIN32	// Windows version
			printf("Avg MegaBytes/Sec: %6.3f,", MBTransferred);
			printf("\t Avg CPU Util: %lld\n", pSummaryStats->CPUUtilTotal / pSummaryStats->Samples);
			if (pcfgParams->bVerbose == TRUE)
			{
				printf("Avg Reqs/Sec: %lld, ", 
					pSummaryStats->TransfersPerSecondTotal / pSummaryStats->Samples);
				printf("\tAvg Ints/Sec: %lld, ", 
					pSummaryStats->IntsPerSecondTotal / pSummaryStats->Samples);
				printf("\tAvg DPCs/Sec: %lld\n", 
					pSummaryStats->DPCsPerSecondTotal / pSummaryStats->Samples);
			}
#else // Linux version
       		printf("Avg MegaBytes/Sec: %6.3f,", MBTransferred);
       		printf("\t Avg CPU Util: %u\n", (UINT32)(pSummaryStats->CPUUtilTotal / pSummaryStats->Samples));
			if (pcfgParams->bVerbose == TRUE)
			{
           		printf("Avg Reqs/Sec: %qd, ", 
                   	pSummaryStats->TransfersPerSecondTotal / pSummaryStats->Samples);
				printf("\t Avg Ints/Sec: %qd, ",
					pSummaryStats->IntsPerSecondTotal / pSummaryStats->Samples);
				printf("\t Avg Tasklets/Sec: %qd\n",
					pSummaryStats->DPCsPerSecondTotal / pSummaryStats->Samples);
			}
#endif	// Windows vs. Linux
		}
	}
}

/*!
 * Function:
 *		DisplayFormattedBytes
 *
 * Description:
 *   Display a buffer dump in a nice format
 */
VOID
DisplayFormattedBytes(
   	BOOLEAN				bQuiet,
	UINT64				Offset,
   	PUINT8				pBuffer,
	INT32				BytesToDisplay
)
{
	if (!bQuiet)
	{
		// print out the buffer
		for (INT32 i = 0; i < BytesToDisplay; i++)
		{
			if ((i & 0x0f) == 0)
			{
				printf("0x%04llx: 0x", i + Offset);
			}
//			if (i%4 == 0)
//			{
//				printf (" 0x");
//			}
			printf ("%02x ", pBuffer[i]);
			if (i%16 == 15)
			{
				printf("\n");
			}
		}
		printf("\n");
	}
}

/*! DisplayErrorInfo
 * 
 * \brief Display the Error information
 * \param bQuiet
 * \param Status
 */
UINT32 
DisplayErrorInfo(
	BOOLEAN		bQuiet,
	UINT32		Status
)
{
	if (!bQuiet)
	{
#ifdef WIN32	// Windows version
		printf("\nError returned from Driver. return code = 0x%x:\n\t", Status);
		switch (Status)
		{
			case (UINT32)ERROR_IO_DEVICE:			// STATUS_DRIVER_INTERNAL_ERROR
			{
				printf("Internal driver / OS fault.");
				break;
			}
			case (UINT32)ERROR_ADAP_HDW_ERR:		// STATUS_ADAPTER_HARDWARE_ERROR
			{
				printf("DMA Transaction Error.");
				break;
			}
			case (UINT32)ERROR_INVALID_FUNCTION:	// STATUS_INVALID_DEVICE_REQUEST
			{
				printf("Incompatible DMA Channel specified or transfer size is too large.");
				break;
			}
			case (UINT32)ERROR_BAD_COMMAND:			// STATUS_INVALID_DEVICE_STATE
			{
				printf("No Buffer present.");
				break;
			}
			case (UINT32)ERROR_INVALID_PARAMETER:	// STATUS_INVALID_PARAMETER
			{
				printf("Invalid parameter, parameter out of range.");
				break;
			}
			case (UINT32)ERROR_NO_SYSTEM_RESOURCES:	// STATUS_INSUFFICIENT_RESOURCES
			{
				printf("The transfer size requested is too large.");
				break;
			}
			case (UINT32)ERROR_FILE_NOT_FOUND:		// STATUS_NO_SUCH_DEVICE
			{
				printf("Invalid device or incorrect DMA Channel specified.");
				break;
			}
			case ERROR_NOT_ENOUGH_MEMORY:			// STATUS_NO_MEMORY
			{
				printf("Unable to allocate enough DMA Elements.");
				break;
			}
			case ERROR_NOT_SUPPORTED:				// STATUS_NOT_SUPPORTED
			{
				printf("Requested function is not supported.");
				break;
			}
			case ERROR_SEM_TIMEOUT:					// STATUS_IO_TIMEOUT
			{
				printf("The request timed out.");
				break;
			}
			case (UINT32)ERROR_BUSY:				// STATUS_DEVICE_BUSY
			{
				printf("The devices DMA Channel is busy.");
				break;
			}
			default:
				printf("Unknown error code");
		}
#else // Linux version
		printf("\nError returned from Driver. errno = %d:\n\t", Status);
		switch (Status)
		{
			case ENODEV:
			{
				printf("Non-existent DMA Channel specified.");
				break;
			}
			case EFAULT:
			{
				printf("Internal driver / OS fault.");
				break;
			}
			case EIO:
			{
				printf("DMA Transaction Error.");
				break;
			}
			case ENOSYS:
			{
				printf("Incompatible DMA Channel specified.");
				break;
			}
			case ENOBUFS:
			{
				printf("No Buffer present.");
				break;
			}
			case EINVAL:
			{
				printf("Invalid parameter, parameter out of range.");
				break;
			}
			case ENOMEM:
			{
				printf("Failed to allocate enough memory for the transaction.");
				break;
			}
			case EMSGSIZE:
			{
				printf("The transfer size requested is too large.");
				break;
			}
			case STATUS_INVALID_MODE:
			{
				printf("Invalid parameter, incorrect DMA Channel specified.");
				break;
			}
			default:
				printf("Unknown error code");
		}
#endif	// Windows vs. Linux
		printf("\n");
	}
	return Status;
}

/*! __strtoul
 * 
 * \brief Convert a string to a unsigned long
 * \param nptr
 * \return result
 */
UINT32
__strtoul(
    const PCHAR		nptr
)
{
    UINT32			result = 0;

	result = strtoul(nptr, NULL, 10);
	if (!result)
	{
		result = strtoul(nptr, NULL, 16);
	}
    return (result);
}

/*! __strtoull
 * 
 * \brief Convert a string to a unsigned long long (64 bit)
 * \param nptr
 * \return result
 */
UINT64 
__strtoull(
	const PCHAR		nptr
)
{
    UINT64			result = 0;
	result = strtoull_l(nptr, NULL, 10);
	if (!result)
	{
		result = strtoull_l(nptr, NULL, 16);
	}
	return (result);
}

/*! strtoull_l
 * \brief Internal function that converts a string to an unsigned long long integer.
 * \note Assumes that the upper and lower case
 *	alphabets and digits are each contiguous.
 * \param nptr
 * \param endptr
 * \param base
 */
UINT64
strtoull_l(
	const char * __restrict	nptr, 
	char ** __restrict		endptr, 
	INT32					base
)
{
	const char *			s;
	UINT64					acc;
	CHAR					c;
	UINT64					cutoff;
	INT32					neg, 
							any, 
							cutlim;

	s = nptr;
	do {
		c = * s++;
	} while (isspace((unsigned char)c));
	if (c == '-') 
	{
		neg = 1;
		c = * s++;
	} 
	else 
	{
		neg = 0;
		if (c == '+')
		{
			c = *s++;
		}
	}
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) 
	{
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
	{
		base = c == '0' ? 8 : 10;
	}
	acc = any = 0;
	if (base < 2 || base > 36)
	{
		goto noconv;
	}

	cutoff = MY_ULONGLONG_MAX / base;
	cutlim = MY_ULONGLONG_MAX % base;
	for ( ; ; c = *s++) 
	{
		if (c >= '0' && c <= '9')
		{
			c -= '0';
		}
		else if (c >= 'A' && c <= 'Z')
		{
			c -= 'A' - 10;
		}
		else if (c >= 'a' && c <= 'z')
		{
			c -= 'a' - 10;
		}
		else
		{
			break;
		}
		if (c >= base)
		{
			break;
		}
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
		{
			any = -1;
		}
		else
		{
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) 
	{
		acc = MY_ULONGLONG_MAX;
	} 
	else if (!any) 
	{
noconv:
		acc = 0;
	} //else if (neg)
		//acc = -acc;
	if (endptr != NULL)
	{
		*endptr = (PCHAR)(any ? s - 1 : nptr);
	}
	return (acc);
}

/*!	__hextoul
 *
 * \brief Converts a hex value to ULONG
 * \param value
 * \return finalNum
 */
UINT32 
_hexttoul(
	const PCHAR		value
)
{
	UINT32			finalNum	= 0;
	const char *	nextChar	= value;
	CHAR			upperChar;
	INT32			length		= 0;

	while ((* nextChar != 0) && (length < 8))
	{
		upperChar = (CHAR)toupper(*nextChar);
		if ((upperChar >= '0') && (upperChar <= '9'))
		{
			// add next char
			finalNum <<= 4;
			finalNum += (upperChar - '0');
		}
		else if ((upperChar >= 'A') && (upperChar <= 'F'))
		{
			// add next char
			finalNum <<= 4;
			finalNum += (upperChar - 'A') + 10;
		}
		else if (upperChar == 'X')
		{
			// chop off beginning 0x
			finalNum = 0;
			length = 0;
		}

		// next char
		nextChar++;
		length++;
	}
	return finalNum;
}

/*! atohx
 *
 * \brief Convert an ASCII hex string to a binary string
 * \param dst
 * \param src
 * \return Length
 */
INT32 
atohx(
	char *			dst, 
	const char *	src
)
{	
	PCHAR			dstStart	= dst;
	INT32			Length		= 0;
	INT32			hexlsb, 
					hexmsb;

	hexmsb = tolower(* src);
	hexlsb = tolower(*(src + 1));

	// strip of the 0x if present
	if (hexmsb == '0')
	{
		if (hexlsb == 'x')
		{
			src += 2;
		}
	}

	for (INT32 lsb, msb; * src; src += 2)
	{	
		msb = tolower(*src);
		lsb = tolower(*(src + 1));
		msb -= isdigit(msb) ? 0x30 : 0x57;
		lsb -= isdigit(lsb) ? 0x30 : 0x57;
		// Make sure we had a hex value converted
		if((msb < 0x0 || msb > 0xf) || (lsb < 0x0 || lsb > 0xf))
		{
			// If not a valid hex value, exit out
			*dstStart = 0;
			return 0;
		}
		* dst++ = (CHAR)(lsb | (msb << 4));  
		Length++;
	}
	* dst = 0;
	return Length;
}

/*! isHexStr
 *  
 * \brief Returns true if the string is a Hex value (0x0 format included)
 * \param pHexStr
 * \return ValidValue
 */
BOOLEAN 
isHexStr(
	PCHAR		pHexStr
)
{
	CHAR		hexId[]		= "x";
	BOOLEAN		ValidValue	= TRUE;

	// Make sure it is a hex numeric value
	for (INT32 b = 0; b < (INT32)strlen(pHexStr); b++)
	{
		if (!isxdigit(pHexStr[b]))
		{
			if (hexId[0] != tolower(pHexStr[b]))
			{
				ValidValue = FALSE;
				break;
			}
		}
	}
	return ValidValue;
}


/*! reverse_string
 *  
 * \brief Reverses the byte order of a string
 * \param str
 * \return nothing
 */
void 
reverse_string(
	PCHAR		str)
{
    /* skip null */
    if (str == 0)
    {
    	return;
    }

    /* skip empty string */
    if (*str == 0)
    {
    	return;
    }

    /* get range */
    PCHAR	start = str;
    PCHAR	end = start + strlen((PCHAR)str) - 1; /* -1 for \0 */
    CHAR	temp;

    /* reverse */
    while (end > start)
    {
    	/* swap */
    	temp = *start;
    	*start = *end;
    	*end = temp;

    	/* move */
    	++start;
    	--end;
    }
}
