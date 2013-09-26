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
//
//------------------------------------------------------------------------------
//
//  File:  image_cfg.h
//
//  Defines configuration parameters used to create the NK and Bootloader
//  program images.
//
#ifndef __RASPI_IRQ_H
#define __RASPI_IRQ_H

#if __cplusplus
extern "C" {
#endif

//
// Hardware static SysIntr numbers
//
#define SYSINTR_UART0        (SYSINTR_FIRMWARE+0)
#define SYSINTR_USBHOST      (SYSINTR_FIRMWARE+1)

#define SYSINTR_ENDOFSTATIC  (SYSINTR_FIRMWARE+1)


// Discovered in the Raspberry Pi forums...

// Note: Many of the bits in the IRQ1 and IRQ2 regs have duplicates
// in the Base reg. When checking for a pending IRQ, you should use the 
// base reg bits instead of the irq1 and irq2 regs because if there are
// duplicates, the base reg bit indicating a flag in irq1 or irq2 won't
// be set.

// Bit defs for base pending reg
#define  RASPI_BASE_ARMTIMER	(1 << 0)		//
#define  RASPI_BASE_ARMMAILBOX	(1 << 1)		//
#define  RASPI_BASE_ARMDOORBEL0	(1 << 2)		//
#define  RASPI_BASE_ARMDOORBEL1	(1 << 3)		//
#define  RASPI_BASE_GPU0HALT	(1 << 4)		//
#define  RASPI_BASE_GPU1HALT	(1 << 5)		//
#define  RASPI_BASE_BADACCESS1	(1 << 6)		//
#define  RASPI_BASE_BADACCESS0	(1 << 7)		//

#define  RASPI_BASE_JPEG		(1 << 10)		//GPU_IRQ07
#define  RASPI_BASE_USB			(1 << 11)		//GPU_IRQ09
#define  RASPI_BASE_3D			(1 << 12)		//GPU_IRQ10
#define  RASPI_BASE_DMA2		(1 << 13)		//GPU_IRQ18
#define  RASPI_BASE_DMA3		(1 << 14)		//GPU_IRQ19
#define  RASPI_BASE_I2C			(1 << 15)		//GPU_IRQ53
#define  RASPI_BASE_SPI			(1 << 16)		//GPU_IRQ54
#define  RASPI_BASE_I2SPCM		(1 << 17)		//GPU_IRQ55
#define  RASPI_BASE_SDIO		(1 << 18)		//GPU_IRQ56
#define  RASPI_BASE_UART0		(1 << 19)		//GPU_IRQ57
#define  RASPI_BASE_ARASANSDIO	(1 << 20)		//GPU_IRQ62

// Bit defs for first IRQ reg
#define  RASPI_IRQR1_TIMER0			(1 << 0)
#define  RASPI_IRQR1_TIMER1			(1 << 1)
#define  RASPI_IRQR1_TIMER2			(1 << 2)
#define  RASPI_IRQR1_TIMER3			(1 << 3)
#define  RASPI_IRQR1_CODEC0			(1 << 4)
#define  RASPI_IRQR1_CODEC1			(1 << 5)
#define  RASPI_IRQR1_CODEC2			(1 << 6)
#define  RASPI_IRQR1_VCJPEG			(1 << 7)   // Use the bit in the base reg for pending irq check
#define  RASPI_IRQR1_ISP			(1 << 8)
#define  RASPI_IRQR1_VC_USB			(1 << 9)   // Use the bit in the base reg for pending irq check
#define  RASPI_IRQR1_VC_3D			(1 << 10)  // Use the bit in the base reg for pending irq check
#define  RASPI_IRQR1_TRANSPOSER		(1 << 11)
#define  RASPI_IRQR1_MCSYNC0		(1 << 12)
#define  RASPI_IRQR1_MCSYNC1		(1 << 13)
#define  RASPI_IRQR1_MCSYNC2		(1 << 14)
#define  RASPI_IRQR1_MCSYNC3		(1 << 15)
#define  RASPI_IRQR1_DMA0			(1 << 16)
#define  RASPI_IRQR1_DMA1			(1 << 17)
#define  RASPI_IRQR1_VC_DMA2		(1 << 18)  // Use the bit in the base reg for pending irq check
#define  RASPI_IRQR1_VC_DMA3		(1 << 19)  // Use the bit in the base reg for pending irq check
#define  RASPI_IRQR1_DMA4			(1 << 20)
#define  RASPI_IRQR1_DMA5			(1 << 21)
#define  RASPI_IRQR1_DMA6			(1 << 22)
#define  RASPI_IRQR1_DMA7			(1 << 23)
#define  RASPI_IRQR1_DMA8			(1 << 24)
#define  RASPI_IRQR1_DMA9			(1 << 25)
#define  RASPI_IRQR1_DMA10			(1 << 26)
#define  RASPI_IRQR1_DMA11			(1 << 27)
#define  RASPI_IRQR1_DMA12			(1 << 28)
#define  RASPI_IRQR1_AUX			(1 << 29)
#define  RASPI_IRQR1_ARM			(1 << 30)
#define  RASPI_IRQR1_VPUDMA			(1 << 31)

// Bit defs for second IRQ reg
#define  RASPI_IRQR2_HOSTPORT		(1 << 0) //GPU32
#define  RASPI_IRQR2_VIDSCALER		(1 << 1) //GPU33
#define  RASPI_IRQR2_CCP2TX			(1 << 2) //GPU34
#define  RASPI_IRQR2_SDC			(1 << 3) //GPU35
#define  RASPI_IRQR2_DSIO			(1 << 4) //GPU36
#define  RASPI_IRQR2_AVE			(1 << 5) //GPU37
#define  RASPI_IRQR2_CAM0			(1 << 6) //GPU38
#define  RASPI_IRQR2_CAM1			(1 << 7) //GPU39
#define  RASPI_IRQR2_HDMI0			(1 << 8) //GPU40
#define  RASPI_IRQR2_HDMI1			(1 << 9) //GPU41
#define  RASPI_IRQR2_PIXELVALV1		(1 << 10) //GPU42
#define  RASPI_IRQR2_I2CSPISLAVE	(1 << 11) //GPU43
#define  RASPI_IRQR2_DSI1			(1 << 12) //GPU44
#define  RASPI_IRQR2_PWA0			(1 << 13) //GPU45
#define  RASPI_IRQR2_PWA1			(1 << 14) //GPU46
#define  RASPI_IRQR2_CPR			(1 << 15) //GPU47
#define  RASPI_IRQR2_SMI			(1 << 16) //GPU48
#define  RASPI_IRQR2_GPIO0			(1 << 17) //GPU49
#define  RASPI_IRQR2_GPIO1			(1 << 18) //GPU50
#define  RASPI_IRQR2_GPIO2			(1 << 19) //GPU51
#define  RASPI_IRQR2_GPIO3			(1 << 20) //GPU52
#define  RASPI_IRQR2_VC_I2C			(1 << 21) //GPU53  // Use bit in base reg for pending irq check
#define  RASPI_IRQR2_VC_SPI			(1 << 22) //GPU54  // Use bit in base reg for pending irq check
#define  RASPI_IRQR2_I2SPCM			(1 << 23) //GPU55  // Use bit in base reg for pending irq check
#define  RASPI_IRQR2_SDIO			(1 << 24) //GPU56  // Use bit in base reg for pending irq check
#define  RASPI_IRQR2_UART0			(1 << 25) //GPU57  // Use bit in base reg for pending irq check
#define  RASPI_IRQR2_SLIMBUS		(1 << 26) //GPU58
#define  RASPI_IRQR2_VEC			(1 << 27) //GPU59
#define  RASPI_IRQR2_CPG			(1 << 28) //GPU60
#define  RASPI_IRQR2_RNG			(1 << 29) //GPU61
#define  RASPI_IRQR2_VC_ARASANSDIO	(1 << 30) //GPU62 // Use bit in base reg for pending irq check
#define  RASPI_IRQR2_AVSPMON		(1 << 31) //GPU63



#define  RASPI_PHYSIRQ_TIMER0		0 
#define  RASPI_PHYSIRQ_TIMER1		1
#define  RASPI_PHYSIRQ_TIMER2		2
#define  RASPI_PHYSIRQ_TIMER3		3
#define  RASPI_PHYSIRQ_CODEC0		4
#define  RASPI_PHYSIRQ_CODEC1		5
#define  RASPI_PHYSIRQ_CODEC2		6
#define  RASPI_PHYSIRQ_VCJPEG		7
#define  RASPI_PHYSIRQ_ISP			8
#define  RASPI_PHYSIRQ_VC_USB		9
#define  RASPI_PHYSIRQ_VC_3D		10
#define  RASPI_PHYSIRQ_TRANSPOSER	11
#define  RASPI_PHYSIRQ_MCSYNC0		12
#define  RASPI_PHYSIRQ_MCSYNC1		13
#define  RASPI_PHYSIRQ_MCSYNC2		14
#define  RASPI_PHYSIRQ_MCSYNC3		15
#define  RASPI_PHYSIRQ_DMA0			16
#define  RASPI_PHYSIRQ_DMA1			17
#define  RASPI_PHYSIRQ_VC_DMA2		18
#define  RASPI_PHYSIRQ_VC_DMA3		19
#define  RASPI_PHYSIRQ_DMA4			20
#define  RASPI_PHYSIRQ_DMA5			21
#define  RASPI_PHYSIRQ_DMA6			22
#define  RASPI_PHYSIRQ_DMA7			23
#define  RASPI_PHYSIRQ_DMA8			24
#define  RASPI_PHYSIRQ_DMA9			25
#define  RASPI_PHYSIRQ_DMA10		26
#define  RASPI_PHYSIRQ_DMA11		27
#define  RASPI_PHYSIRQ_DMA12		28
#define  RASPI_PHYSIRQ_AUX			29
#define  RASPI_PHYSIRQ_ARM			30
#define  RASPI_PHYSIRQ_VPUDMA		31
#define  RASPI_PHYSIRQ_HOSTPORT		32
#define  RASPI_PHYSIRQ_VIDSCALER	33
#define  RASPI_PHYSIRQ_CCP2TX		34
#define  RASPI_PHYSIRQ_SDC			35
#define  RASPI_PHYSIRQ_DSIO			36
#define  RASPI_PHYSIRQ_AVE			37
#define  RASPI_PHYSIRQ_CAM0			38
#define  RASPI_PHYSIRQ_CAM1			39
#define  RASPI_PHYSIRQ_HDMI0		40
#define  RASPI_PHYSIRQ_HDMI1		41
#define  RASPI_PHYSIRQ_PIXELVALV1	42
#define  RASPI_PHYSIRQ_I2CSPISLAVE	43
#define  RASPI_PHYSIRQ_DSI1			44
#define  RASPI_PHYSIRQ_PWA0			45
#define  RASPI_PHYSIRQ_PWA1			46
#define  RASPI_PHYSIRQ_CPR			47
#define  RASPI_PHYSIRQ_SMI			48
#define  RASPI_PHYSIRQ_GPIO0		49
#define  RASPI_PHYSIRQ_GPIO1		50
#define  RASPI_PHYSIRQ_GPIO2		51
#define  RASPI_PHYSIRQ_GPIO3		52
#define  RASPI_PHYSIRQ_VC_I2C		53
#define  RASPI_PHYSIRQ_VC_SPI		54
#define  RASPI_PHYSIRQ_I2SPCM		55
#define  RASPI_PHYSIRQ_SDIO			56
#define  RASPI_PHYSIRQ_UART0		57
#define  RASPI_PHYSIRQ_SLIMBUS		58
#define  RASPI_PHYSIRQ_VEC			59
#define  RASPI_PHYSIRQ_CPG			60
#define  RASPI_PHYSIRQ_RNG			61
#define  RASPI_PHYSIRQ_VC_ARASANSDIO 62
#define  RASPI_PHYSIRQ_AVSPMON		63

//------------------------------------------------------------------------------

#if __cplusplus
}
#endif

#endif  //__RASPI_IRQ_H

