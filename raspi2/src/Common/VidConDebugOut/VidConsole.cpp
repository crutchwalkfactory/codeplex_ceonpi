//
// Copyright (c) Douglas Boling
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
#include <nkintr.h>
#include <pehdr.h>
#include <romldr.h>
#include "binfilesystem.h"
#include <image_cfg.h>
#include "Video.h"
#include "VidConsole.h"
#include "Debugfuncs.h"

extern "C" ROMHDR * volatile const pTOC;

//
// Define colors for console
//
#if VID_BITSPP == 16
//#define FORECOLOR  0xffff //White
#define FORECOLOR  0xffc0 //Yellow
#define FILLCOLOR  0x0000 //White

#elif VID_BITSPP == 24
#define FORECOLOR  0x0000
#define FILLCOLOR  0x00ffffff

#else
#define FORECOLOR  0x0000
#define FILLCOLOR  0x00ffffff
#endif

#define CON_COLS     120

#define TABSPACE							8		// number of spaces for a TAB

#define DEFAULT_EOF_CHAR					'\r'
#define DEFAULT_APPEND_CHAR					'\n'

typedef struct {
	BYTE bCharWidth;		
	BYTE bCharHeight;
	WORD wEnable;
	DWORD dwForeClr;
	DWORD dwBkClr;
	DWORD dwConLeft;
	DWORD dwConTop;
	DWORD dwConRight;
	DWORD dwConBottom;
	DWORD dwCurX;
	DWORD dwCurY;
	HSURF hVidFrame;
	HSURF hFont;
} CONDATASTRUCT, *PCONDATASTRUCT;

//------------------------------ GLOBALS -------------------------------------------
//LPVOID g_lpVidMem = NULL;								// Handle to mapped video memory

CONDATASTRUCT stConData;

// Constructed surface for the FONT info
SURFINFO siFont;


void ClearScreen(PCONDATASTRUCT pstCon, DWORD dwColor);
void MoveCursor(UINT newPos);
void VidCON_WriteDebugByte(BYTE ucChar);

//----------------------------------------------------------------------------------
//
//
int VidCON_InitDebugSerial (BOOL fVirtualMem)
{
	PBITMAPFILEHEADER pbmfh;
	PBITMAPINFOHEADER pbmih;
	BITMAPINFOHEADER bmihFont;
	
	Debug (TEXT("VidCON_InitDebugSerial++\r\n"));

	// Initialize the video system
	InitVideoSystem (VID_WIDTH, VID_HEIGHT, VID_BITSPP, &stConData.hVidFrame, fVirtualMem);

	// Load the font file
	Debug (TEXT("Calling BinFindFileData...\r\n"));
	int rc = 1234;
	const unsigned char *pBitmap;
	DWORD dwSize;
	rc = BinFindFileData((DWORD)pTOC, "font.bmp", &pBitmap, &dwSize);
	if (rc == 0)
	{
		Debug (TEXT("BinFindFileData returned = %d, pcFontData=%x dwSize=%d\r\n"), rc, pBitmap, dwSize);

		pbmfh = (PBITMAPFILEHEADER)pBitmap;
		pbmih = (PBITMAPINFOHEADER)(pBitmap + sizeof (BITMAPFILEHEADER));

		Debug (TEXT("pbmih  %x\r\n"), pbmih);

		// Align up the structure to prevent alignment problems.
		PBYTE pDst, pSrc;
		pSrc = (PBYTE)pbmih;
		pDst = (PBYTE)&bmihFont;	

		for (int i = 0; i < sizeof (BITMAPINFOHEADER); i++)
			*pDst++ = *pSrc++;
		pbmih = &bmihFont;

		//Debug (TEXT("biSize %x\r\n"), pbmih->biSize);
		//Debug (TEXT("biWidth %x\r\n"), pbmih->biWidth);
		//Debug (TEXT("biHeight %x\r\n"), pbmih->biHeight);
		//Debug (TEXT("biPlanes %x\r\n"), pbmih->biPlanes);
		//Debug (TEXT("biBitCount %x\r\n"), pbmih->biBitCount);
		//Debug (TEXT("biCompression %x\r\n"), pbmih->biCompression);
		//Debug (TEXT("biSizeImage %x\r\n"), pbmih->biSizeImage);
		//Debug (TEXT("biXPelsPerMeter %x\r\n"), pbmih->biXPelsPerMeter);
		//Debug (TEXT("biYPelsPerMeter %x\r\n"), pbmih->biYPelsPerMeter);
		//Debug (TEXT("biClrUsed %x\r\n"), pbmih->biClrUsed);
		//Debug (TEXT("biClrImportant %x\r\n"), pbmih->biClrImportant);

		// Create a surface structure
		siFont.nWidth = pbmih->biWidth;
		siFont.nHeight = pbmih->biHeight;
		siFont.nBitsPerPixel = pbmih->biBitCount;
		siFont.dwStride = ((pbmih->biWidth * (pbmih->biBitCount/8)) + 3) & 0xfffffffc; 
		siFont.pBuffer = (PBYTE)(DWORD)pBitmap + pbmfh->bfOffBits;
		siFont.dwVidBuffSize = dwSize;

//		Debug (TEXT("BinFindFileData returned = %d, pcFontData=%x dwSize=%d\r\n"), rc, stConData.pFontData, stConData.dwFontSize);

		//// Blit the entire font bmp
		//VidCopy(stConData.hVidFrame, 100, 100, pbmih->biWidth, pbmih->biHeight, 
		//           (HSURF)pBitmap, 0, 0);

		// The values are hard coded and are derived from the font image I created.
		stConData.bCharWidth = 8;
		stConData.bCharHeight = 16;
		stConData.dwConLeft = 0;
		stConData.dwConTop = 0;
		stConData.dwConRight = CON_COLS * stConData.bCharWidth;
		stConData.dwConBottom = VID_HEIGHT;

		stConData.hFont =(HSURF)&siFont;

		stConData.dwForeClr = FORECOLOR;
		stConData.dwBkClr =   FILLCOLOR;

		// Enable video console output
		stConData.wEnable=1;
	}
	else
		Debug (TEXT("Error finding Font File rc %d\r\n"), rc);

	// Clear the console
	ClearScreen (&stConData, FILLCOLOR);

	Debug (TEXT("VidCON_InitDebugSerial-- %d\r\n"),rc);
	return rc;
}

//----------------------------------------------------------------------------------
// VidCON_DisableDebugSerial - Disables video debug out streaming.
// Once the 'real' video driver starts, we don't want to be writing to the frame
// buffer.
//
int VidCON_DisableDebugSerial (BOOL fVirtualMem)
{
	// Disable video console output
	stConData.wEnable=0;

	return 0;
}

//----------------------------------------------------------------------------------
//
//
void ClearScreen(PCONDATASTRUCT pstCon, DWORD dwColor)
{
	VidSet(pstCon->hVidFrame, pstCon->dwConLeft, pstCon->dwConTop, 
	       pstCon->dwConRight - pstCon->dwConLeft, pstCon->dwConBottom - pstCon->dwConTop, dwColor);

	pstCon->dwCurX = pstCon->dwConLeft;
	pstCon->dwCurY = pstCon->dwConTop;
	return;
}

//----------------------------------------------------------------------------------
// UpdateCursor - Moves cursor to new location on screen
//
int UpdateCursor (PCONDATASTRUCT pstCon, DWORD dX, DWORD dY)
{
	int nScroll = 0;
	if (dX < 0)
	{
		if (pstCon->dwCurX > (pstCon->bCharWidth * dX))
		{
			pstCon->dwCurX -= (pstCon->bCharWidth * dX);
		}
		else if (pstCon->dwCurY > pstCon->bCharHeight)
		{
			pstCon->dwCurX = pstCon->dwConRight - pstCon->bCharWidth;
			pstCon->dwCurY -= pstCon->bCharHeight;
		}
	}
	else if (dX > 0)
	{
		pstCon->dwCurX += (pstCon->bCharWidth * dX);

		if (pstCon->dwCurX > pstCon->dwConRight)
		{
			pstCon->dwCurX = pstCon->dwConLeft;
			dY++;
		}
	}
	if (dY < 0)
	{
		if (pstCon->dwCurY > pstCon->bCharHeight)
		{
			pstCon->dwCurY -= pstCon->bCharHeight;
		}
	}
	else if (dY > 0)
	{
		pstCon->dwCurY += pstCon->bCharHeight;
		if (pstCon->dwCurY + pstCon->bCharHeight > pstCon->dwConBottom)
		{
			pstCon->dwCurY = pstCon->dwConBottom - pstCon->bCharHeight;
			nScroll++;
		}
	}
	return nScroll;
}

//----------------------------------------------------------------------------------
//
//
void ScrollConsole (PCONDATASTRUCT pstCon, int nX, int nY)
{
	// For now, just scroll up one line
	VidCopy(pstCon->hVidFrame,						//Dest surface
	           pstCon->dwConLeft,						//Dest X
	           pstCon->dwConTop,						//Dest Y
	           pstCon->dwConRight - pstCon->dwConLeft,	//Dest width
			   pstCon->dwConBottom - pstCon->dwConTop - pstCon->bCharHeight, //Dest height
		       pstCon->hVidFrame,						//Dest surface
			   pstCon->dwConLeft,						//Src X
			   pstCon->dwConTop + pstCon->bCharHeight);	//Src Y

	// Clear new line
	VidSet(pstCon->hVidFrame, 
		      pstCon->dwConLeft, 
			  pstCon->dwConBottom - pstCon->bCharHeight,
	          pstCon->dwConRight - pstCon->dwConLeft, 
			  pstCon->bCharHeight, 
			  FILLCOLOR);
	return;
}
//----------------------------------------------------------------------------------
//
//
void BlitChar(PCONDATASTRUCT pstCon, int nX, int nY, BYTE ucChar, DWORD dwForeClr, DWORD dwBkClr)
{
	// See if whitespace
	if ((ucChar <= ' ') && (ucChar != '\t'))
	{
		ucChar = ' ';
	}

	// Draw the character.
	int sX = pstCon->bCharWidth * ((ucChar >> 5) & 0x07);
	int sY = pstCon->bCharHeight * (ucChar & 0x1f);

	//Debug (TEXT("CharBlt c=%c c=%x x=%d y=%d\r\n"),ucChar, ucChar, sX, sY);

	VidTxtBlt(pstCon->hVidFrame, nX, nY, pstCon->bCharWidth, pstCon->bCharHeight, 
		      pstCon->hFont, sX, sY, dwForeClr, dwBkClr);
	return;
}

//----------------------------------------------------------------------------------
//
//
void VidCON_WriteDebugByte(PCONDATASTRUCT pstCon, BYTE ucChar)
{
	CHAR c = ucChar;
	int dY = 0, dX = 0;

	// Make sure the video output is enabled.
	if (pstCon->wEnable == 0)
		return;

	switch(c)
	{
	case '\n':								// Line feed - move down one line
		dY++;
		break;

	case '\r':								// Carriage return - move to beginning of line
		pstCon->dwCurX = pstCon->dwConLeft;
		break;

	case '\b':								// Backspace - erase the last character
		dX--;
		break;

	case '\t':								// Horizontal tab - move TABSPACE characters
		dX+=TABSPACE;
		break;

	case NULL:								// Don't output NULL characters to screen
		return;

	default:								// Regular character - output and move cursor
		BlitChar(pstCon, pstCon->dwCurX, pstCon->dwCurY, c, pstCon->dwForeClr, pstCon->dwBkClr);
		dX++;
		break;
	}

	int nScroll = UpdateCursor(pstCon, dX, dY);
	//Debug (TEXT("nScroll %d\r\n"), nScroll);
	if (nScroll)
	{
		ScrollConsole (pstCon, 0, 1);
		//DumpDWORD(0x12341234, nScroll);
	}
	return;
}

//----------------------------------------------------------------------------------
//
//
void VidCON_WriteDebugString(LPWSTR pBuffer)
{
	while (*pBuffer)
	{
		VidCON_WriteDebugByte (&stConData, *(UCHAR *)pBuffer);
		pBuffer++;
	}
}

//----------------------------------------------------------------------------------
//
//
void VidCON_WriteDebugByteExtern(BYTE ucChar)
{
	VidCON_WriteDebugByte (&stConData, ucChar);
	return;
}

//----------------------------------------------------------------------------------
//
//
int VidCON_DrawSplash(int x, int y, char *szFileName)
{
	int rc = 0;
	PBITMAPFILEHEADER pbmfh;
	PBITMAPINFOHEADER pbmih;
	const unsigned char *pBitmap;
	DWORD dwSize;

	
	Debug (TEXT("VidCON_DrawSplash++ x:%x, y:%x f:%S\r\n"), x, y, szFileName);

	// Load the font file
	Debug (TEXT("Calling BinFindFileData\r\n"));

	rc = BinFindFileData((DWORD)pTOC, szFileName, &pBitmap, &dwSize);
	if (rc == 0)
	{
		Debug (TEXT("BinFindFileData returned = %d, pcFontData=%x dwSize=%d\r\n"), rc, pBitmap, dwSize);

		pbmfh = (PBITMAPFILEHEADER)pBitmap;
		pbmih = (PBITMAPINFOHEADER)(pBitmap + sizeof (BITMAPFILEHEADER));
		if (pbmfh->bfType != 'MB')
			Debug (TEXT("Bad bmp file format header.\r\n"));

		//Debug (TEXT("biSize %x\r\n"), pbmih->biSize);
		//Debug (TEXT("biWidth %x\r\n"), pbmih->biWidth);
		//Debug (TEXT("biHeight %x\r\n"), pbmih->biHeight);
		//Debug (TEXT("biPlanes %x\r\n"), pbmih->biPlanes);
		//Debug (TEXT("biBitCount %x\r\n"), pbmih->biBitCount);
		//Debug (TEXT("biCompression %x\r\n"), pbmih->biCompression);
		//Debug (TEXT("biSizeImage %x\r\n"), pbmih->biSizeImage);
		//Debug (TEXT("biXPelsPerMeter %x\r\n"), pbmih->biXPelsPerMeter);
		//Debug (TEXT("biYPelsPerMeter %x\r\n"), pbmih->biYPelsPerMeter);
		//Debug (TEXT("biClrUsed %x\r\n"), pbmih->biClrUsed);
		//Debug (TEXT("biClrImportant %x\r\n"), pbmih->biClrImportant);

//		Debug (TEXT("BinFindFileData returned = %d, pcFontData=%x dwSize=%d\r\n"), rc, stConData.pFontData, stConData.dwFontSize);

		// Blit the image
		VidXBlt (stConData.hVidFrame, x, y, pbmih->biWidth, pbmih->biHeight-1, (HSURF)pBitmap, 0, 0, 0xffffffff);
		//VidXBlt(stConData.hVidFrame, x, y, pbmih->biWidth, pbmih->biHeight, (HSURF)pBitmap, 0, 0);
	}
	else
		Debug (TEXT("Error finding image file rc %d\r\n"), rc);


	VidSet(stConData.hVidFrame, 20, 600, 40, 40, 0x003f);
	VidSet(stConData.hVidFrame, 20, 640, 40, 40, 0x07c0);
	VidSet(stConData.hVidFrame, 20, 680, 40, 40, 0xf800);

	VidSet(stConData.hVidFrame, 60, 600, 40, 40, 0x003f);
	VidSet(stConData.hVidFrame, 60, 640, 40, 40, 0x001f << 6);
	VidSet(stConData.hVidFrame, 60, 680, 40, 40, 0x001f << 11);

	return rc;
}

