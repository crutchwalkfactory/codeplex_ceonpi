//------------------------------------------------------------------------------
//
// File:        DMA.h
//
// Description: This header file defines DMA defs for the Broadcomm bcm2835
//
// Copyright (c) Boling Consulting Inc.  All rights reserved.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
//------------------------------------------------------------------------------
#ifndef __DMA_H
#define __DMA_H


#define DMA_GLB_INTSTAT_OFFSET   0x0fe0     // Global Interrupt pending reg
#define DMA_GLB_ENABREG_OFFSET   0x0ff0     // Global DMA enable reg

#define DMA_CHAN_DELTA           0x0100     // 

#define DMA_GLOB_IRQ_REG         -1
#define DMA_GLOB_EN_REG          -2

// Simple routine to get ptr to regs for specific channel
inline PBYTE GetDMARegPtr (PBYTE pGlobalRegs, int nDMAChan)
{
	if (nDMAChan < 15) return pGlobalRegs + DMA_CONTROLLER_BASEOFF + (nDMAChan * DMA_CHAN_DELTA);
	else if (nDMAChan == 15) return pGlobalRegs + 0x00E05000;
	else if (nDMAChan == -1) return pGlobalRegs + DMA_CONTROLLER_BASEOFF + DMA_GLB_INTSTAT_OFFSET;
	else if (nDMAChan == -2) return pGlobalRegs + DMA_CONTROLLER_BASEOFF + DMA_GLB_ENABREG_OFFSET;
	else {ASSERT(0); return 0;}
}

#define ISDMALITE(a)  ((a < 7) ? 0 : 1)

#define DMA_CHAN00_OFFSET        0x0000     // 
#define DMA_CHAN01_OFFSET        0x0100     // 
#define DMA_CHAN02_OFFSET        0x0200     // 
#define DMA_CHAN03_OFFSET        0x0300     // 
#define DMA_CHAN04_OFFSET        0x0400     // 
#define DMA_CHAN05_OFFSET        0x0500     // 
#define DMA_CHAN06_OFFSET        0x0600     // 
#define DMA_CHAN07_OFFSET        0x0700     // 
#define DMA_CHAN08_OFFSET        0x0800     // 
#define DMA_CHAN09_OFFSET        0x0900     // 
#define DMA_CHAN10_OFFSET        0x0A00     // 
#define DMA_CHAN11_OFFSET        0x0B00     // 
#define DMA_CHAN12_OFFSET        0x0C00     // 
#define DMA_CHAN13_OFFSET        0x0D00     // 
#define DMA_CHAN14_OFFSET        0x0E00     // 


//#define DMA_CHAN15_BASE			(PERIPHERAL_PA_BASE + 0x00E05000)// 0x20E05000 

//------------------------------------
// DMA Control and Status reg
typedef struct {
	volatile DWORD Active:1;			// 00 Activate this chanel
	volatile DWORD End:1;				// 01 Set by DMA when complete
	volatile DWORD IntStat:1;			// 02 Current Irq status
	volatile DWORD DreqState:1;			// 03 DREQ state
	volatile DWORD Paused:1;			// 04 DMA Paused state
	volatile DWORD DReqStopsDma:1;		// 05 DMA Paused by DREQ State
	volatile DWORD Waiting4Writes:1;	// 06 DMA is waiting for last write to complete
	volatile DWORD Reserved1:1;			// 07
	volatile DWORD Error:1;				// 08 DMA Error
	volatile DWORD Reserved2:7;			// 09-15
	volatile DWORD Priority:3;			// 16-19 AXI bus priority
	volatile DWORD PanicPriority:3;		// 20-23 AXI bus priority in panic mode
	volatile DWORD Reserved3:4;			// 24-27
	volatile DWORD Wait4Writes:1;		// 28 Don't signal cmplete until all writes done
	volatile DWORD DisDebugPause:1;		// 29 Ignore debug pause signal
	volatile DWORD Abort:1;				// 30 Abort current DMA
	volatile DWORD Reset:1;				// 31 Reset DMA channel
} DMACTLSTATREG_Bit;
typedef union {
    volatile DMACTLSTATREG_Bit bit;
    volatile DWORD ul;
} DMACTLSTATREG, *PDMACTLSTATREG;

//------------------------------------
// DMA Transfer Information reg
typedef struct {
	volatile DWORD IntEn:1;				// 00 Interrupt when complete
	volatile DWORD TDMode:1;			// 01 Enable 2d mode
	volatile DWORD Reserved:1;			// 02 
	volatile DWORD Wait4WriteResp:1;	// 03 Wait for Write response on AXI bus
	volatile DWORD DestInc:1;			// 04 Set if dest address increments
	volatile DWORD DestWidth:1;			// 05 1-use 128 bit wide writes, 0-32 bit writes
	volatile DWORD DestDReq:1;			// 06 device DReq will gate writes
	volatile DWORD DestNoWrite:1;		// 07 Don't write to destination 
	volatile DWORD SrcInc:1;			// 08 Set if src address increments
	volatile DWORD SrcWidth:1;			// 09 1-use 128 bit wide reads, 0-32 bit reads
	volatile DWORD SrcDReq:1;			// 10 device DReq will gate reads
	volatile DWORD SrcNoRead:1;			// 11 Don't read from source addr
	volatile DWORD BurstLen:4;			// 12-15 Burst length
	volatile DWORD PerMap:5;			// 16-20 Peripheral bit map select
	volatile DWORD NumWaitCycles:5;		// 21-25 Add wait cycles
	volatile DWORD DisWideWrites:1;		// 26 Disable wide writes
	volatile DWORD Reserved2:5;			// 27-31
} XFERINFOREG_Bit;
typedef union {
    volatile XFERINFOREG_Bit bit;
    volatile DWORD ul;
} XFERINFOREG, *PXFERINFOREG;

typedef union {
	struct {
		DWORD XLen:16;
		DWORD YLen:14;
	} TdLen;
	DWORD DMALen;
} DMAXFERLENREG, *PDMAXFERLENREG;

typedef struct {
    volatile short SrcStride;
    volatile short DestStride;
} DMA2DSTRIDEREG, *PDMA2DSTRIDEREG;


#define DMACONBLK_ALIGNMENT       0x0000001f
#define DMACONBLK_MASK            ~DMACONBLK_ALIGNMENT

// Control block structure used to chain DMA actions.
// Control block structures must be aligned 256 bit (32 byte) addresses.

typedef struct _dmaconblk {
	XFERINFOREG XferInfo;
	DWORD dwSrcAddr;
	DWORD dwDstAddr;
	DMAXFERLENREG XferLen;
	DMA2DSTRIDEREG TdStride;
//	_dmaconblk *pNextConBk;
	DWORD dwPhysNextConBk;
	DWORD dwReserved[2];
} DMACONBLKSTRUCT, *PDMACONBLKSTRUCT;



#endif    // __DMA_H