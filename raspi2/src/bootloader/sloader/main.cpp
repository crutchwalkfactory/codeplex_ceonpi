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
//#include <windows.h>
#include <boottypes.h>
#include <nkintr.h>
#include <pehdr.h>
#include <romldr.h>
#include <bcm2835.h>
#include "image_cfg.h"

#include "debugfuncs.h"
#include "vidconsole.h"

// Function defines
extern "C" {
int DrawSplash (int x, int y, char *szFileName);
BOOL SetupCopySection(ROMHDR *pTOC);
void SetOKLED();
//typedef void (*PFN_IMAGE_LAUNCH)();
int NKwvsprintfW(LPWSTR szBuffer, LPCWSTR szFormat, va_list pArgList, int maxChars);
}

//
// Globals variables.
//
extern "C" ROMHDR * volatile const pTOC = (ROMHDR *)-1;

extern "C" OEMWRITEDEBUGBYTPROC *pWriteCharFunc;

// Define version of loader
#define VERSION_RELEASE    TEXT("0.20")

// Signon message
unsigned short *szSignOn =
    TEXT ("\r\nBoot Test for Raspberry Pi.\n\r")
	TEXT ("Version: ") VERSION_RELEASE TEXT("\n\rBuilt ")
	TEXT(__DATE__)TEXT(" ")TEXT (__TIME__)
    TEXT ("\n\rCopyright (c) 2012  Boling Consulting Inc.\n\r");


//#define DEBUGMSG(
#define dim(a) (sizeof(a)/sizeof(a[0]))

int PropTest();


#define  RASPI_PROP_FIRMWAREVERSION     0
#define  RASPI_PROP_GETBOARDMODEL       1
#define  RASPI_PROP_GETBOARDREVISION    2
#define  RASPI_PROP_GETMACADDRESS       3
#define  RASPI_PROP_GETBOARDSERIALNUM   4
#define  RASPI_PROP_GETARMMEMORYSIZE    5
#define  RASPI_PROP_GETVCMEMORYSIZE     6
#define  RASPI_PROP_GETCMDLINE          7
#define  RASPI_PROP_GETDMACHANS         8

int GetRASPIProperty (int idProperty, PBYTE pDataBuff, DWORD dwBuffSize, PDWORD pdwBytesReturned);


int aaa;

int MyParseString (char **ppStr, char **ppName, char **ppVal)
{
	char *pEnd, *pStart, *pVal;
	char *pTok, ch;

	pTok = *ppStr;

	//Move to the name/value pair.
	for (; (*pTok == ' ') && (*pTok != '\0'); pTok++);
	// See if at end of string
	if (*pTok == '\0')
	{
		*ppStr = pTok;
		return 0;
	}

	pStart = pTok;
	// Move to end of the name value pair
	for (; *pTok != ' ' && (*pTok != '\0'); pTok++);
	pEnd = pTok;
	ch = *pEnd;
	*pEnd = '\0';

	// Find the = to separate the name from the value
	for (pVal = pStart; *pVal != '=' && (*pVal != '\0'); pVal++);
	if (pVal != pEnd)
		*pVal++ = '\0';

	*ppName = pStart;
	*ppVal = pVal;

	//Debug (TEXT("Name  >%S<\r\n"), (char *)pStart);
	//Debug (TEXT("Value >%S<\r\n\r\n"), (char *)pVal);

	if (ch == '\0')
	{
		*ppStr = pEnd;
	}
	*ppStr = pEnd+1;
	return 1;
}


//----------------------------------------------------------------------
// Called from Startup.s
//----------------------------------------------------------------------
void main(void)
{
	int zz;
	BYTE bBuff[256];
#define  RASPI_PROP_FIRMWAREVERSION     0
#define  RASPI_PROP_GETBOARDMODEL       1
#define  RASPI_PROP_GETBOARDREVISION    2
#define  RASPI_PROP_GETMACADDRESS       3
#define  RASPI_PROP_GETBOARDSERIALNUM   4
#define  RASPI_PROP_GETARMMEMORYSIZE    5
#define  RASPI_PROP_GETVCMEMORYSIZE     6
#define  RASPI_PROP_GETCMDLINE          7
#define  RASPI_PROP_GETDMACHANS         8
#define  RASPI_PROP_GETPHYSDISPDIM      9
#define  RASPI_PROP_GETVIDVIRTDIM       10
#define  RASPI_PROP_GETVIDDEPTH         11

	DWORD dwSize;
	int rc;

	// Set up copy section. (initialize globals)
    SetupCopySection((ROMHDR *)pTOC);

	// Initialize the serial debug output
	OEMDebugInit ();

	Debug (TEXT("Address of Main       %x\r\n"), main);
	Debug (TEXT("Address of stack  var %x\r\n"), &zz);
	Debug (TEXT("Address of static var %x\r\n"), &aaa);

	// Send out the copyright message
	OEMWriteDebugString(szSignOn);



	rc = GetRASPIProperty (RASPI_PROP_GETPHYSDISPDIM      , bBuff, sizeof(bBuff), &dwSize);
	Debug (TEXT("Video W/H      :   %d    %08x-%08x\r\n"), dwSize, *(DWORD *)bBuff,*(DWORD *)&bBuff[4]);

	rc = GetRASPIProperty (RASPI_PROP_GETVIDVIRTDIM      , bBuff, sizeof(bBuff), &dwSize);
	Debug (TEXT("Video Virt W/H :   %d    %08x-%08x\r\n"), dwSize, *(DWORD *)bBuff,*(DWORD *)&bBuff[4]);

	rc = GetRASPIProperty (RASPI_PROP_GETVIDDEPTH      , bBuff, sizeof(bBuff), &dwSize);
	Debug (TEXT("Video Pix depth:   %d    %08x\r\n"), dwSize, *(DWORD *)bBuff);


	//
	// Mailbox test...
	//
	Debug (TEXT("Calling VidCON_InitDebugSerial\r\n"));

	VidCON_InitDebugSerial ();

	Debug (TEXT("VidCON_InitDebugSerial returned.  +++\r\n"));

	// Redirect the debug output to the video console
	pWriteCharFunc = &VidCON_WriteDebugByteExtern;

	// Send out the copyright message
	OEMWriteDebugString(szSignOn);

	//DrawSplash (4000, 100, "RPI.bmp");

	// Test the fonts.
	//for (int i = 0; i < 40; i++)
	{
		VidCON_WriteDebugString(TEXT("this is a test string loop\r\n"));
		VidCON_WriteDebugString(TEXT("the quick brown fox jumped over the lazy dog\r\n"));
		VidCON_WriteDebugString(TEXT("THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG\r\n"));
		VidCON_WriteDebugString(TEXT("0123456789!@#$%^&*()_+=-,.;<>:/?[]\\{}|G\r\n"));
	}

	//VidCON_WriteDebugString(TEXT("  this is a test string loop\r\n"));
	//VidCON_WriteDebugString(TEXT("  the quick brown fox jumped over the lazy dog\r\n"));
	//VidCON_WriteDebugString(TEXT("  THE QUICK BROWN FOX JUMPED OVER THE LAZY DOG\r\n"));
	//VidCON_WriteDebugString(TEXT("  0123456789!@#$%^&*()_+=-,.;<>:/?[]\\{}|1234567890\r\n"));

	//PropTest();

	rc = GetRASPIProperty (RASPI_PROP_FIRMWAREVERSION  , bBuff, sizeof(bBuff), &dwSize);
	Debug (TEXT("Firmware Version: %d    %04x\r\n"), dwSize, *(DWORD *)bBuff);
	
	rc = GetRASPIProperty (RASPI_PROP_GETBOARDMODEL    , bBuff, sizeof(bBuff), &dwSize);
	Debug (TEXT("Board Model:      %d    %04x\r\n"), dwSize, *(DWORD *)bBuff);

	rc = GetRASPIProperty (RASPI_PROP_GETBOARDREVISION , bBuff, sizeof(bBuff), &dwSize);
	Debug (TEXT("Board Version:    %d    %d\r\n"), dwSize, *(DWORD *)bBuff);

	rc = GetRASPIProperty (RASPI_PROP_GETMACADDRESS    , bBuff, sizeof(bBuff), &dwSize);
	Debug (TEXT("Mac Address:      %d    %02x-%02x-%02x-%02x-%02x-%02x\r\n"), dwSize,
	       bBuff[0], bBuff[1], bBuff[2], bBuff[3], bBuff[4], bBuff[5]);

	rc = GetRASPIProperty (RASPI_PROP_GETBOARDSERIALNUM, bBuff, sizeof(bBuff), &dwSize);
	Debug (TEXT("Board Serial Num: %d    %08x-%08x\r\n"), dwSize, *(DWORD *)bBuff, *(DWORD *)&bBuff[4]);

	rc = GetRASPIProperty (RASPI_PROP_GETARMMEMORYSIZE , bBuff, sizeof(bBuff), &dwSize);
	Debug (TEXT("RAM Memory:       %d    %0x (%d)\r\n"), dwSize, *(DWORD *)bBuff, *(DWORD *)bBuff);

	rc = GetRASPIProperty (RASPI_PROP_GETVCMEMORYSIZE  , bBuff, sizeof(bBuff), &dwSize);
	Debug (TEXT("Vid Memory:       %d    %0x (%d)\r\n"), dwSize, *(DWORD *)bBuff, *(DWORD *)bBuff);

	rc = GetRASPIProperty (RASPI_PROP_GETDMACHANS      , bBuff, sizeof(bBuff), &dwSize);
	Debug (TEXT("Free DMA Chans:   %d    %0x\r\n"), dwSize, *(DWORD *)bBuff);

	rc = GetRASPIProperty (RASPI_PROP_GETPHYSDISPDIM      , bBuff, sizeof(bBuff), &dwSize);
	Debug (TEXT("Video W/H     :   %d    %08x-%08x\r\n"), dwSize, *(DWORD *)bBuff,*(DWORD *)&bBuff[4]);




	rc = GetRASPIProperty (RASPI_PROP_GETCMDLINE       , bBuff, sizeof(bBuff), &dwSize);
	Debug (TEXT("Commmand line:    %d    >%S<\r\n"), dwSize, (char *)bBuff);

	//Debug (TEXT("Spinning...\r\n"));
	//while (1);

	//char *pEnd, *pStart, *pVal;
	//char *pTok, ch;
	//for (pTok = (char *)bBuff; (*pTok == ' ') && (*pTok != '\0'); pTok++);
	//while (*pTok != '\0')
	//{
	//	pStart = pTok;
	//	for (; *pTok != ' ' && (*pTok != '\0'); pTok++);
	//	pEnd = pTok;
	//	ch = *pEnd;
	//	*pEnd = '\0';
	//	for (pVal = pStart; *pVal != '=' && (*pVal != '\0'); pVal++);
	//	if (pVal != pEnd)
	//		*pVal++ = '\0';
	//	
	//	Debug (TEXT("Name  >%S<\r\n"), (char *)pStart);
	//	Debug (TEXT("Value >%S<\r\n\r\n"), (char *)pVal);
	//	if (ch == '\0')
	//		break;
	//	for (pTok = pEnd+1; (*pTok == ' ') && (*pTok != '\0'); pTok++);
	//}

	char *pTok, *pName, *pVal;
	pTok = (char *)bBuff;
	int i = 0;
	while (MyParseString (&pTok, &pName, &pVal) != 0)
	{
		Debug (TEXT("Name  >%S<\r\n"), (char *)pName);
		Debug (TEXT("Value >%S<\r\n\r\n"), (char *)pVal);
		if (i++ > 20)
		{
			Debug (TEXT("Bail!\r\n"));
			break;
		}
	}


	Debug (TEXT("Done!\r\n"));

	SetOKLED();

	while (1);
}

//----------------------------------------------------------------------
//
//  Function:  SetupCopySection
//
//  Copies image's copy section data (initialized globals) to the correct
//  fix-up location.  Once completed, initialized globals are valid.
//
BOOL SetupCopySection(ROMHDR * pTOCLocal)
{
	BOOL rc = FALSE;
	UINT i, loop, count;
	COPYentry *pCopyEntry;
	const uint8_t *pSrc;
	uint8_t *pDst;
	//int i;

	//QuickDebugOut('1');

	if (pTOCLocal == (ROMHDR *const) -1) goto cleanUp;

	//QuickDebugOut('2');

	//DumpDWORD('pTOC', (DWORD)pTOCLocal);
	//DumpDWORD('CpOf', (DWORD)pTOCLocal->ulCopyOffset);
	//DumpDWORD('CpEn', (DWORD)pTOCLocal->ulCopyEntries);


	pCopyEntry = (COPYentry *)pTOCLocal->ulCopyOffset;

	for (loop = 0; loop < pTOCLocal->ulCopyEntries; loop++)
	{
//		QuickDebugOut('3');
		count = pCopyEntry->ulCopyLen;
		pDst = (uint8_t*)pCopyEntry->ulDest;
		pSrc = (uint8_t*)pCopyEntry->ulSource; 

	//DumpDWORD('pSrc', (DWORD)pSrc);
	//DumpDWORD('pDst', (DWORD)pDst);
	//DumpDWORD('cnt ', (DWORD)count);

		for (i = 0; i < count; i++)
			*pDst++ = *pSrc++;
		//memcpy(pDst, pSrc, count);

		pDst += count;
		count = pCopyEntry->ulDestLen - pCopyEntry->ulCopyLen;

		for (i = 0; i < count; i++)
			*pDst++ = 0;
//		memset(pDst, 0, count);

	}
//	QuickDebugOut('4');

	rc = true;

cleanUp:    
//	QuickDebugOut('5');
    return rc;
}
//----------------------------------------------------------------------
//  DrawSplash - Draws a bitmap on the screen
//
int DrawSplash (int x, int y, char *szFileName)
{
	int rc;
	Debug (TEXT("DrawSplash++ x:%x, y:%x file:%S\r\n"), x, y, szFileName);

	// This routine is in the video console code since it knows how to
	// do this type of stuff.
	rc = VidCON_DrawSplash (x, y, szFileName);

	Debug (TEXT("DrawSplash-- rc:%d\r\n"), rc);
	return rc;
}
// ---------------------------------------------------------------------------
// Debug
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
