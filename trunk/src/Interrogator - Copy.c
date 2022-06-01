// -------------------------------------------------------------------------
/*!
    Exxeno Hub - Interrogator
    \file       Interrogator.c
    Configurable E2 Polling Utility.
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

#define V1 FALSE
#define DEBUG FALSE

// Includes
#include "../include/Interrogator.h"
#include "../include/Exxeno.h"
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include "../include/curl.h"
#include "../include/cJSON.h"
#include "../include/libconfig.h"

// Constants
const char * DefaultFileName = "Interrogator.conf"; // User designated config file to use.

/*!
	\note
    We use the Java RPC calls to communicate with the Emerson E2 controller.

	const char * getVal = "{\"id\":1,\"method\":\"E2.GetConfigValues\",\"params\":[[\"PCW-E2:GENERAL SERV:7336\",\"PCW-E2:GENERAL SERV:7023\"]]}";
	const char * getMulti = "{\"id\":2,\"method\":\"E2.GetMultiPointInfo\",\"params\":[[\"PCW-E2:GLOBAL DATA:2048\",\"PCW-E2:GLOBAL DATA:2049\",\"PCW-E2:GLOBAL DATA:2060\",\"PCW-E2:GLOBAL DATA:2050\",\"PCW-E2:ADVISORY SERV:2048\",\"PCW-E2:GLOBAL DATA:2057\",\"PCW-E2:GLOBAL DATA:2059\"]]}";
	const char * getMultiEx = "{\"id\":3,\"method\":\"E2.GetMultiExpandedStatus\",\"params\":[[\"PCW-E2:DI_SENSOR:2048\",\"PCW-E2:GLOBAL DATA:2049\",\"PCW-E2:GLOBAL DATA:2060\",\"PCW-E2:GLOBAL DATA:2050\",\"PCW-E2:ADVISORY SERV:2048\",\"PCW-E2:GLOBAL DATA:2057\",\"PCW-E2:GLOBAL DATA:2059\"]]}";
	const char * getMultiEx = "{\"id\":3,\"method\":\"E2.GetMultiExpandedStatus\",\"params\":[[\"PCW-E2:GLOBAL DATA:2048\",\"PCW-E2:GLOBAL DATA:2049\",\"PCW-E2:GLOBAL DATA:2060\",\"PCW-E2:GLOBAL DATA:2050\",\"PCW-E2:ADVISORY SERV:2048\",\"PCW-E2:GLOBAL DATA:2057\",\"PCW-E2:GLOBAL DATA:2059\"]]}";
	const char * getTimeDate = "{\"id\":4,\"method\":\"E2.GetConfigValues\",\"params\":[[\"PCW-E2:TIME SERVICES:7000\",\"PCW-E2:TIME SERVICES:7001\"]]}";

    The following is the Java RPC strings to perform commands on the E2.
    The commented lines are directly from the e2json.js javascript file.
*/

//'{"id":' + %d + ',"method":"E2.GetCellList","params":["' + controllerName + '"]}');
const char * hdGCL = ",\"method\":\"E2.GetCellList\",\"params\":[\"";

// '{"id":' + %d + ',"method":"E2.Override","params":["' + point + '",' + override + ',' + overrideType + ',"' + overrideTime + '","' + overrideValue + '"]}');
const char * hdOvr = ",\"method\":\"E2.Override\",\"params\":[\"";

//'{"id":' + %d + ',"method":"E2.MultiOverride","params":[[' + str + ']]}');
const char * hdMOvr = ",\"method\":\"E2.MultiOverride\",\"params\":[\"";

// '{"id":' + %d + ',"method":"E2.GetPointInfo","params":["' + point + '"]}');
const char * hdGPI = ",\"method\":\"E2.GetPointInfo\",\"params\":[\"";

// '{"id":' + %d + ',"method":"E2.GetMultiPointInfo","params":[[' + str + ']]}');
const char * hdMul = ",\"method\":\"E2.GetMultiPointInfo\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.GetExpandedStatus","params":["' + point + '"]}');
const char * hdGES = ",\"method\":\"E2.GetExpandedStatus\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.GetMultiExpandedStatus","params":[[' + str + ']]}');
// '{"id":' + %d + ',"method":"E2.GetMultiExpandedStatus","params":[["' + _controllerName + ':ADVISORY SERV:ALARM OUTPUT"]]} ');
const char * hdMEx = ",\"method\":\"E2.GetMultiExpandedStatus\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.GetExpandedInfo","params":["' + point + '"]}');
const char * hdGEI = ",\"method\":\"E2.GetExpandedInfo\",\"params\":[[\"";

//'{"id":' + %d + ',"method":"E2.GetLogDataRaw","params":["' + point + '"]}');
const char * hdGLDR = ",\"method\":\"E2.GetLogDataRaw\",\"params\":[[\"";

//'{"id":' + %d + ',"method":"E2.GetAlarmList","params":["' + controller + '", ' + (filter?true:false) + ']}');
const char * hdGAL = ",\"method\":\"E2.GetAlarmList\",\"params\":[\"";

// '{"id":' + %d + ',"method":"E2.AlarmAction","params":[' + action + ',[' + str + ']]}');
const char * hdAA = ",\"method\":\"E2.AlarmAction\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.GetDeviceInfoForRoute","params":["' + route + '"]}');
const char * hdGDIFR = ",\"method\":\"E2.GetDeviceInfoForRoute\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.GetConfigValues","params":[[' + str + ']]}');
const char * hdGCV = ",\"method\":\"E2.GetConfigValues\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.SetConfigValues","params":[[' + str + ']]}');
const char * hdSCV = ",\"method\":\"E2.SetConfigValues\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.GetAuxProps","params":["' +point+'",[' + str + ']]}');
const char * hdGAP = ",\"method\":\"E2.GetAuxProps\",\"params\":[1]}";

// '{"id":' + %d + ',"method":"E2.SetAuxProps","params":["' +point+'",[' + str + ']]}');
const char * hdSAP = ",\"method\":\"E2.SetAuxProps\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.GetMultiAuxProps","params":[[' + str + ']]}');
const char * hdGMAP = ",\"method\":\"E2.GetMultiAuxProps\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.SetMultiAuxProps","params":[['+str+']]}');
const char * hdSMAP = ",\"method\":\"E2.SetMultiAuxProps\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.GetLoginInfo","params":[]} ');
const char * hdGLI = ",\"method\":\"E2.GetLoginInfo\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.GetAppConfig","params":["'+app+'"]} ');
const char * hdGAC = ",\"method\":\"E2.GetAppConfig\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.SetAppConfig","params":["'+app+'","'+cfg+'"]} ');
const char * hdSAC = ",\"method\":\"E2.SetAppConfig\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.GetBatteryStatus","params":[]}');
const char * hdGBS = ",\"method\":\"E2.GetBatteryStatus\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.BatteryReplaced","params":[]}');
const char * hdBR = ",\"method\":\"E2.BatteryReplaced\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.GetOutputPointers","params":[[' + str + ']]}');
const char * hdGOP = ",\"method\":\"E2.GetOutputPointers\",\"params\":[[\"";

// '{"id":' + %d + ',"method":"E2.GetControllerList","params":[1]}');
const char * hdGCNTL = ",\"method\":\"E2.GetControllerList\",\"params\":[1]}";

const char * hdID = "{\"id\":";
const char * hdsTerm = "\"]}";
const char * hdTerm = "\"]]}";
const char * hdnqTerm = "]}";
const char * hdSep = "\":\"";

//const char * prID = " \"id\": ";
const char * prID = "\"id\": ";

const char * CntrlName    = "setControllerName(";

/*!
 * parse_object:
 * \brief
 * Walks the JSON response looking to a match to 'target'. If a match
 * is found then the call back will be called with the parsed value
 * \param pE2Req - Pointer to the main E2 configuration data structure.
 * \param item - pointer to the JSON parsed data
 * \param target - name to match
 * \param pCB - Callback routine
 * \return None
 */
int
parse_object(
	PE2_PARAMS  pE2Req,
	cJSON *     item,
	char *      target,
	parserCallback  pCB)
{
	cJSON * subitem;
	int 	i;
	int		result = -1; // Assume false until we do something.

	if (item == NULL)
	{
		printf("%i\n", result);
		return result; // item is null, the result is false.
	}
	subitem = item->child;

	while (subitem)
	{
		for (i = 0; i < cJSON_GetArraySize(subitem); i++)
		{
			cJSON *nextitem = cJSON_GetArrayItem(subitem, i);
			if (nextitem)
			{
				if (nextitem->string != NULL)
				{
					if (target != NULL)
					{
						if (strcmp(nextitem->string, target) == 0)
						{
							//printf("\nJSON name: %s", nextitem->string);
							switch (nextitem->type)
							{
								// Looking for a Boolean Data Type in the JSON.
							case cJSON_False:
								pE2Req->lastBool = TW_FALSE;
								result = 0;
								//printf(" FALSE");
								break;

							case cJSON_True:
								pE2Req->lastBool = TW_TRUE;
								//printf(" TRUE");
								result = 0;
								break;

							case cJSON_NULL:
								pE2Req->lastBool = TW_NOT_SET;
								//printf(" NULL");
								result = 0;
								break;

							case cJSON_Number:
								// Looking for an Integer / Number Data Type in the JSON.
								if (pCB != NULL)
								{
									pCB(pE2Req, &nextitem->valueint);
									result = 0;
								}
								else
								{
									pE2Req->lastInt = nextitem->valueint;
									//printf(" %i", nextitem->valueint);
									result = 0;
								}
								break;

							case cJSON_String:
								// Looking for a String Data Type in the JSON.
								memset(pE2Req->lastStr, 0, E2_MAX_STR_SIZE);
								if (strlen(nextitem->valuestring) > 0)
								{
									//printf(" JSON STRING: %s", nextitem->valuestring);
									if (pCB != NULL)
									{
										pCB(pE2Req, nextitem->valuestring);
										result = 0;
									}
									else
									{
										{
											result = 0;
#ifdef WIN
											strncpy_s(pE2Req->lastStr, MAX_STR_SIZE, nextitem->valuestring, MAX_STR_SIZE);
#else
											strncpy(pE2Req->lastStr, nextitem->valuestring, E2_MAX_STR_SIZE);
#endif
										}
									}
									break;
							default:
								break;
							}

						}
					}
				}
			}
		}
			else
			{
				printf("Nextitem == NULL\n");
			}

	}

		if (subitem->child)
		{
			result = parse_object(pE2Req, subitem->child, target, pCB);
		}
		else
		{
			/*! DEBUG
			printf("checking subitem->next\n");

			if (subitem->next)
			{
			subitem = subitem->next;
			if (subitem->string != NULL)
			{
			printf("JSON next name: %s  ", subitem->string);
			}
			printf("JSON next (%p) Type: %d, Array size: %d\n", subitem, subitem->type, cJSON_GetArraySize(subitem));
			}
			*/
		}
		subitem = subitem->next;
}
	return result;
}
/*!
* read_callback:
* \brief
* Curl callback function for read requests
* \param ptr
* \param size
* \param nmemb
* \param userp
* \return None
*/
size_t
read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	UNREFERENCED_PARAMETER(userp);

    if ((size * nmemb) < 1)
    {
        return 0;
    }
    // printf("\n\n\tRead_Callback: ptr:0x%p, size:%d, members:%d\n\n", ptr, (int)size, (int)nmemb);
    return (size * nmemb);
}

/*!
* page_callback:
* \brief
* Curl callback function for main webpage requests
* \param ptr
* \param size
* \param nmemb
* \param userp
* \return None
*/
size_t
page_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	PE2_PARAMS  pE2Req = (PE2_PARAMS)userp;
    char *      pCtlName;
    char *      pch;

    // DEBUG fprintf(stderr,"page_callback\n");
//#if 1 // Deprecated - libcurl error.
    if ((size * nmemb) < 1)
    {
        return 0;
    }
    pCtlName = strstr((char *)ptr, CntrlName);
    if (pCtlName != NULL)
    {
        pCtlName += (strlen(CntrlName) + 1);
        pch = strchr(pCtlName, 0x27);
#ifndef WIN
		strncpy((char *)pE2Req->E2Name, pCtlName, (pch - pCtlName));
#else
		strncpy_s((char *)pE2Req->E2Name, E2_NAME_SIZE, pCtlName, (pch - pCtlName));
#endif
        pE2Req->E2Name[(pch - pCtlName)] = '\0';
        printf("Controller Name=, %s\n", (char *)pE2Req->E2Name);
    }
//#endif
    return (size * nmemb);
}

/*!
* controller_callback:
* \brief
* Curl callback function for Contoller list method requests.
* \param ptr
* \param size
* \param nmemb
* \param userp
* \return None
*/
size_t
controller_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	PE2_PARAMS  pE2Req = (PE2_PARAMS)userp;
    cJSON *     root;

    if ((size * nmemb) < 1)
    {
        return 0;
    }
    root=cJSON_Parse(ptr);
	if (root != NULL)
	{
		parse_object((PE2_PARAMS)pE2Req, root, NULL, NULL);
		cJSON_Delete(root);
	}
	// DEBUG printf("E2 Controller Path,%s,", (char *)ptr);
    return (size * nmemb);
}

/*!
* cellStrCallback:
* \brief
* Parser callback function for Cell List string values.
* \param pE2Req
* \param data
* \return None
*/
void
cellStrCallback(PE2_PARAMS pE2Req, void * data)
{
    static int      cellIndex = 0;
    char *          strData = (char *)data;

//! DEBUG: printf("cellStrCallback: %s\n", strData);
#ifndef WIN
	strncpy(pE2Req->cellList[cellIndex].cellName,strData, E2_MAX_STR_SIZE);
#else
	strncpy_s(pE2Req->cellList[cellIndex].cellName, MAX_STR_SIZE, strData, MAX_STR_SIZE);
#endif
    cellIndex++;
}

/*!
* cellIntCallback:
* \brief
* Parser callback function for Cell List interger values.
* \param pE2Req
* \param data
* \return None
*/
void
cellIntCallback(PE2_PARAMS pE2Req, void * data)
{
    static int      cellIndex = 0;
    int *           pIntData = (int *)data;

    pE2Req->cellList[cellIndex].cellType = *pIntData;
    cellIndex++;

//! DEBUG: printf("cellIntCallback: %i\n", *pIntData);
}

/*!
* cell_list_callback:
* \brief
* Curl callback function for Cell List method requests.
* \param ptr
* \param size
* \param nmemb
* \param userp
* \return None
*/
size_t
cell_list_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	PE2_PARAMS     pE2Req = (PE2_PARAMS)userp;
    cJSON *     root;
    static char *   pCellBuf = NULL;
    static int      cellBufSize = 0;

    if ((size * nmemb) < 1)
    {
        return 0;
    }

    pCellBuf = realloc(pCellBuf, ((size * nmemb) + cellBufSize));
    memcpy(pCellBuf + cellBufSize, ptr, (size * nmemb));
    cellBufSize += (size * nmemb);
    if (strstr(pCellBuf, prID) != NULL)
    {
        root=cJSON_Parse(pCellBuf);
		if (root != NULL)
		{
			parse_object((PE2_PARAMS)pE2Req, root, "cellname", cellStrCallback);
			parse_object((PE2_PARAMS)pE2Req, root, "celltype", cellIntCallback);
			cJSON_Delete(root);
		}
        free(pCellBuf);
        pCellBuf = NULL;
        cellBufSize = 0;
    }
    return (size * nmemb);
}

/*!
* alarm_list_callback:
* \brief
* Curl callback function for Cell List method requests.
* \param ptr
* \param size
* \param nmemb
* \param userp
* \return None
*/
size_t
alarm_list_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	UNREFERENCED_PARAMETER(userp);
    cJSON *     root;
    char *      out;
    static char *   pAlarmBuf = NULL;
    static int      alarmBufSize = 0;

    if ((size * nmemb) < 1)
    {
        return 0;
    }

    pAlarmBuf = realloc(pAlarmBuf, ((size * nmemb) + alarmBufSize));
    memcpy(pAlarmBuf + alarmBufSize, ptr, (size * nmemb));
    alarmBufSize += (size * nmemb);
    if (strstr(pAlarmBuf, prID) != NULL)
    {
        root=cJSON_Parse(pAlarmBuf);
		if (root != NULL)
		{
			out=cJSON_PrintUnformatted(root);
			if (out != NULL)
			{
				printf("Alarm List:\n%s\n", out);
				free(out);
			}
			cJSON_Delete(root);
		}
        free(pAlarmBuf);
        pAlarmBuf = NULL;
        alarmBufSize = 0;
    }
    return (size * nmemb);
}

/*!
* point_info_callback:
* \brief
* Curl callback function for Get Multi Point Info method requests.
* \param ptr
* \param size
* \param nmemb
* \param userp
* \return None
*/
size_t
point_info_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	PE2_PARAMS     pE2Req = (PE2_PARAMS)userp;
    cJSON *     root;
    char *      out;

    if ((size * nmemb) < 1)
    {
        return 0;
    }

    root=cJSON_Parse(ptr);
	if (root != NULL)
	{
		parse_object((PE2_PARAMS)pE2Req, root, "resolved", NULL);
		//! DEBUG : printf("INFO(root): %s\n", root);
		if (pE2Req->lastBool == TW_TRUE)
		{
			pE2Req->lastBool = TW_FALSE;
			parse_object((PE2_PARAMS)pE2Req, root, "visible", NULL);
			if (pE2Req->lastBool == TW_TRUE)
			{
                {
                    parse_object((PE2_PARAMS)pE2Req, root, "engUnits",NULL);
                    if (strlen(pE2Req->lastStr) > 0)
                    {
                        out=cJSON_Print(root);
                        if (out!=NULL)
                        {
                            free(out);
                        }
                    }
                }
			}
		}
		cJSON_Delete(root);
	}
    return (size * nmemb);
}

/*!
* config_values_callback:
* \brief
* Curl callback function for Get Config Values method requests.
* \param ptr
* \param size
* \param nmemb
* \param userp
* \return None
*/
size_t
config_values_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	PE2_PARAMS     pE2Req = (PE2_PARAMS)userp;
    cJSON *     root;
    char *      out;

    if ((size * nmemb) < 1)
    {
        return 0;
    }

    root=cJSON_Parse(ptr);
	if (root != NULL)
	{
		parse_object((PE2_PARAMS)pE2Req, root, "value", NULL);
		if (strlen(pE2Req->lastStr) > 0)
		{
			out=cJSON_Print(root);
			if (out != NULL)
			{
				//! DEBUG: printf(",Register Config,\n,,%s", out); <-- ADDITIONAL JSON OUTPUT
				free(out);
			}
		}
		cJSON_Delete(root);
	}
    return (size * nmemb);
}

/*!
* status_callback:
* \brief
* Curl callback function for Get Multi Expanded Status method requests
* \param ptr
* \param size
* \param nmemb
* \param userp
* \return None
*/
size_t
status_callback(
void *ptr,
size_t size,
size_t nmemb,
void *userp)
{
	PE2_PARAMS  pE2Req = (PE2_PARAMS)userp;
	char *      out;
	cJSON *     root;

	if ((size * nmemb) < 1)
	{
		return 0;
	}

	root = cJSON_Parse(ptr);
	if (root != NULL)
	{
		parse_object(pE2Req, root, "value", NULL);
		{
			//printf("%s\n",pE2Req->lastStr);
			strcpy(pE2Req->lastResponse0, (pE2Req->lastStr));
		}
		
		parse_object((PE2_PARAMS)pE2Req, root, "fail", NULL);
		{
			//printf("%s\n",pE2Req->lastStr);
			pE2Req->lastFail = pE2Req->lastBool;
		}
		
		parse_object((PE2_PARAMS)pE2Req, root, "alarm", NULL);
		{
			//printf("%s\n",pE2Req->lastStr);
			pE2Req->lastAlarm = pE2Req->lastBool;
		}
		
		parse_object((PE2_PARAMS)pE2Req, root, "notice", NULL);
		{
			//printf("%s\n",pE2Req->lastStr);
			pE2Req->lastNotice = pE2Req->lastBool;
		}
		
		parse_object((PE2_PARAMS)pE2Req, root, "override", NULL);
		{
			//printf("%s\n",pE2Req->lastStr);
			pE2Req->lastOverride = pE2Req->lastBool;
		}		
		cJSON_Delete(root);
	}
	return (size * nmemb);
}

/*!
* write_callback:
* \brief
* Curl callback function for generic write method requests
* \param ptr
* \param size
* \param nmemb
* \param userp
* \return None
*/
size_t
write_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	UNREFERENCED_PARAMETER(userp);

    if ((size * nmemb) < 1)
    {
        return 0;
    }
    printf("GENERIC, %s\n", (char *)ptr);
    return (size * nmemb);
}

/*!
* main:
* \brief
* Setup and test CURL E2 interface.
* \param argv
* \param argc
* \return status
*/
int
main(int argc, char **argv)
{
	PE2_PARAMS		pE2;
	char *          pParamBuf;
    CURLcode        res;
    unsigned int    t;
    int             instance;
	//! DEBUG:  unsigned int    i =1;
    char            sIPAddr[40];
    int				numCells;
    unsigned int    idx;
    unsigned int    regs;
	char			fileName[E2_MAX_STR_SIZE];

	// Program title, Copyright information.
	fprintf(stderr, "EXXENO(TM) eXXaminer(TM), \n");
	fprintf(stderr, "Emerson E2 Interrogation Utility, Version 1.1,\n");
	fprintf(stderr, "Copyright 2017, All Rights Reserved,\n");
    fprintf(stderr, "Proprietary Information\n\n");

	if (argc > 1)
	{
		//! DEBUG: argv[1] = config filename
#ifndef WIN
		strncpy(fileName, argv[1], E2_MAX_STR_SIZE);
#else // Linux
		strncpy_s(fileName, argv[1], MAX_STR_SIZE);
#endif // Linux | Windows
	}
	else
	{
	    fprintf(stderr, "No Config File Specified! Using default Interrogator.conf \n");
#ifndef WIN
		strncpy(fileName, DefaultFileName, E2_MAX_STR_SIZE);
#else
		strncpy(fileName, DefaultFileName, MAX_STR_SIZE);
#endif // Linux | Windows
	}
	printf("EXXENO(TM) Configuration Polling Utility\n");
	printf("Version=, 1.5\n");
	printf("Conffile=, %s\n", fileName);


	// Allocate E2 Param Buffer
	pE2 = (PE2_PARAMS)malloc(sizeof(E2_PARAMS));
	if (pE2 == NULL)
	{
		return -1;
	}
	memset(pE2, 0, sizeof(E2_PARAMS));

	//
	pParamBuf = (char *)malloc(PARAM_BUFFER_SIZE);
    if (pParamBuf == NULL)
    {
		free(pE2);
        return -2;
    }

    // Don't cache printf output
	setbuf(stdout, NULL);

	// Get Configuration Data for E2
	if (!getE2Config(pE2, 0, (char *)&fileName))
	{
		free(pE2);
		free(pParamBuf);
		fprintf(stderr, "Error: Configuration File Invalid or Not Found.\n");
		return -3;
	}

    if (argc > 2)
    {
	    // IP Address Argument Override
#ifndef WIN
        strncpy((char*)&pE2->IPAddr, argv[2], MAX_IP_ADDR_CHARS);
#else // Windows
        strncpy_s((char*)&pE2->IPAddr, argv[2], MAX_IP_ADDR_CHARS);
#endif // Linux | Windows

	    fprintf(stderr, "Ip Address Override: %s\n", pE2->IPAddr);



    }
    else
    {
        fprintf(stderr,"IP Address: %s\n", pE2->IPAddr);
    }
    curl_global_init(CURL_GLOBAL_ALL);

	pE2->curl = curl_easy_init();
	if (pE2->curl)
    {
    // Verbose cURL output selected.
		curl_easy_setopt(pE2->curl, CURLOPT_VERBOSE, 0L);

		// Establish connection with the E2's web server.
		// Default FSD Port is 14106.
#ifdef WIN
		sprintf_s((char *)&sIPAddr, 40, "http://%s:%s/home.asp", pE2->IPAddr, pE2->FSDPort);
#else
		sprintf((char*)&sIPAddr, "http://%s:%s/home.asp", pE2->IPAddr, pE2->FSDPort);
#endif
//		fprintf(stderr, "request: %s \n", sIPAddr);

        // I.e.: curl_easy_setopt(pE2Req->curl, CURLOPT_URL, "http://192.168.2.91:14106/home.asp");
		curl_easy_setopt(pE2->curl, CURLOPT_URL, sIPAddr);

		// The E2 home.asp is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(pE2->curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(pE2->curl, CURLOPT_HEADER, 0L);
		curl_easy_setopt(pE2->curl, CURLOPT_NOBODY, 0L);
		curl_easy_setopt(pE2->curl, CURLOPT_NOPROGRESS, 1L);

		// Setup our read function
		curl_easy_setopt(pE2->curl, CURLOPT_READFUNCTION, read_callback);
		// Setup our write function
		curl_easy_setopt(pE2->curl, CURLOPT_WRITEFUNCTION, page_callback);

		curl_easy_setopt(pE2->curl, CURLOPT_WRITEDATA, pE2);

		// Perform the request, res will get the return code
		res = curl_easy_perform(pE2->curl);
		// Check for errors
        if (res != CURLE_OK)
        {
            fprintf(stderr, "Error. Cannot Find E2.\n Please Refer to Quick Start Guide and Ensure the Following:\n");
			fprintf(stderr, "\tVerify the IP Address of the E2\n");
			fprintf(stderr, "\tThe interrogation machine is on the same subnet\n");
            fprintf(stderr, "\tThe interrogation machine firewall port is open for port 14106\n");
            fprintf(stderr, "\tThe network cable is plugged in and Link is present\n\n");
			curl_easy_cleanup(pE2->curl);
            curl_global_cleanup();
            free(pParamBuf);
            free(pE2);
            return -1;
        }

		printf("IP Address=, %s,\n", pE2->IPAddr);
#ifndef WIN
		sprintf((char *)&sIPAddr, "http://%s:%s/JSON-RPC?", pE2->IPAddr, pE2->FSDPort);
#else
		sprintf_s((char *)&sIPAddr, 40, "http://%s:%s/JSON-RPC?", pE2->IPAddr, pE2->FSDPort);
#endif
		// Set the target to the E2 JSON RPC handler
		curl_easy_setopt(pE2->curl, CURLOPT_URL, sIPAddr);

		// Get the Controller ands Cell info.
		createPostCmd((PE2_PARAMS)pE2, GET_CONTROLLER_LIST, NULL);
		createPostCmd((PE2_PARAMS)pE2, GET_CELL_LIST, NULL);
		fprintf(stderr, "Emerson E2 Controller Found @ http://%s:%s/home.asp", pE2->IPAddr, pE2->FSDPort);
		printf("\nCellInstance,CellType,InstanceName,\n");
        for (t = 0; t < MAX_CELL_ENTRIES; t++)
        {
			if (pE2->cellList[t].cellType != 0)
            {
                printf("%d,%d,%s,\n",
					t, pE2->cellList[t].cellType, pE2->cellList[t].cellName);
            }
        }

		for (idx = 0; idx < pE2->numActiveRecords; idx++)
        {
			printf("\n");
			printf("\nCellType,%d, Record # %d,\n", pE2->Records[idx].CellType, idx);
			if (pE2->Records[idx].CellType == 0)
			{
				break;
			}
			fprintf(stderr, "\nGetting CellType: %u\n", pE2->Records[idx].CellType);
			regs = 0;
			while (pE2->Records[idx].Register[regs] != 0)
			{
				//! DEBUG: printf("CellType ID,%d,CellType Instance Count,%d,\n,Register Number,%d,", MasterList[idx].CellType, instance, pRegList[regs]);
				//! DEBUG: sprintf("CellType, %d, Instance, %d, \n,Register Number, %d,\n", MasterList[idx].CellType, instance, pRegList->Register[regs], pFile);
				fprintf(stderr, ". %u\n", pE2->Records[idx].Register[regs]);
				instance = 0;
				// There should not be more instances than # records.
				while (instance < MAX_CELL_ENTRIES)
				{
					memset(pParamBuf, 0, PARAM_BUFFER_SIZE);
					if (createParamCmd((PE2_PARAMS)pE2, pParamBuf, pE2->Records[idx].CellType, (unsigned short)pE2->Records[idx].Register[regs], instance) == NULL)
					{
						// Instance not found, go on to the next register.
						break;
					}
					createPostCmd((PE2_PARAMS)pE2, GET_MULTI_POINT_INFO, pParamBuf);
					if (pE2->lastBool == 1)
					{
						printf("\nInstance=,%u,Reg=,%u,", instance, pE2->Records[idx].Register[regs]);
						printf("Info.engUnits=,%s,",pE2->lastStr);
						createPostCmd((PE2_PARAMS)pE2, GET_CONFIG_VALUES, pParamBuf);
						if (strlen((char *)pE2->lastStr) > 0)
						{
							// After createPostCmd for GET_CONFIG_VALUES, print register config.value to stdout.
							printf("Cfg.value=,%s", pE2->lastStr);
							if (createPostCmd((PE2_PARAMS)pE2, GET_MULTI_EXPANDED_STATUS, pParamBuf) == 0)
							{
								// After createPostCmd for GET_MULTI_STATUS, print register status.value to stdout
								printf(",Stat.value=,%s,", pE2->lastStr);
								printf("Stat.override=,%i,", pE2->lastOverride);
								printf("Stat.fail=,%i,", pE2->lastFail);
								printf("Stat.alarm=,%i,", pE2->lastAlarm);
								printf("Stat.notice=,%i,", pE2->lastNotice);
							}
						}
					}
					//printf("%s\n", pE2->lastStr);
					instance++;
				}
                regs++;
            }
        }

		printf("\nInterrogation completed.");
		/* always cleanup */
		curl_easy_cleanup(pE2->curl);
    }
    curl_global_cleanup();
	free(pE2);
    free(pParamBuf);
    return 0;
}

/*!
* createParamCmd:
* \brief
* Build the JSON parameter.
* \param pE2Req
* \param pParamBuf
* \param paramType
* \param E2Reg
* \param instance
* \return result
*/
char *
createParamCmd(
	PE2_PARAMS         pE2Req,
    char *          pParamBuf,
    PARAM_TYPE      paramType,
    unsigned int    E2Reg,
    int             instance
    )
{
    char *          paramStr;
    char *          result = NULL;

	paramStr = findNextCellName((PE2_PARAMS)pE2Req, paramType, instance);
    if (paramStr != NULL)
    {
#ifndef WIN
		sprintf(pParamBuf, "%s:%4u", paramStr, E2Reg);
#else
		sprintf_s(pParamBuf, PARAM_BUFFER_SIZE, "%s:%4u", paramStr, E2Reg);
#endif
        result = pParamBuf + strlen(pParamBuf);
    }
    return result;
}

/*!
* createParamCmdSet:
* \brief
* Build the JSON parameter.
* \param pE2Req
* \param pParamBuf
* \param paramType
* \param E2Reg
* \param instance
* \param setString
* \param setValue
* \return result
*/
char *
createParamCmdSet(
	PE2_PARAMS         pE2Req,
    char *          pParamBuf,
    PARAM_TYPE      paramType,
    unsigned int    E2Reg,
    int             instance,
    char *          setString,
    char *          setValue
    )
{
    char *          paramStr;
    char *          result = NULL;

	paramStr = findNextCellName((PE2_PARAMS)pE2Req, paramType, instance);
    if (paramStr != NULL)
    {
#ifndef WIN
		sprintf(pParamBuf, "%4u\",\"%s:%s", E2Reg, setString, setValue);
#else
		sprintf_s(pParamBuf, PARAM_BUFFER_SIZE, "%4u\",\"%s:%s", E2Reg, setString, setValue);
#endif
        result = pParamBuf + strlen(pParamBuf);
    }
    printf("createParamCmdSet pParamBuf: %s\n", pParamBuf);
    return result;
}

/*!
 * createPostCmd:
 * \brief
 * Creates the full JSON method request based on method type and send it.
 * \param pE2Req
 * \param msgType
 * \param params
 * \return result
 */
int
createPostCmd(
	PE2_PARAMS         pE2Req,
    MSG_TYPE        msgType,
    char *          params
    )
{
    char            postBuf[POST_BUFFER_SIZE];
    CURLcode        res;
    int     result = 0;

    memset(postBuf, 0, POST_BUFFER_SIZE);

    pE2Req->lastBool = TW_NOT_SET;
    pE2Req->lastInt = 0;
    memset(pE2Req->lastResponse0, 0, E2_MAX_STR_SIZE);
    memset(pE2Req->lastResponse1, 0, E2_MAX_STR_SIZE);
    memset(pE2Req->lastResponse2, 0, E2_MAX_STR_SIZE);
    memset(pE2Req->lastResponse3, 0, E2_MAX_STR_SIZE);
    memset(pE2Req->lastResponse4, 0, E2_MAX_STR_SIZE);
    memset(pE2Req->lastResponse5, 0, E2_MAX_STR_SIZE);

    // Always pass back the E2Req structure in all call backs.
    curl_easy_setopt(pE2Req->curl, CURLOPT_WRITEDATA, pE2Req);
    // default call back function
    curl_easy_setopt(pE2Req->curl, CURLOPT_WRITEFUNCTION, write_callback);

    switch (msgType)
    {
        case GET_CONTROLLER_LIST:
            {
                //! GET_CONTROLLER_LIST '{"id":' + %d + ',"method":"E2.GetControllerList","params":[1]}');
                curl_easy_setopt(pE2Req->curl, CURLOPT_WRITEFUNCTION, controller_callback);
#ifndef WIN
				sprintf(postBuf, "%s%u%s", hdID, pE2Req->ReqID, hdGCNTL);
#else
				sprintf_s(postBuf, POST_BUFFER_SIZE, "%s%u%s", hdID, pE2Req->ReqID, hdGCNTL);

#endif
            }
            break;

        case GET_CELL_LIST:
            {
                //! GET_CELL_LIST '{"id":' + %d + ',"method":"E2.GetCellList","params":["' + controllerName + '"]}');
                curl_easy_setopt(pE2Req->curl, CURLOPT_WRITEFUNCTION, cell_list_callback);
#ifndef WIN
				sprintf(postBuf, "%s%u%s%s%s", hdID, pE2Req->ReqID, hdGCL, pE2Req->E2Name, hdsTerm);
#else
				sprintf_s(postBuf, POST_BUFFER_SIZE, "%s%u%s%s%s", hdID, pE2Req->ReqID, hdGCL, pE2Req->E2Name, hdsTerm);
#endif
            }
            break;

        case GET_CONFIG_VALUES:
            {
                //! GET_CONFIG_VALUES '{"id":' + %d + ',"method":"E2.GetConfigValues","params":[[' + str + ']]}');
                curl_easy_setopt(pE2Req->curl, CURLOPT_WRITEFUNCTION, config_values_callback);
#ifndef WIN
				sprintf(postBuf, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdGCV, pE2Req->E2Name, params, hdTerm);
#else
				sprintf_s(postBuf, POST_BUFFER_SIZE, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdGCV, pE2Req->E2Name, params, hdTerm);
#endif
                //! DEBUG: printf("GetConfigValues: %s\n", postBuf);
            }
            break;

        case GET_MULTI_POINT_INFO:
            {
                //! GET_MULTI_POINT_INFO '{"id":' + %d + ',"method":"E2.GetMultiPointInfo","params":[[' + str + ']]}');
                curl_easy_setopt(pE2Req->curl, CURLOPT_WRITEFUNCTION, point_info_callback);
#ifndef WIN
				sprintf(postBuf, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdMul, pE2Req->E2Name, params, hdTerm);
#else
				sprintf_s(postBuf, POST_BUFFER_SIZE, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdMul, pE2Req->E2Name, params, hdTerm);
#endif
            }
            break;

        case GET_MULTI_EXPANDED_STATUS:
            {
                //!  GET_MULTI_EXPANDED_STATUS '{"id":' + %d + ',"method":"E2.GetMultiExpandedStatus","params":[[' + str + ']]}');
                //!  GET_MULTI_EXPANDED_STATUS '{"id":' + %d + ',"method":"E2.GetMultiExpandedStatus","params":[["' + _controllerName + ':ADVISORY SERV:ALARM OUTPUT"]]} ');
                curl_easy_setopt(pE2Req->curl, CURLOPT_WRITEFUNCTION, status_callback);
#ifndef WIN
				sprintf(postBuf, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdMEx, pE2Req->E2Name, params, hdTerm);
#else
				sprintf_s(postBuf, POST_BUFFER_SIZE, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdMEx, pE2Req->E2Name, params, hdTerm);
#endif
            }
            break;

        case OVERRIDE:
            {
                //! OVERRIDE '{"id":' + %d + ',"method":"E2.Override","params":["' + point + '",' + override + ',' + overrideType + ',"' + overrideTime + '","' + overrideValue + '"]}');
                result = -1;
            }
            break;

        case MULTI_OVERRIDE:
            {
                //! MULTI_OVERRIDE '{"id":' + %d + ',"method":"E2.MultiOverride","params":[[' + str + ']]}');
                result = -1;
            }
            break;

        case GET_POINT_INFO:
            {
                //! GET_POINT_INFO '{"id":' + %d + ',"method":"E2.GetPointInfo","params":["' + point + '"]}');
#ifndef WIN
			sprintf(postBuf, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdGPI, pE2Req->E2Name, params, hdTerm);
#else
			sprintf_s(postBuf, POST_BUFFER_SIZE, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdGPI, pE2Req->E2Name, params, hdTerm);
#endif
            }
            break;

        case GET_EXPANDED_STATUS:
            {
                //! GET_EXPANDED_STATUS '{"id":' + %d + ',"method":"E2.GetExpandedStatus","params":["' + point + '"]}');
#ifndef WIN
			sprintf(postBuf, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdGES, pE2Req->E2Name, params, hdTerm);
#else
			sprintf_s(postBuf, POST_BUFFER_SIZE, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdGES, pE2Req->E2Name, params, hdTerm);
#endif

            }
            break;

        case GET_EXPANDED_INFO:
            {
                //! GET_EXPANDED_INFO '{"id":' + %d + ',"method":"E2.GetExpandedInfo","params":["' + point + '"]}');
#ifndef WIN
			sprintf(postBuf, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdGEI, pE2Req->E2Name, params, hdTerm);
#else
			sprintf_s(postBuf, POST_BUFFER_SIZE, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdGEI, pE2Req->E2Name, params, hdTerm);
#endif
            }
            break;

        case GET_LOG_DATA_RAW:
            {
                //! GET_LOG_DATA_RAW '{"id":' + %d + ',"method":"E2.GetLogDataRaw","params":["' + point + '"]}');
#ifndef WIN
			sprintf(postBuf, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdGLDR, pE2Req->E2Name, params, hdTerm);
#else
			sprintf_s(postBuf, POST_BUFFER_SIZE, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdGLDR, pE2Req->E2Name, params, hdTerm);
#endif
            }
            break;

        case GET_ALARM_LIST:
            {
                //! GET_ALARM_LIST '{"id":' + %d + ',"method":"E2.GetAlarmList","params":["' + controller + '", ' + (filter?true:false) + ']}');
                curl_easy_setopt(pE2Req->curl, CURLOPT_WRITEFUNCTION, alarm_list_callback);
#ifndef WIN
				sprintf(postBuf, "%s%u%s%s\", %s%s", hdID, pE2Req->ReqID, hdGAL, pE2Req->E2Name, params, hdnqTerm);
#else
				sprintf_s(postBuf, POST_BUFFER_SIZE, "%s%u%s%s\", %s%s", hdID, pE2Req->ReqID, hdGAL, pE2Req->E2Name, params, hdnqTerm);
#endif
            }
            break;

        case ALARM_ACTION:
            {
                //! ALARM_ACTION '{"id":' + %d + ',"method":"E2.AlarmAction","params":[' + action + ',[' + str + ']]}');
                result = -1;
            }
            break;

        case GET_DEVICE_INFO_FOR_ROUTE:
            {
                //! GET_DEVICE_INFO_FOR_ROUTE '{"id":' + %d + ',"method":"E2.GetDeviceInfoForRoute","params":["' + route + '"]}');
                result = -1;
            }
            break;

        case SET_CONFIG_VALUES:
            {
                //! SET_CONFIG_VALUES '{"id":' + %d + ',"method":"E2.SetConfigValues","params":[[' + str + ']]}');
                curl_easy_setopt(pE2Req->curl, CURLOPT_WRITEFUNCTION, config_values_callback);
#ifndef WIN
				sprintf(postBuf, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdSCV, pE2Req->E2Name, params, hdTerm);
#else
				sprintf_s(postBuf, POST_BUFFER_SIZE, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdSCV, pE2Req->E2Name, params, hdTerm);
#endif

                // DEBUG: printf("SetConfigValues: %s\n", postBuf);
            }
            break;

        case GET_AUX_PROPS:
            {
                //! GET_AUX_PROPS '{"id":' + %d + ',"method":"E2.GetAuxProps","params":["' +point+'",[' + str + ']]}');
                result = -1;
            }
            break;

        case SET_AUX_PROPS:
            {
                //! SET_AUX_PROPS '{"id":' + %d + ',"method":"E2.SetAuxProps","params":["' +point+'",[' + str + ']]}');
                result = -1;
            }
            break;

        case GET_MULTI_AUX_PROPS:
            {
                //! GET_MULTI_AUX_PROPS '{"id":' + %d + ',"method":"E2.GetMultiAuxProps","params":[[' + str + ']]}');
                result = -1;
            }
            break;

        case SET_MULTI_AUX_PROPS:
            {
                //! SET_MULTI_AUX_PROPS '{"id":' + %d + ',"method":"E2.SetMultiAuxProps","params":[['+str+']]}');
                result = -1;
            }
            break;

        case GET_LOGIN_INFO:
            {
                //! GET_LOGIN_INFO '{"id":' + %d + ',"method":"E2.GetLoginInfo","params":[]} ');
#ifndef WIN
				sprintf(postBuf, "%s%u%s%s", hdID, pE2Req->ReqID, hdGLI, hdTerm);
#else
				sprintf_s(postBuf, POST_BUFFER_SIZE, "%s%u%s%s", hdID, pE2Req->ReqID, hdGLI, hdTerm);
#endif
            }
            break;

        case GET_APP_CONFIG:
            {
                //! GET_APP_CONFIG '{"id":' + %d + ',"method":"E2.GetAppConfig","params":["'+app+'"]} ');
                result = -1;
            }
            break;

        case SET_APP_CONFIG:
            {
                //! SET_APP_CONFIG '{"id":' + %d + ',"method":"E2.SetAppConfig","params":["'+app+'","'+cfg+'"]} ');
                result = -1;
            }
            break;

        case GET_BATTERY_STATUS:
            {
                //! GET_BATTERY_STATUS '{"id":' + %d + ',"method":"E2.GetBatteryStatus","params":[]}');
#ifndef WIN
			sprintf(postBuf, "%s%u%s%s", hdID, pE2Req->ReqID, hdGBS, hdTerm);
#else
			sprintf_s(postBuf, POST_BUFFER_SIZE, "%s%u%s%s", hdID, pE2Req->ReqID, hdGBS, hdTerm);
#endif
            }
            break;

        case BATTERY_REPLACED:
            {
                //! BATTERY_REPLACED '{"id":' + %d + ',"method":"E2.BatteryReplaced","params":[]}');

                result = -1;
            }
            break;

        case GET_OUTPUT_POINTERS:
            {
                //! GET_OUTPUT_POINTERS '{"id":' + %d + ',"method":"E2.GetOutputPointers","params":[[' + str + ']]}');
#ifndef WIN
			sprintf(postBuf, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdGOP, pE2Req->E2Name, params, hdTerm);
#else
			sprintf_s(postBuf, POST_BUFFER_SIZE, "%s%u%s%s:%s%s", hdID, pE2Req->ReqID, hdGOP, pE2Req->E2Name, params, hdTerm);
#endif
            }
            break;

        default:
            result = -1;
    }

    if (result == 0)
    {
        curl_easy_setopt(pE2Req->curl, CURLOPT_POSTFIELDS, postBuf);
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(pE2Req->curl);
        /* Check for errors */
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            result = -2;
        }
        pE2Req->ReqID++;
    }
    return result;
}

/*!
 * findNextCellName:
 * \brief
 * Return the cell name indicated by Cell Type or NULL.
 * \param pE2Req
 * \param cellType
 * \param startIndex
 * \return result
 */
char *
findNextCellName(
	PE2_PARAMS     pE2Req,
    int         cellType,
    int         startIndex
    )
{
    int     t;
    int     foundCount = 0;

    for (t = 0; t < MAX_CELL_ENTRIES; t++)
    {
        if (cellType == pE2Req->cellList[t].cellType)
        {
            if (foundCount == startIndex)
            {
                return pE2Req->cellList[t].cellName;
            }
            foundCount++;
        }
    }
    return NULL;
}

/*!
 * getNumCellsList:
 * \brief Return the number of the same cell types found in the Cell List from the
 * \param pE2Req - E2 Program context
 * \param   CellType - Target Cell Type to count
 * \return Number of 'Cell Type"s found in the Cell List from the E2.
 */
 int
getNumCellsList(
	PE2_PARAMS     pE2Req,
    int		CellType)
{
    int     t;
    int     foundCount = 0;

    for (t = 0; t < MAX_CELL_ENTRIES; t++)
    {
        if (CellType == pE2Req->cellList[t].cellType)
        {
            foundCount++;
        }
    }
    return foundCount;
}
