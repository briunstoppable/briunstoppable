// -------------------------------------------------------------------------
// 
// PRODUCT:			Expresso Multi-Channel DMA Driver
// MODULE NAME:     Expresso.cpp
// 
// MODULE DESCRIPTION: 
// 
// Entry point for the Command Line Interpreter (CLI) application.
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
#include "../Include/version.h"
#include <time.h>

const char * Title		= "Expresso Test App\n(C) Copyright " VER_LEGALCOPYRIGHT_STR "\n";
const char * Version = "Version " VER_PRODUCTVERSION_STR "\n";
const char * usage = "ExpressoCli [-options] <command> <command params>\n";

#define DisplayError(s) { printf("\nError: %s\n",s);}

// --------------------------------------------------------------------------------------
// Global Values
// --------------------------------------------------------------------------------------

PCFG_PARAMS			pcfgParams;

// --------------------------------------------------------------------------------------
// Local Prototypes
// --------------------------------------------------------------------------------------

INT32 
help(
	PCFG_PARAMS			pcfgParams
);

INT32 
FindCmd(
	command_t *			cmd_list, 
	INT32				n_cmd_list, 
	const PCHAR			args[], 
	INT32				n_args, 
	PCFG_PARAMS			pcfgParams
);


/*! SignalHandler
 *
 * \brief Handles any Control-C, or other abrupt termination requests.
 * \param sig
 * \return none
 */
static void 
SignalHandler(
	INT32 			sig
)
{
	UNREFERENCED_PARAMETER(sig);

	pcfgParams->Seconds = 1;
	fprintf(stderr, "\nUser requested application stop, shutting down ExpressoCli.\n");
}

/*! CLI Option setting functions  */

/*! SetAlignment
 *
 * \brief (-a) memory alignment.
 * \param pcfgParams
 * \param Value
 * \return None
 */
VOID 
SetAlignment(
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	if (Value == 0)
	{
		pcfgParams->bNonAligned = TRUE;
	}
}

/*! SetDataSeed 
 *
 * \brief (-b) Data Seed Pattern
 * \param pcfgParams
 * \param Value
 * \return None
 */
VOID
SetDataSeed(
	PCFG_PARAMS		pcfgParams, 
	UINT32			Value
)
{
	if (Value < 0x100)
	{
		pcfgParams->DataSeedSize = 1;
	}
	else
	{
		pcfgParams->DataSeedSize = 4;
	}
	pcfgParams->DataSeedPattern = Value;
}


/*! SetCardNumber
 *
 * \brief (-c) Card Number. 
 * \param pcfgParams
 * \param Value
 * \return None
 */
VOID
SetCardNumber(
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	pcfgParams->cardNum = Value;
}
              
/*! SetReadDMAChannel
 *
 * \brief (-dr) Read DMA Channel.
 * \param pcfgParams
 * \param Value
 * \return None
 */
VOID	
SetReadDMAChannel(
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	pcfgParams->ReadDMAChannel = Value;
	if (pcfgParams->ReadDMAChannel > 1023)
	{
		DisplayError("-dr option DMA Channel number out of range, option ignored");
		pcfgParams->ReadDMAChannel = 0;
	}
}
        
/*! SetWriteDMAChannel
 *
 * \brief (-dw) Write DMA Channel.
 * \param pcfgParams
 * \param Value
 * \return None
 */
VOID
SetWriteDMAChannel(
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	pcfgParams->WriteDMAChannel = Value;
	if (pcfgParams->WriteDMAChannel > 1023)
	{
		DisplayError("-dw option DMA Channel number out of range, option ignored");
		pcfgParams->WriteDMAChannel = 0;
	}
}
        
/*! SetStopOnError
 *
 * \brief (-e) Stop on Error. 
 * \param pcfgParams
 * \param Value
 * \return None
 */
VOID 
SetStopOnError(
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	UNREFERENCED_PARAMETER(Value);
	
	pcfgParams->bStopOnError = TRUE;
}
        
/*! SetReadFIFO
 *
 * \brief (-fr) Read FIFO mode.
 * \param pcfgParams
 * \param Value
 * \return None.
 */
VOID 
SetReadFIFO(
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	UNREFERENCED_PARAMETER(Value);
	
	pcfgParams->bReadFIFO = TRUE;
}

/*! SetWriteFIFO
*
* \brief (-fw) Turn on Write FIFO mode.
* \param pcfgParams
* \param Value
* \return None
*/
VOID	
SetWriteFIFO(
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	UNREFERENCED_PARAMETER(Value);

	pcfgParams->bWriteFIFO = TRUE;
}
        
/*! SetIterations
*
* \brief Number of Iterations.
* \param pcfgParams
* \param Value
* \return None
*/
VOID	
SetIterations(
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	pcfgParams->Iterations = Value;
}
        
/*! SetMaxDMAAddress
*
* \brief Maximum DMA Address size.
* \param pcfgParams
* \param Value
* \return None
*/
VOID 
SetMaxDMAAddress(
	PCFG_PARAMS			pcfgParams,
	UINT32				Value
)
{
	pcfgParams->MaxDMAAddress = Value;
}

/*! SetDataPattern
*
* \brief Pattern type to format data buffer.
* \param pcfgParams
* \param Value
* \return None
*/
VOID
SetDataPattern(
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	if (Value > PATTERN_RANDOM)
	{
		DisplayError("-p option pattern number out range, option ignored");
	}
	else
	{
		pcfgParams->DataPatternType		= Value;
		pcfgParams->bPatternSpecified	= TRUE;
	}
}
        
/*! SetQuiet 
 *
 * \brief Turn on Quiet mode.
 * \param pcfgParams
 * \param Value
 * \return None
 */
VOID 
SetQuiet(
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	UNREFERENCED_PARAMETER(Value);

	pcfgParams->bQuiet = TRUE;
}
        
/*! SetRandom
 * 
 * \brief Turn on Randomize mode.
 * \param pcfgParams
 * \param Value
 * \return None
 */
VOID 
SetRandom(
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	UNREFERENCED_PARAMETER(Value);

	pcfgParams->bRandomize = TRUE;
}

/*! SetSeconds
*
* \brief Number of Seconds to run the test.
* \param pcfgParams
* \param Value
* \return None
*/
VOID	
SetSeconds(
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	pcfgParams->Seconds = Value;
}
        
/*! SetThreads
*
* \brief Set the number of threads.
* \param pcfgParams
* \param Value
* \return None
*/
VOID
SetThreads(
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	if ((Value < 1) || (Value > MAX_THREADS))
	{
		printf("Error: %d is the Maximum Threads allowed, option ignored\n", MAX_THREADS);
	}
	else
	{
		pcfgParams->NumberThreads = Value;
	}
}
        
/*! SetVerbose
*
* \brief Enable verbose message output.
* \param pcfgParams
* \param Value
* \return None
*/
VOID
SetVerbose(
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	UNREFERENCED_PARAMETER(Value);

	pcfgParams->bVerbose = TRUE;
}
        
/*! SetDebug 
 * 
 * \brief Turn on Debug mode. 
 * \param Value
 * \param pcfgParams
 * \return None
 */
VOID
SetDebug(	
	PCFG_PARAMS			pcfgParams, 
	UINT32				Value
)
{
	UNREFERENCED_PARAMETER(Value);

	pcfgParams->bDebug = TRUE;
}
// --------------------------------------------------------------------------------------
// CLI Option table
// --------------------------------------------------------------------------------------
const option_t Options[] = {
    {
        "a",
        SetAlignment,
		TYPE_ULONG,
        "Set memory alignment, 0=no alignment preference, 1=aligned buffers,\n\tdefault is aligned"
    },
    {
        "b",
        SetDataSeed,
		TYPE_ULONG,
        "Data Seed Pattern, use with -p"
    },
    {
        "c",
        SetCardNumber,
		TYPE_ULONG,
        "Set Card Number, default is 0"
    },
    {
        "dr",
        SetReadDMAChannel,
		TYPE_ULONG,
        "Set Read DMA Channel number, default is 0"
    },
    {
        "dw",
        SetWriteDMAChannel,
		TYPE_ULONG,
		"Set Write DMA Channel number, default is 0"
    },
    {
        "e",
        SetStopOnError,
		TYPE_NONE,
        "Enables Stop on Error"
    },
    {
        "fr",
        SetReadFIFO,
		TYPE_NONE,
		"Set Read source with No address increment, default is off"
    },
	    {
        "fw",
        SetWriteFIFO,
		TYPE_NONE,
		"Set Write destination with No address increment, default is off"
    },
    {
        "i",
        SetIterations,
		TYPE_ULONG,
		"Set the number of test iterations, default is 1,000,000."
    },
    {
        "m",
        SetMaxDMAAddress,
		TYPE_ULONG,
		"Set Maximum DMA Address, default is 131072"
    },
    {
        "p",
        SetDataPattern,
		TYPE_ULONG,
		"Set Data Pattern, 0=Increment, 1=Fill, 2=Random, based on seed (-b)"
    },
    {
        "q",
        SetQuiet,
		TYPE_NONE,
		"Set Quiet mode, only display summary"
    },
    {
        "r",
        SetRandom,
		TYPE_NONE,
		"Set random size transfers and Card Offset if applicable"
    },
    {
        "s",
        SetSeconds,
		TYPE_ULONG,
		"Set number of seconds to run the test, default is 60 seconds."
    },
    {
        "t",
        SetThreads,
		TYPE_ULONG,
		"Number of threads to use, default is 1."
    },
    {
        "v",
        SetVerbose,
		TYPE_NONE,
		"Verbose mode, prints additional performance information."
    },
    {
        "z",
        SetDebug,
		TYPE_NONE,
		"Set Debug mode, displays step by step debug information."
    },
};

// --------------------------------------------------------------------------------------
// CLI Command Table
// --------------------------------------------------------------------------------------

const command_t Commands[] = {
    {
        "help",
        help,
		TYPE_HELP,
        "\n\tDispalys this information"
    },
    {
        "getboardcfg",
        getBoardCfg,
		TYPE_GEN,
		"\n\tDisplays the PCI Config table"
    },
    {
        "writemem",
        writeMem,
		TYPE_GEN,
        "<BARNum> <CardOffset> <Length> <Data...>\n\tPerforms a Memory Write to the card BARNum at CardOffset for Length"
    },
    {
        "readmem",
        readMem,
		TYPE_GEN,
        "<BARNum> <CardOffset> <Length>\n\tPerforms a Memory Read from the card BARNum at CardOffset for Length"
    },
    {
        "readdma",
        ReadDMA,
		TYPE_DMA,
        "<CardOffset> <Length>\n\tPerforms a DMA Read at CardOffset for Length"
    },
    {
        "writedma",
        WriteDMA,
		TYPE_DMA,
		"<CardOffset> <Length>\n\tPerforms a DMA Write of Data Pattern to CardOffset for Length"
    },
    {
        "wrdma",
        WriteReadDMA,
		TYPE_DMA,
		"<CardOffset> <Length>\n\tPerforms a DMA Write followed by a DMA Read of Data Pattern to\n\tCardOffset for Length"
    },
    {
        "fifos2c",
        FIFOS2C,
		TYPE_DMA,
        "<Length>\n\tPerforms a FIFO System to Card DMA transfer using Length transfer size"
    },
    {
        "fifoc2s",
        FIFOC2S,
		TYPE_DMA,
        "<Length>\n\tPerforms a FIFO Card to System DMA transfer using Length transfer size"
    },
    {
        "writepci",
        writePCI,
		TYPE_GEN,
        "<Offset> <Length> <Data...>\n\tPerforms a write to PCI Configuration space of Data at Offset for Length"
    },
    {
        "readpci",
        readPCI,
		TYPE_GEN,
        "<Offset> <Length>\n\tPerforms a read of PCI Configuration space at Offset for Length"
    },
};


/*! SetOptions
 * 
 *	\param option_list
 *	\param args
 *	\param n_args
 *  \param n_option_list
 */
VOID
SetOptions(
    const option_t * option_list,
    INT32			n_option_list,
    const PCHAR		args[],
    INT32			n_args,
	PCFG_PARAMS		pcfgParams
)
{
	CHAR			Option[MAX_OPTION_LENGTH+1];
	INT32			OptionLength;
	CHAR			delims[] = "-/\\";
	CHAR			delims2[] = " =:";
	PCHAR			OptionPtr;
	PCHAR			OptionValue = NULL;
	UINT32			Value;
	BOOLEAN			OptionFound;
	BOOLEAN			ValidValue;

	for (int argc = 1; argc < n_args; argc++)
	{
		// First look for a valid delimiter i.e. -x or /x or =x
		for (int t = 0; t < 3; t++)
		{
			OptionPtr = strchr((PCHAR)args[argc], delims[t]);
			if (OptionPtr != NULL)
			{
				OptionPtr++;	// Increment past the delimiter
				// convert option to uppercase to simplify matching
				OptionLength = ((INT32)strlen(OptionPtr) > MAX_OPTION_LENGTH) ? 
											MAX_OPTION_LENGTH : (INT32)strlen(OptionPtr);
				for (CHAR c = 0; c < OptionLength; c++)
				{
					Option[c] = (CHAR)tolower(OptionPtr[c]);
				}
				Option[OptionLength] = 0;
	
		        Value = 0;
				OptionFound = FALSE;
				ValidValue = FALSE;
				// Look through all the possible options in the table for a match
			    for (int x = 0; x < n_option_list; x++)
			    {
					if (strncmp(Option, option_list[x].opt, strlen(option_list[x].opt)) == 0)
					{
						// We found a match, make sure there is a valid function before we go to far.
						if (option_list[x].func != NULL)
						{
							OptionFound = TRUE;
							// Does it have a parameter associated?
							if (option_list[x].type == TYPE_NONE)
							{
								ValidValue = TRUE;
							}
							// Is the parameter suppose to be a ULONG?
							else if (option_list[x].type == TYPE_ULONG)
							{
								// Move the pointer to past the option letter(s)
								OptionPtr += strlen(option_list[x].opt);
								// Check to see if this is the end of the string, 
								//   the value could be in the next arg
								if (strlen(OptionPtr) == 0)
								{
									OptionPtr = (PCHAR)args[++argc];
									if (OptionPtr != NULL)
									{
										// Make sure the next value isn't another option
										for (int d = 0; d < 3; d++)
										{
											if (strchr((PCHAR)OptionPtr, delims[d]) != NULL)
											{
												argc--;	// Backup the argument pointer.
												OptionPtr = NULL;
												break;
											}
										}
									}
								}
								if (OptionPtr != NULL)
								{
									// Search through the possible delimiters
									for (INT32 i = 0; i < 3; i++)
									{
										// Any of delimiters in this string?
										OptionValue = strchr(OptionPtr, delims2[i]);
										// We found a delimiter
										if (OptionValue != NULL)
										{	
											// Make sure there is enough data to convert
											if (strlen(OptionValue) > 1)
											{
												OptionValue++;	// Increment past the delimiter
											}
											else
											{
												// else the data might be in the next arg
												OptionValue = (PCHAR)args[++argc];
											}
											break;
										}
									}
								}
								// Did we find a parameter value in either the next arg or after a delimiter?
								if (OptionValue != NULL)
								{
									// Make sure it is a hex numeric value
									ValidValue = isHexStr(OptionValue);
									if (ValidValue)
									{
										Value = __strtoul(OptionValue);
									}
								}
								// If not is the data still in this sring?
								else if (OptionPtr != NULL)
								{
									// Make sure it is a hex numeric value
									ValidValue = isHexStr(OptionPtr);
									if (ValidValue)
									{
										Value = __strtoul(OptionPtr);
									}
								}
								// After all this parsing do we have a good parameter value?
								if (!ValidValue)
								{
									printf("Error: The -%s requires a parameter, option ignored.\n", option_list[x].opt);
								}
							} // if TYPE_NONE or TYPE_ULONG check

							// Only if we have a good parameter value do we process the option
							if (ValidValue)
							{
								#if _DEBUG
									printf("Processing -%s with a parameter of 0x%lx\n", option_list[x].opt, Value);
								#endif // DEBUG Version
								(Options[x].func)(pcfgParams, Value);
							}
							break;
						}
					}
				}
				if (!OptionFound)
				{
					printf("Error: -%s option is not recognized\n", Option);
				}
				break;
			}
		}
	}
}

/*! FindCmd
 *
 *	\param cmd_list
 *	\param n_cmd_list
 *  \param args
 *  \param n_args
 *  \param pcfgParams
 *  \return x or -1
 */
INT32 
FindCmd(
    const command_t *   cmd_list,
    INT32				n_cmd_list,
    const PCHAR			args[],
    INT32				n_args,
	PCFG_PARAMS			pcfgParams
)
{
    INT32				x;
	INT32				argc;
	INT8				command[MAX_CMD_LENGTH+1];
	INT32				commandLength;

	for (argc = 0; argc < n_args; argc++)
	{
		// convert command to uppercase to simplify matching
		commandLength = ((INT32)strlen(args[argc]) > MAX_CMD_LENGTH) ? 
										MAX_CMD_LENGTH : (INT32)strlen(args[argc]);
		for (char i=0; i<commandLength; i++)
		{
			command[i] = (INT8)tolower(args[argc][i]);
		}
		command[commandLength] = 0;

	    for (x = 0; x < n_cmd_list; x++)
	    {
			if (strcmp((PCHAR)command, cmd_list[x].cmd) == 0)
			{
	            if (cmd_list[x].func != NULL)
		        {
					pcfgParams->Index = x;
					pcfgParams->CmdArgc = n_args - argc;
					pcfgParams->CmdArgv = &args[argc];
					pcfgParams->Type = cmd_list[x].type;
					return (x);
		        }
			}
		}
	}
    return -1;
}

/*! main
 *
 * \brief Main entry point for application. It all starts and ends here.
 * \param argc
 * \param argv
 * \return status
 */
INT32
main(
	INT32			argc, 
	const PCHAR		argv[]
)
{
	INT32			status = 0;
#ifdef WIN32
#ifdef USER_INTERRUPT_SUPPORT
	HANDLE			hUserIntThreadHandle = NULL;
#endif // USER_INTERRUPT_SUPPORT
#endif // WIN32

	pcfgParams = (PCFG_PARAMS)malloc(sizeof(CFG_PARAMS));
	if (pcfgParams == NULL)
	{
		return -1;
	}
	memset(pcfgParams, 0, sizeof(CFG_PARAMS));

	// Setup the defaults
	pcfgParams->cardNum			= DEFAULT_CARD_NUMBER;
	pcfgParams->BufferSize		= DEFAULT_BUFFER_SIZE;
	pcfgParams->ReadDMAChannel	= DEFAULT_READ_DMA_CHANNEL;
	pcfgParams->WriteDMAChannel = DEFAULT_WRITE_DMA_CHANNEL;
	pcfgParams->Iterations		= (UINT32)DEFAULT_ITERATIONS;
	pcfgParams->Seconds			= (UINT32)DEFAULT_SECONDS;
	pcfgParams->NumberThreads	= DEFAULT_NUMBER_THREAD;
    pcfgParams->MaxDMAAddress	= DEFAULT_ADDR_DMA_MAX;
	pcfgParams->MaxTransferSize	= DEFAULT_MAX_TRANSFER_SIZE;
	pcfgParams->bVerbose		= DEFAULT_VERBOSE;
    pcfgParams->CardAddress		= 0;
	pcfgParams->Index			= -1;
	pcfgParams->Type			= -1;
	pcfgParams->bNonAligned		= FALSE;
	pcfgParams->bReadFIFO		= FALSE;
	pcfgParams->bWriteFIFO		= FALSE;
	pcfgParams->bDebug			= DEFAULT_DEBUG;
	pcfgParams->bQuiet			= DEFAULT_QUIET;
	pcfgParams->DataPatternType = PATTERN_INCREMENTING;
	pcfgParams->DataSeedSize	= 1;
	pcfgParams->DataSeedPattern	= DEFAULT_SEED_PATTERN;
	pcfgParams->CmdArgc			= argc;
	pcfgParams->CmdArgv			= &(argv[1]);
	pcfgParams->bStopOnError	= DEFAULT_STOP_ON_ERROR;
	pcfgParams->iPass			= 0;
	pcfgParams->bRandomize		= DEFAULT_RANDOMIZE;
	pcfgParams->bPatternSpecified = FALSE;

	// Print the sign-on message
	printf("\n%s%s\n" , Title, Version);
	/* initialize random seed: */
	srand ((UINT32)time(NULL) );

	// Handle the signals, I want to try and shutdown as gracefully	as possible.
    signal(SIGTERM, SignalHandler);
    signal(SIGABRT, SignalHandler);
    signal(SIGINT,  SignalHandler);

#ifndef WIN32	// Linux version
    // Handle the additional Linux signals.
    signal(SIGQUIT, SignalHandler);
    signal(SIGHUP,  SignalHandler);
	signal(SIGTSTP, SignalHandler);
	signal(SIGSTOP, SignalHandler);
#endif // Linux Version

	/* Setup any OS Specific features */
	SetupOSSpecifics(pcfgParams);
	/* Parse and set all options found on the command line */
	SetOptions(Options, _NELEM(Options), argv, argc, pcfgParams);
	/* Index past the program name */
	argv++;
	argc--;

	// Open the generic driver interface
	pcfgParams->hDrvDevice = ConnectDriver(pcfgParams->cardNum, NULL);

#ifdef WIN32
#ifdef USER_INTERRUPT_SUPPORT
	if (pcfgParams->hDrvDevice != INVALID_HANDLE_VALUE)
	{
		// Start the User Interrupt Thread
		hUserIntThreadHandle = CreateThread((LPSECURITY_ATTRIBUTES)NULL,	// No security.
			(DWORD)0,														// Same stack size.
			(LPTHREAD_START_ROUTINE)UserIntThreadProc,						// Thread procedure.
			(LPVOID)pcfgParams,												// Pointer the config parameters.
			(DWORD)0,														// Start immediately.
			NULL);															// Thread ID.

		if (hUserIntThreadHandle == NULL)
		{
			fprintf(stderr, "User Interrupt CreateThread Failed, returned %d", GetLastError());
			return -2;
		}
	}
	else
	{
		printf("Error: Driver for board number %d not found\n", pcfgParams->cardNum);
		pcfgParams->iPass = ERROR_NO_BOARD_FOUND;
	}
#endif	// USER_INTERRUPT_SUPPORT
#endif	// Windows

	if (argc > 0)
	{
		if (FindCmd(Commands, _NELEM(Commands), argv, argc, pcfgParams) != -1)
		{
			if (pcfgParams->Type == TYPE_HELP)
			{
				status = (Commands[pcfgParams->Index].func)(pcfgParams);
			}
			else if (pcfgParams->Type == TYPE_DMA)
			{
				// The connection to the driver is handled in the individual commands
				status = (Commands[pcfgParams->Index].func)(pcfgParams);
			}
			else
			{
				status = (Commands[pcfgParams->Index].func)(pcfgParams);
			}
		}
		else
		{
			DisplayError("No recognized commands found");
			pcfgParams->iPass = ERROR_BAD_TEST_COMMAND;
		}
	}
	else
	{
		help(pcfgParams);
	}

#ifdef WIN32
#ifdef USER_INTERRUPT_SUPPORT
	// Shut down the User Interrupt Thread
	CloseHandle(hUserIntThreadHandle);
#endif	// USER_INTERRUPT_SUPPORT
#endif	// WIN32

	CloseHandle(pcfgParams->hDrvDevice);
	pcfgParams->hDrvDevice = INVALID_HANDLE_VALUE;

	if (pcfgParams->iPass != 0)
	{
		status = pcfgParams->iPass;
	}
	ShutdownOSSpecifics(pcfgParams);

	free(pcfgParams);
	return status;
}

/*! help
 *
 * \brief Handle the help command
 * \param pcfgParams
 * \return 0
 */
INT32 
help(
    PCFG_PARAMS		pcfgParams
)
{
	UNREFERENCED_PARAMETER(pcfgParams);

	INT32	i;
	printf("\n%s\n", usage);
	printf("\nOptional (options) parameters:\n");
	for (i = 0; i < _NELEM(Options); i++)
	{
		printf("  -%s\t%s\n", Options[i].opt, Options[i].help);
	}
	printf("\nCommands:\n");
	for (i = 0; i < _NELEM(Commands); i++)
	{
		printf("  %s %s\n", Commands[i].cmd, Commands[i].help);
	}
    return 0;
}
