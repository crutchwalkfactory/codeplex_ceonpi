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
#include <pkfuncs.h>
#include <nkexport.h>
#include <ddhal.h>
#include <wingdi.h>

#include <image_cfg.h>
#include <vidConsole.h>

// ddi.c
// The comments in this file will vary from OS version to version.
//
// This file implements functions that support the IOCTL_HAL_DDI handler.
//
// All functions in this file fall into one of three categories:
// REQUIRED - you must implement this function for HALFlat functionality
// OPTIONAL - you may implement this function to enable specific
//            HALFlat functionality, however you must implement a stub
//            at the minimum.
// 
// Since all of the HALFlat functions are called through OEMIoControl,
// the names are modifiable so long as the caller is modified to match.  The
// only things held constant are the IOCTL codes.
//

// Function prototypes
static BOOL GetNumModes(PVOID lpOutBuf, DWORD nOutBufSize,
                        LPDWORD lpBytesReturned);
static BOOL GetModeInfo(ULONG modeId, PVOID lpOutBuf, DWORD nOutBufSize,
                        LPDWORD lpBytesReturned);
static BOOL SetMode(ULONG modeId);
static BOOL SetPalette(const LPVOID lpInBuf, DWORD nInBufSize);
static BOOL Power(BOOL powerOff);

// ---------------------------------------------------------------------------
// OALIoctlHalDdi: REQUIRED
//
// This function is the general-purpose handler for the IOCTL_HAL_DDI code.
// It calls the appropriate subhandler function based on the subcode.  Any
// of the optional HALFlat subcode handlers that are implemented as stubs
// may be omitted from this function, but all subcodes are handled here for
// demonstration's sake.
//
BOOL OALIoctlHalDdi(UINT32 dwIoControlCode, VOID *lpInBuf, UINT32 nInBufSize,
                    VOID *lpOutBuf, UINT32 nOutBufSize, UINT32 *lpBytesReturned)
{
  BOOL powerOff, rc = FALSE;
  ULONG modeId, subCode = 0;
  const PALETTEENTRY* paletteInBuf;
  DWORD paletteBufSize;

#ifdef DEBUG
	NKDbgPrintfW(L"OALIoctlHalDdi++  code:%d  pOut%x sizOut:%d  pbret:%x\r\n", subCode, lpOutBuf, nOutBufSize, lpBytesReturned);
#endif

	// Validate inputs
	if (lpInBuf == NULL || nInBufSize < sizeof(ULONG))
	{
		NKSetLastError(ERROR_INVALID_PARAMETER);
		if (lpBytesReturned != NULL) *lpBytesReturned = 0;
		goto cleanUp;
	}

	// Since we're called by a kernel-mode driver, we can trust this buffer
	subCode = (*(const ULONG*)lpInBuf);


	switch (subCode)
	{
		case DDHAL_COMMAND_GET_NUM_MODES:
			rc = GetNumModes(lpOutBuf, nOutBufSize, lpBytesReturned);
			break;

		case DDHAL_COMMAND_GET_MODE_INFO:
			if (nInBufSize < 2 * sizeof(ULONG))
			{
				NKSetLastError(ERROR_INVALID_PARAMETER);
				if (lpBytesReturned != NULL) *lpBytesReturned = 0;
				goto cleanUp;
			}
			modeId = ((ULONG*)lpInBuf)[1];
			rc = GetModeInfo(modeId, lpOutBuf, nOutBufSize, lpBytesReturned);
			break;

		case DDHAL_COMMAND_SET_MODE:
			if (lpBytesReturned != NULL) *lpBytesReturned = 0;
			if (nInBufSize < 2 * sizeof(ULONG))
			{ 
				NKSetLastError(ERROR_INVALID_PARAMETER);
				goto cleanUp;
			}
			modeId = ((ULONG*)lpInBuf)[1];
			rc = SetMode(modeId);
			break;

		case DDHAL_COMMAND_SET_PALETTE:
			if (lpBytesReturned != NULL) *lpBytesReturned = 0;
			// SetPalette uses mode from the input buffer,
			// so skip the DDHAL_COMMAND parameter.
			paletteInBuf = (const PALETTEENTRY*)((UCHAR*)lpInBuf + sizeof(ULONG));
			paletteBufSize = nInBufSize - sizeof(ULONG);
			rc = SetPalette(lpInBuf, nInBufSize);  
			break;

		case DDHAL_COMMAND_POWER:
			if (nInBufSize < 2 * sizeof(ULONG))
			{
				NKSetLastError(ERROR_INVALID_PARAMETER);
				goto cleanUp;
			}
			powerOff = ((ULONG*)lpInBuf)[1];
			rc = Power(powerOff);
	}

cleanUp:
	return rc;
}

// ---------------------------------------------------------------------------
// GetNumModes: REQUIRED
//
// This function returns the number of display modes supported by the
// HALFlat implementation.
//
static BOOL GetNumModes(PVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned)
{
	BOOL fRc = FALSE;

	if (nOutBufSize >= sizeof (DWORD))
	{
		*(DWORD *)lpOutBuf = 1;
		if (lpBytesReturned)
			*lpBytesReturned = sizeof (DWORD);
		fRc = TRUE;
	}
	return fRc;
}

// ---------------------------------------------------------------------------
// GetModeInfo: REQUIRED
//
// This function returns information about the requested display mode in
// the form of a DDHAL_MODE_INFO structure followed by a UINT32 for each
// (optional) palette color.
//
static BOOL GetModeInfo(ULONG modeId, PVOID lpOutBuf, DWORD nOutBufSize,
                        LPDWORD lpBytesReturned)
{
	BOOL fRc = FALSE;
	DWORD *pColors;
	DDHAL_MODE_INFO *pModeInfo = (DDHAL_MODE_INFO *)lpOutBuf;
	MYDISPLAYSETTTNGS DSData;

#ifdef DEBUG
	NKDbgPrintfW(L"OALIoctlHalDdi::GetModeInfo++  pOut%x sizOut:%d  pbret:%x\r\n", lpOutBuf, nOutBufSize, lpBytesReturned);
#endif

	// Check to get our current video config.
	fRc = GetVideoSettings (&DSData);
	if (fRc)
	{
		if (nOutBufSize >= sizeof (DDHAL_MODE_INFO) + (3 * sizeof (DWORD)))
		{
			if (lpBytesReturned)
			{
				*lpBytesReturned = sizeof (DDHAL_MODE_INFO);
			}

			pModeInfo->entries = 0;						// Number of color entries
			pModeInfo->modeId = 1;						// Mode identification
			pModeInfo->flags = 0;						// Option flags DDHAL_HW_ROTATE, DDHAL_CACHE_FRAME_MEMORY 
			pModeInfo->width = DSData.nWidth;           // Display width in pixels
			pModeInfo->height = DSData.nHeight;			// Display height in pixels
			pModeInfo->bpp = DSData.nBitsPerPixel;      // Color deep (bits per pixel)
			pModeInfo->frequency = 60;					// Mode refresh rate
			switch (DSData.nBitsPerPixel)
			{
			case 32:
				pModeInfo->format = EGPE_FORMAT_32BPP;		// EGPE color format
				pModeInfo->paletteMode = PAL_RGB;			// Palette mode
				break;
			case 24:
				pModeInfo->format = EGPE_FORMAT_24BPP;		// EGPE color format
				pModeInfo->paletteMode = PAL_RGB;			// Palette mode
				break;
			case 16:
				pModeInfo->format = EGPE_FORMAT_16BPP;		// EGPE color format
				pModeInfo->paletteMode = PAL_BITFIELDS;//PAL_RGB;			// Palette mode
				pModeInfo->entries = 3;
				// We always request enough space for these DWORDs to be written
				pColors = (DWORD *)&pModeInfo[1]; //Hack to get a pointer to just past the Info structure
				*pColors++ = DSData.dwColorMaskRed;
				*pColors++ = DSData.dwColorMaskGreen;
				*pColors++ = DSData.dwColorMaskBlue;
				if (lpBytesReturned)
				{
					*lpBytesReturned = sizeof (DDHAL_MODE_INFO) + (3 * sizeof (DWORD));
				}
				break;
			case 8:
				pModeInfo->format = EGPE_FORMAT_8BPP;		// EGPE color format
				pModeInfo->paletteMode = PAL_INDEXED;		// Palette mode
				break;
			default:
				pModeInfo->format = EGPE_FORMAT_8BPP;		// EGPE color format
				pModeInfo->paletteMode = PAL_INDEXED;		// Palette mode
				NKDbgPrintfW(L"***ERROR*** OALIoctlHalDdi::GetModeInfo++  Unsupported pixel depth. %d bpp\r\n", DSData.nBitsPerPixel);
				break;
			}
			pModeInfo->phFrameBase = (UINT64)DSData.pFrameBuffer; // Frame buffer physical address
			pModeInfo->frameSize = DSData.dwFBSize;		// Frame buffer size
			pModeInfo->frameStride = DSData.dwStride;   // Frame stride (bytes per line)
		}
		else
		{
			NKSetLastError (ERROR_INSUFFICIENT_BUFFER);
			if (lpBytesReturned)
			{
				*lpBytesReturned = sizeof (DDHAL_MODE_INFO) + (4 * sizeof (DWORD));
#ifdef DEBUG
				NKDbgPrintfW(L"OALIoctlHalDdi::GetModeInfo needs %d bytes\r\n", *lpBytesReturned);
#endif
				fRc = FALSE;
			}
		}
	}
#ifdef DEBUG
	NKDbgPrintfW(L"OALIoctlHalDdi::GetModeInfo-- rc:%d\r\n", fRc);
#endif
	return fRc;  //fRc will be set by GetVideoSettings and is false otherwise.
}

//------------------------------------------------------------------------------
// SetMode: REQUIRED
//
// This function sets the hardware to the mode specified.
//
static BOOL SetMode(ULONG modeId)
{
	// Call over to the debug out code to disable debug output that may
	// be written to the screen.
	VidCON_DisableDebugSerial (TRUE);
	return TRUE;
}    

//------------------------------------------------------------------------------
// SetPalette: OPTIONAL
//
// This function sets the current color palette to the input index.
//
static BOOL SetPalette(const LPVOID lpInBuf, DWORD nInBufSize)
{
	// Fill in palette code here.

	return TRUE;
}

//------------------------------------------------------------------------------
// Power: OPTIONAL
//
// This function implements the power code for the display driver, turning the
// hardware on or off according to the input.
//
static BOOL Power(BOOL powerOff)
{
	// Fill in power code here

	return TRUE;
}    
