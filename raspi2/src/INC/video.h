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

#ifndef __VIDEO_H
#define __VIDEO_H

#if __cplusplus
extern "C" {
#endif

#define HSURF   DWORD

// Defined here because the VidConsole code needs to create a fake surfinfo struct for FONTS
typedef struct {
	WORD wID;
	WORD wReserved;
	int nWidth;
	int nHeight;
	int nBitsPerPixel;
	DWORD dwStride;
	PBYTE pBuffer;
	DWORD dwVidBuffSize;
} SURFINFO, *PSURFINFO;

//----------------------------------------------------------------------------------
// InitVideoSystem - Initialize the video system and return frame buff pointer
//
int InitVideoSystem(int nWidth, int nHeight, int nBitsPerPixel, HSURF *phVidFrame, BOOL fVirtualMem);

//----------------------------------------------------------------------------------
// GetVideoSettings - Returns the parameters of the video frame buffer
//
BOOL GetVideoSettings (PMYDISPLAYSETTTNGS pSettings);

//----------------------------------------------------------------------------------
// VidSet - Simple setblt.  Assumes we're writing to the frame buffer
//
void VidSet(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, DWORD dwColor);

//----------------------------------------------------------------------------------
// VidCopy - Simple copyblt.  This assumes that the src surface is the same
// format as the destination and that the destination is the frame buffer.
//
void VidCopy(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
			 HSURF hSrc,  int nXSrc, int nYSrc);

//----------------------------------------------------------------------------------
// VidTxtBlt - Applies fore/background colors to monochrome font bitmaps.
//
void VidTxtBlt(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
			   HSURF hSrc,  int nXSrc, int nYSrc, DWORD dwForeClr, DWORD dwBkClr);

//----------------------------------------------------------------------------------
// VidTxtBlt - Complex copyblt that understands transparency
//
void VidXBlt(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
			 HSURF hSrc,  int nXSrc, int nYSrc, DWORD dwXClr);

#if __cplusplus
}
#endif

#endif

