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


#define VID_HEIGHT   1080
#define VID_WIDTH    1920
//#define VID_HEIGHT   720
//#define VID_WIDTH    1280

#define VID_BITSPP   16
//#define VID_BITSPP   24

#define HSURF   DWORD


//----------------------------------------------------------------------------------
// InitVideoSystem - Initialize the video system and return frame buff pointer
//
int InitVideoSystem(int nWidth, int nHeight, int nBitsPerPixel, HSURF *phVidFrame);

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