// -------------------------------------------------------------------------
/*!
    Exxeno Hub - ExxDiscover
    \file       E2.h
    Contains the Exxeno communication hub defines, structs, includes, etc for
    the Emerson E2 Controller
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

#ifndef __E2_H_
#define __E2_H_

#define MAX_E2S                 8		/*!< \brief Maximum E2 controllers supported. */

#define MAX_E2_SLOTS            9999	/*!< \brief Maximum E2 register slots supported. */

#define PARAM_BUFFER_SIZE       512

#define POST_BUFFER_SIZE        512
#define PARAM_BUFFER_SIZE       512

#define REGISTER_TO_READ        10000	/*!< \brief Total E2 registers. */
#define MAX_INSTANCE_COUNT      2048	/*!< \brief Maximum instances allowed of any celltype. */
#define MAX_E2_STR_SIZE			16
#define MAX_E2_LONG_STR_SIZE	42
#define E2_NAME_SIZE			128
#define MAX_CELL_ENTRIES		2048
#define MAX_E2_RECORDS          9999
#define MAX_FSD_ADDR_CHARS		8
#define MAX_STR_ARRAY			15
#ifndef WIN32
#define UNREFERENCED_PARAMETER(userp);
#endif

#define OFILE_BUFFER_SIZE		2048
#define CONFIG_REG_START        0;
#define PARAM_REG_START         0;

// Allocate enough space for 1024 entries.
#define CELL_LIST_BUFFER_SIZE   128000 //65536

/*
    We use the Java RPC calls to communicate with the Emerson E2 controller.
*/
//const char * getVal = "{\"id\":1,\"method\":\"E2.GetConfigValues\",\"params\":[[\"PCW-E2:GENERAL SERV:7336\",\"PCW-E2:GENERAL SERV:7023\"]]}";
//const char * getMulti = "{\"id\":2,\"method\":\"E2.GetMultiPointInfo\",\"params\":[[\"PCW-E2:GLOBAL DATA:2048\",\"PCW-E2:GLOBAL DATA:2049\",\"PCW-E2:GLOBAL DATA:2060\",\"PCW-E2:GLOBAL DATA:2050\",\"PCW-E2:ADVISORY SERV:2048\",\"PCW-E2:GLOBAL DATA:2057\",\"PCW-E2:GLOBAL DATA:2059\"]]}";
//const char * getMultiEx = "{\"id\":3,\"method\":\"E2.GetMultiExpandedStatus\",\"params\":[[\"PCW-E2:DI_SENSOR:2048\",\"PCW-E2:GLOBAL DATA:2049\",\"PCW-E2:GLOBAL DATA:2060\",\"PCW-E2:GLOBAL DATA:2050\",\"PCW-E2:ADVISORY SERV:2048\",\"PCW-E2:GLOBAL DATA:2057\",\"PCW-E2:GLOBAL DATA:2059\"]]}";
//const char * getMultiEx = "{\"id\":3,\"method\":\"E2.GetMultiExpandedStatus\",\"params\":[[\"PCW-E2:GLOBAL DATA:2048\",\"PCW-E2:GLOBAL DATA:2049\",\"PCW-E2:GLOBAL DATA:2060\",\"PCW-E2:GLOBAL DATA:2050\",\"PCW-E2:ADVISORY SERV:2048\",\"PCW-E2:GLOBAL DATA:2057\",\"PCW-E2:GLOBAL DATA:2059\"]]}";
//const char * getTimeDate = "{\"id\":4,\"method\":\"E2.GetConfigValues\",\"params\":[[\"PCW-E2:TIME SERVICES:7000\",\"PCW-E2:TIME SERVICES:7001\"]]}";

#include "curl.h"

/*! \brief MSG_TYPE */
typedef enum {
    GET_CELL_LIST,
    OVERRIDE,
    MULTI_OVERRIDE,
    GET_POINT_INFO,
    GET_MULTI_POINT_INFO,
    GET_EXPANDED_STATUS,
    GET_MULTI_EXPANDED_STATUS,
    GET_EXPANDED_INFO,
    GET_LOG_DATA_RAW,
    GET_ALARM_LIST,
    ALARM_ACTION,
    GET_DEVICE_INFO_FOR_ROUTE,
    GET_CONFIG_VALUES,
    SET_CONFIG_VALUES,
    GET_AUX_PROPS,
    SET_AUX_PROPS,
    GET_MULTI_AUX_PROPS,
    SET_MULTI_AUX_PROPS,
    GET_LOGIN_INFO,
    GET_APP_CONFIG,
    SET_APP_CONFIG,
    GET_BATTERY_STATUS,
    BATTERY_REPLACED,
    GET_OUTPUT_POINTERS,
    GET_CONTROLLER_LIST
} MSG_TYPE;

/*! \brief PARAM_TYPE */
typedef enum {
    HARDWARE_AI     = 1,	HARDWARE_DI		= 2,	HARDWARE_EAI		= 3,
    HARDWARE_EDI    = 4,	HARDWARE_AO     = 32,	HARDWARE_DO			= 33,
	PULSE_OUT		= 36,	ONESHOT_OUT		= 37,	BASE_LOG			= 73,
	CONTROLLER_NAME = 74,	PULSE_ACCUM		= 87,	DEVICE_SUMM			= 89,
	GLOBAL_DATA     = 91,	ANALOG_SENSOR	= 94,	SEQUENCE_SENSOR		= 95,
	DIGITAL_SENSOR  = 96,	CONVERSION_CELL = 97,	LIGHTING_CONTROL	= 98,
	IMPULSE_CTRL	= 99,	BOARD_16AI      = 165,	BOARD_8RO			= 166,
	BOARD_8DO       = 167,	BOARD_4AO       = 168,	NOTE_PAD			= 224,
	ADVISORY_SERV   = 225,	ACCESS_SERVICE  = 226,	GENERAL_SERV		= 227,
	TIME_SERVICES   = 228,	NetworkService  = 229,	NV_HANDLER			= 230,
	REMOTE_DIAL     = 233,	Advisory_Log    = 234,	ACCESS_LOG			= 236,
	OVERRIDE_LOG    = 237,	RXSetupWizard   = 238,	SERVICE_LOG			= 241
} PARAM_TYPE;

/*! \brief POINT_TABLE */
typedef struct _POINTS_TABLE {
    char    pointName[E2_MAX_STR_SIZE];
    char    pointType[E2_MAX_STR_SIZE];
} POINTS_TABLE, *PPOINTS_TABLE;

/*! \brief CELL_TABLE */
typedef struct _CELL_TABLE {
    int             cellType;
    char            cellName[MAX_E2_STR_SIZE];
    char            cellLongName[MAX_E2_LONG_STR_SIZE];
} CELL_TABLE, *PCELL_TABLE;

#if 0 // Pending config file changeover.
/*! \brief REGISTER_TABLE */
typedef struct _REGISTER_TABLE {
    unsigned short        Register[1];
} REGISTER_TABLE, *PREGISTER_TABLE;

/*! \brief CELL_REGISTER_TABLE */
typedef struct _CELL_REGISTER_TABLE {
    int             CellType;
} CELL_REGISTER_TABLE, *PCELL_REGISTER_TABLE;

/*! \brief CELL_REGISTER_TABLE */
typedef struct _CELL_REGISTER_TABLE {
    int             CellType;
    unsigned short  Register[MAX_E2_SLOTS];
} CELL_REGISTER_TABLE, *PCELL_REGISTER_TABLE;
#endif //// Pending config file changeover.

/*! /brief ExxComm E2 Record configuration parameters structure  */
typedef struct _E2_RECORD {
    int 				CellType;                   /*!< E2 Cell Type. */
    int                 NumRegisters;               /*!< E2 Number of Registers for this cell type. */
    unsigned short      Register[MAX_E2_SLOTS];     /*!< E2 Registers */
} E2_RECORD, *PE2_RECORD;

/*! /brief E2 Configuration parameters structure  */
typedef struct _E2_PARAMS	 {
    PCFG_PARAMS         pcfgParams;                  /*!< Pointer back to main program context. 	*/
//	pthread_t	        ThreadID;					 /*!< E2 Poll thread handle. */
    unsigned char       MACAddr[MAX_MAC_ADDR_CHARS]; /*!< E2 MAC Address. */
	unsigned char		FSDPort[MAX_FSD_ADDR_CHARS];
	unsigned char       IPAddr[MAX_IP_ADDR_CHARS];   /*!< E2 IP Address. */
    unsigned int        BaseRecordID;                /*!< Base Record ID for E2 data. */
    unsigned char       sensorID;                    /*!< E2 ID Number. */
    CURL *              curl;                        /*!< CURL handle for JSON requests. */
    unsigned int        ReqID;                       /*!< CURL Request ID. */
    char                E2Name[E2_NAME_SIZE];        /*!< E2 Name to make requests. */
	int                 nextStrArrayIdx;
    threeWay            lastBool;                    /*!< Last boolean value returned from JSON request. */
	threeWay			lastAlarm;
	threeWay			lastOverride;
	threeWay			lastNotice;
	threeWay			lastFail;
	char				arrLogValues[1024];
	double				intLogValueAverage;
    int                 lastInt;                     /*!< Last integer value returned from JSON request. */
	char                lastStr[MAX_STR_ARRAY][E2_MAX_STR_SIZE];  /*!< \brief Last string value returned from JSON request. */
	char				lastOPtr[E2_MAX_STR_SIZE];
	char                lastResponse0[E2_MAX_STR_SIZE];       
	char                lastResponse1[E2_MAX_STR_SIZE];
	char                lastResponse2[E2_MAX_STR_SIZE];
	char                lastResponse3[E2_MAX_STR_SIZE];
	char                lastResponse4[E2_MAX_STR_SIZE];
	char                lastResponse5[E2_MAX_STR_SIZE];
    CELL_TABLE          cellList[MAX_CELL_ENTRIES];  /*!< E2 Cell list table. */
    int                 numActiveRecords;            /*!< Number of active records for this E2. */
    E2_RECORD           Records[MAX_E2_RECORDS];     /*!< Cell/Instance configuration records. */
    int                 Schedule[MAX_E2_RECORDS];    /*!< E2 Polling schedule. */
} E2_PARAMS, *PE2_PARAMS;

typedef void(*parserCallback)(PE2_PARAMS pE2Req, void *);

extern char * pCellListBuf;

boolean getE2Config(PE2_PARAMS pE2, int index, char * fileName);
int parse_object(PE2_PARAMS pE2Req, cJSON * item, char * target, parserCallback  pCB);
size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp);
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userp);
size_t controller_callback(void *ptr, size_t size, size_t nmemb, void *userp);
void cellStrCallback(PE2_PARAMS pE2Req, void * data);
void cellIntCallback(PE2_PARAMS pE2Req, void * data);
size_t cell_list_callback(void *ptr, size_t size, size_t nmemb, void *userp);
size_t alarm_list_callback(void *ptr, size_t size, size_t nmemb, void *userp);
size_t point_info_callback(void *ptr, size_t size, size_t nmemb, void *userp);
size_t config_values_callback(void *ptr, size_t size, size_t nmemb, void *userp);
size_t status_callback(void *ptr, size_t size, size_t nmemb, void *userp);
size_t page_callback(void *ptr, size_t size, size_t nmemb, void *userp);
char * createParamCmd(PE2_PARAMS pE2Req, char * pParamBuf, PARAM_TYPE paramType, unsigned int E2Reg, int instance);
int createPostCmd(PE2_PARAMS pE2Req, MSG_TYPE msgType, char * params);
char * findNextCellName(PE2_PARAMS  pE2Req, int cellType, int startIndex);
int getNumCellsList(PE2_PARAMS  pE2Req, int CellType);

#endif // __E2_H_


