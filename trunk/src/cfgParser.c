// -------------------------------------------------------------------------
/*!
    Exxeno Hub - ExxComm
    \file       cfgParser.c
    Contains the Exxeno configuration parsing utilities.
*/

// ------------------------- CONFIDENTIAL ----------------------------------
//
//              Copyright (c) 2017 by Exxeno, LLC.
//                       All rights reserved.
//
// Trade Secret of Exxeno, LLC.  Do not disclose.
//
// Use of this source code in any form or means is permitted only
// with a valid, written license agreement with Exxeno, Inc.
//
// Licensee shall keep all information contained herein confidential
// and shall protect same in whole or in part from disclosure and
// dissemination to all third parties.
//
//
// Exxeno, LLC.
// Hillsboro, OR 97124, USA
//
//
// -------------------------------------------------------------------------
//

/*!
 * \note
 * This file parses the Interrogator.conf file to determine which CellTypes and
 * respective registers to poll for information from a target Emerson E2 Controller.
 * This version of cfgParser closely mirrors that of ExxComm, but does not include
 * all of the same functionality, and is only targeted for the 
 * Exxeno Interrogation Utility.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/Interrogator.h"
#include "../include/Exxeno.h"
#include "../include/libconfig.h"

//~~ Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*!
    \brief  Retrieves the E2 Configuration for the given index
    \param  pE2 - pointer to E2_PARAMS for given sensor
    \param  index - Sensor number of Configuration
    \return True = success, False = Config not found
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
boolean
getE2Config(PE2_PARAMS pE2, int index, char * fileName)
{
    config_t cfg;
    config_setting_t *setting, *RecSetting, *childSetting;
    const char * string;
	boolean		 	result = FALSE;
	int			res;

	// DEBUGP(LOG_NOTICE, "Entered getE2Config - index: %i", index);

    config_init(&cfg);

    /* Read the file. If there is an error, report it and exit. */
	res = config_read_file(&cfg, fileName);
    if (!res) 
	{
        // DEBUGP(LOG_ERR, "EXXCOMM ERROR: Configuration file not found or corrupted\n");
		fprintf(stderr, "config_read_file: %s : %d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
		config_destroy(&cfg);
        return result;
	}
	
    /* Output a list of all controllers. */
    setting = config_lookup(&cfg, "Interrogate");
    if (setting != NULL)
    {
        int count = config_setting_length(setting);

		if (config_setting_lookup_string(setting, "IPAddress", &string))
		{
			strncpy(pE2->IPAddr, string, MAX_IP_ADDR_CHARS);
		}
		else
		{
			// DEBUGP(LOG_ERR, "EXXCOMM ERROR: Failed to find IPAddress in controller config\n");
		}
		if (config_setting_lookup_string(setting, "FSDPort", &string))
		{
			strncpy(pE2->FSDPort, string, MAX_FSD_ADDR_CHARS);
		}
		else
		{
			// DEBUGP(LOG_ERR, "EXXCOMM ERROR: Failed to find FSD Port in controller config\n");
		}

		//config_setting_t *controller = config_setting_get_elem(setting, index);

        if (index < count)
        {

            RecSetting = config_setting_get_member(setting, "CellTypes");
            if (RecSetting != NULL)
            {
                int RecCount = config_setting_length(RecSetting);
                int t;

                if (RecCount > MAX_E2_RECORDS)
                    RecCount = MAX_E2_RECORDS;

				pE2->numActiveRecords = RecCount;

                for (t = 0; t < RecCount; ++t)
                {
                    config_setting_t *record = config_setting_get_elem(RecSetting, t);

                    pE2->Records[t].NumRegisters = 0;
              
					if (!config_setting_lookup_int(record, "CellType", &pE2->Records[t].CellType))
                    {
						// DEBUGP(LOG_ERR, "EXXCOMM ERROR: Failed to find CellType in controller config\n");
					}
					childSetting = config_setting_get_member(record, "Registers");
					if (childSetting != NULL)
					{
						int childCount = config_setting_length(childSetting);
						int i;

						if (childCount > MAX_E2_SLOTS)
						childCount = MAX_E2_SLOTS;

						for (i = 0; i < childCount; ++i)
						{
							pE2->Records[t].Register[i] = config_setting_get_int_elem(childSetting, i);
							pE2->Records[t].NumRegisters++;
						}
						result = TRUE;
					}
					else
					{
						// DEBUGP(LOG_ERR, "EXXCOMM ERROR: Failed to find Registers in controller config\n");
					}
				}
			}
			// DEBUGP(LOG_NOTICE, "E2 Config: MAC:%s - IP: %s",	pE2->MACAddr, pE2->IPAddr);
		}
		else
		{
		// DEBUGP(LOG_ERR, "EXXCOMM ERROR: Failed to find Records in controller config\n");
			// If the index is larger than the count then there is no E2's at this Index
			// DEBUGP(LOG_DEBUG, "E2 Config: Index out of range");
		}
    }
	else
	{
        // DEBUGP(LOG_NOTICE, "E2 Config: Interrogate not found");
	}

    config_destroy(&cfg);
    return result;
}
