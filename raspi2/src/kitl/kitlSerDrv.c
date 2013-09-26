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
/*++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Module Name:
    kitlser.c

Abstract:

    Platform specific code for serial KITL services.

Functions:


Notes:

--*/

#include <windows.h>
#include <bsp.h>
#include <image_cfg.h>
#include <kitlprot.h>
#include "kernel.h"
#include "..\mykitlcore\kitlp.h"

void TimerDumpList (LPSTR psz);

//------------------------------------------------------------------------------
// Local Variables
//
volatile PBYTE g_p2835Regs = 0;
volatile PDWORD pUARTFlags = 0;
volatile PDWORD pUARTData = 0;
volatile PDWORD pUARTFifoLvlReg = 0;
volatile PDWORD pUARTIrqMask = 0;
volatile PDWORD pUARTRawIrq = 0;
volatile PDWORD pUARTMaskedIrq = 0;
volatile PDWORD pUARTIrqClr = 0;

static DWORD KitlIoPortBase;

extern KITLPRIV g_kpriv;

#ifdef USE_INTERRUPTS

//(db) increased to test USB drvr impact to dbgmsgs #define SERMEMSIZE  (1024 * 4 * 2) // 8 pages
#define SERMEMSIZE  (1024 * 4 * 16) // 16 pages each way
BYTE bBuff[SERMEMSIZE];

#endif //USE_INTERRUPTS

PKITLSERIALBUFFSTRUCT pSerBuff = 0;

BOOL fIntsEnabled = FALSE;
CRITICAL_SECTION csRead;
CRITICAL_SECTION csWrite;

/* UART_Init
 *
 *  Called by PQOAL KITL framework to initialize the serial port
 *
 *  Return Value:
 */
BOOL UART_Init (KITL_SERIAL_INFO *pSerInfo)
{
    //KITLOutputDebugString ("[KITL] ++UART_Init()\r\n");

	pSerInfo->bestSize = 48 * 1024;
//	pSerInfo->bestSize = 1;

    KitlIoPortBase = (DWORD)pSerInfo->pAddress;


	// Map in the SOC registers.  We don't do this in the
	g_p2835Regs=(PBYTE)NKCreateStaticMapping((DWORD)0x20000000>>8,32*1024*1024);

	if (g_p2835Regs == 0)
		return FALSE;

	// Save ptrs to the regs we need
	pUARTFlags = (PDWORD) (g_p2835Regs + UART0_BASEOFF + UART0_OFF_FR);
	pUARTData = (PDWORD)  (g_p2835Regs + UART0_BASEOFF + UART0_OFF_DR);

	pUARTFifoLvlReg = (PDWORD) (g_p2835Regs + UART0_BASEOFF + UART0_OFF_IFLS);
	pUARTIrqMask = (PDWORD) (g_p2835Regs + UART0_BASEOFF + UART0_OFF_IMSC);
	pUARTRawIrq = (PDWORD) (g_p2835Regs + UART0_BASEOFF + UART0_OFF_RIS);
	pUARTMaskedIrq = (PDWORD) (g_p2835Regs + UART0_BASEOFF + UART0_OFF_MIS);
	pUARTIrqClr = (PDWORD) (g_p2835Regs + UART0_BASEOFF +  UART0_OFF_ICR);

#ifdef USE_INTERRUPTS
	{
		PBYTE p;
		//pSerBuff = (PKITLSERIALBUFFSTRUCT) VMAlloc (g_pprcNK, 0, SERMEMSIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		//pSerBuff = (PKITLSERIALBUFFSTRUCT) AllocMem (SERMEMSIZE);
		pSerBuff = (PKITLSERIALBUFFSTRUCT) bBuff;
		// Force page in of buffer
		p = bBuff;

	    KITLOutputDebugString ("[KITL] ++UART_Init() Setting up kitl interrupt structures at %x  %s  %x.\r\n", pSerBuff, bBuff, &bBuff[0]);
		do 
		{
			BYTE b = *p;
			p += 4096;
		} while (p < bBuff + sizeof (bBuff));

		//b = LockPages (pSerBuff, SERMEMSIZE, NULL, LOCKFLAG_READ | LOCKFLAG_WRITE);

		pSerBuff->pRecvBuff = (PBYTE) ((DWORD)pSerBuff + sizeof (KITLSERIALBUFFSTRUCT));
		pSerBuff->dwRecvLen = ((SERMEMSIZE - sizeof (KITLSERIALBUFFSTRUCT))/2 & 0xfffffff3);
		pSerBuff->dwRecvHead = 0;
		pSerBuff->dwRecvTail = 0;
		pSerBuff->pTransBuff = (PBYTE) ((DWORD)pSerBuff + sizeof (KITLSERIALBUFFSTRUCT)) + pSerBuff->dwRecvLen;
		pSerBuff->dwTransLen = pSerBuff->dwRecvLen;
		pSerBuff->dwTransHead = 0;
		pSerBuff->dwTransTail = 0;

	    KITLOutputDebugString ("[KITL] ++UART_Init() pRecvBuff = %x.\r\n", pSerBuff->pRecvBuff);
	    KITLOutputDebugString ("[KITL] ++UART_Init() dwRecvLen = %x.\r\n", pSerBuff->dwRecvLen);
	    KITLOutputDebugString ("[KITL] ++UART_Init() dwRecvHead  = %x.\r\n", pSerBuff->dwRecvHead );
	    KITLOutputDebugString ("[KITL] ++UART_Init() dwRecvTail  = %x.\r\n", pSerBuff->dwRecvTail );
	    KITLOutputDebugString ("[KITL] ++UART_Init() pTransBuff  = %x.\r\n", pSerBuff->pTransBuff );
	    KITLOutputDebugString ("[KITL] ++UART_Init() dwTransLen  = %x.\r\n", pSerBuff->dwTransLen );
	    KITLOutputDebugString ("[KITL] ++UART_Init() dwTransHead = %x.\r\n", pSerBuff->dwTransHead);
	    KITLOutputDebugString ("[KITL] ++UART_Init() dwTransTail = %x.\r\n", pSerBuff->dwTransTail);

		b = OEMIoControl (IOCTL_HAL_SET_KITL_SERIAL_BUFFERS, pSerBuff, sizeof(KITLSERIALBUFFSTRUCT), 0, 0, NULL);
	}
#endif 

    //KITLOutputDebugString ("[KITL] --UART_Init()\r\n");
    return TRUE;
}

/* UART_WriteData
 *
 *  Block until the byte is sent
 *
 *  Return Value: TRUE on success, FALSE otherwise
 */
UINT16 UART_WriteData (UINT8 *pch, UINT16 length)
{
	int count = 0;
	DWORD dwTmp;

//if (fIntsEnabled)
//{
	//KITLOutputDebugString ("UART_writeData++ %x len:%d  %x %x\r\n", 
	//				   pch, length, pSerBuff->pTransBuff, pSerBuff->pTransBuff+pSerBuff->dwTransHead);
//	KITLOutputDebugString ("WR:%d:%d", length, pSerBuff->dwTransHead);
//}
    if (KitlIoPortBase && g_p2835Regs)
    {
		if (fIntsEnabled)
		{
			EnterCriticalSection(&csWrite);
			while (length)
			{
				// If buffer empty, we need to prime the fifo
				if ((pSerBuff->dwTransHead == pSerBuff->dwTransTail) && 				
					((*pUARTFlags & FR_TXFF) == 0))
				{
					*pUARTData = *pch++;
				}
				else
				{
					// Check that buffer not full
					dwTmp = pSerBuff->dwTransHead + 1;
					if (dwTmp > pSerBuff->dwTransLen) dwTmp = 0;

					// Wait for room in the buffer
					if (dwTmp == pSerBuff->dwTransTail)
					{
						KITLOutputDebugString ("UART_writeData xmit buf full %d %d %d\r\n", 
							pSerBuff->dwTransHead, dwTmp, pSerBuff->dwTransTail);
						*pUARTIrqMask = *pUARTIrqMask | IRQ_TX_BIT;

						while (dwTmp == pSerBuff->dwTransTail)
							;
						KITLOutputDebugString ("UART_writeData xmit buf no longer full\r\n");
					}
					// Add the bytes to the buffer
//						pSerBuff->pTransBuff[pSerBuff->dwTransHead] = *pch++;
					*(pSerBuff->pTransBuff+pSerBuff->dwTransHead) = *pch++;
					pSerBuff->dwTransHead = dwTmp;
				}
				length--;
				count++;
			}
			// Turn on transmit int
			*pUARTIrqMask = *pUARTIrqMask | IRQ_TX_BIT;

			LeaveCriticalSection(&csWrite);
		}
		else //Ints not enabled or not yet enabled
		{
			while (length)
			{
				// Wait for the transmit Fifo to have room.
				while ((*pUARTFlags & FR_TXFF) != 0)
					;

				//if ((*pUARTFlags & FR_TXFF) != 0)
				//	break;

				// Write the data
				*pUARTData = *pch++;

				length--;
				count++;
			}
		}
    }
	//if (fIntsEnabled)
	//{
	//KITLOutputDebugString ("UART_writeData-- %x len:%d  %x %x\r\n", 
	//					   pch, length, pSerBuff->pTransBuff, pSerBuff->pTransBuff+pSerBuff->dwTransHead);
	//}
	return count;
}

/* UART_ReadData
 *
 *  Called from PQOAL KITL to read a byte from serial port
 *
 *  Return Value: TRUE on success, FALSE otherwise
 */
UINT16 UART_ReadData (UINT8 *pch, UINT16 length)
{
    UINT16 count = 0;
#ifdef DEBUG
	UINT16 len = length;
	int nAvail;
#endif
	DWORD dwTmp = 0xffffffff;

//	KITLOutputDebugString ("UART_ReadData++ %x, s:%d\r\n", pch, length);
//KITLOutputDebugString ("<");

    if (KitlIoPortBase && g_p2835Regs)
    {
//			if (TRUE)
		if (fIntsEnabled)
		{
			EnterCriticalSection (&csRead);
			while (length)
			{
				// While there is data in the output queue
				if (pSerBuff->dwRecvHead != pSerBuff->dwRecvTail) 
				{
					// Read the data into the recv buffer
					*pch++ = *(pSerBuff->pRecvBuff + pSerBuff->dwRecvTail);

					// Update the tail value
					dwTmp = pSerBuff->dwRecvTail + 1;

					// Check for wrap
					if (dwTmp > pSerBuff->dwRecvLen) dwTmp = 0;

					pSerBuff->dwRecvTail = dwTmp;  //Update tail
				}
				else
				{
					//KITLOutputDebugString ("UART_readData no data available h:%d t:%d\r\n", pSerBuff->dwRecvHead, pSerBuff->dwRecvTail);
					break;
				}
				length--;
				count++;
			}
#ifdef DEBUG
			nAvail = (int)pSerBuff->dwRecvHead - (int)pSerBuff->dwRecvTail;
			if (nAvail < 0) nAvail += pSerBuff->dwRecvLen;
#endif
			LeaveCriticalSection (&csRead);
		}
		else
		{
			while (length)
			{
				// Return if no data. Don't Wait!
				if ((*pUARTFlags & FR_RXFE) != 0)
					break;

				// Read the data
				dwTmp = *pUARTData;
				if (dwTmp & (DR_OE | DR_BE | DR_PE | DR_FE))
				{	
					KITLOutputDebugString ("UART_readData Error! %x\r\n", dwTmp);
				} 
				else
				{
					*pch++ = (BYTE)dwTmp;
					length--;
					count++;
				}
			}
		}
//KITLOutputDebugString (">");
    }
	//if (count)
//#ifdef DEBUG
//		KITLOutputDebugString ("UART_ReadData-- asked:%d ret:%d  avail:%d\r\n", len, count, nAvail);
//#endif
    return count;
}

VOID UART_EnableInt (void)
{
	KITLOutputDebugString ("UART_EnableInt++\r\n");

	// Init the critical sections here because its too early in the Init routine.
	InitializeCriticalSection (&csRead);
	InitializeCriticalSection (&csWrite);

	// Set fifo levels
	*pUARTFifoLvlReg = IFLS_RX_78FULL | IFLS_TX_18FULL;
//	*pUARTFifoLvlReg = IFLS_RX_78FULL | IFLS_TX_12FULL;

	// Clear all pending irqs
	*pUARTIrqClr = IRQ_ALL_BITS;

	// Enable only the receive irq
	// Setting the bit in the mask reg enables that interrupt.
	//*pUARTIrqMask = IRQ_RX_BIT; //TX bit set when data needs transmitting
	*pUARTIrqMask = IRQ_RX_BIT | IRQ_TX_BIT; //TX bit set when data needs transmitting

	fIntsEnabled = TRUE;

	KITLOutputDebugString ("UART_EnableInt--\r\n");
}

VOID UART_DisableInt (void)
{
	// Mask all irqs but recv irq
	*pUARTIrqMask = 0;

	// Clear all pending irqs
	*pUARTIrqClr = IRQ_ALL_BITS;

	fIntsEnabled = FALSE;
}

void UART_PowerOff(void)
{
    KITLOutputDebugString ("[KITL] UART_PowerOff()\r\n");

    return;
}

void UART_PowerOn(void)
{
}

// KITL Serial Driver function pointer
OAL_KITL_SERIAL_DRIVER DrvSerial =
{
    UART_Init,		// pfnInit
    NULL,			// pfnDeinit
    UART_WriteData,	// pfnSend
    NULL,			// pfnSendComplete
    UART_ReadData,	// pfnRecv
    UART_EnableInt,	// pfnEnableInts
    UART_DisableInt,// pfnDisableInts
    UART_PowerOff,	// pfnPowerOff
    UART_PowerOn,	// pfnPowerOn
    NULL,			// pfnFlowControl
};

const OAL_KITL_SERIAL_DRIVER *GetKitlSerialDriver (void)
{
    return &DrvSerial;
}
