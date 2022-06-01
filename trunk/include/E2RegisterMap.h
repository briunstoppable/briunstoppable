
// -------------------------------------------------------------------------
// 
// PRODUCT:			Exxeno Power Sensor Hub
// MODULE NAME:		E2RegisterMap.h
// 
// MODULE DESCRIPTION: 
// 
// Provides arrays of register values for each of the different applications
// on the Emerson E2 Controller.
// 
// $Revision:  $
//
// ------------------------- CONFIDENTIAL ----------------------------------
// 
//              Copyright (c) 2014 by Exxeno, LLC.    
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

/*
;  
; When utilizing the following register map, first the name of the E2 must be resolved. 
;  
; "\<THIS_E2>:<APPICATION>:<REGISTER_VALUE>\" 
; 
;  
;  
*/

// End of Register List
#define REG_EOL = 32760

// Placeholders
#define INPUT_PH = 00
#define OUTPUT_PH = 2000
#define TIMESTAMP = 0


// GENERAL SERV Register Array
const int GeneralServRegisters	[] =    
{
    7001,   // SITE NAME
    7002,   // SITE PHONE NUM.
    7017,   // E2 TYPE
    5035,   // REFER TYPE
    7028,   // E2 IP ADDR
    7323,   // E2 MAC ADDR
    REG_EOL, // END OF LIST
};

// GLOBAL DATA Register Array                       
const int GlobalDataRegisters	[] =    
{
    1,      // OUTDOOR TEMP. INPUT
    2048,   // OUTDOOR TEMP. OUTPUT
    2,      // OUTDOOR REL. HUM. INPUT
    2049,   // OUTDOOR REL. HUM. OUTPUT
    3,      // INDOOR REL. HUM. INPUT
    2050,   // INDOOR REL. HUM. OUTPUT
    7,      // LIGHT LEVEL INPUT
    2054,   // LIGHT LEVEL OUTPUT
    13,     // CURRENT SEASONAL SETTING
    2074,   // CURRENT DAY OF THE WEEK                                                    
    6,      // ANALOG SPARE INPUT 1
    2053,   // ANALOG SPARE OUTPUT 1
    22,     // ANALOG SPARE INPUT 2
    2076,   // ANALOG SPARE OUTPUT 2
    23,     // ANALOG SPARE INPUT 3
    2077,   // ANALOG SPARE OUTPUT 3
    24,     // ANALOG SPARE INPUT 4
    2078,   // ANALOG SPARE OUTPUT 4
    25,     // ANALOG SPARE INPUT 5
    2079,   // ANALOG SPARE OUTPUT 5
    26,     // ANALOG SPARE INPUT 6
    2080,   // ANALOG SPARE OUTPUT 6
    27,     // ANALOG SPARE INPUT 7
    2081,   // ANALOG SPARE OUTPUT 7
    28,     // ANALOG SPARE INPUT 8
    2082,   // ANALOG SPARE OUTPUT 8                                                     
    INPUT_PH,     // DIGITAL SPARE INPUT 1
    OUTPUT_PH,   // DIGITAL SPARE OUTPUT 1
    INPUT_PH,     // DIGITAL SPARE INPUT 2
    OUTPUT_PH,   // DIGITAL SPARE OUTPUT 2
    INPUT_PH,     // DIGITAL SPARE INPUT 3
    OUTPUT_PH,   // DIGITAL SPARE OUTPUT 3
    INPUT_PH,     // DIGITAL SPARE INPUT 4
    OUTPUT_PH,   // DIGITAL SPARE OUTPUT 4
    INPUT_PH,     // DIGITAL SPARE INPUT 5
    OUTPUT_PH,   // DIGITAL SPARE OUTPUT 5
    INPUT_PH,     // DIGITAL SPARE INPUT 6
    OUTPUT_PH,   // DIGITAL SPARE OUTPUT 6
    INPUT_PH,     // DIGITAL SPARE INPUT 7
    OUTPUT_PH,   // DIGITAL SPARE OUTPUT 7
    INPUT_PH,     // DIGITAL SPARE INPUT 8
    OUTPUT_PH,   // DIGITAL SPARE OUTPUT 8
};

// TIME SERVICES Register Array
const int TimeServiceRegisters	[] = 
{
    6000,       // ??
    6001,       // ??
    6002,       // LONGITUDE
    6003,       // LATTITUDE
    7000,       // CURRENT TIME
    7001,       // CURRENT DATE
    7002,       // DST ENABLED
    7012,       // CURRENT TIME ZONE
    7014,       // ZIPCODE
};

// INPUT POINT (FOR CELL TYPES 1,2) Register Array
const int InputPointRegisters	[] = 
{

};

// OUTPUT POINT (FOR CELL TYPE 31,32) Register Array 
const int OutputPointRegisters	[] = 
{

};

// RELAY POINT (CELL TYPE 33) Register Array 
const int RelayPointRegisters	[] = 
{

};

// HOLIDAYS & CUSTOM DATES -- 
// ** Must use GLOBAL DATA application **
const int SpecialDayRegisters   [] = 
{
    7067,   // HOLIDAY 1 NAME
    7065,   // HOLIDAY 1 START
    7066,   // HOLIDAY 1 END
    7068,   // HOLIDAY 1 ANNUAL ENABLED
            
    7071,   // HOLIDAY 2 NAME
    7069,   // HOLIDAY 2 START
    7070,   // HOLIDAY 2 END
    7072,   // HOLIDAY 2 ANNUAL ENABLED
            
    7075,   // HOLIDAY 3 NAME
    7073,   // HOLIDAY 3 START
    7074,   // HOLIDAY 3 END
    7076,   // HOLIDAY 3 ANNUAL ENABLED
            
    7079,   // HOLIDAY 4 NAME
    7077,   // HOLIDAY 4 START
    7078,   // HOLIDAY 4 END
    7080,   // HOLIDAY 4 ANNUAL ENABLED
            
    7083,   // HOLIDAY 5 NAME
    7081,   // HOLIDAY 5 START
    7082,   // HOLIDAY 5 END
    7084,   // HOLIDAY 5 ANNUAL ENABLED 
               
    7087,   // HOLIDAY 6 NAME
    7085,   // HOLIDAY 6 START
    7086,   // HOLIDAY 6 END
    7088,   // HOLIDAY 6 ANNUAL ENABLED
            
    7091,   // HOLIDAY 7 NAME
    7089,   // HOLIDAY 7 START
    7090,   // HOLIDAY 7 END
    7092,   // HOLIDAY 7 ANNUAL ENABLED
            
    7095,   // HOLIDAY 8 NAME
    7093,   // HOLIDAY 8 START
    7094,   // HOLIDAY 8 END
    7096,   // HOLIDAY 8 ANNUAL ENABLED
            
    7099,   // HOLIDAY 9 NAME
    7097,   // HOLIDAY 9 START
    7098,   // HOLIDAY 9 END
    7100,   // HOLIDAY 9 ANNUAL ENABLED
            
    7103,   // HOLIDAY 10 NAME
    7101,   // HOLIDAY 10 START
    7102,   // HOLIDAY 10 END
    7104,   // HOLIDAY 10 ANNUAL ENABLED
            
            
    7107,   // HOLIDAY 11 NAME
    7105,   // HOLIDAY 11 START
    7106,   // HOLIDAY 11 END
    7108,   // HOLIDAY 11 ANNUAL ENABLED
            
    7111,   // HOLIDAY 12 NAME
    7109,   // HOLIDAY 12 START
    7110,   // HOLIDAY 12 END
    7112,   // HOLIDAY 12 ANNUAL ENABLED


    7202,   // CUSTOM DATE 1 NAME
    7200,   // CUSTOM DATE 1 START
    7201,   // CUSTOM DATE 1 END
    7202,   // CUSTOM DATE 1 ANNUAL ENABLED

    7206,   // CUSTOM DATE 2 NAME
    7203,   // CUSTOM DATE 2 START
    7204,   // CUSTOM DATE 2 END
    7205,   // CUSTOM DATE 2 ANNUAL ENABLED

    7210,   // CUSTOM DATE 3 NAME
    7208,   // CUSTOM DATE 3 START
    7209,   // CUSTOM DATE 3 END
    7211,   // CUSTOM DATE 3 ANNUAL ENABLED

    7214,   // CUSTOM DATE 4 NAME
    7212,   // CUSTOM DATE 4 START
    7213,   // CUSTOM DATE 4 END
    7215,   // CUSTOM DATE 4 ANNUAL ENABLED

    7218,   // CUSTOM DATE 5 NAME
    7216,   // CUSTOM DATE 5 START
    7217,   // CUSTOM DATE 5 END
    7219,   // CUSTOM DATE 5 ANNUAL ENABLED

    7222,   // CUSTOM DATE 6 NAME
    7220,   // CUSTOM DATE 6 START
    7221,   // CUSTOM DATE 6 END
    7223,   // CUSTOM DATE 6 ANNUAL ENABLED

    7226,   // CUSTOM DATE 7 NAME
    7224,   // CUSTOM DATE 7 START
    7225,   // CUSTOM DATE 7 END
    7227,   // CUSTOM DATE 7 ANNUAL ENABLED

    7230,   // CUSTOM DATE 8 NAME
    7228,   // CUSTOM DATE 8 START
    7229,   // CUSTOM DATE 8 END
    7231,   // CUSTOM DATE 8 ANNUAL ENABLED

    7234,   // CUSTOM DATE 9 NAME
    7232,   // CUSTOM DATE 9 START
    7233,   // CUSTOM DATE 9 END
    7235,   // CUSTOM DATE 9 ANNUAL ENABLED

    7238,   // CUSTOM DATE 10 NAME
    7236,   // CUSTOM DATE 10 START
    7237,   // CUSTOM DATE 10 END
    7239,   // CUSTOM DATE 10 ANNUAL ENABLED

    7242,   // CUSTOM DATE 11 NAME
    7240,   // CUSTOM DATE 11 START
    7241,   // CUSTOM DATE 11 END
    7243,   // CUSTOM DATE 11 ANNUAL ENABLED

    7246,   // CUSTOM DATE 12 NAME
    7244,   // CUSTOM DATE 12 START
    7245,   // CUSTOM DATE 12 END
    7247,   // CUSTOM DATE 12 ANNUAL ENABLED




};

// NETWORK SERVICES Register Array
const int NetworkServRegisters  [] =
{

};  

// GetRegularStatusDigitalIn
// For Each Cell Type 1,
const int RegularStatusDI   [] = 
{
    5015,       // BOARD NUM.
    5016,       // POINT NUM.
    // TIMESTAMP FROM CommHub
    1,          // INPUT VALUE
    7001,       // INPUT VALUE UNIT
    REG_EOL     // END OF LINE
};

// GetRegularStatusAI
// For Each Cell Type 2
const int RegularStatusAI   [] =
{
    5015,       // BOARD NUM.
    5016,       // POINT NUM.
    // TIMESTAMP FROM HUB
    2048,       // INPUT VALUE
    7001,       // INPUT VALUE UNIT
    REG_EOL     // END OF LINE
};
//  
// GetRegularStatusAO
// For Each Cell Type 31
const int RegularStatusAO   [] =
{
    5015,       // BOARD NUM.
    5016,       // POINT NUM.
    // TIMESTAMP FROM HUB
    1,          // INPUT VALUE
    7001,       // INPUT VALUE UNIT
    REG_EOL     // END OF LINE
};
// GetRegularStatusDO
// For Each Cell Type 32,
const int RegularStatusDO   [] =
{
    5015,       // BOARD NUM.
    5016,       // POINT NUM.
    // TIMESTAMP FROM HUB
    1,          // INPUT VALUE
    7001,       // INPUT VALUE UNIT
    REG_EOL     // END OF LINE
};
// GetRegularStatusRO
// For Each Cell Type 33,
const int RegularStatusRO   [] =
{
    5015,       // BOARD NUM.
    5016,       // POINT NUM.
    // TIMESTAMP FROM HUB
    1,          // INPUT VALUE
    7001,       // INPUT VALUE UNIT
    REG_EOL     // END OF LINE
};
 

