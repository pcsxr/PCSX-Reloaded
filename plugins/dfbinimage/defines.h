/************************************************************************

defines.h

Copyright (C) 2007 Virus
Copyright (C) 2002 mooby

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

************************************************************************/

#pragma warning(disable:4786)


#ifndef __DEFINES_H
#define __DEFINES_H

#if defined _WINDOWS || defined __CYGWIN32__
#include <windows.h>
#else
#define CALLBACK
#endif

#include <stdint.h>

// PSEMU DEFINES

// header version
#define _PPDK_HEADER_VERSION		1

#define PLUGIN_VERSION				1

// plugin type returned by PSEgetLibType (types can be merged if plugin is multi type!)
#define PSE_LT_CDR					1

// every function in DLL if completed sucessfully should return this value
#define PSE_ERR_SUCCESS				0
// undefined error but fatal one, that kills all functionality
#define PSE_ERR_FATAL				-1

// XXX_Init return values
// Those return values apply to all libraries

// initialization went OK
#define PSE_INIT_ERR_SUCCESS		0

// this driver is not configured
#define PSE_INIT_ERR_NOTCONFIGURED	-2

// this driver can not operate properly on this hardware or hardware is not detected
#define PSE_INIT_ERR_NOHARDWARE		-3


/*         CDR PlugIn          */

//	CDR_Test return values

// sucess, everything configured, and went OK.
#define PSE_CDR_ERR_SUCCESS 0

// ERRORS
#define PSE_CDR_ERR -40
// this driver is not configured
#define PSE_CDR_ERR_NOTCONFIGURED	PSE_CDR_ERR - 0
// if this driver is unable to read data from medium
#define PSE_CDR_ERR_NOREAD			PSE_CDR_ERR - 1

// WARNINGS
#define PSE_CDR_WARN 40
// if this driver emulates lame mode ie. can read only 2048 tracks and sector header is emulated
// this might happen to CDROMS that do not support RAW mode reading - surelly it will kill many games
#define PSE_CDR_WARN_LAMECD			PSE_CDR_WARN + 0






//  FPSE DEFINES

// Return values (should be applied to the entire code).
#define FPSE_OK         0
#define FPSE_ERR        -1
#define FPSE_WARN       1

typedef int8_t INT8;
typedef int16_t   INT16;

typedef uint8_t      UINT8;
typedef uint16_t UINT16;

#if !defined _WINDOWS && !defined __CYGWIN32__
typedef int32_t    INT32;
typedef uint32_t UINT32;
#endif

#define	INT64 int64_t

#define FPSE_CDROM  5


// New MDEC from GPU plugin.
typedef struct {
    int     (*MDEC0_Read)();
    int     (*MDEC0_Write)();
    int     (*MDEC1_Read)();
    int     (*MDEC1_Write)();
    int     (*MDEC0_DmaExec)();
    int     (*MDEC1_DmaExec)();
} MDEC_Export;

#if defined _WINDOWS || defined __CYGWIN32__
// Main Struct for initialization
typedef struct {
    UINT8        *SystemRam;   // Pointer to the PSX system ram
    UINT32        Flags;       // Flags to plugins
    UINT32       *IrqPulsePtr; // Pointer to interrupt pending reg
    MDEC_Export   MDecAltern;  // Use another MDEC engine
    int         (*ReadCfg)();  // Read an item from INI
    int         (*WriteCfg)(); // Write an item to INI
    void        (*FlushRec)(); // Tell where the RAM is changed
    HWND          HWnd;        // Window handle
    HINSTANCE     HInstance;
} FPSEWin32;

// cdr stat struct
struct CdrStat
{
 unsigned long Type;
 unsigned long Status;
 unsigned char Time[3]; // current playing time
};

#endif

// Main Struct for initialization
typedef struct {
    UINT8        *SystemRam;   // Pointer to the PSX system ram
    UINT32        Flags;       // Flags to plugins
    UINT32       *IrqPulsePtr; // Pointer to interrupt pending reg
    MDEC_Export   MDecAltern;  // Use another MDEC engine
    int         (*ReadCfg)();  // Read an item from INI
    int         (*WriteCfg)(); // Write an item to INI
    void        (*FlushRec)(); // Tell where the RAM is changed
} FPSElinux;

// Info about a plugin
typedef struct {
    UINT8   PlType;             // Plugin type: GPU, SPU or Controllers
    UINT8   VerLo;              // Version High
    UINT8   VerHi;              // Version Low
    UINT8   TestResult;         // Returns if it'll work or not
    char    Author[64];         // Name of the author
    char    Name[64];           // Name of plugin
    char    Description[1024];  // Description to put in the edit box
} FPSEAbout;



/* PS2 defines */

#if defined(_WINDOWS)

typedef __int8  s8;
typedef __int16 s16;
typedef __int32 s32;
typedef __int64 s64;

typedef unsigned __int8  u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

#elif defined(__LINUX__)

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#endif

typedef struct { // NOT bcd coded
	u8 minute;
	u8 second;
	u8 frame;
} cdvdLoc;

typedef struct {
	u8 strack;
	u8 etrack;
} cdvdTN;

#define PS2E_LT_CDVD 0x8


#endif  //__DEFINES_H
