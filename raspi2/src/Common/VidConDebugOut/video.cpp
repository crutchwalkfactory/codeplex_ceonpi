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
#include "mailbox.h"
#include "Debugfuncs.h"
#include <image_cfg.h>
#include "video.h"

//
// Structure used by video mailbox
//
typedef struct _mailbox_frambuff_struct {
	DWORD	dwWidth;
	DWORD	dwHeight;
	DWORD	dwVirtWidth;
	DWORD	dwVirtHeight;
	DWORD	dwPitch;
	DWORD	dwDepth;
	DWORD	dwXOffset;
	DWORD	dwYOffset;
	BYTE *	pFrameBuff;
	DWORD	dwFrameBuffSize;
} MAILBOXFRAMEBUFFSTRUCT, *PMAILBOXFRAMEBUFFSTRUCT;


typedef void (VIDSETPROC)(HSURF, int, int, int, int, DWORD);
typedef void (VIDCOPYPROC)(HSURF, int, int, int, int, HSURF, int, int);
typedef void (VIDTXTBLTPROC)(HSURF, int, int, int, int, HSURF, int, int, DWORD, DWORD);

typedef struct {
	VIDSETPROC    *pSetBlt;
	VIDCOPYPROC   *pCopyBlt;
	VIDTXTBLTPROC *pTxtBlt;
} VIDBLTPROCSTRUCT, *PVIDBLTPROCSTRUCT;

VIDBLTPROCSTRUCT BltPtrs;

// Structure containing info about the video system
SURFINFO g_vbInfo;

//BYTE bBuff1[sizeof (MAILBOXFRAMEBUFFSTRUCT)+15];

//----------------------------------------------------------------------------------
// 16 bit Blts
//
void VidSet_16(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, DWORD dwColor);
void VidCopy_16(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
				  HSURF hSrc,  int nXSrc, int nYSrc);
void VidTxtBlt_16(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
				  HSURF hSrc,  int nXSrc, int nYSrc, DWORD dwForeClr, DWORD dwBkClr);

//----------------------------------------------------------------------------------
// 24 bit Blts
//
void VidSet_24(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, DWORD dwColor);
void VidCopy_24(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
				HSURF hSrc,  int nXSrc, int nYSrc);
void VidTxtBlt_24(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
				  HSURF hSrc,  int nXSrc, int nYSrc, DWORD dwForeClr, DWORD dwBkClr);

void VidXBlt_i(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
			 HSURF hSrc,  int nXSrc, int nYSrc, DWORD dwXClr);
//----------------------------------------------------------------------------------
// InitVideoSystem
//
int InitVideoSystem(int nWidth, int nHeight, int nBitsPerPixel, HSURF *phVidFrame, BOOL fVirtualMem)
{
	int nRetryCnt, rc = -1;
	DWORD dwData, dwFill;
	PMAILBOXFRAMEBUFFSTRUCT pFBS;

	//Debug (TEXT("InitVideoSystem++\r\n"));

	// Get a pointer to a safe place to put this structure
	rc = MailboxGetMailboxBuffer (TRUE, (DWORD *)&pFBS, &dwData);

	nRetryCnt = 200;
	while (nRetryCnt > 0)
	{
		pFBS->dwWidth = nWidth;
		pFBS->dwHeight = nHeight;
		pFBS->dwVirtWidth = nWidth;
		pFBS->dwVirtHeight = nHeight;
		pFBS->dwPitch = 0;
		pFBS->dwDepth = nBitsPerPixel;
		pFBS->dwXOffset = 0;
		pFBS->dwYOffset = 0;
		pFBS->pFrameBuff = 0;
		pFBS->dwFrameBuffSize = 0;

		// Send the info to the video subsystem
		rc = MailboxSendMail (0, MAILBOX_CHAN_FRAMEBUFF, (DWORD)pFBS >> 4);
		if (rc == 0)
		{
			// Read back ack from the video subsystem. The data is updated in the existing structure.  
			// The value read is simply a return code.
			rc = MailboxGetMail (0, MAILBOX_CHAN_FRAMEBUFF, &dwData);
			if (rc != 0)
			{
				Debug (TEXT("Read failed\r\n"), nRetryCnt);
			}

			// Only leave if we have a frame buffer.
			if (pFBS->pFrameBuff != 0)
				break;
		}
		else
		{
			Debug (TEXT("Write failed\r\n"), nRetryCnt);
		}
		nRetryCnt--;
	}
	//Debug (TEXT("Framebuff == %x retry cnt %d\r\n"), pFBS->pFrameBuff, nRetryCnt);

	//Debug (TEXT("pFBS->dwWidth = %x (%d)\r\n"), pFBS->dwWidth, pFBS->dwWidth);
	//Debug (TEXT("pFBS->dwHeight = %x (%d)\r\n"), pFBS->dwHeight, pFBS->dwHeight);
	//Debug (TEXT("pFBS->dwVirtWidth = %x\r\n"), pFBS->dwVirtWidth);
	//Debug (TEXT("pFBS->dwVirtHeight = %x\r\n"), pFBS->dwVirtHeight);
	//Debug (TEXT("pFBS->dwPitch = %x\r\n"), pFBS->dwPitch);
	//Debug (TEXT("pFBS->dwDepth = %x\r\n"), pFBS->dwDepth);
	//Debug (TEXT("pFBS->dwXOffset = %x\r\n"), pFBS->dwXOffset);
	//Debug (TEXT("pFBS->dwYOffset = %x\r\n"), pFBS->dwYOffset);
	//Debug (TEXT("pFBS->pFrameBuff = %x\r\n"), pFBS->pFrameBuff);
	//Debug (TEXT("pFBS->dwFrameBuffSize = %x\r\n"), pFBS->dwFrameBuffSize);

	// See if we have a valid frame buffer
	if (pFBS->pFrameBuff != 0)
	{
		//
		// The value returned is in the 0x4xxxxxxx range.  Mask off the 4xxx xxxx and
		// or it into the Axxx xxxx block.  This means that the full RAM range must
		// be included in the OEMAddressTable.
		//
		PVOID pUVA;
		DWORD dwPhysBuff;
		if (fVirtualMem)
		{
			dwPhysBuff = (DWORD)pFBS->pFrameBuff;
			pFBS->pFrameBuff = (PBYTE)((DWORD)pFBS->pFrameBuff & 0x1fffffff);
			pUVA = (PBYTE)((DWORD)pFBS->pFrameBuff | 0xA0000000);

			//Debug (TEXT("pFrameBuff      virt:%x phys:%x  size:%d\r\n"), pUVA, pFBS->pFrameBuff, pFBS->dwFrameBuffSize);
			//Debug (TEXT("pFrameBuff end  virt:%x phys:%x\r\n"), (DWORD)pUVA+pFBS->dwFrameBuffSize, (DWORD)pFBS->pFrameBuff+pFBS->dwFrameBuffSize);

			g_vbInfo.wID = 'SF';
			g_vbInfo.nWidth = pFBS->dwWidth;
			g_vbInfo.nHeight = pFBS->dwHeight;
			g_vbInfo.nBitsPerPixel = pFBS->dwDepth;
			g_vbInfo.dwStride = pFBS->dwPitch;
			//g_vbInfo.dwPhysFrameBuffer = dwPhysBuff;
			g_vbInfo.pBuffer = (PBYTE)pUVA;
			g_vbInfo.dwVidBuffSize = pFBS->dwFrameBuffSize;

		}
		else
		{
			g_vbInfo.wID = 'SF';
			g_vbInfo.nWidth = pFBS->dwWidth;
			g_vbInfo.nHeight = pFBS->dwHeight;
			g_vbInfo.nBitsPerPixel = pFBS->dwDepth;
			g_vbInfo.dwStride = pFBS->dwPitch;
			g_vbInfo.pBuffer = pFBS->pFrameBuff;
			//g_vbInfo.dwPhysFrameBuffer = pFBS->pFrameBuff;
			g_vbInfo.dwVidBuffSize = pFBS->dwFrameBuffSize;
		}

		// Return the frame buffer info as a 'surface'.
		*phVidFrame = (HSURF)&g_vbInfo;

		switch (g_vbInfo.nBitsPerPixel)
		{
		case 24:
			BltPtrs.pSetBlt = &VidSet_24;
			BltPtrs.pCopyBlt = &VidCopy_24;
			BltPtrs.pTxtBlt = &VidTxtBlt_24;
			dwFill = 0x00ff0000;
			break;
		case 16:
			BltPtrs.pSetBlt = &VidSet_16;
			BltPtrs.pCopyBlt = &VidCopy_16;
			BltPtrs.pTxtBlt = &VidTxtBlt_16;
			dwFill = 0x003f;
			break;
		default:
			Debug (TEXT("***** ERROR ***** Unsupported pixel depth %d (0x%x)\r\n"), g_vbInfo.nBitsPerPixel, g_vbInfo.nBitsPerPixel);

		}

		// Clear the screen
		VidSet((HSURF)&g_vbInfo, 0, 0, g_vbInfo.nWidth, g_vbInfo.nHeight, dwFill);
		rc = 0;
	}
	else
	{
		Debug (TEXT("***** ERROR ***** Failed to get frame buffer pointer! Is the video monitor attached?\r\n"), rc);
	}

	//Debug (TEXT("InitVideoSystem-- rc=%d\r\n"), rc);
	return rc;
}
// ---------------------------------------------------------------------------
// GetVideoSettings 
//
// This function returns the video settings. It's called by a HAL IOCTL
// call when the vidoe driver loads.
//
BOOL GetVideoSettings (PMYDISPLAYSETTTNGS pSettings) 
{
    BOOL rc = FALSE;

	//Debug (L"GetVideoSettings++\r\n");
	if (g_vbInfo.nWidth != 0)
	{
		pSettings->nWidth = g_vbInfo.nWidth;
		pSettings->nHeight = g_vbInfo.nHeight;
		pSettings->nBitsPerPixel = g_vbInfo.nBitsPerPixel;
		pSettings->dwStride = g_vbInfo.dwStride;
		pSettings->pFrameBuffer = g_vbInfo.pBuffer;
		pSettings->dwFBSize = g_vbInfo.dwVidBuffSize;
		if (g_vbInfo.nBitsPerPixel == 24)
		{
			pSettings->dwColorMaskRed = 0xff000000;
			pSettings->dwColorMaskGreen = 0x00ff0000;
			pSettings->dwColorMaskBlue = 0x0000ff00;
			pSettings->dwColorMaskAlpha = 0;
		}
		else if (g_vbInfo.nBitsPerPixel == 16)
		{
			//pSettings->dwColorMaskRed =   0x0000f800;
			//pSettings->dwColorMaskGreen = 0x000007e0;
			//pSettings->dwColorMaskBlue =  0x0000001f;

			pSettings->dwColorMaskRed =   0x0000f800;
			pSettings->dwColorMaskGreen = 0x000007e0;
			pSettings->dwColorMaskBlue =  0x0000001f;
			pSettings->dwColorMaskAlpha = 0;
		}
		rc = TRUE;
	} else
		Debug (L"Error calling GetVideoSettings. Video not initialized yet.\r\n", rc);

	//Debug (L"GetVideoSettings-- rc:%d\r\n", rc);
	return rc;
}


//==================================================================================
// Generic simple blits
//==================================================================================
//----------------------------------------------------------------------------------
// VidSet - Simple setblt. 
//
void VidSet(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, DWORD dwColor)
{
	(BltPtrs.pSetBlt)(hDest, nXDest, nYDest, nCX, nCY, dwColor);
}

//----------------------------------------------------------------------------------
// VidCopy - Simple copyblt.  
//
void VidCopy(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
			 HSURF hSrc,  int nXSrc, int nYSrc)
{
	(BltPtrs.pCopyBlt)(hDest, nXDest, nYDest, nCX, nCY, hSrc, nXSrc, nYSrc);
	//VidCopy_16(hDest, nXDest, nYDest, nCX, nCY, hSrc, nXSrc, nYSrc);
}

//----------------------------------------------------------------------------------
// VidTxtBlt - provides foreground/background colors for fonts
//
void VidTxtBlt(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
			   HSURF hSrc,  int nXSrc, int nYSrc, DWORD dwForeClr, DWORD dwBkClr)
{
	(BltPtrs.pTxtBlt)(hDest, nXDest, nYDest, nCX, nCY, hSrc, nXSrc, nYSrc, dwForeClr, dwBkClr);
	//VidTxtBlt_16(hDest, nXDest, nYDest, nCX, nCY, hSrc, nXSrc, nYSrc);
}

//----------------------------------------------------------------------------------
// VidTxtBlt - Slower copyblt that understands bitmaps
//
void VidXBlt(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
			 HSURF hSrc,  int nXSrc, int nYSrc, DWORD dwXClr)
{
	VidXBlt_i(hDest, nXDest, nYDest, nCX, nCY, hSrc, nXSrc, nYSrc, dwXClr);
}

//==================================================================================
// 16 bit simple blits
//==================================================================================
//----------------------------------------------------------------------------------
// VidSet_16 - Simple setblt.  Assumes we're writing to the frame buffer
//
void VidSet_16(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, DWORD dwColor)
{
	int i, j;
	PSURFINFO pDestSurf = (PSURFINFO)hDest;

	BYTE *pFB = pDestSurf->pBuffer;
	BYTE *pDestRow;

	PBYTE pDColStart = pDestSurf->pBuffer + (nXDest * pDestSurf->nBitsPerPixel/8);

	//Debug (TEXT("VidSet_16++ hDest:%08x x=%d y=%d cx=%d cy=%d pdst=%x ColStart %x\r\n"), 
	//       hDest, nXDest, nYDest, nCX, nCY, pDestSurf->pBuffer, pDColStart);

	// For each row
	for (i = nYDest; i < (int)nCY + nYDest; i++)
	{
		// Each col
		pDestRow = pDColStart + ((DWORD)i * pDestSurf->dwStride);
		//Debug (TEXT("r:%d pDestRow=%x \r\n"), i, pDestRow);
		
		for (j = nXDest; j < (int)nCX + nXDest; j++)
		{
			*(WORD *)pDestRow = (WORD)dwColor;
			pDestRow += 2;
		}
	}
	return;
}
//----------------------------------------------------------------------------------
// VidCopy_16 - Simple copyblt.  
//
//
void VidCopy_16(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
				HSURF hSrc,  int nXSrc, int nYSrc)
{
	int i, j, k;
	PSURFINFO pDestSurf = (PSURFINFO)hDest;
	PSURFINFO pSrcSurf = (PSURFINFO)hSrc;
	BYTE *pDestRow, *pSrcRow;

	// Compute starting column offset
	PBYTE pDColStart = pDestSurf->pBuffer + (nXDest * pDestSurf->nBitsPerPixel/8);
	PBYTE pSColStart = pSrcSurf->pBuffer +  (nXSrc *  pSrcSurf->nBitsPerPixel/8);

	//Debug (TEXT("VidCopy_16++ x=%d y=%d cx=%d cy=%d pSrc=%x %d %d pD=%x pS=%x\r\n"), nXDest, nYDest, nCX, nCY, pSrcSurf->pBuffer, 
	//       nXSrc, nYSrc, pDColStart, pSColStart);

	int nWidth = nXDest + nCX;
	// See if src and dest start on both even or odd pixels
	if (((nXDest & 1) ^ (nXSrc & 1)) == 0)
	{
		int nBlk = nWidth / 2;
		int nRem = nWidth % 2;
		// If we have an odd start and even blocks, we need to dec blks by 1 since singles are on both ends
		if (((nBlk % 2) == 0) && (nXDest & 1))
			nBlk--;

		// Process the copy mostly with DWORD copies
		for (i = nYDest, k = nYSrc; i < (int)nYDest + nCY; i++, k++)
		{
			// Each col
			pDestRow = pDColStart + ((DWORD)i * pDestSurf->dwStride);
			pSrcRow =  pSColStart + ((DWORD)k * pSrcSurf->dwStride);

			// See if starting on an odd pixel
			if (nXDest & 1)
			{
				*((WORD *)pDestRow) = *((WORD *)pSrcRow);
				pDestRow += 2;
				pSrcRow += 2;
				nRem--;
			}
			// Copy bulk of row with DWORDS
			for (j = 0; j < nBlk; j++)
			{
				*((DWORD *)pDestRow) = *((DWORD *)pSrcRow);
				pDestRow += 4;
				pSrcRow += 4;
			}
			// If started even but odd rows, copy last pixel
			if (nRem)
			{
				*((WORD *)pDestRow) = *((WORD *)pSrcRow);
			}
		}
	}
	else
	{
		// simple and slow case, copy each pixel individually
		for (i = nYDest, k = nYSrc; i < (int)nYDest + nCY; i++, k++)
		{
			// Each col
			pDestRow = pDColStart + ((DWORD)i * pDestSurf->dwStride);
			pSrcRow =  pSColStart + ((DWORD)k * pSrcSurf->dwStride);

			//Debug (TEXT("%d %d dst:%x src:%x\r\n"), i, k, pDestRow, pSrcRow);

			for (j = nXDest; j < nWidth; j++)
			{
				*((WORD *)pDestRow) = *((WORD *)pSrcRow);
				pDestRow += 2;
				pSrcRow += 2;
			}
		}
	}
	return;
}
//----------------------------------------------------------------------------------
// VidTxtBlt_16 - Converts the source (font) bitmap into a font. The font pixels 
// are considered monochrome whatever the bit count for the BMP. The font bmp
// has a white background and black letters.
//
void VidTxtBlt_16(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
				  HSURF hSrc,  int nXSrc, int nYSrc, DWORD dwForeClr, DWORD dwBkClr)
{
	int i, j, k;
	PSURFINFO pDestSurf = (PSURFINFO)hDest;
	PSURFINFO pSrcSurf = (PSURFINFO)hSrc;
	int nBytesPerPixel;

	nBytesPerPixel = pSrcSurf->nBitsPerPixel/8;

	//Debug (TEXT("VidTxtBlt_16++ x=%d y=%d cx=%d cy=%d pSrc=%x %d %d \r\n"), nXDest, nYDest, nCX, nCY, pSrcSurf->pBuffer, nXSrc, nYSrc);

	BYTE *pDestBuff = pDestSurf->pBuffer;
	BYTE *pDestRow, *pSrcRow;

	// Compute starting column offset
	PBYTE pDColStart = pDestSurf->pBuffer + (nXDest * pDestSurf->nBitsPerPixel/8);
	PBYTE pSColStart = pSrcSurf->pBuffer +  (nXSrc *  pSrcSurf->nBitsPerPixel/8);

	for (i = nYDest, k = nYSrc; i < (int)nYDest + nCY; i++, k++)
	{
		// Each col
		pDestRow = pDColStart + ((DWORD)i * pDestSurf->dwStride);
		pSrcRow =  pSColStart + ((DWORD)k * pSrcSurf->dwStride);

		//Debug (TEXT("%d %d dst:%x src:%x\r\n"), i, k, pDestRow, pSrcRow);

		for (j = nXDest; j < (int)nXDest + nCX; j++)
		{
			// Look at 1st byte of font pixel for B/W
			if (*pSrcRow == 0)
				*((WORD *)pDestRow) = (WORD)dwForeClr;
			else
				*((WORD *)pDestRow) = (WORD)dwBkClr;

			pDestRow += 2;
			pSrcRow += nBytesPerPixel;
		}
	}
	return;
}

//==================================================================================
// 24 bit simple blits
//==================================================================================
//----------------------------------------------------------------------------------
// VidSet_24 - Simple setblt.  Assumes we're writing to the frame buffer
//
void VidSet_24(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, DWORD dwColor)
{
	int i, j;
	PSURFINFO pDestSurf = (PSURFINFO)hDest;

	BYTE *pFB = pDestSurf->pBuffer;
	BYTE *pDestRow, R, G, B;

	Debug (TEXT("VidSet_24++ x=%d y=%d cx=%d cy=%d clr=%x\r\n"), nXDest, nYDest, nCX, nCY, dwColor);

	R = dwColor & 0xff;
	G = (dwColor >> 8) & 0xff;
	B = (dwColor >> 16) & 0xff;

	Debug (TEXT("pbuff=%x stride=%x clr=%x.%x.%x\r\n"), pFB, pDestSurf->dwStride, R, G, B);

	PBYTE pDColStart = pDestSurf->pBuffer + (nXDest * pDestSurf->nBitsPerPixel/8);
	// For each row
	for (i = nYDest; i < (int)nCY + nYDest; i++)
	{
		// Each col
		pDestRow = pDColStart + ((DWORD)i * pDestSurf->dwStride);
		
		for (j = nXDest; j < (int)nCX + nXDest; j++)
		{
			*pDestRow++ = R;
			*pDestRow++ = G;
			*pDestRow++ = B;
		}
	}
	return;
}

//----------------------------------------------------------------------------------
// VidCopy_24 - Simple copyblt.  This assumes that the src surface is the same
// format as the destination and that the destination is the frame buffer.
//
//
void VidCopy_24(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
				HSURF hSrc,  int nXSrc, int nYSrc)
{
	int d, j, s;
	//PBITMAPFILEHEADER pbmfh;
	//PBITMAPINFOHEADER pbmih;
	PSURFINFO pDestSurf = (PSURFINFO)hDest;
	PSURFINFO pSrcSurf = (PSURFINFO)hSrc;
	int nSrcStart, nSrcInc;

	nSrcStart = nYSrc;
	nSrcInc = 1;

	BYTE *pDestBuff = pDestSurf->pBuffer;
	BYTE *pDestRow, *pSrcRow;

	// Compute starting column offset
	PBYTE pDColStart = pDestSurf->pBuffer + (nXDest * pDestSurf->nBitsPerPixel/8);
	PBYTE pSColStart = pSrcSurf->pBuffer +  (nXSrc *  pSrcSurf->nBitsPerPixel/8);

	//Debug (TEXT("VidCopy_24++ x=%d y=%d cx=%d cy=%d pSrc=%x %d %d pD=%x pS=%x\r\n"), nXDest, nYDest, nCX, nCY, pSrcSurf->pBuffer, 
	//       nXSrc, nYSrc, pDColStart, pSColStart);

	int nWidth = nXDest + nCX;
	int nBlk = nWidth / 4;
	int nRem = nWidth % 4;

	for (d = nYDest, s = nSrcStart; d < (int)nYDest + nCY; d++, s += nSrcInc)
	{
		// Each col
		pDestRow = pDColStart + ((DWORD)d * pDestSurf->dwStride);
		pSrcRow =  pSColStart + ((DWORD)s * pSrcSurf->dwStride);

		//Debug (TEXT("%d %d dst:%x src:%x\r\n"), i, k, pDestRow, pSrcRow);

//		for (j = nXDest; j < (int)nXDest + nCX; j++)
		for (j = 0; j < nBlk; j++)
		{
			*((DWORD *)pDestRow) = *((DWORD *)pSrcRow);
			*((DWORD *)(pDestRow+4)) = *((DWORD *)(pSrcRow+4));
			*((DWORD *)(pDestRow+8)) = *((DWORD *)(pSrcRow+8));
			pDestRow += 12;
			pSrcRow += 12;

		}
		for (j = 0; j < nRem; j++)
		{
			*pDestRow++ = *pSrcRow++;
			*pDestRow++ = *pSrcRow++;
			*pDestRow++ = *pSrcRow++;
		}
	}
	return;
}

//----------------------------------------------------------------------------------
// VidTxtBlt_24 - Converts the source (font) bitmap into a font. The font pixels 
// are considered monochrome whatever the bit count for the BMP. The font bmp
// has a white background and black letters.
//
void VidTxtBlt_24(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
				  HSURF hSrc,  int nXSrc, int nYSrc, DWORD dwForeClr, DWORD dwBkClr)
{
	int i, j, k;
	PSURFINFO pDestSurf = (PSURFINFO)hDest;
	PSURFINFO pSrcSurf = (PSURFINFO)hSrc;
	BYTE fR,fG,fB,bR,bG,bB;

	//Debug (TEXT("VidTxtBlt_24++. x=%d y=%d cx=%d cy=%d\r\n"), nXDest, nYDest, nCX, nCY);

	fR = dwForeClr & 0xff;
	fG = (dwForeClr >> 8) & 0xff;
	fB = (dwForeClr >> 16) & 0xff;

	bR = dwBkClr & 0xff;
	bG = (dwBkClr >> 8) & 0xff;
	bB = (dwBkClr >> 16) & 0xff;

	int nBytesPerPixel = pSrcSurf->nBitsPerPixel/8;

	//Debug (TEXT("VidTxtBlt_24++ x=%d y=%d cx=%d cy=%d pSrc=%x %d %d \r\n"), nXDest, nYDest, nCX, nCY, pSrcSurf->pBuffer, nXSrc, nYSrc);

	BYTE *pDestBuff = pDestSurf->pBuffer;
	BYTE *pDestRow, *pSrcRow;

	// Compute starting column offset
	PBYTE pDColStart = pDestSurf->pBuffer + (nXDest * pDestSurf->nBitsPerPixel/8);
	PBYTE pSColStart = pSrcSurf->pBuffer +  (nXSrc *  pSrcSurf->nBitsPerPixel/8);

	int nWidth = nXDest + nCX;
	int nBlk = nWidth / 4;
	int nRem = nWidth % 4;

	for (i = nYDest, k = nYSrc; i < (int)nYDest + nCY; i++, k++)
	{
		// Each col
		pDestRow = pDColStart + ((DWORD)i * pDestSurf->dwStride);
		pSrcRow =  pSColStart + ((DWORD)k * pSrcSurf->dwStride);

		//Debug (TEXT("%d %d dst:%x src:%x\r\n"), i, k, pDestRow, pSrcRow);
		for (j = nXDest; j < (int)nXDest + nCX; j++)
		{
			//We don't care the color of the pixel. If it
			if (*pSrcRow == 0)
			{
				*pDestRow++ = fR;
				*pDestRow++ = fG;
				*pDestRow++ = fB;
			}
			else
			{
				*pDestRow++ = bR;
				*pDestRow++ = bG;
				*pDestRow++ = bB;
			}
			pSrcRow += nBytesPerPixel;
		}
	}
	return;
}
//----------------------------------------------------------------------------------
// VidCopy_24 - Simple textblt.  This assumes that the src surface is the same
// format as the destination and that the destination is the frame buffer.
//
//
void VidTxtBlt_24sav(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
				  HSURF hSrc,  int nXSrc, int nYSrc)
{
	int i, j, k;
	PBITMAPFILEHEADER pbmfh;
	PBITMAPINFOHEADER pbmih;
	PSURFINFO pDestSurf = (PSURFINFO)hDest;
	PSURFINFO pSrcSurf = (PSURFINFO)hSrc;

	SURFINFO siSrc;			// Needed if src is a bitmap

	// The source can be a surface or a Bitmap file. See which it is
	pbmfh = (PBITMAPFILEHEADER)hSrc;
	if (pbmfh->bfType == 'MB')
	{
		pbmih = (PBITMAPINFOHEADER)(hSrc + sizeof (BITMAPFILEHEADER));

		// Create a fake surf struct.
		siSrc.nWidth = pbmih->biWidth;
		siSrc.nHeight = pbmih->biHeight;
		siSrc.nBitsPerPixel = pbmih->biBitCount;
		siSrc.dwStride = ((pbmih->biWidth * (pbmih->biBitCount/8)) + 3) & 0xfffffffc;
		siSrc.pBuffer = (PBYTE)hSrc + pbmfh->bfOffBits;
		siSrc.dwVidBuffSize = pbmfh->bfSize;

		pSrcSurf = &siSrc;
	}
	int nBytesPerPixel = pbmih->biBitCount/8;

	//Debug (TEXT("VidTxtBlt_24++ x=%d y=%d cx=%d cy=%d pSrc=%x %d %d \r\n"), nXDest, nYDest, nCX, nCY, pSrcSurf->pBuffer, nXSrc, nYSrc);

	BYTE *pDestBuff = pDestSurf->pBuffer;
	BYTE *pDestRow, *pSrcRow;

	// Compute starting column offset
	PBYTE pDColStart = pDestSurf->pBuffer + (nXDest * pDestSurf->nBitsPerPixel/8);
	PBYTE pSColStart = pSrcSurf->pBuffer +  (nXSrc *  pSrcSurf->nBitsPerPixel/8);

	int nWidth = nXDest + nCX;
	int nBlk = nWidth / 4;
	int nRem = nWidth % 4;

	for (i = nYDest, k = nYSrc; i < (int)nYDest + nCY; i++, k++)
	{
		// Each col
		pDestRow = pDColStart + ((DWORD)i * pDestSurf->dwStride);
		pSrcRow =  pSColStart + ((DWORD)k * pSrcSurf->dwStride);

		//Debug (TEXT("%d %d dst:%x src:%x\r\n"), i, k, pDestRow, pSrcRow);

		for (j = nXDest; j < (int)nXDest + nCX; j++)
		{
			*pDestRow++ = *pSrcRow++;
			*pDestRow++ = *pSrcRow++;
			*pDestRow++ = *pSrcRow++;
		}
	}
	return;
}

//----------------------------------------------------------------------------------
// VidXBlt_i - Draws an 8 bit bitmap onto either a 24 or 16 bpp surface. Used for
// splash screen display. Supports a transparent color.
//
void VidXBlt_i(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
			 HSURF hSrc,  int nXSrc, int nYSrc, DWORD dwXClr)
{
	int d, j, s;
	int nSrcStart, nSrcInc;
	PBITMAPFILEHEADER pbmfh;
	PBITMAPINFOHEADER pbmih;
	PSURFINFO pDestSurf = (PSURFINFO)hDest;
	PSURFINFO pSrcSurf = (PSURFINFO)hSrc;
	PBYTE pPal;
	SURFINFO siSrc;			// Needed if src is a bitmap

	// The source can be a surface or a Bitmap file. See which it is
	pbmfh = (PBITMAPFILEHEADER)hSrc;
	if (pbmfh->bfType == 'MB')
	{
		pbmih = (PBITMAPINFOHEADER)(hSrc + sizeof (BITMAPFILEHEADER));

		// Create a fake surf struct.
		siSrc.nWidth = pbmih->biWidth;
		siSrc.nHeight = pbmih->biHeight;
		siSrc.nBitsPerPixel = pbmih->biBitCount;
		siSrc.dwStride = ((pbmih->biWidth * (pbmih->biBitCount/8)) + 3) & 0xfffffffc;
		siSrc.pBuffer = (PBYTE)hSrc + pbmfh->bfOffBits;
		siSrc.dwVidBuffSize = pbmfh->bfSize;

		pSrcSurf = &siSrc;

		nSrcStart = nYSrc + nCY;
		nSrcInc = -1;
	}
	else
	{
		nSrcStart = nYSrc;
		nSrcInc = 1;
	}

	int nSrcBytePP = pSrcSurf->nBitsPerPixel/8;
	int nDstBytePP = pDestSurf->nBitsPerPixel/8;

	//Debug (TEXT("VidTxtBlt_24++ x=%d y=%d cx=%d cy=%d pSrc=%x %d %d \r\n"), nXDest, nYDest, nCX, nCY, pSrcSurf->pBuffer, nXSrc, nYSrc);

	BYTE *pDestBuff = pDestSurf->pBuffer;
	BYTE *pDestRow, *pSrcRow;

	// Compute starting column offset
	PBYTE pDColStart = pDestSurf->pBuffer + (nXDest * pDestSurf->nBitsPerPixel/8);
	PBYTE pSColStart = pSrcSurf->pBuffer +  (nXSrc *  pSrcSurf->nBitsPerPixel/8);

	Debug (TEXT("nSrcBytePP %d\r\n"), nSrcBytePP);

	int nWidth = nXDest + nCX;
	int nBlk = nWidth / 4;
	int nRem = nWidth % 4;

	BYTE xRed, xGrn, xBlu, clr;
	WORD wrd;
	switch (nSrcBytePP)
	{
	case 3:
		xRed = *(pSColStart+2);
		xGrn = *(pSColStart+1);
		xBlu = *(pSColStart+0);
		break;
	case 2:
		wrd = *(WORD *)pSColStart;
		xRed = wrd & 0x001f;
		xGrn = (wrd >> 5) & 0x001f;
		xBlu = (wrd >> 10) & 0x002f;
		break;
	case 1:
		pPal = (PBYTE)pbmih + pbmih->biSize;
		clr = *pSColStart;
		xRed = *(pPal + (clr*4) + 2);
		xGrn = *(pPal + (clr*4) + 1);
		xBlu = *(pPal + (clr*4) + 0);
		break;
	}
	Debug (TEXT("VidXBlt_i x=%d y=%d cx=%d cy=%d pSrc=%x %d %d xRGB %x:%x:%x\r\n"), nXDest, nYDest, nCX, nCY, pSrcSurf->pBuffer, nXSrc, nYSrc, xRed, xGrn, xBlu);
	Debug (TEXT("pSrc %x pBuff %x dif %x pal %x xRGB %x:%x:%x\r\n"), pSColStart, pSrcSurf, pSColStart - (PBYTE)pSrcSurf, pPal - (PBYTE)pSrcSurf, xRed, xGrn, xBlu);

	BYTE r, g, b;
	for (d = nYDest, s = nSrcStart; d < (int)nYDest + nCY; d++, s += nSrcInc)
	{
		// Each col
		pDestRow = pDColStart + ((DWORD)d * pDestSurf->dwStride);
		pSrcRow =  pSColStart + ((DWORD)s * pSrcSurf->dwStride);

		//Debug (TEXT("%d %d dst:%x src:%x\r\n"), i, k, pDestRow, pSrcRow);
		for (j = nXDest; j < (int)nXDest + nCX; j++)
		{
			switch (nSrcBytePP)
			{
			case 3:
				r = *(pSrcRow+2);
				g = *(pSrcRow+1);
				b = *(pSrcRow+0);
				break;
			case 2:
				wrd = *(WORD *)pSrcRow;
				r = wrd & 0x001f;
				g = (wrd >> 5) & 0x001f;
				b = (wrd >> 10) & 0x002f;
				break;
			case 1:
				clr = *pSrcRow;
				r = *(pPal + (clr*4) + 2);
				g = *(pPal + (clr*4) + 1);
				b = *(pPal + (clr*4) + 0);
				break;
			}
			// Check for transparent color
			//if ((r != xRed) || (g != xGrn) || (b != xBlu))
			{
				switch (nDstBytePP)
				{
				case 3:
					*(pDestRow+0) = r;
					*(pDestRow+1) = g;
					*(pDestRow+2) = b;
					break;
				case 2:

					//wrd = (((b >> 0) & 0x001f) << 0);
					//wrd |= (((g >> 0) & 0x003f) << 5); //good
					//wrd |= (((r >> 0) & 0x001f) << 11);
wrd = 0;
					wrd =  ((((b << 1)+1) & 0x003f) << 0);
					wrd |= ((((g << 1)+1) & 0x001f) << 6);
					wrd |= ((((r << 1)+1) & 0x001f) << 11);

					*(WORD *)pDestRow = wrd;
					break;
					// 8 bit display not supported
				case 1:
					break;

				}
			}
			pSrcRow += nSrcBytePP;
			pDestRow += nDstBytePP;
		}
	}
	return;
}

