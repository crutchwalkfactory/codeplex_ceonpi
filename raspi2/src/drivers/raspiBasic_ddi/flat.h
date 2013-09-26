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
//------------------------------------------------------------------------------
//
//  File:  flat.h
//
#ifndef __FLAT_H
#define __FLAT_H

#include <defbus.h>
#include <ddhal.h>
#include <bcm2835.h>
#include <image_cfg.h>
#include <dma.h>


#define GPE_ZONE_PDD        DEBUGZONE(14)

// These enable specific DMA acceleration routines
//#define ENABLE_TEXT_DMAACCEL
//#define ENABLE_FILL_DMAACCEL 
//#define ENABLE_SRC_COPY_DMAACCEL

typedef struct {
	int   nHeight;
	int   nWidth;
	int   nBPP;
	int   nBytesPerPixel;
	DWORD dwFrameStride;
	DWORD dwFrameSize;
	PBYTE pFrame;
	// DMA fields
	PBYTE pRegBase;
	int   nChan;
	int   nNumConBlks;
	PDMACONBLKSTRUCT pVirtConBlock;
	DWORD dwPhysConBlock;
	int   nSolidDataCnt;
	PDWORD pdwVirtSolidData;
	DWORD dwPhysSolidData;
} DMAVIDINFOSTRUCT, *PDMAVIDINFOSTRUCT;


//------------------------------------------------------------------------------

class FlatGPE : public GPE
{

public:
    FlatGPE();
    virtual ~FlatGPE();

    virtual int InVBlank();
    virtual int NumModes();

    virtual SCODE SetPalette(const PALETTEENTRY*, WORD, WORD);

    virtual SCODE AllocSurface(GPESurf**, int, int, EGPEFormat, int);
    virtual SCODE Line(GPELineParms*, EGPEPhase);
    virtual SCODE BltPrepare(GPEBltParms*);
    virtual SCODE BltComplete(GPEBltParms*);

    virtual SCODE GetModeInfo(GPEMode*, int);
    virtual SCODE SetMode(int, HPALETTE*);
    virtual SCODE SetPointerShape(GPESurf*, GPESurf*, int, int, int, int);
    virtual SCODE MovePointer(int, int);

    virtual VOID PowerHandler(BOOL bOff);

private:
    
    // Actual mode info (width, height, bpp, frequency, and format)
    GPEMode m_gpeMode;

    // Driver emulate cursor
    BOOL   m_cursorDisabled;
    BOOL   m_cursorVisible;
    BOOL   m_cursorForcedOff;
    RECTL  m_cursorRect;
    POINTL m_cursorSize;
    POINTL m_cursorHotspot;
    UCHAR* m_cursorStore;
    UCHAR* m_cursorXor;
    UCHAR* m_cursorAnd;

	DMAVIDINFOSTRUCT m_dmaVidData;
	BOOL   BltInit(void);
#ifdef ENABLE_FILL_DMAACCEL
	int    DispatchConBlk2DMAChan (DWORD dwPhysDMABlk, int nChan, BOOL fWaitForComplete);
	int    DMASolidClrFill (PBYTE pDest, int nX, int nY, int nCX, int nCY, COLORREF clr);
	SCODE  DMAFillRect24(GPEBltParms *pBltParms );
#endif
#ifdef ENABLE_TEXT_DMAACCEL
	SCODE  DMATextBlt24(GPEBltParms *pBltParms );
#endif
#ifdef ENABLE_SRC_COPY_DMAACCEL
    SCODE  DMASrcCopyBlt24(GPEBltParms *pBltParms );
#endif


    // Helper functions
    VOID CursorOn();
    VOID CursorOff();
    SCODE WrappedEmulatedLine(GPELineParms*);
};

//------------------------------------------------------------------------------

#endif
