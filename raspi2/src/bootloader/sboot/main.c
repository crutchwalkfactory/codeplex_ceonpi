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
/*++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Module Name:  
    main.c
    
Abstract:  
    Serial boot loader main module. This file contains the C main
    for the boot loader.    NOTE: The firmware "entry" point (the real
    entry point is _EntryPoint in init assembler file.

    The Windows CE boot loader is the code that is executed on a Windows CE
    development system at power-on reset and loads the Windows CE
    operating system. The boot loader also provides code that monitors
    the behavior of a Windows CE platform between the time the boot loader
    starts running and the time the full operating system debugger is 
    available. Windows CE OEMs are supplied with sample boot loader code 
    that runs on a particular development platform and CPU.
    
Functions:


Notes: 

--*/
#include <windows.h>
#include <ethdbg.h>
#include <nkintr.h>

#include <wdm.h>
#include <ceddk.h>
#include <pehdr.h>
#include <romldr.h>
#include "blcommon.h"
#include "oal_blserial.h"

#include <image_cfg.h>
#include <bcm2835.h>

#include "VidConsole.h"

#define PLATFORM_STRING "RASP"

// OS launch function type
typedef void (*PFN_LAUNCH)();

// prototypes
extern BOOL  SerialReadData(DWORD cbData, LPBYTE pbData);
extern BOOL  SerialSendBlockAck(DWORD uBlockNumber);
extern BOOL  SerialSendBootRequest(const char * platformString);
extern DWORD SerialWaitForJump(VOID);
extern BOOL  SerialWaitForBootAck(BOOL *pfJump);

BOOL Serial_OEMDebugInit (void);
extern OEMWRITEDEBUGBYTPROC *pWriteCharFunc;

// Define version of loader
#define VERSION_RELEASE    "0.30"

// Signon message
unsigned char *szSignOn = "\r\nSerial bootloader for Raspberry Pi.\n\r" 
	"Version: " VERSION_RELEASE "\n\rBuilt " __DATE__" " __TIME__ \
    "\n\rCopyright (c) 2012 Boling Consulting Inc.\n\r";


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void
SpinForever(void)
{
    KITLOutputDebugString("SpinForever...\r\n");
    while(1);
}

//------------------------------------------------------------------------------
// OEMDebugInit 
//
BOOL OEMDebugInit (void)
{
	Serial_OEMDebugInit ();

	//
    // Redirect the debug serial to the console so I can use 
    // the serial port for KITL.
    //
	VidCON_InitDebugSerial (FALSE);
	pWriteCharFunc = &VidCON_WriteDebugByteExtern;

    KITLOutputDebugString("OEMDebugInit--\r\n", __DATE__);

    return TRUE;
}

//------------------------------------------------------------------------------
//
//
BOOL OEMPlatformInit (void)
{    
    extern void BootUp (void);
    KITLOutputDebugString(szSignOn);

    KITLOutputDebugString("OEMPlatformInit--\n");
    
    return TRUE;
}

//------------------------------------------------------------------------------
//
//
DWORD OEMPreDownload (void)
{   
    BOOL fGotJump = FALSE;
    const char * platformString = PLATFORM_STRING;

    // send boot requests indefinitely
    do
    {
        KITLOutputDebugString("Sending boot request...\r\n");
        if(!SerialSendBootRequest(platformString))
        {
            KITLOutputDebugString("Failed to send boot request\r\n");
            return BL_ERROR;
        }
    }
    while(!SerialWaitForBootAck(&fGotJump));

    // ack block zero to start the download
    SerialSendBlockAck(0);

    KITLOutputDebugString("Recvd boot request ack... starting download\r\n");

    return fGotJump  ? BL_JUMP : BL_DOWNLOAD;
}

//------------------------------------------------------------------------------
void OEMLaunch (DWORD dwImageStart, DWORD dwImageLength, DWORD dwLaunchAddr, const ROMHDR *pRomHdr)
//------------------------------------------------------------------------------
{    
    KITLOutputDebugString ("Download successful! Jumping to image at %Xh...\r\n", dwLaunchAddr);
    //// wait for jump packet indefinitely
    //pBootArgs->KitlTransport = (WORD)SerialWaitForJump();
    ((PFN_LAUNCH)(dwLaunchAddr))();

    SpinForever ();
}

//------------------------------------------------------------------------------
// Call into common serial code to read serial packet
//
BOOL OEMReadData (DWORD cbData, LPBYTE pbData)
{
    //KITLOutputDebugString("RD %d p%x\r\n", cbData, pbData);
    return SerialReadData(cbData, pbData);
}

//------------------------------------------------------------------------------
// Memory mapping related functions
//
LPBYTE OEMMapMemAddr (DWORD dwImageStart, DWORD dwAddr)
{
    // map address into physical address
    return (LPBYTE) (dwAddr & ~0x80000000);
}

//------------------------------------------------------------------------------
// Pi doesn't have FLASH, LED, stub the related functions
//
void OEMShowProgress (DWORD dwPacketNum)
{
}

BOOL OEMIsFlashAddr (DWORD dwAddr)
{
    return FALSE;
}

BOOL OEMStartEraseFlash (DWORD dwStartAddr, DWORD dwLength)
{
    return FALSE;
}

void OEMContinueEraseFlash (void)
{
}

BOOL OEMFinishEraseFlash (void)
{
    return FALSE;
}

BOOL OEMWriteFlash (DWORD dwStartAddr, DWORD dwLength)
{
    return FALSE;
}

////------------------------------------------------------------------------------
//BYTE CMOS_Read( BYTE offset )
//{
//    BYTE cAddr, cResult;
//    
//    // Remember, we only change the low order 5 bits in address register
//    cAddr = _inp( CMOS_ADDR );
//    _outp( CMOS_ADDR, (cAddr & RTC_ADDR_MASK) | offset );    
//    cResult = _inp( CMOS_DATA );
//    
//    return (cResult);
//}

BOOL IsTimeEqual(LPSYSTEMTIME lpst1, LPSYSTEMTIME lpst2) 
{
    if (lpst1->wYear != lpst2->wYear)           return(FALSE);
    if (lpst1->wMonth != lpst2->wMonth)         return(FALSE);
    if (lpst1->wDayOfWeek != lpst2->wDayOfWeek) return(FALSE);
    if (lpst1->wDay != lpst2->wDay)             return(FALSE);
    if (lpst1->wHour != lpst2->wHour)           return(FALSE);
    if (lpst1->wMinute != lpst2->wMinute)       return(FALSE);
    if (lpst1->wSecond != lpst2->wSecond)       return(FALSE);

    return (TRUE);
}

//------------------------------------------------------------------------------
//  OEMGetSecs
//
//  Return a count of seconds from some arbitrary time (the absolute value 
//  is not important, so long as it increments appropriately).
//
//------------------------------------------------------------------------------
DWORD
OEMGetSecs_my()
{
    //SYSTEMTIME st;
    //Bare_GetRealTime( &st );
    //return ((60UL * (60UL * (24UL * (31UL * st.wMonth + st.wDay) + st.wHour) + st.wMinute)) + st.wSecond);
	return 1234;
}

#define TIMEOUT_RECV    3 // seconds

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL OEMSerialRecvRaw(LPBYTE pbFrame, PUSHORT pcbFrame, BOOLEAN bWaitInfinite)
{
    USHORT ct = 0;

	volatile PDWORD pUARTFlags = (PDWORD)UART0_PA_FR;
	volatile PDWORD pUARTCtlReg = (PDWORD)UART0_PA_CR;
	volatile PDWORD pUARTData = (PDWORD)UART0_PA_DR;
	
	// Loop through buffer		
    for(ct = 0; ct < *pcbFrame; ct++)
    {
        //if (!bWaitInfinite)
        //{
        //    KITLOutputDebugString("rcv_to\r\n", uStatus);
        //}

		while ((*pUARTFlags & FR_RXFE) != 0)
        {
			;
            //if(!bWaitInfinite && (OEMGetSecs() - tStart > TIMEOUT_RECV))
            //{
            //    *pcbFrame = 0;
            //    WRITE_PORT_UCHAR(DlIoPortBase+comModemControl, (UCHAR)(uCtrl));
            //    return FALSE;
            //}            
        }
		// Read the byte into the buffer
        *(pbFrame + ct) = (BYTE)*pUARTData;

    }
    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL OEMSerialSendRaw(LPBYTE pbFrame, USHORT cbFrame)
{
    UINT ct;
    // block until send is complete; no timeout
    for(ct = 0; ct < cbFrame; ct++)
    {            
    //    // check that send transmitter holding register is empty
    //    while(!(READ_PORT_UCHAR(DlIoPortBase+comLineStatus) & LS_THR_EMPTY))
    //        (VOID)NULL;

    //    // write character to port
    //    WRITE_PORT_UCHAR(DlIoPortBase+comTxBuffer, (UCHAR)*(pbFrame+ct));

		volatile PDWORD pUARTFlags = (PDWORD)UART0_PA_FR;
		PDWORD pUARTData = (PDWORD)UART0_PA_DR;

		// Wait for the transmit Fifo to have room.
		while ((*pUARTFlags & FR_TXFF) != 0)
			;
		// Write the data
		*pUARTData = (UCHAR)*(pbFrame+ct);
    }
    
    return TRUE;
}

//------------------------------------------------------------------------------
// BootPAtoVA - Physical Addr to Virtual addr conversion. 
// For bootloader, this is an identity mapping.
//
VOID* OALPAtoVA (UINT32 pa, BOOL cached)
{
    UNREFERENCED_PARAMETER(cached);
    return (VOID*)pa;
}

//------------------------------------------------------------------------------
// BootVAtoPA - Virtual addr to Physical addr conversion. 
// For bootloader, this is an identity mapping.
//
UINT32 OALVAtoPA (__in VOID *va)
{
    return (DWORD)va;
}

//------------------------------------------------------------------------------
// BootVAtoPA - Virtual addr to Physical addr conversion. 
// For bootloader, this is an identity mapping.
//
void Debug(LPCTSTR szFormat, ...)
{
	// Uncomment this code only when debugging the video console. Otherwise
	// the early spew out the serial port will confuse Platform Builder.
#if 0
    TCHAR   szBuffer[1024];
    va_list pArgs;

    va_start(pArgs, szFormat);

	//NKwvsprintfW (szBuffer, szFormat, va_list pArgList, int maxChars) 
	NKwvsprintfW (szBuffer, szFormat, pArgs, 1024);

    va_end(pArgs);

    OEMWriteDebugString(szBuffer);
#endif
}
