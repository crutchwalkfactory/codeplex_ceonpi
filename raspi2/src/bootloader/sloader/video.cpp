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

#include "image_cfg.h"
#include "binfilesystem.h"
#include "mailbox.h"
#include "Debugfuncs.h"
#include "video.h"

#include <dma.h>

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

int InitVidDMA (PSURFINFO pvbInfo);

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

PBYTE pRegBase = (PBYTE)PERIPHERAL_PA_BASE;

//----------------------------------------------------------------------------------
// 16 bit Blts
//
void VidSet_16(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, DWORD dwColor);
void VidCopy_16(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
				  HSURF hSrc,  int nXSrc, int nYSrc);
void VidTxtBlt_16(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
				  HSURF hSrc,  int nXSrc, int nYSrc, DWORD dwForeClr, DWORD dwBkClr);

void DMACopyBlt_16(HSURF hDest, int nXDst, int nYDst, int nCX, int nCY, 
				   HSURF hSrc,  int nXSrc, int nYSrc);
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
int InitVideoSystem(int nWidth, int nHeight, int nBitsPerPixel, HSURF *phVidFrame)
{
	int nRetryCnt, rc = -1;
	DWORD dwData, dwFill, dwSize;
	PMAILBOXFRAMEBUFFSTRUCT pFBS;

	Debug (TEXT("InitVideoSystem++\r\n"));

	// Get a pointer to a safe place to put this structure
	rc = MailboxGetMailboxBuffer (FALSE, (DWORD *)&pFBS, &dwSize);

	//pFBS = (PMAILBOXFRAMEBUFFSTRUCT) (0x1f00); // Ptr must be 16 byte aligned
	Debug (TEXT("pFBS == %x\r\n"), pFBS);

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
	Debug (TEXT("Framebuff == %x retry cnt %d\r\n"), pFBS->pFrameBuff, nRetryCnt);

	Debug (TEXT("pFBS->dwWidth = %x (%d)\r\n"), pFBS->dwWidth, pFBS->dwWidth);
	Debug (TEXT("pFBS->dwHeight = %x (%d)\r\n"), pFBS->dwHeight, pFBS->dwHeight);
	Debug (TEXT("pFBS->dwVirtWidth = %x\r\n"), pFBS->dwVirtWidth);
	Debug (TEXT("pFBS->dwVirtHeight = %x\r\n"), pFBS->dwVirtHeight);
	Debug (TEXT("pFBS->dwPitch = %x\r\n"), pFBS->dwPitch);
	Debug (TEXT("pFBS->dwDepth = %x\r\n"), pFBS->dwDepth);
	Debug (TEXT("pFBS->dwXOffset = %x\r\n"), pFBS->dwXOffset);
	Debug (TEXT("pFBS->dwYOffset = %x\r\n"), pFBS->dwYOffset);
	Debug (TEXT("pFBS->pFrameBuff = %x\r\n"), pFBS->pFrameBuff);
	Debug (TEXT("pFBS->dwFrameBuffSize = %x\r\n"), pFBS->dwFrameBuffSize);

	// See if we have a valid frame buffer
	if (pFBS->pFrameBuff != 0)
	{
		g_vbInfo.wID = 'SF';
		g_vbInfo.nWidth = pFBS->dwWidth;
		g_vbInfo.nHeight = pFBS->dwHeight;
		g_vbInfo.nBitsPerPixel = pFBS->dwDepth;
		g_vbInfo.dwStride = pFBS->dwPitch;
		g_vbInfo.pBuffer = pFBS->pFrameBuff;
		g_vbInfo.dwVidBuffSize = pFBS->dwFrameBuffSize;

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
			//BltPtrs.pCopyBlt = &DMACopyBlt_16;
			BltPtrs.pTxtBlt = &VidTxtBlt_16;
			dwFill = 0x003f;
			break;
		default:
			Debug (TEXT("***** ERROR ***** Unsupported pixel depth %d (0x%x)\r\n"), g_vbInfo.nBitsPerPixel, g_vbInfo.nBitsPerPixel);

		}

		InitVidDMA (&g_vbInfo);

		// Clear the screen
		VidSet((HSURF)&g_vbInfo, 0, 0, g_vbInfo.nWidth, g_vbInfo.nHeight, dwFill);
		rc = 0;
	}
	else
	{
		Debug (TEXT("***** ERROR ***** Failed to get frame buffer pointer! Is the video monitor attached?\r\n"), rc);
	}

	Debug (TEXT("InitVideoSystem-- rc=%d\r\n"), rc);
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
	PBITMAPFILEHEADER pbmfh;
	PBITMAPINFOHEADER pbmih;
	PSURFINFO pDestSurf = (PSURFINFO)hDest;
	PSURFINFO pSrcSurf = (PSURFINFO)hSrc;
	int nBytesPerPixel;
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
		siSrc.dwStride = pbmih->biWidth * (pbmih->biBitCount/8);
		siSrc.pBuffer = (PBYTE)hSrc + pbmfh->bfOffBits;
		siSrc.dwVidBuffSize = pbmfh->bfSize;

		pSrcSurf = &siSrc;
	}
	nBytesPerPixel = pbmih->biBitCount/8;

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
	PBITMAPFILEHEADER pbmfh;
	PBITMAPINFOHEADER pbmih;
	PSURFINFO pDestSurf = (PSURFINFO)hDest;
	PSURFINFO pSrcSurf = (PSURFINFO)hSrc;
	int nSrcStart, nSrcInc;
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
		//siSrc.dwStride = pbmih->biWidth * (pbmih->biBitCount/8);
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
	PBITMAPFILEHEADER pbmfh;
	PBITMAPINFOHEADER pbmih;
	PSURFINFO pDestSurf = (PSURFINFO)hDest;
	PSURFINFO pSrcSurf = (PSURFINFO)hSrc;
	BYTE fR,fG,fB,bR,bG,bB;

	SURFINFO siSrc;			// Needed if src is a bitmap

	fR = dwForeClr & 0xff;
	fG = (dwForeClr >> 8) & 0xff;
	fB = (dwForeClr >> 16) & 0xff;

	bR = dwBkClr & 0xff;
	bG = (dwBkClr >> 8) & 0xff;
	bB = (dwBkClr >> 16) & 0xff;

	// The source can be a surface or a Bitmap file. See which it is
	pbmfh = (PBITMAPFILEHEADER)hSrc;
	if (pbmfh->bfType == 'MB')
	{
		pbmih = (PBITMAPINFOHEADER)(hSrc + sizeof (BITMAPFILEHEADER));

		// Create a fake surf struct.
		siSrc.nWidth = pbmih->biWidth;
		siSrc.nHeight = pbmih->biHeight;
		siSrc.nBitsPerPixel = pbmih->biBitCount;
		siSrc.dwStride = pbmih->biWidth * (pbmih->biBitCount/8);
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


typedef struct {
	int   nHeight;
	int   nWidth;
	int   nBPP;
	int   nBytesPerPixel;
	DWORD dwFrameStride;
	DWORD dwFrameSize;
	PBYTE pFrame;
	// DMA fields
	int   nChan;
	int   nNumConBlks;
	PDMACONBLKSTRUCT pVirtConBlock;
	DWORD dwPhysConBlock;
	int   nSolidDataCnt;
	PDWORD pdwVirtSolidData;
	DWORD dwPhysSolidData;
} DMAVIDINFOSTRUCT, *PDMAVIDINFOSTRUCT;

DMAVIDINFOSTRUCT dmaVidData;

#define DMA_CHAN           1
#define DMA_SOLIDBUFF_CNT  8  //8 DWORDs is the smallest src buffer 


inline DWORD VAtoPA (DWORD p) {return (DWORD)p & 0x0fffffff;}

#define Sleep(a) 0

BYTE buff1[1024 * 4];

//PBYTE AllocPhysMem (DWORD dwSize, DWORD flags, DWORD a , DWORD b, ULONG *pdwPhys)
extern "C" LPVOID AllocPhysMem(DWORD a, DWORD b, DWORD c, DWORD d, DWORD *pdwPhys)
{
	*pdwPhys = (DWORD)buff1;
	return (PBYTE)&buff1;
}


//---------------------------------------------------------------------------------------
//
//
int InitVidDMA (PSURFINFO pvbInfo)
{
	int rc = 0;

	dmaVidData.nHeight = pvbInfo->nHeight;
	dmaVidData.nWidth = pvbInfo->nWidth;
	dmaVidData.nBPP = pvbInfo->nBitsPerPixel;
	dmaVidData.nBytesPerPixel = pvbInfo->nBitsPerPixel/8;
	dmaVidData.dwFrameStride = pvbInfo->dwStride;
	dmaVidData.dwFrameSize = pvbInfo->dwVidBuffSize;
	dmaVidData.pFrame = (PBYTE)pvbInfo->pBuffer;

	// Allocate the DMA control memory using Physical mem

	//
	// Get access to CPU regs
	//


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

	dmaVidData.nChan = DMA_CHAN;
	dmaVidData.nNumConBlks = cnt;
	dmaVidData.pVirtConBlock = (PDMACONBLKSTRUCT)pVirt;
	dmaVidData.dwPhysConBlock = dwPhys;
	dmaVidData.nSolidDataCnt = DMA_SOLIDBUFF_CNT;
	dmaVidData.pdwVirtSolidData = (DWORD *)(pVirt + (sizeof (DMACONBLKSTRUCT) * cnt));
	dmaVidData.dwPhysSolidData = dwPhys + (sizeof (DMACONBLKSTRUCT) * cnt);

	DEBUGMSG(1, (L" nHeight                 %d\r\n", dmaVidData.nHeight));
	DEBUGMSG(1, (L" nWidth                  %d\r\n", dmaVidData.nWidth));
	DEBUGMSG(1, (L" nBPP                    %x\r\n", dmaVidData.nBPP));
	DEBUGMSG(1, (L" nBytesPerPixel          %x\r\n", dmaVidData.nBytesPerPixel));
	DEBUGMSG(1, (L" dwFrameStride           %x\r\n", dmaVidData.dwFrameStride));
	DEBUGMSG(1, (L" dwFrameSize             %x\r\n", dmaVidData.dwFrameSize));
	DEBUGMSG(1, (L" pFrame                  %x\r\n", dmaVidData.pFrame));
	DEBUGMSG(1, (L" nChan                   %x\r\n", dmaVidData.nChan));
	DEBUGMSG(1, (L" nNumConBlks             %x\r\n", dmaVidData.nNumConBlks));
	DEBUGMSG(1, (L" pVirtConBlock           %x\r\n", dmaVidData.pVirtConBlock));
	DEBUGMSG(1, (L" dwPhysConBlock          %x\r\n", dmaVidData.dwPhysConBlock));
	DEBUGMSG(1, (L" nSolidDataCnt           %x\r\n", dmaVidData.nSolidDataCnt));
	DEBUGMSG(1, (L" pdwVirtSolidData        %x\r\n", dmaVidData.pdwVirtSolidData));
	DEBUGMSG(1, (L" dwPhysSolidData         %x\r\n", dmaVidData.dwPhysSolidData));
	DEBUGMSG(1, (L" sizeof(DMACONBLKSTRUCT) %d\r\n", sizeof(DMACONBLKSTRUCT)));

	//BOOL b = FreePhysMem (pVirt);
	//DEBUGMSG(1, (L"FreePhysMem returned %x\r\n", b));

	return rc;
}

//---------------------------------------------------------------------------------------
// DispatchConBlk2DMAChan
//
int DispatchConBlk2DMAChan (DWORD dwPhysDMABlk, int nChan, BOOL fWaitForComplete)
{
	int rc = 0;

	PBYTE pDMA = (PBYTE)GetDMARegPtr (pRegBase, nChan);
	DWORD dw;
	DMACTLSTATREG ChStatReg;
	DWORD *pConBlkAdr;
	if (pDMA)
	{
		PDWORD pDMAEn = (PDWORD)GetDMARegPtr (pRegBase, DMA_GLOB_EN_REG);

		// BUGBUG:: Don't really need to enable channel for each action
		// Enable the DMA channel
		dw = (*pDMAEn & 0xffff) | 1 << dmaVidData.nChan;
		*(DWORD *)pDMAEn = dw;

		// Write the control block ptr
		pConBlkAdr = (DWORD*)(pDMA + DMA_OFF_CONBLK_AD);
//		*pConBlkAdr = dmaVidData.dwPhysConBlock;
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

//---------------------------------------------------------------------------------------
// DMASolidClrFill24 
//
int DMASolidClrFill24 (int nX, int nY, int nCX, int nCY, COLORREF clr)
{
	int rc = 0;
	
	// Fill in the src color. The color needs to be packed on a byte basis
	dmaVidData.pdwVirtSolidData[0] = clr;

	//
	// Block 1.  This fills in the first row of the rectangle
	//

	// Init the Control block Transfer descriptor
	PDMACONBLKSTRUCT pDMABlk = (PDMACONBLKSTRUCT) (&dmaVidData.pVirtConBlock[0]);

	memset (pDMABlk, 0, sizeof (DMACONBLKSTRUCT));
	pDMABlk->XferInfo.bit.TDMode = 1;			// 01 Enable 2d mode
	pDMABlk->XferInfo.bit.DestInc = 1;			// 04 Set if dest address increments
	pDMABlk->XferInfo.bit.DestWidth = 0;		// 05 1-use 128 bit wide writes, 0-32 bit writes
	pDMABlk->XferInfo.bit.SrcInc = 1;			// 08 Set if src address increments
	pDMABlk->XferInfo.bit.SrcWidth = 0;			// 09 1-use 128 bit wide writes, 0-32 bit reads
	pDMABlk->XferInfo.bit.BurstLen = 0;			// 12-15 Burst length

	// Set the src and dest addresses, they must be true physical addresses	
	pDMABlk->dwSrcAddr = (DWORD)dmaVidData.dwPhysSolidData;
	pDMABlk->dwDstAddr = (DWORD)dmaVidData.pFrame + (nX * dmaVidData.nBytesPerPixel) +
	                                                (nY * dmaVidData.dwFrameStride);
	// Hack way to convert the virtual frame buff address to a physical one
	pDMABlk->dwDstAddr = pDMABlk->dwDstAddr & 0x0fffffff;

	// Set the x and y length
	pDMABlk->XferLen.TdLen.XLen = dmaVidData.nBytesPerPixel;
	pDMABlk->XferLen.TdLen.YLen = nCX;

	// Set the stride
	pDMABlk->TdStride.SrcStride = -dmaVidData.nBytesPerPixel;
	pDMABlk->TdStride.DestStride = (WORD)0;

	//
	// Block 2. This replicates the first line vertically down the rectangle.
	//
	if (nCY > 1)
	{
		// Tie first block to the next
		pDMABlk->dwPhysNextConBk = dmaVidData.dwPhysConBlock + sizeof (DMACONBLKSTRUCT);

		// Init the second Control block Transfer descriptor
		pDMABlk = (PDMACONBLKSTRUCT) (&dmaVidData.pVirtConBlock[1]);

		memset (pDMABlk, 0, sizeof (DMACONBLKSTRUCT));
		pDMABlk->XferInfo.bit.TDMode = 1;			// 01 Enable 2d mode
		pDMABlk->XferInfo.bit.DestInc = 1;			// 04 Set if dest address increments
		pDMABlk->XferInfo.bit.DestWidth = 0;		// 05 1-use 128 bit wide writes, 0-32 bit writes
		pDMABlk->XferInfo.bit.SrcInc = 1;			// 08 Set if src address increments
		pDMABlk->XferInfo.bit.SrcWidth = 0;			// 09 1-use 128 bit wide writes, 0-32 bit reads
		pDMABlk->XferInfo.bit.BurstLen = 0;			// 12-15 Burst length

		// Set the src and dest addresse.
		// The src is the line of data written by the first control block
		pDMABlk->dwSrcAddr = (DWORD)dmaVidData.pFrame + (nX * dmaVidData.nBytesPerPixel) +
														(nY * dmaVidData.dwFrameStride);
		
		pDMABlk->dwDstAddr = (DWORD)dmaVidData.pFrame + (nX * dmaVidData.nBytesPerPixel) +
														((nY+1) * dmaVidData.dwFrameStride);

		// Hack way to convert the virtual frame buff address to a physical one
		pDMABlk->dwDstAddr = VAtoPA (pDMABlk->dwDstAddr);
		pDMABlk->dwSrcAddr = VAtoPA (pDMABlk->dwSrcAddr);

		// Set the x and y length
		pDMABlk->XferLen.TdLen.XLen = nCX * dmaVidData.nBytesPerPixel;
		pDMABlk->XferLen.TdLen.YLen = nCY-1;

		// Set the stride
		pDMABlk->TdStride.SrcStride = -(nCX * dmaVidData.nBytesPerPixel);
		pDMABlk->TdStride.DestStride = (WORD)(dmaVidData.dwFrameStride - (DWORD)pDMABlk->XferLen.TdLen.XLen);
	}

	// Terminate block chain
	pDMABlk->dwPhysNextConBk = NULL; 

	// Dispatch the blocks to the DMA hardware
	rc = DispatchConBlk2DMAChan (dmaVidData.dwPhysConBlock, dmaVidData.nChan, TRUE);

	return rc;
}

//---------------------------------------------------------------------------------------
// DMACopyBlt_16
//
void DMACopyBlt_16(HSURF hDest, int nXDest, int nYDest, int nCX, int nCY, 
				   HSURF hSrc,  int nXSrc, int nYSrc)
{
	int rc = 0;

	//
	// Block 1.  This fills in the first row of the rectangle
	//

	// Init the Control block Transfer descriptor
	PDMACONBLKSTRUCT pDMABlk = (PDMACONBLKSTRUCT) (&dmaVidData.pVirtConBlock[0]);

	memset (pDMABlk, 0, sizeof (DMACONBLKSTRUCT));
	pDMABlk->XferInfo.bit.TDMode = 1;			// 01 Enable 2d mode
	pDMABlk->XferInfo.bit.DestInc = 1;			// 04 Set if dest address increments
	pDMABlk->XferInfo.bit.DestWidth = 0;		// 05 1-use 128 bit wide writes, 0-32 bit writes
	pDMABlk->XferInfo.bit.SrcInc = 1;			// 08 Set if src address increments
	pDMABlk->XferInfo.bit.SrcWidth = 0;			// 09 1-use 128 bit wide writes, 0-32 bit reads
	pDMABlk->XferInfo.bit.BurstLen = 0;			// 12-15 Burst length

	// Set the src and dest addresses.
	if ((nYSrc > nYDest) ||
		((nYSrc == nYDest) && (nXSrc > nXDest)))
	{
		pDMABlk->dwSrcAddr = (DWORD)dmaVidData.pFrame + (nXSrc * dmaVidData.nBytesPerPixel) +
														(nYSrc * dmaVidData.dwFrameStride);
	
		pDMABlk->dwDstAddr = (DWORD)dmaVidData.pFrame + (nXDest * dmaVidData.nBytesPerPixel) +
														(nYDest * dmaVidData.dwFrameStride);
		// Set the stride
		pDMABlk->TdStride.SrcStride = (WORD)(dmaVidData.dwFrameStride - (DWORD)pDMABlk->XferLen.TdLen.XLen);
		pDMABlk->TdStride.DestStride = (WORD)(dmaVidData.dwFrameStride - (DWORD)pDMABlk->XferLen.TdLen.XLen);
	} 
	else
	{
		pDMABlk->dwSrcAddr = (DWORD)dmaVidData.pFrame + (nXSrc * dmaVidData.nBytesPerPixel) +
														((nYSrc + nCY) * dmaVidData.dwFrameStride);
	
		pDMABlk->dwDstAddr = (DWORD)dmaVidData.pFrame + (nXDest * dmaVidData.nBytesPerPixel) +
														((nYDest + nCY) * dmaVidData.dwFrameStride);
		// Set the stride
		pDMABlk->TdStride.SrcStride =  (WORD)((dmaVidData.dwFrameStride - (DWORD)pDMABlk->XferLen.TdLen.XLen) * -2);
		pDMABlk->TdStride.DestStride = (WORD)((dmaVidData.dwFrameStride - (DWORD)pDMABlk->XferLen.TdLen.XLen) * -2);
	}

	// Hack way to convert the virtual frame buff address to a physical one
	pDMABlk->dwDstAddr = VAtoPA (pDMABlk->dwDstAddr);
	pDMABlk->dwSrcAddr = VAtoPA (pDMABlk->dwSrcAddr);

	// Set the x and y length
	pDMABlk->XferLen.TdLen.XLen = nCX * dmaVidData.nBytesPerPixel;
	pDMABlk->XferLen.TdLen.YLen = nCY;

	// Terminate block chain
	pDMABlk->dwPhysNextConBk = NULL; 

	// Dispatch the blocks to the DMA hardware
	rc = DispatchConBlk2DMAChan (dmaVidData.dwPhysConBlock, dmaVidData.nChan, TRUE);

	return;
}

