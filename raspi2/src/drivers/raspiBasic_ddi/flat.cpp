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
//  File:  flat.cpp
//
//  This file implements basic flat display driver. Informations about
//  supported display modes and switching between them are offloaded to kernel
//  via KernelIoControl calls.
//
#define DDI 1
#include <windows.h>
#include <types.h>
#include <winddi.h>
#include <gpe.h>
#include <emul.h>
#include <ceddk.h>
#include <dispperf.h>
#include <pkfuncs.h>
#include <ddhal.h>
#include "flat.h"

//------------------------------------------------------------------------------
// Debug Zones

INSTANTIATE_GPE_ZONES(0x0003, "ddi_hflat", "PDD", "Unused")
//INSTANTIATE_GPE_ZONES(0x4003, "ddi_hflat", "PDD", "Unused")

//------------------------------------------------------------------------------
// Static variables

static GPE *g_pGPE;
static ULONG g_bitMasks[] = { 0, 0, 0};

//------------------------------------------------------------------------------
// External functions

BOOL APIENTRY GPEEnableDriver(
   ULONG version, ULONG cj, DRVENABLEDATA *pData, ENGCALLBACKS *pCallbacks
);

//------------------------------------------------------------------------------
// Local macros

#define INREG32(x)          READ_REGISTER_ULONG((ULONG*)(x))
#define OUTREG32(x, y)      WRITE_REGISTER_ULONG((ULONG*)(x), (ULONG)(y))

//------------------------------------------------------------------------------
//
//  Function:  DisplayInit
//
//  The GWES will invoke this routine once prior to making any other calls
//  into the driver. This routine needs to save its instance path information
//  and return TRUE.  If it returns FALSE, GWES will abort the display
//  initialization.
//
BOOL DisplayInit(LPCTSTR pszInstance, DWORD monitors)
{
    DEBUGMSG(GPE_ZONE_PDD, (
        L"+DisplayInit(%s, %d)\r\n", pszInstance, monitors
    ));

    g_pGPE = new FlatGPE;

    DEBUGMSG(GPE_ZONE_PDD, (L"-DisplayInit(rc = 1)\r\n"));
    return TRUE;
}

//------------------------------------------------------------------------------
//
//  Function:  DrvEnableDriver
//
//  This function must be exported from display driver. As long as we use
//  GPE class implementation we don't need do anything else than call
//  GPEEnableDriver library function.
//
BOOL DrvEnableDriver(
    ULONG version, ULONG cj, DRVENABLEDATA *pData, ENGCALLBACKS *pCallbacks
) {
    BOOL rc;
    
    DEBUGMSG(GPE_ZONE_PDD, (
        L"+DrvEnableDriver(0x%08x, %d, 0x%08x, 0x%08x)\r\n", version, cj, pData,
        pCallbacks
    ));
    
    rc = GPEEnableDriver(version, cj, pData, pCallbacks);

    DEBUGMSG(GPE_ZONE_PDD, (L"-DrvEnableDriver(rc = %d)\r\n", rc));
    return rc;
}

//------------------------------------------------------------------------------
//
//  Function:  DrvGetMasks
//
ULONG *APIENTRY DrvGetMasks(DHPDEV dhpdev)
{
    return g_bitMasks;
}

//------------------------------------------------------------------------------
//
//  Function:  RegisterDDHALAPI
//
VOID RegisterDDHALAPI()
{
    DEBUGMSG(GPE_ZONE_PDD, (L"+RegisterDDHALAPI\r\n"));
    DEBUGMSG(GPE_ZONE_PDD, (L"-RegisterDDHALAPI\r\n"));
}

//------------------------------------------------------------------------------
//
//  Function:  GetGPE
//
//  This function is called from GPE class library to get pointer to class
//  deriver from GPE.
//
GPE* GetGPE()
{
    return g_pGPE;
}    

//------------------------------------------------------------------------------
//
//  Constructor
//
FlatGPE::FlatGPE()
{
    m_cursorVisible = FALSE;
    m_cursorDisabled = TRUE;
    m_cursorForcedOff = FALSE;
    memset(&m_cursorRect, 0x0, sizeof(m_cursorRect));
    m_cursorStore = NULL;
    m_cursorXor = NULL;
    m_cursorAnd = NULL;
}

//------------------------------------------------------------------------------
//
//  Dectructor
//
FlatGPE::~FlatGPE()
{
    delete [] m_cursorStore; m_cursorStore = NULL;
    delete [] m_cursorXor; m_cursorXor = NULL;
    delete [] m_cursorAnd; m_cursorAnd = NULL;
}

//------------------------------------------------------------------------------
//
//  Method:  NumModes
//
//  This method returns number of modes supported by display.
//
int FlatGPE::NumModes()
{
    ULONG command;
    int count;

    DEBUGMSG(GPE_ZONE_PDD, (L"+FlatGPE::NumModes()\r\n"));

    // Ask HAL how many modes we support
    command = DDHAL_COMMAND_GET_NUM_MODES;
    if (!KernelIoControl(
        IOCTL_HAL_DDI, &command, sizeof(command), &count, sizeof(count), NULL
    )) {
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::NumModes: "
            L"IOCTL_HAL_DDI!GET_NUM_MODES call failed\r\n"
        ));            
        count = 0;
    }

	// Initialize the DMA blt stuff
	BltInit ();

    DEBUGMSG(GPE_ZONE_PDD, (L"-FlatGPE::NumModes(count = %d)\r\n", count));
    return count;
}

//------------------------------------------------------------------------------
//
//  Method:  GetModeInfo
//
//  This method returns mode information for mode number of modes supported
//  by display.
//
SCODE FlatGPE::GetModeInfo(GPEMode* pMode, int modeNumber)
{
    SCODE sc = E_INVALIDARG;
    BOOL rc;
    ULONG commands[2], size;
    UINT8 *pBuffer = NULL;
    DDHAL_MODE_INFO *pInfo;

    DEBUGMSG(GPE_ZONE_PDD, (
        L"+FlatGPE::GetModeInfo(0x%08x, %d)\r\n", pMode, modeNumber
    ));

    // Ask HAL about mode, first call fails because output buffer is NULL
    commands[0] = DDHAL_COMMAND_GET_MODE_INFO;
    commands[1] = modeNumber;
    rc = KernelIoControl(IOCTL_HAL_DDI, commands, sizeof(commands), NULL, 0, &size);

	DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::GetModeInfo: rc=%d  exrc:%d\r\n", rc, GetLastError()));

    // This shouldn't happen
    if (rc || GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::GetModeInfo: "
            L"IOCTL_HAL_DDI!GET_MODE_INFO failed for mode %d\r\n", modeNumber
        ));
        sc = E_FAIL;
        goto cleanUp;
    }

    // Allocate buffer
    pBuffer = new UINT8[size];

    // Call mode info second time
    if (!KernelIoControl(IOCTL_HAL_DDI, commands, sizeof(commands), pBuffer, size, &size)) 
	{
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::GetModeInfo: "
            L"IOCTL_HAL_DDI!GET_MODE_INFO failed for mode %d\r\n", modeNumber
        ));
        sc = E_FAIL;
        goto cleanUp;
    }

    // Set mode information
    pInfo = (DDHAL_MODE_INFO*)pBuffer;
    pMode->modeId    = pInfo->modeId;
    pMode->width     = pInfo->width;
    pMode->height    = pInfo->height;
    pMode->Bpp       = pInfo->bpp;
    pMode->frequency = pInfo->frequency;
    pMode->format    = (EGPEFormat)pInfo->format;

    // Done
    sc = S_OK;

cleanUp:
    if (pBuffer != NULL) delete [] pBuffer;
    DEBUGMSG(GPE_ZONE_PDD, (L"-FlatGPE::GetModeInfo(sc = 0x%08x)\r\n", sc));
    return sc;
}

//------------------------------------------------------------------------------
//
//  Method:  SetMode
//
//  This method should set display hardware to given mode.
//
SCODE FlatGPE::SetMode(int modeNumber, HPALETTE *pPalette)
{
    SCODE sc = S_OK;
    BOOL rc;
    UINT8 *pBuffer = NULL;
    VOID *pFrame = NULL;
    DDHAL_MODE_INFO *pInfo;
    ULONG commands[2], *pColors;
    PHYSICAL_ADDRESS pha;
    ULONG size;

    DEBUGMSG(GPE_ZONE_PDD, (
        L"+FlatGPE::SetMode(%d, 0x%08x)\r\n", modeNumber, pPalette
    ));

	DEBUGMSG(GPE_ZONE_PDD, (L"FlatGPE::SetMode: 1\r\n"));

    // Ask HAL about mode, first call fails because output buffer is NULL
    commands[0] = DDHAL_COMMAND_GET_MODE_INFO;
    commands[1] = modeNumber;
    rc = KernelIoControl(IOCTL_HAL_DDI, commands, sizeof(commands), NULL, 0, &size);
    
	DEBUGMSG(GPE_ZONE_PDD, (L"FlatGPE::SetMode: 2  rc %d\r\n", rc));

	// This shouldn't happen
    if (rc || GetLastError() != ERROR_INSUFFICIENT_BUFFER) 
	{
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetMode: "
            L"IOCTL_HAL_DDI!GET_MODE_INFO failed for mode %d\r\n", modeNumber
        ));
        sc = E_FAIL;
        goto cleanUp;
    }
	DEBUGMSG(GPE_ZONE_PDD, (L"FlatGPE::SetMode: 3  rc %d\r\n", rc));

    // Allocate buffer
    pBuffer = new UINT8[size];
    if (pBuffer == NULL) {
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetMode: "
            L"Failed allocate %d byte buffer\r\n", size
        ));
        sc = E_FAIL;
        goto cleanUp;
    }
	DEBUGMSG(GPE_ZONE_PDD, (L"FlatGPE::SetMode: 4  rc %d\r\n", rc));

    // Call mode info second time
    if (!KernelIoControl(IOCTL_HAL_DDI, commands, sizeof(commands), pBuffer, size, &size)) 
	{
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetMode: "
            L"IOCTL_HAL_DDI!GET_MODE_INFO failed for mode %d\r\n", modeNumber
        ));
        sc = E_FAIL;
        goto cleanUp;
    }

	DEBUGMSG(GPE_ZONE_PDD, (L"FlatGPE::SetMode: 5  rc %d\r\n", rc));

	// Ask HAL to reconfigure hardware
    commands[0] = DDHAL_COMMAND_SET_MODE;
    commands[1] = modeNumber;
    if (!KernelIoControl(IOCTL_HAL_DDI, commands, sizeof(commands), NULL, 0, NULL)) 
	{
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetMode: "
            L"IOCTL_HAL_DDI!SET_MODE failed for mode %d\r\n", modeNumber
        ));
        sc = E_FAIL;
        goto cleanUp;
    }        
	DEBUGMSG(GPE_ZONE_PDD, (L"FlatGPE::SetMode: 6  rc %d\r\n", rc));

    // Unmap existing frame and delete primary surface
    if (m_pPrimarySurface != NULL) {
        MmUnmapIoSpace(m_pPrimarySurface->Buffer(), 0);
        delete m_pPrimarySurface; m_pPrimarySurface = NULL;
    }

    // Set mode information
    pInfo = (DDHAL_MODE_INFO*)pBuffer;
    m_gpeMode.modeId    = pInfo->modeId;
    m_gpeMode.width     = pInfo->width;
    m_gpeMode.height    = pInfo->height;
    m_gpeMode.Bpp       = pInfo->bpp;
    m_gpeMode.frequency = pInfo->frequency;
    m_gpeMode.format    = (EGPEFormat)pInfo->format;
    m_pMode = &m_gpeMode;
    m_nScreenWidth  = m_pMode->width;
    m_nScreenHeight = m_pMode->height;

    // Allocate IoSpace
    pha.QuadPart = pInfo->phFrameBase;

//(db) don't need to map the frame buffer
	//pFrame = MmMapIoSpace(
    //    pha, pInfo->frameSize, (pInfo->flags & DDHAL_CACHE_FRAME_MEMORY) != 0
    //);
	pFrame = (PVOID)pInfo->phFrameBase;

    if (pFrame == NULL) {
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetMode: "
            L"MmMapIoSpace failed for address %08x:%08x size %d\r\n",
            pha.HighPart, pha.LowPart, pInfo->frameSize
        ));
        sc = E_FAIL;
        goto cleanUp;
    }
    
    // Create surface
    m_pPrimarySurface = new GPESurf(
        m_pMode->width, m_pMode->height, pFrame, pInfo->frameStride, 
        m_pMode->format
    );
    if (m_pPrimarySurface == NULL) {
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetMode: "
            L"Can't allocate GPESurf for display surface\r\n"
        ));
        sc = E_OUTOFMEMORY;
        goto cleanUp;
    }

	DEBUGMSG(GPE_ZONE_PDD, (L"FlatGPE::SetMode: Success 7  rc %d\r\n", rc));
    // Create palette
    pColors = (ULONG*)&pInfo[1];

    // There are three different ways to call EngCreatePalette depending on
    // what pInfo->paletteMode is.
    if (pInfo->paletteMode == PAL_INDEXED) 
	{
	DEBUGMSG(GPE_ZONE_PDD, (L"FlatGPE::SetMode: Success 7.1  rc %d\r\n", rc));
        *pPalette = EngCreatePalette(pInfo->paletteMode, pInfo->entries, pColors, 0, 0, 0);
    } 
	else if (pInfo->paletteMode == PAL_BITFIELDS) 
	{
	DEBUGMSG(GPE_ZONE_PDD, (L"FlatGPE::SetMode: Success 7.2  rc %d\r\n", rc));
        *pPalette = EngCreatePalette(pInfo->paletteMode, 0, NULL, pColors[0], pColors[1], pColors[2]);
    }
	else 
	{
	DEBUGMSG(GPE_ZONE_PDD, (L"FlatGPE::SetMode: Success 7.3  rc %d\r\n", rc));
        *pPalette = EngCreatePalette(pInfo->paletteMode, 0, NULL, 0, 0, 0);
    }
	DEBUGMSG(GPE_ZONE_PDD, (L"FlatGPE::SetMode: Success 8  rc %d\r\n", rc));

    if (*pPalette == NULL) {
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetMode: "
            L"Can't create pallete (mode: %d, entries: %d)\r\n",
            pInfo->paletteMode, pInfo->entries    
        ));
        sc = E_OUTOFMEMORY;
        goto cleanUp;
    }

    // Set bit masks
    g_bitMasks[0] = pColors[0];
    g_bitMasks[1] = pColors[1];
    g_bitMasks[2] = pColors[2];
	DEBUGMSG(GPE_ZONE_PDD, (L"FlatGPE::SetMode: Success 90 r:%x g:%x b:%x\r\n", g_bitMasks[0], g_bitMasks[1], g_bitMasks[2]));
    
cleanUp:    
	DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetMode: 99  rc %d\r\n", rc));
    DEBUGMSG(GPE_ZONE_PDD, (L"-FlatGPE::SetMode(sc = 0x%08x)\r\n", sc));
    return sc; 
}

//------------------------------------------------------------------------------
//
//  Method:  AllocSurface
//
//  This method executes when the driver must allocate storage for surface
//  pixels. In our case there isn't video memory which can be used for this
//  purpose. In case that video memory is required we should fail call
//  otherwise normal memory chunk will be allocated.
//
SCODE FlatGPE::AllocSurface(
    GPESurf **ppSurf, int width, int height, EGPEFormat format, int flags
) {
    SCODE sc = S_OK;

	DEBUGMSG(GPE_ZONE_PDD, (L"FlatGPE::AllocSurface++ w:%d h:%d\r\n", width, height));

    // There isn't extra video memory, so fail if it is required
    if ((flags & GPE_REQUIRE_VIDEO_MEMORY) != 0) {
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::AllocSurface: "
            L"Flat display driver can't allocate extra video memory\r\n"
        ));
        *ppSurf = NULL;
        sc = E_OUTOFMEMORY;
        goto cleanUp;
    }

    // Allocate surface and check result
    *ppSurf = new GPESurf(width, height, format);
    if (*ppSurf == NULL || (*ppSurf)->Buffer() == NULL) {
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::AllocSurface: "
            L"Failed allocate surface (width: %d, height: %d, format %d\r\n",
            width, height, format
        ));
        delete *ppSurf; *ppSurf = NULL;
        sc = E_OUTOFMEMORY;
        goto cleanUp;
    }
    
cleanUp:
    DEBUGMSG(GPE_ZONE_PDD, (L"FlatGPE::AllocSurface--(sc = 0x%08x)\r\n", sc));
    return sc;
}

//------------------------------------------------------------------------------
//
//  Method: WrapperEmulatedLine
//
//  This function is wrapped around emulated line implementation. It must
//  be implemented only if software pointer is used. It switch off/on cursor
//  if line cross it.
//
SCODE FlatGPE::WrappedEmulatedLine (GPELineParms *lineParameters)
{
    SCODE sc;
    RECT bounds;
    int N_plus_1;

    // If cursor is on check for line overlap
    if (m_cursorVisible && !m_cursorDisabled) {

        // Calculate the bounding-rect to determine overlap with cursor
        if (lineParameters->dN) {
            // The line has a diagonal component
            N_plus_1 = 2 + (
                (lineParameters->cPels * lineParameters->dN)/lineParameters->dM
            );
        } else {
            N_plus_1 = 1;
        }

        switch (lineParameters->iDir) {
        case 0:
            bounds.left = lineParameters->xStart;
            bounds.top = lineParameters->yStart;
            bounds.right = lineParameters->xStart + lineParameters->cPels + 1;
            bounds.bottom = bounds.top + N_plus_1;
            break;
        case 1:
            bounds.left = lineParameters->xStart;
            bounds.top = lineParameters->yStart;
            bounds.bottom = lineParameters->yStart + lineParameters->cPels + 1;
            bounds.right = bounds.left + N_plus_1;
            break;
        case 2:
            bounds.right = lineParameters->xStart + 1;
            bounds.top = lineParameters->yStart;
            bounds.bottom = lineParameters->yStart + lineParameters->cPels + 1;
            bounds.left = bounds.right - N_plus_1;
            break;
        case 3:
            bounds.right = lineParameters->xStart + 1;
            bounds.top = lineParameters->yStart;
            bounds.left = lineParameters->xStart - lineParameters->cPels;
            bounds.bottom = bounds.top + N_plus_1;
            break;
        case 4:
            bounds.right = lineParameters->xStart + 1;
            bounds.bottom = lineParameters->yStart + 1;
            bounds.left = lineParameters->xStart - lineParameters->cPels;
            bounds.top = bounds.bottom - N_plus_1;
            break;
        case 5:
            bounds.right = lineParameters->xStart + 1;
            bounds.bottom = lineParameters->yStart + 1;
            bounds.top = lineParameters->yStart - lineParameters->cPels;
            bounds.left = bounds.right - N_plus_1;
            break;
        case 6:
            bounds.left = lineParameters->xStart;
            bounds.bottom = lineParameters->yStart + 1;
            bounds.top = lineParameters->yStart - lineParameters->cPels;
            bounds.right = bounds.left + N_plus_1;
            break;
        case 7:
            bounds.left = lineParameters->xStart;
            bounds.bottom = lineParameters->yStart + 1;
            bounds.right = lineParameters->xStart + lineParameters->cPels + 1;
            bounds.top = bounds.bottom - N_plus_1;
            break;
        default:
            DEBUGMSG(GPE_ZONE_ERROR,(L"FlatGPE::WrappedEmulatedLine: "
                L"Invalid direction: %d\r\n", lineParameters->iDir
            ));
            sc = E_INVALIDARG;
            goto cleanUp;
        }

        // If line overlap cursor, turn if off
        RECTL cursorRect = m_cursorRect;
        RotateRectl (&cursorRect);

        if (cursorRect.top < bounds.bottom && 
            cursorRect.bottom > bounds.top &&
            cursorRect.left < bounds.right && 
            cursorRect.right > bounds.left
        ) { 
            CursorOff();
            m_cursorForcedOff = TRUE;
        }            
    }

    // Do emulated line
    sc = EmulatedLine(lineParameters);

    // If cursor was forced off turn it back on
    if (m_cursorForcedOff) {
        m_cursorForcedOff = FALSE;
        CursorOn();
    }

cleanUp:
    return sc;
}

//------------------------------------------------------------------------------
//
//  Method: Line
//
//  This method executes before and after a sequence of line segments,
//  which are drawn as a path. It examines the line parameters to determine
//  whether the operation can be accelerated. It also places a pointer to
//  a function to execute once per line segment into the pLine member
//  of the GPELineParms structure.
//
SCODE FlatGPE::Line(GPELineParms *pLineParms, EGPEPhase phase)
{
    if (phase == gpeSingle || phase == gpePrepare) {
        if ((pLineParms->pDst != m_pPrimarySurface)) {
            pLineParms->pLine = &GPE::EmulatedLine;
        } else {
            pLineParms->pLine = 
                (SCODE (GPE::*)(struct GPELineParms *))&FlatGPE::WrappedEmulatedLine;
        }            
    }
    return S_OK;
}

//------------------------------------------------------------------------------
//
//  Method:  CursorOn
//
VOID FlatGPE::CursorOn()
{
    UCHAR *pFrame;
    UCHAR *pFrameLine, *pStoreLine, *pXorLine, *pAndLine, data;
    int bytesPerPixel, bytesPerLine;
    int xf, yf, xc, yc, i;

    DEBUGMSG(GPE_ZONE_PDD, (L"+FlatGPE::CursonOn\r\n"));

    // If cursor should not be visible or already is then exit
    if (m_cursorForcedOff || m_cursorDisabled || m_cursorVisible) goto cleanUp;

    if (m_cursorStore == NULL) {
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::CursorOn: "
            L"No cursor store available\r\n"
        ));
        goto cleanUp;
    }

    // We support only 1,2,3 and 4 bytes per pixel
    bytesPerPixel = (m_gpeMode.Bpp + 7) >> 3;
    if (bytesPerPixel <= 0 || bytesPerPixel > 4) goto cleanUp;
    
    // Get some base metrics
    pFrame = (UCHAR*)m_pPrimarySurface->Buffer();
    bytesPerLine = m_pPrimarySurface->Stride();

    for (yf = m_cursorRect.top, yc = 0; yf < m_cursorRect.bottom; yf++, yc++) {
        // Check if we are done
        if (yf < 0) continue;
        if (yf >= m_gpeMode.height) break;
    
        pFrameLine = &pFrame[yf * bytesPerLine];
        pStoreLine = &m_cursorStore[yc * m_cursorSize.x * bytesPerPixel];
        pAndLine = &m_cursorAnd[yc * m_cursorSize.x * bytesPerPixel];
        pXorLine = &m_cursorXor[yc * m_cursorSize.x * bytesPerPixel];
    
        for (
            xf = m_cursorRect.left, xc = 0; xf < m_cursorRect.right; xf++, xc++
        ) {
            // Check if we are done
            if (xf < 0) continue;
            if (xf >= m_gpeMode.width) break;

            // Depending on bytes per pixel
            switch (bytesPerPixel) {
            case 1:
                pStoreLine[xc] = pFrameLine[xf];
                pFrameLine[xf] &= pAndLine[xc];
                pFrameLine[xf] ^= pXorLine[xc];
                break;
            case 2:
                ((USHORT*)pStoreLine)[xc]  = ((USHORT*)pFrameLine)[xf];
                ((USHORT*)pFrameLine)[xf] &= ((USHORT*)pAndLine)[xc];
                ((USHORT*)pFrameLine)[xf] ^= ((USHORT*)pXorLine)[xc];
                break;
            case 3:
                for (i = 0; i < bytesPerPixel; i++) {
                    data = pFrameLine[xf * bytesPerPixel + i];
                    pStoreLine[xc * bytesPerPixel + i] = data;
                    data &= pAndLine[xc * bytesPerPixel + i];
                    data ^= pXorLine[xc * bytesPerPixel + i];
                    pFrameLine[xf * bytesPerPixel + i] = data;
                }                    
                break;
            case 4:
                ((ULONG*)pStoreLine)[xc]  = ((ULONG*)pFrameLine)[xf];
                ((ULONG*)pFrameLine)[xf] &= ((ULONG*)pAndLine)[xc];
                ((ULONG*)pFrameLine)[xf] ^= ((ULONG*)pXorLine)[xc];
                break;
            }                    
        }
    }
    
    // Cursor is visible now
    m_cursorVisible = TRUE;

cleanUp: 
    DEBUGMSG(GPE_ZONE_PDD, (L"-FlatGPE::CursonOn\r\n"));
    return;
}

//------------------------------------------------------------------------------

VOID FlatGPE::CursorOff()
{
    UCHAR *pFrame, *pFrameLine, *pStoreLine, data;
    int bytesPerPixel, bytesPerLine;
    int xf, yf, xc, yc, i;

    DEBUGMSG(GPE_ZONE_PDD, (L"+FlatGPE::CursonOff\r\n"));

    if (m_cursorForcedOff || m_cursorDisabled || !m_cursorVisible) goto cleanUp;

    if (m_cursorStore == NULL) {
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::CursorOff: "
            L"No cursor store available\r\n"
        ));
        goto cleanUp;
    }

    // We support only 1,2,3 and 4 bytes per pixel
    bytesPerPixel = (m_gpeMode.Bpp + 7) >> 3;
    if (bytesPerPixel <= 0 || bytesPerPixel > 4) goto cleanUp;

    // Get some base metrics
    pFrame = (UCHAR*)m_pPrimarySurface->Buffer();
    bytesPerLine = m_pPrimarySurface->Stride();

    for (yf = m_cursorRect.top, yc = 0; yf < m_cursorRect.bottom; yf++, yc++) {
        // Check if we are done
        if (yf < 0) continue;
        if (yf >= m_gpeMode.height) break;

        pFrameLine = &pFrame[yf * bytesPerLine];
        pStoreLine = &m_cursorStore[yc * m_cursorSize.x * bytesPerPixel];
    
        for (
            xf = m_cursorRect.left, xc = 0; xf < m_cursorRect.right; xf++, xc++
        ) {
            // Check if we are done
            if (xf < 0) continue;
            if (xf >= m_gpeMode.width) break;
    
            // Depending on bytes per pixel
            switch (bytesPerPixel) {
            case 1:
                pFrameLine[xf] = pStoreLine[xc];
                break;
            case 2:
                ((USHORT*)pFrameLine)[xf] = ((USHORT*)pStoreLine)[xc];
                break;
            case 3:
                for (i = 0; i < bytesPerPixel; i++) {
                    data = pStoreLine[xc * bytesPerPixel + i];
                    pFrameLine[xf * bytesPerPixel + i] = data;
                }                    
                break;
            case 4:
                ((ULONG*)pFrameLine)[xf] = ((ULONG*)pStoreLine)[xc];
                break;
            }                    
        }
    }

    // Cursor isn't visible now
    m_cursorVisible = FALSE;

cleanUp: 
    DEBUGMSG(GPE_ZONE_PDD, (L"-FlatGPE::CursonOff\r\n"));
    return;
}

//------------------------------------------------------------------------------
//
//  Method:  SetPointerShape
//
//  This method sets the shape of the pointer, the hot spot of the pointer,
//  and the colors to use for the cursor if the cursor is multicolored.
//
SCODE FlatGPE::SetPointerShape(
    GPESurf* pMask, GPESurf* pColorSurf, int xHotspot, int yHotspot, 
    int xSize, int ySize 
) {
    SCODE sc = S_OK;
    UCHAR *pAndPtr, *pXorPtr, *pAndLine, *pXorLine;
    UCHAR andPtr, xorPtr, mask;
    ULONG size;
    int bytesPerPixel;
    int row, col, i;


    DEBUGMSG(GPE_ZONE_PDD, (
        L"+FlatGPE::SetPointerShape(0x%08x, 0x%08x, %d, %d, %d, %d)\r\n",
        pMask, pColorSurf, xHotspot, yHotspot, xSize, ySize
    ));

    // Turn current cursor off
    CursorOff();

    // Release memory associated with old cursor
    delete [] m_cursorStore; m_cursorStore = NULL;
    delete [] m_cursorXor; m_cursorXor = NULL;
    delete [] m_cursorAnd; m_cursorAnd = NULL;

    // Is there a new mask?
    if (pMask == NULL) {

        // No, so tag as disabled
        m_cursorDisabled = TRUE;

    } else {

        // Yes, so tag as not disabled
        m_cursorDisabled = FALSE;

        // Check if cursor size is correct
        if (xSize > m_nScreenWidth || ySize > m_nScreenHeight) {
            DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetPointerShape: "
                L"Invalid cursor size %d, %d\r\n", xSize, ySize
            ));
            sc = E_FAIL;
            goto cleanUp;
        }
        
        // How many bytes we need per pixel on screen
        bytesPerPixel = (m_gpeMode.Bpp + 7) >> 3;

        // Cursor mask & store size
        size = xSize * ySize * bytesPerPixel;
        
        // Allocate memory based on new cursor size
        m_cursorStore = new UCHAR[size];
        m_cursorXor   = new UCHAR[size];
        m_cursorAnd   = new UCHAR[size];

        if (
            m_cursorStore == NULL || m_cursorXor == NULL || m_cursorAnd == NULL
        ) {
            DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetPointerShape: "
                L"Memory allocation for cursor buffers failed\r\n"
            ));
            sc = E_OUTOFMEMORY;
            goto cleanUp;
        }

        // Store size and hotspot for new cursor
        m_cursorSize.x = xSize;
        m_cursorSize.y = ySize;
        m_cursorHotspot.x = xHotspot;
        m_cursorHotspot.y = yHotspot;

        // Pointers to AND and XOR masks
        pAndPtr = (UCHAR*)pMask->Buffer();
        pXorPtr = (UCHAR*)pMask->Buffer() + (ySize * pMask->Stride());

        // store OR and AND mask for new cursor
        for (row = 0; row < ySize; row++) {
            pAndLine = &m_cursorAnd[row * xSize * bytesPerPixel];
            pXorLine = &m_cursorXor[row * xSize * bytesPerPixel];
            for (col = 0; col < xSize; col++) {
                andPtr = pAndPtr[row * pMask->Stride() + (col >> 3)];
                xorPtr = pXorPtr[row * pMask->Stride() + (col >> 3)];
                mask = 0x80 >> (col & 0x7);
                for (i = 0; i < bytesPerPixel; i++) {
                    pAndLine[col * bytesPerPixel + i] = andPtr&mask?0xFF:0x00;
                    pXorLine[col * bytesPerPixel + i] = xorPtr&mask?0xFF:0x00;
                }                    
            }
        }
        
    }

cleanUp:
    DEBUGMSG(GPE_ZONE_PDD, (L"-FlatGPE::SetPointerShape(sc = 0x%08x)\r\n", sc));
    return sc;
}

//------------------------------------------------------------------------------
//
//  Method:  MovePointer
//
//  This method executes from applications either to move the hot spot
//  of the cursor to a specific screen location or to hide the cursor (x == -1).
//
SCODE FlatGPE::MovePointer(int x, int y)
{
    DEBUGMSG(GPE_ZONE_PDD, (L"+FlatGPE::MovePointer(%d, %d)\r\n", x, y));

    CursorOff();

    if (x != -1 || y != -1) {
        // Compute new cursor rect
        m_cursorRect.left = x - m_cursorHotspot.x;
        m_cursorRect.right = m_cursorRect.left + m_cursorSize.x;
        m_cursorRect.top = y - m_cursorHotspot.y;
        m_cursorRect.bottom = m_cursorRect.top + m_cursorSize.y;
        CursorOn();
    }

    DEBUGMSG(GPE_ZONE_PDD, (L"-FlatGPE::MovePointer(sc = 0x%08x)\r\n", S_OK));
    return  S_OK;
}

//------------------------------------------------------------------------------
//
//  Method:  InVBlank
//
int FlatGPE::InVBlank()
{
    return 1;
}

//------------------------------------------------------------------------------
//
//  Method:  SetPalette
//
SCODE FlatGPE::SetPalette(
    const PALETTEENTRY *pSource, WORD firstEntry, WORD numEntries
) {
    SCODE sc = S_OK;
    UINT8 *pBuffer = NULL;
    ULONG command, size, paletteSize;
    DDHAL_SET_PALETTE *pInfo;

    DEBUGMSG(GPE_ZONE_PDD, (
        L"+FlatGPE::SetPalette(0x%08x, %d, %d)\r\n", pSource, firstEntry,
        numEntries
    ));

    paletteSize = numEntries * sizeof(PALETTEENTRY);
    size = sizeof(DDHAL_SET_PALETTE) + paletteSize;
    pBuffer = new UINT8[size];
    if (pBuffer == 0) {
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetPalette: "
            L"Memory allocation for support buffer failed\r\n"
        ));
        sc = E_OUTOFMEMORY;
        goto cleanUp;
    }

    // Set palette info & copy palette
    pInfo = (DDHAL_SET_PALETTE *)pBuffer;
    pInfo->firstEntry = firstEntry;
    pInfo->entries = numEntries;
    memcpy(&pInfo[1], pSource, paletteSize);
    
    // Ask HAL to set palette
    command = DDHAL_COMMAND_SET_PALETTE;
    if (!KernelIoControl(
        IOCTL_HAL_DDI, &command, sizeof(command), pBuffer, size, NULL
    )) {
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetPalette: "
            L"IOCTL_HAL_DDI_SET_PALETTE failed\r\n"
        ));
        sc = E_FAIL;
        goto cleanUp;
    }

cleanUp:    
    DEBUGMSG(GPE_ZONE_PDD, (L"-FlatGPE::SetPalette(sc = 0x%08x)\r\n", sc));
    return sc;
}

//------------------------------------------------------------------------------
//
//  Method:  PowerHandler
//
VOID FlatGPE::PowerHandler(BOOL off)
{
    ULONG commands[2];
    
    // Ask HAL to set power
    commands[0] = DDHAL_COMMAND_POWER;
    commands[1] = off;
    if (!KernelIoControl(
        IOCTL_HAL_DDI, commands, sizeof(commands), NULL, 0, NULL
    )) {
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::PowerHandler: "
            L"IOCTL_HAL!DDI_POWER failed for off flag %d\r\n", off
        ));
    }
}

//------------------------------------------------------------------------------

