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
#include <oemglobal.h>

#include <bsp.h> 
#include <nkintr.h>
#include "vidconsole.h"
#include "video.h"

// Uncommend the below equate to send the pre-kitl debug output to the display.
// This can be set with the build environment var BSP_USEVIDASDEBUGPORT=1
//#define VIDEO_DEBUG_OUT  

void OEMWriteDebugByte_Serial(BYTE ch);

OEMWRITEDEBUGBYTPROC *pWriteCharFunc = 0;

int OEMReadDebugByteDummy(void);
void Debug(LPCTSTR szFormat, ...);


//------------------------------------------------------------------------------
// Globals
//
PBYTE g_p2835Regs;

//------------------------------------------------------------------------------
// Local Variables
//
volatile PDWORD pUARTFlags = 0;
volatile PDWORD pUARTData = 0;

volatile PDWORD pGPIOSetReg0 = 0;
volatile PDWORD pGPIOClrReg0 = 0;


// ---------------------------------------------------------------------------
// OEMInitDebugSerial: REQUIRED
//
// This function initializes the debug serial port on the target device,
// useful for debugging OAL bringup.
//
// This is the first OAL function that the kernel calls, before the OEMInit
// function and before the kernel data section is fully initialized.
//
// OEMInitDebugSerial can use global variables; however, these variables might
// not be initialized and might subsequently be cleared when the kernel data
// section is initialized.
//
void OEMInitDebugSerial(void)
{
	DWORD dwSurf;

	// Map in the SOC registers.  We don't do this in the
	g_p2835Regs = (PBYTE)NKCreateStaticMapping((DWORD)PERIPHERAL_PA_BASE>>8,PERIPHERAL_PA_SIZE);

	if (g_p2835Regs == 0)
		return;

	// Save ptrs to the regs we need
	pUARTFlags = (PDWORD) (g_p2835Regs + UART0_BASEOFF + UART0_OFF_FR);
	pUARTData = (PDWORD)  (g_p2835Regs + UART0_BASEOFF + UART0_OFF_DR);

	// These are used to toggle the LED on GPIO 16
	pGPIOSetReg0 = (PDWORD) (g_p2835Regs + GPIO_BASEOFF + GPSET0_OFFSET);
	pGPIOClrReg0 = (PDWORD) (g_p2835Regs + GPIO_BASEOFF + GPCLR0_OFFSET);

#ifndef SERIAL_KITL
	OEMWriteDebugString(L"OEMInitDebugSerial++\r\n");
	Debug (L"g_p2835Regs:%x\r\n", g_p2835Regs);
#endif
	
#ifdef VIDEO_DEBUG_OUT
	//
    // Redirect the debug serial to the console so I can use 
    // the serial port for KITL.
    //
	if (VidCON_InitDebugSerial (TRUE) == 0)
	{
		Debug (L"VidCon init good, Redirecting debugoutput to screen\r\n");

		//VidCON_WriteDebugString (L"Video out test string.\r\n");

		// default is to use the serial port for debug.
		pWriteCharFunc = &VidCON_WriteDebugByteExtern;

		Debug (L"Debug Out Redirected to screen... Res: %dx%d %dbpp\r\n\r\n", VID_WIDTH, VID_HEIGHT, VID_BITSPP);
	}
#else  //VIDEO_DEBUG_OUT

	// Initialize the video system
	if (InitVideoSystem (VID_WIDTH, VID_HEIGHT, VID_BITSPP, &dwSurf, TRUE) == 0)
	{
		Debug (L"Video init good. \r\n");
	}
	else
		Debug (L"Video init failed\r\n");

#endif //VIDEO_DEBUG_OUT

	return;
}
// ---------------------------------------------------------------------------
// OEMWriteDebugByte_Serial: 
//
// This function outputs a byte to the debug monitor port.
//
void OEMWriteDebugByte_Serial(BYTE bChar)
{
	// Make sure we've got the regs ptr
	if (g_p2835Regs == 0)
		return;

// We don't want any debug serial output on the UART if the build is
// using the same UART for KITL support.
#ifdef SERIAL_KITL
	return;
#endif

	// Wait for the transmit Fifo to have room.
	while ((*pUARTFlags & FR_TXFF) != 0)
		;
	// Write the data
	*pUARTData = bChar;

  return;
}

// ---------------------------------------------------------------------------
// OEMWriteDebugByte: REQUIRED	
//
// This function outputs a byte to the debug monitor port.
//
void OEMWriteDebugByte(BYTE bChar)
{
	if (pWriteCharFunc)
		pWriteCharFunc(bChar);
	else
		OEMWriteDebugByte_Serial(bChar);

  return;
}

// ---------------------------------------------------------------------------
// OEMWriteDebugString: REQUIRED
//
// This function writes a byte to the debug monitor port.
//
void OEMWriteDebugString(LPWSTR pszStr)
{

	while (*pszStr != L'\0') OEMWriteDebugByte((UINT8)*pszStr++);
	return;
}

// ---------------------------------------------------------------------------
// OEMReadDebugByteDummy
//
// This function retrieves a byte to the debug monitor port.
//
int OEMReadDebugByteDummy(void)
{
	int ch = OEM_DEBUG_READ_NODATA;
	return ch;
}
// ---------------------------------------------------------------------------
// OEMReadDebugByte: OPTIONAL
//
// This function retrieves a byte to the debug monitor port.
//
int OEMReadDebugByteSerial(void)
{
	int ch = OEM_DEBUG_READ_NODATA;
	
#ifdef SERIAL_KITL
	return OEM_DEBUG_READ_NODATA;
#endif

	// Make sure we've got the regs ptr
	if (g_p2835Regs != 0)
	{
		// Wait for the transmit Fifo to have data.
		while ((*pUARTFlags & FR_RXFE) != 0)
			;

		// Read the data
		ch = (BYTE)*pUARTData;
	}
	return ch;
}

// ---------------------------------------------------------------------------
// OEMReadDebugByte: OPTIONAL
//
// This function retrieves a byte to the debug monitor port.
//
int OEMReadDebugByte(void)
{
	return OEMReadDebugByteSerial();
}

// ---------------------------------------------------------------------------
// OEMWriteDebugLED: OPTIONAL
//
// This function outputs a byte to the target device's specified LED port.
//
void OEMWriteDebugLED(WORD wIndex, DWORD dwPattern)
{
	// Make sure we've got the regs ptr
	if (g_p2835Regs != 0)
	{
		// Set or clear the bit depending on the LSB
		if (dwPattern & 0x01)
		{
			*pGPIOSetReg0 = GPIO16_R0_BIT;
		}
		else
		{
			*pGPIOClrReg0 = GPIO16_R0_BIT;
		}
	}
	return;
}
// ---------------------------------------------------------------------------
// Debug: 
//
// This function outputs a byte to the target device's specified LED port.
//
void Debug(LPCTSTR szFormat, ...)
{
    TCHAR   szBuffer[1024];
    va_list pArgs;

    va_start(pArgs, szFormat);

	//NKwvsprintfW (szBuffer, szFormat, va_list pArgList, int maxChars) 
	NKwvsprintfW (szBuffer, szFormat, pArgs, 1024);

    va_end(pArgs);

    OEMWriteDebugString(szBuffer);
}


