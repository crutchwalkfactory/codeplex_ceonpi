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

#define DMA_CHAN           1
#define DMA_SOLIDBUFF_CNT  8  //8 DWORDs is the smallest src buffer 

inline DWORD VAtoPA (DWORD p) {return (DWORD)p & 0x0fffffff;}

//------------------------------------------------------------------------------
//
//  Function:  BltInit
//
//  The GWES will invoke this routine once prior to making any other calls
//  into the driver. This routine needs to save its instance path information
//  and return TRUE.  If it returns FALSE, GWES will abort the display
//  initialization.
//
BOOL FlatGPE::BltInit(void)
{
	int rc = 0;
	DWORD dwOutSize = sizeof (DWORD);
	DWORD commands[4];
    DDHAL_MODE_INFO *pInfo;
	int modeNumber = 0;
	DWORD size = 256;
    UINT8 *pBuffer = NULL;
	MYHARDWAREPTRSTRUCT hwsHWData;
	
    DEBUGMSG(GPE_ZONE_PDD, (L"+BltInit\r\n"));
	
	//
    // Query HAL for video information
	//
    pBuffer = new UINT8[size];
    if (pBuffer == NULL) 
	{
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetMode: Failed allocate %d byte buffer\r\n", size));
        rc = E_FAIL;
        goto cleanUp;
    }

	// Ask HAL to reconfigure hardware
    commands[0] = DDHAL_COMMAND_GET_MODE_INFO;
    commands[1] = modeNumber;
    if (!KernelIoControl(IOCTL_HAL_DDI, commands, sizeof(commands), pBuffer, size, &size)) 
	{
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetMode: IOCTL_HAL_DDI!SET_MODE failed for mode %d\r\n", modeNumber));
        rc = E_FAIL;
        goto cleanUp;
    }        
	DEBUGMSG(1, (L"FlatGPE::SetMode: 6  rc %d\r\n", rc));

    // Save information
    pInfo = (DDHAL_MODE_INFO*)pBuffer;
	m_dmaVidData.nHeight = pInfo->height;
	m_dmaVidData.nWidth = pInfo->width;
	m_dmaVidData.nBPP = pInfo->bpp;
	m_dmaVidData.nBytesPerPixel = pInfo->bpp/8;
	m_dmaVidData.dwFrameStride = pInfo->frameStride;
	m_dmaVidData.dwFrameSize = pInfo->frameSize;
	m_dmaVidData.pFrame = (PBYTE)pInfo->phFrameBase;

	// Allocate the DMA control memory using Physical mem

	//
	// Get access to CPU regs
	//
    if (!KernelIoControl(IOCTL_HAL_MY_GET_HARDWARE_POINTERS, 0, 0, &hwsHWData, sizeof (hwsHWData), &size)) 
	{
        DEBUGMSG(GPE_ZONE_ERROR, (L"FlatGPE::SetMode: IOCTL_HAL_MY_GET_HARDWARE_POINTERS failed\r\n"));
        rc = E_FAIL;
        goto cleanUp;
    }        

	// Save virtual address of CPU regs
	m_dmaVidData.pRegBase = hwsHWData.pSOCRegs;
    if (!m_dmaVidData.pRegBase) 
	{
        goto cleanUp;
    }

	//
	// Alloc buff for DMA structs. Use a page since its the smallest we can allocate
	//
	DWORD dwPhys;
#define DMACTLBUFFSIZE  4096
	PBYTE pVirt = (PBYTE)AllocPhysMem (DMACTLBUFFSIZE, PAGE_READWRITE, 0, 0, (ULONG *) &dwPhys);

	// See how many DMA ctl structures we can fit.  Make sure we have room for the solid clr buff
	int cnt = (DMACTLBUFFSIZE - (sizeof (DWORD) * DMA_SOLIDBUFF_CNT)) / sizeof (DMACONBLKSTRUCT);

	// If we put the DMA Control Blocks at the start of hte page they'll be properly aligned
	DWORD dwOffset = (((DWORD)pVirt + DMACONBLK_ALIGNMENT) & DMACONBLK_MASK) - (DWORD) pVirt;
	PDMACONBLKSTRUCT pDMABlk = (PDMACONBLKSTRUCT) (pVirt + dwOffset);

	// Put the src buffer after the DMA control structure
	DWORD dwOffset2Color = (dwOffset + sizeof (DMACONBLKSTRUCT) + 3) & 0xfffffffc; // make dword aligned
	DWORD *pColor = (DWORD *)  (pVirt + dwOffset2Color);

	m_dmaVidData.nChan = DMA_CHAN;
	m_dmaVidData.nNumConBlks = cnt;
	m_dmaVidData.pVirtConBlock = (PDMACONBLKSTRUCT)pVirt;
	m_dmaVidData.dwPhysConBlock = dwPhys;
	m_dmaVidData.nSolidDataCnt = DMA_SOLIDBUFF_CNT;
	m_dmaVidData.pdwVirtSolidData = (DWORD *)(pVirt + (sizeof (DMACONBLKSTRUCT) * cnt));
	m_dmaVidData.dwPhysSolidData = dwPhys + (sizeof (DMACONBLKSTRUCT) * cnt);

	DEBUGMSG(1, (L" nHeight                 %d\r\n", m_dmaVidData.nHeight));
	DEBUGMSG(1, (L" nWidth                  %d\r\n", m_dmaVidData.nWidth));
	DEBUGMSG(1, (L" nBPP                    %x\r\n", m_dmaVidData.nBPP));
	DEBUGMSG(1, (L" nBytesPerPixel          %x\r\n", m_dmaVidData.nBytesPerPixel));
	DEBUGMSG(1, (L" dwFrameStride           %x\r\n", m_dmaVidData.dwFrameStride));
	DEBUGMSG(1, (L" dwFrameSize             %x\r\n", m_dmaVidData.dwFrameSize));
	DEBUGMSG(1, (L" pFrame                  %x\r\n", m_dmaVidData.pFrame));
	DEBUGMSG(1, (L" nChan                   %x\r\n", m_dmaVidData.nChan));
	DEBUGMSG(1, (L" nNumConBlks             %x\r\n", m_dmaVidData.nNumConBlks));
	DEBUGMSG(1, (L" pVirtConBlock           %x\r\n", m_dmaVidData.pVirtConBlock));
	DEBUGMSG(1, (L" dwPhysConBlock          %x\r\n", m_dmaVidData.dwPhysConBlock));
	DEBUGMSG(1, (L" nSolidDataCnt           %x\r\n", m_dmaVidData.nSolidDataCnt));
	DEBUGMSG(1, (L" pdwVirtSolidData        %x\r\n", m_dmaVidData.pdwVirtSolidData));
	DEBUGMSG(1, (L" dwPhysSolidData         %x\r\n", m_dmaVidData.dwPhysSolidData));
	DEBUGMSG(1, (L" sizeof(DMACONBLKSTRUCT) %d\r\n", sizeof(DMACONBLKSTRUCT)));

cleanUp:
    DEBUGMSG(GPE_ZONE_PDD, (L"-BltInit(rc = 1)\r\n"));
    return TRUE;
}


//------------------------------------------------------------------------------
//
//  Method:  BltPrepare
//
//  This method identifies the appropriate functions needed to perform
//  individual blits. This function executes before a sequence of clipped blit
//  operations.
//
SCODE FlatGPE::BltPrepare(GPEBltParms *pBltParms)
{
    RECTL rect;
    LONG swapTmp;
#ifdef ENABLE_FILL_DMAACCEL
	int cx, cy;
#endif

    pBltParms->pBlt = &GPE::EmulatedBlt;

    // Check if destination overlap with cursor
    if (
        pBltParms->pDst == m_pPrimarySurface &&
        m_cursorVisible && !m_cursorDisabled
    ) { 
        if (pBltParms->prclDst != NULL) {
            rect = *pBltParms->prclDst;     // if so, use it

            // There is no guarantee of a well
            // ordered rect in blitParamters
            // due to flipping and mirroring.
            if (rect.top > rect.bottom) {
                swapTmp = rect.top;
                rect.top = rect.bottom;
                rect.bottom = swapTmp;
            }
            if (rect.left > rect.right) {
                swapTmp    = rect.left;
                rect.left  = rect.right;
                rect.right = swapTmp;
            }
        } else {
            rect = m_cursorRect;
        }

        // Turn off cursor if it overlap
        if (
            m_cursorRect.top <= rect.bottom &&
            m_cursorRect.bottom >= rect.top &&
            m_cursorRect.left <= rect.right &&
            m_cursorRect.right >= rect.left
        ) {
            CursorOff();
            m_cursorForcedOff = TRUE;
        }
    }

    // Check if source overlap with cursor
    if (
        pBltParms->pSrc == m_pPrimarySurface &&
        m_cursorVisible && !m_cursorDisabled
    ) {
        if (pBltParms->prclSrc != NULL) {
            rect = *pBltParms->prclSrc;
        } else {
            rect = m_cursorRect;
        }
        if (
            m_cursorRect.top < rect.bottom && m_cursorRect.bottom > rect.top &&
            m_cursorRect.left < rect.right && m_cursorRect.right > rect.left
        ) {
            CursorOff();
            m_cursorForcedOff = TRUE;
        }
    }

    if ( EGPEFormatToBpp[pBltParms->pDst->Format()] == 24)
    {
#ifdef ENABLE_FILL_DMAACCEL
		PRECTL   prcDst;
#endif
    	switch( pBltParms->rop4 )
    	{
#ifdef ENABLE_FILL_DMAACCEL
    	case 0x0000:	// BLACKNESS
			prcDst = pBltParms->prclDst;
			if (prcDst)
			{
				if (pBltParms->pDst->Stride() < 0)
					break;
				cy = prcDst->bottom - prcDst->top;
				cx = prcDst->right  - prcDst->left;
				if (cx + cy > 16)
				{
    				DEBUGMSG(GPE_ZONE_BLT_LO,(TEXT("DMAFillRect24 - BLACKNESS\r\n")));
    				pBltParms->solidColor = 0;
    				pBltParms->pBlt = (SCODE (GPE::*)(struct GPEBltParms *))&FlatGPE::DMAFillRect24;
				}
			}
    		break;
    	case 0xFFFF:	// WHITENESS
			prcDst = pBltParms->prclDst;
			if (prcDst)
			{
				if (pBltParms->pDst->Stride() < 0)
					break;
				cy = prcDst->bottom - prcDst->top;
				cx = prcDst->right  - prcDst->left;
				if (cx + cy > 16)
				{
    				DEBUGMSG(GPE_ZONE_BLT_LO,(TEXT("DMAFillRect24 - WHITENESS\r\n")));
    				pBltParms->solidColor = 0x00ffffff;
    				pBltParms->pBlt = (SCODE (GPE::*)(struct GPEBltParms *))&FlatGPE::DMAFillRect24;
				}
			}
    		break;
    	case 0xF0F0:	// PATCOPY
    		if (pBltParms->solidColor != -1)
    		{
				prcDst = pBltParms->prclDst;
				if (prcDst)
				{
					if (pBltParms->pDst->Stride() < 0)
						break;
					cy = prcDst->bottom - prcDst->top;
					cx = prcDst->right  - prcDst->left;
					if (cx + cy > 16)
					{
	    				DEBUGMSG(GPE_ZONE_BLT_LO,(TEXT("DMAFillRect24 - PATCOPY - solid brush\r\n")));
						pBltParms->pBlt = (SCODE (GPE::*)(struct GPEBltParms *))&FlatGPE::DMAFillRect24;
					}
				}
    		}
    		break;
#endif  // ENABLE_FILL_DMAACCEL

#ifdef ENABLE_TEXT_DMAACCEL
    	// Special PATCOPY rop for text output -- fill where mask is 1.
		case 0xAAF0:
            // Not stretching?
    		if( !(pBltParms->bltFlags & BLT_STRETCH) )
    		{
                // Not a pattern brush?
        		if( pBltParms->solidColor != -1 )
                {
            		pBltParms->pBlt = (SCODE (GPE::*)(struct GPEBltParms *))DMATextBlt24;
                }
            }
		break;
#endif  // ENABLE_TEXT_DMAACCEL


    	default:
			break;
    	}
  	}

#ifdef ENABLE_SRC_COPY_DMAACCEL
    // Only accelerate 8bpp
    if ( EGPEFormatToBpp[pBltParms->pDst->Format()] == 24 &&
         pBltParms->pSrc                                 &&
         EGPEFormatToBpp[pBltParms->pSrc->Format()] == 24)
    {
        // Can only handle SRCCOPY, no conversion, no stretch, no transparency
	    if (pBltParms->rop4 == 0xCCCC                &&
            !pBltParms->pLookup                      &&
		    !pBltParms->pConvert                     &&
            !(pBltParms->bltFlags & BLT_TRANSPARENT) &&
		    !(pBltParms->bltFlags & BLT_STRETCH))
        {
   		    pBltParms->pBlt = (SCODE (GPE::*)(struct GPEBltParms *))DMASrcCopyBlt;
		}
	}
#endif // ENABLE_SRC_COPY_DMAACCEL

    return S_OK;
}

//------------------------------------------------------------------------------
//
//  Method:  BltComplete
//
//  This method executes to complete a blit sequence. 
//
SCODE FlatGPE::BltComplete(GPEBltParms *pBtlParms)
{
    // If cursor was forced off turn it back on
    if (m_cursorForcedOff) {
        m_cursorForcedOff = FALSE;
        CursorOn();
    }
    return S_OK;
}

#ifdef ENABLE_FILL_DMAACCEL
//------------------------------------------------------------------------------
//
//  Method:  BltComplete
//
//  This method executes to complete a blit sequence. 
//
SCODE FlatGPE::DMAFillRect24 (GPEBltParms *pBltParms)
{
	DEBUGMSG(GPE_ZONE_BLT_HI,(TEXT("EmulatedFillRect\r\n")));

    UINT32   iDstScanStride  = pBltParms->pDst->Stride();
    BYTE    *pDibBitsDst     = (BYTE *)pBltParms->pDst->Buffer();
    PRECTL   prcDst          = pBltParms->prclDst;
    int      nCY             = prcDst->bottom - prcDst->top;
    int      nCX             = prcDst->right  - prcDst->left;
    COLOR    rgbColor        = pBltParms->solidColor;
    BYTE    *pDest;

    // Caller assures a well-ordered, non-empty rect
    ASSERT(nCX > 0 && nCY > 0);

    // compute pointer to the starting row in the dst bitmap
    pDest = (BYTE *)pDibBitsDst + prcDst->top * iDstScanStride + (prcDst->left * m_dmaVidData.nBytesPerPixel);

	int rc = DMASolidClrFill (pDest, prcDst->left, prcDst->top, nCX, nCY, rgbColor);

	return S_OK;
}

//---------------------------------------------------------------------------------------
// DMASolidClrFill 
//
int FlatGPE::DMASolidClrFill (PBYTE pDest, int nX, int nY, int nCX, int nCY, COLORREF clr)
{
	int rc = 0;
	
	DEBUGMSG (GPE_ZONE_BLT_HI, (TEXT("DMASolidClrFill++ dst:%08x %d,%d cx:%d cy:%d clr:%06x\r\n"), pDest, nX, nY, nCX, nCY, clr));

	// Fill in the src color. The color needs to be packed on a byte basis
	m_dmaVidData.pdwVirtSolidData[0] = clr;

	//
	// Block 1.  This fills in the first row of the rectangle
	//

	// Init the Control block Transfer descriptor
	PDMACONBLKSTRUCT pDMABlk = (PDMACONBLKSTRUCT) (&m_dmaVidData.pVirtConBlock[0]);

	memset (pDMABlk, 0, sizeof (DMACONBLKSTRUCT));
	pDMABlk->XferInfo.bit.TDMode = 1;			// 01 Enable 2d mode
	pDMABlk->XferInfo.bit.DestInc = 1;			// 04 Set if dest address increments
	pDMABlk->XferInfo.bit.DestWidth = 0;		// 05 1-use 128 bit wide writes, 0-32 bit writes
	pDMABlk->XferInfo.bit.SrcInc = 1;			// 08 Set if src address increments
	pDMABlk->XferInfo.bit.SrcWidth = 0;			// 09 1-use 128 bit wide writes, 0-32 bit reads
	pDMABlk->XferInfo.bit.BurstLen = 0;			// 12-15 Burst length

	// Set the src and dest addresses, they must be true physical addresses	
	pDMABlk->dwSrcAddr = (DWORD)m_dmaVidData.dwPhysSolidData;
	//pDMABlk->dwDstAddr = (DWORD)m_dmaVidData.pFrame + (nX * m_dmaVidData.nBytesPerPixel) +
	//                                                (nY * m_dmaVidData.dwFrameStride);
	pDMABlk->dwDstAddr = (DWORD)pDest;

	// Hack way to convert the virtual frame buff address to a physical one
	pDMABlk->dwDstAddr = pDMABlk->dwDstAddr & 0x0fffffff;

	// Set the x and y length
	pDMABlk->XferLen.TdLen.XLen = m_dmaVidData.nBytesPerPixel;
	pDMABlk->XferLen.TdLen.YLen = nCX;

	// Set the stride
	pDMABlk->TdStride.SrcStride = -m_dmaVidData.nBytesPerPixel;
	pDMABlk->TdStride.DestStride = (WORD)0;

	//
	// Block 2. This replicates the first line vertically down the rectangle.
	//
	if (nCY > 1)
	{
		// Tie first block to the next
		pDMABlk->dwPhysNextConBk = m_dmaVidData.dwPhysConBlock + sizeof (DMACONBLKSTRUCT);

		// Init the second Control block Transfer descriptor
		pDMABlk = (PDMACONBLKSTRUCT) (&m_dmaVidData.pVirtConBlock[1]);

		memset (pDMABlk, 0, sizeof (DMACONBLKSTRUCT));
		pDMABlk->XferInfo.bit.TDMode = 1;			// 01 Enable 2d mode
		pDMABlk->XferInfo.bit.DestInc = 1;			// 04 Set if dest address increments
		pDMABlk->XferInfo.bit.DestWidth = 0;		// 05 1-use 128 bit wide writes, 0-32 bit writes
		pDMABlk->XferInfo.bit.SrcInc = 1;			// 08 Set if src address increments
		pDMABlk->XferInfo.bit.SrcWidth = 0;			// 09 1-use 128 bit wide writes, 0-32 bit reads
		pDMABlk->XferInfo.bit.BurstLen = 0;			// 12-15 Burst length

		// Set the src and dest addresse.
		// The src is the line of data written by the first control block
		//pDMABlk->dwSrcAddr = (DWORD)m_dmaVidData.pFrame + (nX * m_dmaVidData.nBytesPerPixel) +
		//												(nY * m_dmaVidData.dwFrameStride);
		//
		//pDMABlk->dwDstAddr = (DWORD)m_dmaVidData.pFrame + (nX * m_dmaVidData.nBytesPerPixel) +
		//												((nY+1) * m_dmaVidData.dwFrameStride);

		pDMABlk->dwSrcAddr = (DWORD)pDest;
		
		pDMABlk->dwDstAddr = (DWORD)pDest + m_dmaVidData.dwFrameStride;

		// Hack way to convert the virtual frame buff address to a physical one
		pDMABlk->dwDstAddr = VAtoPA (pDMABlk->dwDstAddr);
		pDMABlk->dwSrcAddr = VAtoPA (pDMABlk->dwSrcAddr);

		// Set the x and y length
		pDMABlk->XferLen.TdLen.XLen = nCX * m_dmaVidData.nBytesPerPixel;
		pDMABlk->XferLen.TdLen.YLen = nCY-1;

		// Set the stride
		pDMABlk->TdStride.SrcStride = -(nCX * m_dmaVidData.nBytesPerPixel);
		pDMABlk->TdStride.DestStride = (WORD)(m_dmaVidData.dwFrameStride - (DWORD)pDMABlk->XferLen.TdLen.XLen);
	}

	// Terminate block chain
	pDMABlk->dwPhysNextConBk = NULL; 

	// Dispatch the blocks to the DMA hardware
	rc = DispatchConBlk2DMAChan (m_dmaVidData.dwPhysConBlock, m_dmaVidData.nChan, TRUE);

	DEBUGMSG (GPE_ZONE_BLT_HI, (TEXT("DMASolidClrFill-- rc %d\r\n"),rc));
	return rc;
}
//---------------------------------------------------------------------------------------
// DispatchConBlk2DMAChan
//
int FlatGPE::DispatchConBlk2DMAChan (DWORD dwPhysDMABlk, int nChan, BOOL fWaitForComplete)
{
	int rc = 0;

	PBYTE pDMA = (PBYTE)GetDMARegPtr (m_dmaVidData.pRegBase, nChan);
	DWORD dw;
	DMACTLSTATREG ChStatReg;
	DWORD *pConBlkAdr;
	if (pDMA)
	{
		PDWORD pDMAEn = (PDWORD)GetDMARegPtr (m_dmaVidData.pRegBase, DMA_GLOB_EN_REG);

		// BUGBUG:: Don't really need to enable channel for each action
		// Enable the DMA channel
		dw = (*pDMAEn & 0xffff) | 1 << m_dmaVidData.nChan;
		*(DWORD *)pDMAEn = dw;

		// Write the control block ptr
		pConBlkAdr = (DWORD*)(pDMA + DMA_OFF_CONBLK_AD);
		*pConBlkAdr = dwPhysDMABlk;

		// Init the DMA chan status/ctl reg
		ChStatReg.ul = 0;
		ChStatReg.bit.Active = 1; 	
		ChStatReg.bit.Priority = 0;
		ChStatReg.bit.PanicPriority = 0;

		// Write the status reg to start the transfer
		*(DWORD *)(pDMA + DMA_OFF_CS) = ChStatReg.ul;

		//
		// BUGBUG:: make this interrupt driven
		//
		if (fWaitForComplete)
		{
			// Wait for complete
			for (int i = 0; (i < 100000); i++)
			{
				Sleep (0);
				ChStatReg.ul = *(DWORD *)(pDMA + DMA_OFF_CS);
				if (ChStatReg.bit.Active == 0)
				{
					break;
				}
			}
		}
	}
	return rc;
}




#endif //ENABLE_FILL_DMAACCEL
