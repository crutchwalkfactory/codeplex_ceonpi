//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//

#include <windows.h>
#include <image_cfg.h>
#include <bcm2835.h>
#include <nkintr.h>
//#include "debugfuncs.h"

BOOL Serial_OEMDebugInit (void);

void OEMWriteDebugByte_Serial(BYTE ch);

OEMWRITEDEBUGBYTPROC *pWriteCharFunc = 0;
// ---------------------------------------------------------------------------
// OEMDebugInit: REQUIRED
//
BOOL Serial_OEMDebugInit (void)
{
	// UART initialized in startup.s
  
	// default is to use the serial port for debug.
	pWriteCharFunc = &OEMWriteDebugByte_Serial;

	return TRUE;
}

// ---------------------------------------------------------------------------
// OEMWriteDebugString: REQUIRED
//
void OEMWriteDebugString(unsigned short *str)
{
	while (*str)
		OEMWriteDebugByte((unsigned char)*str++);
	return;
}

// ---------------------------------------------------------------------------
// OEMWriteDebugByte_Serial - Original serial port version of this function
//
void OEMWriteDebugByte_Serial(BYTE ch)
{
	volatile PDWORD pUARTFlags = (PDWORD)UART0_PA_FR;
	PDWORD pUARTData = (PDWORD)UART0_PA_DR;

	// Wait for the transmit Fifo to have room.
	while ((*pUARTFlags & FR_TXFF) != 0)
		;
	// Write the data
	*pUARTData = ch;
	return;
}

// ---------------------------------------------------------------------------
// OEMReadDebugByte: OPTIONAL
//
// This function reads a byte from the debug peripheral that was initialized
// in OEMDebugInit.  It is optional with respect to the BLCommon core but if
// any user interaction is desired, it is required.  The most common user
// interaction is through a menu displayed to the user over the debug
// peripheral.
//
int OEMReadDebugByte_Serial (void)
{
	BYTE ch;

	volatile PDWORD pUARTFlags = (PDWORD)UART0_PA_FR;
	PDWORD pUARTData = (PDWORD)UART0_PA_DR;

	// Wait for the transmit Fifo to have data.
	while ((*pUARTFlags & FR_RXFE) != 0)
		;
	// Read the data
	ch = (BYTE)*pUARTData;
	return (int) ch;
}

// ---------------------------------------------------------------------------
// OEMWriteDebugByte: REQUIRED
//
void OEMWriteDebugByte(BYTE ch)
{
	if (pWriteCharFunc)
		pWriteCharFunc(ch);
	return;
}
// ---------------------------------------------------------------------------
// OEMWriteDebugLED - 
// 
// I'm using the serial port here so it's slow.  However its designed to
// not use  any statically defined vars so I can use it for early debugging.
//
void QuickDebugOut(char ch);

void OEMWriteDebugLED(WORD wIndex, DWORD dwPattern)
{
	CHAR ch;
	int i;

	for (i = 0; i < 8; i++)
	{
		ch = (CHAR)((dwPattern & 0xf0000000) >> 28);
		if (ch > 9)
			ch += ('A' - 10);
		else 
			ch += '0';
		QuickDebugOut(ch);

		dwPattern = dwPattern << 4;
	}
	QuickDebugOut('\r');
	QuickDebugOut('\n');
	return;
}

void DumpDWORD(DWORD dwLbl, DWORD dwPattern)
{
	CHAR ch;
	int i;
	QuickDebugOut('\r');
	QuickDebugOut('\n');


	for (i = 0; i < 4; i++)
	{
		ch = (CHAR)((dwLbl & 0xff000000) >> 24);
		QuickDebugOut(ch);

		dwLbl = dwLbl << 8;
	}
	QuickDebugOut(':');
	OEMWriteDebugLED(0, dwPattern);
	return;
}
