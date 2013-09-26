//------------------------------------------------------------------------------
//
// File:        bcm2835.h
//
// Description: This header file defines CPU specific addresses for the 
//              Broadcomm bcm2835
//
// NOTE: This file needs to be synced with BCM2835.inc
//
// Copyright (c) Boling Consulting Inc.  All rights reserved.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
//------------------------------------------------------------------------------
#ifndef __BCM2835_H
#define __BCM2835_H

//----------------------------------------------------------------------
// System Timer
//
#define SYSTEM_TIMER_BASEOFF 0x00003000	
#define SYSTEM_TIMER_PA_BASE (PERIPHERAL_PA_BASE + SYSTEM_TIMER_BASEOFF)   // 0x20003000

#define TIMER_CS_OFFSET		0x0000	//	Timer Control/Status reg R/W
#define TIMER_CHL_OFFSET	0x0004	//	Timer Counter Low reg R/W
#define TIMER_CHI_OFFSET	0x0008	//	Timer Counter High reg R/W
#define TIMER_C0_OFFSET		0x000C	//	Timer Compare 0 reg R/W
#define TIMER_C1_OFFSET		0x0010	//	Timer Compare 1 reg R/W
#define TIMER_C2_OFFSET		0x0014	//	Timer Compare 2 reg R/W
#define TIMER_C3_OFFSET		0x0018	//	Timer Compare 3 reg R/W

#define TIMER_CS_MATCH_0	(1<<0)	//  Set when compare reg 0 matches 
#define TIMER_CS_MATCH_1	(1<<1)	//  Set when compare reg 1 matches 
#define TIMER_CS_MATCH_2	(1<<2)	//  Set when compare reg 2 matches 
#define TIMER_CS_MATCH_3	(1<<3)	//  Set when compare reg 3 matches 

//----------------------------------------------------------------------
// DMA Controller
#define DMA_CONTROLLER_BASEOFF	0x00007000	
#define DMA15_CONTROLLER_BASEOFF 0x00E05000 // 0x20E05000 
#define DMA_CONTROLLER_PA_BASE  (PERIPHERAL_PA_BASE + DMA_CONTROLLER_BASEOFF)     // 0x20007000 

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

// DMA channels 0-n has the following regs. Regs marked Read only are loaded
// through the control block structure.
#define DMA_OFF_CS          (0x0000)  // Control and Status reg
#define DMA_OFF_CONBLK_AD   (0x0004)  // Control Block Address reg
#define DMA_OFF_TI          (0x0008)  // Transfer Information reg (Read only)
#define DMA_OFF_SOURCE_AD   (0x000c)  // Source Address reg (Read only)
#define DMA_OFF_DEST_AD     (0x0010)  // Destination address reg (Read only)
#define DMA_OFF_TXFER_LEN   (0x0014)  // Transfer length reg (Read only)
#define DMA_OFF_STRIDE      (0x0018)  // 2d stride reg (Read only)
#define DMA_OFF_NEXTCONBK   (0x001c)  // Next control block address reg (Read only)
#define DMA_OFF_DEBUG       (0x0020)  // Debug reg

//----------------------------------------------------------------------
// ARM Timer
//
// NOTE: The ARM timer clock is dependent on system clocks that can
// sleep when the chip goes into low power mode.
//
#define ARM_TIMER_BASEOFF	0x0000B000	
#define ARM_TIMER_PA_BASE	(PERIPHERAL_PA_BASE + ARM_TIMER_BASEOFF)	// 0x2000B000

//----------------------------------------------------------------------
// ARM Interrupt Controller
//
// NOTE: The peripheral doc puts the base at the same address
// as the ARM timer (above) and then uses offsets starting at
// 0x200 for the registers to offset them from the timer regs.
// I'm going to follow the same convention so that these addresses
// match the doc
//
#define MAX_PHYS_INTERRUPTS     (64+8)								// 64 ints plus 8 ARM ints

#define ARM_IRQCTLR_BASEOFF		0x0000B000	

#define IRQC_BASICPEND_OFF		(0x00000200)   // 
#define IRQC_IRQPEND1_OFF		(0x00000204)   // 
#define IRQC_IRQPEND2_OFF		(0x00000208)   // 
#define IRQC_FIQCTRL_OFF		(0x0000020C)   // 
#define IRQC_ENABIRQ1_OFF		(0x00000210)   // 
#define IRQC_ENABIRQ2_OFF		(0x00000214)   // 
#define IRQC_ENABBASIC_OFF		(0x00000218)   // 
#define IRQC_DISABIRQ1_OFF		(0x0000021C)   // 
#define IRQC_DISABIRQ2_OFF		(0x00000220)   // 
#define IRQC_DISABBASIC_OFF		(0x00000224)   // 

#define ARM_IRQCTLR_PA_BASE		(PERIPHERAL_PA_BASE + ARM_IRQCTLR_BASEOFF)	// 0x2000B000 (yes, same as timer above)

//#define IRQC_BASICPEND_UVA_REG	(ARM_IRQCTLR_UVA_BASE + 0x00000200)   // 
//#define IRQC_IRQPEND1_UVA_REG	(ARM_IRQCTLR_UVA_BASE + 0x00000204)   // 
//#define IRQC_IRQPEND2_UVA_REG	(ARM_IRQCTLR_UVA_BASE + 0x00000208)   // 
//#define IRQC_FIQCTRL_UVA_REG	(ARM_IRQCTLR_UVA_BASE + 0x0000020C)   // 
//#define IRQC_ENABIRQ1_UVA_REG	(ARM_IRQCTLR_UVA_BASE + 0x00000210)   // 
//#define IRQC_ENABIRQ2_UVA_REG	(ARM_IRQCTLR_UVA_BASE + 0x00000214)   // 
//#define IRQC_ENABBASIC_UVA_REG	(ARM_IRQCTLR_UVA_BASE + 0x00000218)   // 
//#define IRQC_DISABIRQ1_UVA_REG	(ARM_IRQCTLR_UVA_BASE + 0x0000021C)   // 
//#define IRQC_DISABIRQ2_UVA_REG	(ARM_IRQCTLR_UVA_BASE + 0x00000220)   // 
//#define IRQC_DISABBASIC_UVA_REG	(ARM_IRQCTLR_UVA_BASE + 0x00000224)   // 

// Basic Reg IRQ bit defs
#define IRQ_ARM_TIMER_BASICREG				(1 << 0)
#define IRQ_ARM_MAILBOX_BASICREG			(1 << 1)
#define IRQ_ARM_DOORBELL0_BASICREG			(1 << 2)
#define IRQ_ARM_DOORBELL1_BASICREG			(1 << 3)
#define IRQ_GPU0_HALT_BASICREG				(1 << 4)
#define IRQ_GPU1_HALT_BASICREG				(1 << 5)
#define IRQ_BADACCESS1_BASICREG				(1 << 6)
#define IRQ_BADACCESS0_BASICREG				(1 << 7)
#define IRQ_REG1PENDING_BASICREG			(1 << 8)
#define IRQ_REG2PENDING_BASICREG			(1 << 9)
#define IRQ_GPU_IRQ07_BASICREG				(1 << 10)
#define IRQ_GPU_IRQ09_BASICREG				(1 << 11)
#define IRQ_GPU_IRQ10_BASICREG				(1 << 12)
#define IRQ_GPU_IRQ18_BASICREG				(1 << 13)
#define IRQ_GPU_IRQ19_BASICREG				(1 << 14)
#define IRQ_GPU_IRQ53_BASICREG				(1 << 15)
#define IRQ_GPU_IRQ54_BASICREG				(1 << 16)
#define IRQ_GPU_IRQ55_BASICREG				(1 << 17)
#define IRQ_GPU_IRQ56_BASICREG				(1 << 18)
#define IRQ_GPU_IRQ57_BASICREG				(1 << 19)
#define IRQ_GPU_IRQ62_BASICREG				(1 << 20)

// Basic Reg IRQ Enable/Disable bit defs
#define IRQ_ARM_TIMER_BASICENREG			(1 << 0)
#define IRQ_ARM_MAILBOX_BASICENREG			(1 << 1)
#define IRQ_ARM_DOORBELL0_BASICENREG		(1 << 2)
#define IRQ_ARM_DOORBELL1_BASICENREG		(1 << 3)
#define IRQ_GPU0_HALT_BASICENREG			(1 << 4)
#define IRQ_GPU1_HALT_BASICENREG			(1 << 5)
#define IRQ_BADACCESS1_BASICENREG			(1 << 6)
#define IRQ_BADACCESS0_BASICENREG			(1 << 7)

// These bit defs go into the Reg1 sets above
#define IRQ_00_REG1				(1 << 0)
#define IRQ_01_REG1				(1 << 1)
#define IRQ_02_REG1				(1 << 2)
#define IRQ_03_REG1				(1 << 3)
#define IRQ_04_REG1				(1 << 4)
#define IRQ_05_REG1				(1 << 5)
#define IRQ_06_REG1				(1 << 6)
#define IRQ_07_REG1				(1 << 7)
#define IRQ_08_REG1				(1 << 8)
#define IRQ_09_REG1				(1 << 9)
#define IRQ_10_REG1				(1 << 10)
#define IRQ_11_REG1				(1 << 11)
#define IRQ_12_REG1				(1 << 12)
#define IRQ_13_REG1				(1 << 13)
#define IRQ_14_REG1				(1 << 14)
#define IRQ_15_REG1				(1 << 15)
#define IRQ_16_REG1				(1 << 16)
#define IRQ_17_REG1				(1 << 17)
#define IRQ_18_REG1				(1 << 18)
#define IRQ_19_REG1				(1 << 19)
#define IRQ_20_REG1				(1 << 20)
#define IRQ_21_REG1				(1 << 21)
#define IRQ_22_REG1				(1 << 22)
#define IRQ_23_REG1				(1 << 23)
#define IRQ_24_REG1				(1 << 24)
#define IRQ_25_REG1				(1 << 25)
#define IRQ_26_REG1				(1 << 26)
#define IRQ_27_REG1				(1 << 27)
#define IRQ_28_REG1				(1 << 28)
#define IRQ_29_REG1				(1 << 29)
#define IRQ_30_REG1				(1 << 30)
#define IRQ_31_REG1				(1 << 31)

// These bit defs go into the Reg2 sets above
#define IRQ_32_REG2				(1 << 0)
#define IRQ_33_REG2				(1 << 1)
#define IRQ_34_REG2				(1 << 2)
#define IRQ_35_REG2				(1 << 3)
#define IRQ_36_REG2				(1 << 4)
#define IRQ_37_REG2				(1 << 5)
#define IRQ_38_REG2				(1 << 6)
#define IRQ_39_REG2				(1 << 7)
#define IRQ_40_REG2				(1 << 8)
#define IRQ_41_REG2				(1 << 9)
#define IRQ_42_REG2				(1 << 10)
#define IRQ_I2C_SPI_SLV_REG2	(1 << 11)
#define IRQ_44_REG2				(1 << 12)
#define IRQ_PWA0_REG2			(1 << 13)
#define IRQ_PWA1_REG2			(1 << 14)
#define IRQ_47_REG2				(1 << 15)
#define IRQ_SMI_REG2			(1 << 16)
#define IRQ_GPIO0_REG2			(1 << 17)
#define IRQ_GPIO1_REG2			(1 << 18)
#define IRQ_GPIO2_REG2			(1 << 19)
#define IRQ_GPIO3_REG2			(1 << 20)
#define IRQ_I2C_REG2			(1 << 21)
#define IRQ_SPI_REG2			(1 << 22)
#define IRQ_PCM_REG2			(1 << 23)
#define IRQ_56_REG2				(1 << 24)
#define IRQ_UART_REG2			(1 << 25)
#define IRQ_58_REG2				(1 << 26)
#define IRQ_59_REG2				(1 << 27)
#define IRQ_50_REG2				(1 << 28)
#define IRQ_61_REG2				(1 << 29)
#define IRQ_62_REG2				(1 << 30)
#define IRQ_63_REG2				(1 << 31)

//----------------------------------------------------------------------
// Mailbox addresses start at 0x2000 B880
//
// Note: this is just for reference. The mailbox addresses are defined
// in mailbox.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// GPIO Clock Manager
//
#define CLOCK_MGR0_UVA_BASE	(PERIPHERAL_UVA_BASE + 0x00101070)   // 0x20101070
#define CLOCK_MGR1_UVA_BASE	(PERIPHERAL_UVA_BASE + 0x00101078)   // 0x20101070
#define CLOCK_MGR2_UVA_BASE	(PERIPHERAL_UVA_BASE + 0x00101080)   // 0x20101070

//----------------------------------------------------------------------
// GPIO Registers
//
#define GPIO_BASEOFF		0x00200000	
#define GPIO_PA_BASE		(PERIPHERAL_PA_BASE + GPIO_BASEOFF)	// 0x20200000
//#define GPIO_UVA_BASE		(PERIPHERAL_UVA_BASE + 0x00200000)	// 0x20200000

#define GPFSEL0_OFFSET		0x0000	//	GPIO Function Select 0 32 R/W
#define GPFSEL1_OFFSET		0x0004	//	GPIO Function Select 1 32 R/W
#define GPFSEL2_OFFSET		0x0008	//	GPIO Function Select 2 32 R/W
#define GPFSEL3_OFFSET		0x000C	//	GPIO Function Select 3 32 R/W
#define GPFSEL4_OFFSET		0x0010	//	GPIO Function Select 4 32 R/W
#define GPFSEL5_OFFSET		0x0014	//	GPIO Function Select 5 32 R/W

#define GPSET0_OFFSET		0x001C	//	GPIO Pin Output Set 0 32 W
#define GPSET1_OFFSET		0x0020	//	GPIO Pin Output Set 1 32 W

#define GPCLR0_OFFSET		0x0028	//	GPIO Pin Output Clear 0 32 W
#define GPCLR1_OFFSET		0x002C	//	GPIO Pin Output Clear 1 32 W

#define GPLEV0_OFFSET		0x0034	//	GPIO Pin Level 0 32 R
#define GPLEV1_OFFSET		0x0038	//	GPIO Pin Level 1 32 R

#define GPEDS0_OFFSET		0x0040	//	GPIO Pin Event Detect Status 0 32 R/W
#define GPEDS1_OFFSET		0x0044	//	GPIO Pin Event Detect Status 1 32 R/W

#define GPREN0_OFFSET		0x004C	//	GPIO Pin Rising Edge Detect Enable 0 32 R/W
#define GPREN1_OFFSET		0x0050	//	GPIO Pin Rising Edge Detect Enable 1 32 R/W

#define GPFEN0_OFFSET		0x0058	//	GPIO Pin Falling Edge Detect Enable 0 32 R/W
#define GPFEN1_OFFSET		0x005C	//	GPIO Pin Falling Edge Detect Enable 1 32 R/W
			

#define GPHEN0_OFFSET		0x0064	//	GPIO Pin High Detect Enable 0 32 R/W
#define GPHEN1_OFFSET		0x0068	//	GPIO Pin High Detect Enable 1 32 R/W

#define GPLEN0_OFFSET		0x0070	//	GPIO Pin Low Detect Enable 0 32 R/W
#define GPLEN1_OFFSET		0x0074	//	GPIO Pin Low Detect Enable 1 32 R/W

#define GPAREN0_OFFSET		0x007C	//	GPIO Pin Async. Rising Edge Detect 0 32 R/W
#define GPAREN1_OFFSET		0x0080	//	GPIO Pin Async. Rising Edge Detect 1 32 R/W

#define GPAFEN0_OFFSET		0x0088	//	GPIO Pin Async. Falling Edge Detect 0 32 R/W
#define GPAFEN1_OFFSET		0x008C	//	GPIO Pin Async. Falling Edge Detect 1 32 R/W

#define GPPUD_OFFSET		0x0094	//	GPIO Pin Pull-up/down Enable 32 R/W
#define GPPUDCLK0_OFFSET	0x0098	//	GPIO Pin Pull-up/down Enable Clock 0 32 R/W
#define GPPUDCLK1_OFFSET	0x009C	//	GPIO Pin Pull-up/down Enable Clock 1 32 R/W


// Physical Addresses
#define GPFSEL0_PA_REG		(GPIO_PA_BASE+GPFSEL0_OFFSET)	//	0x20200000	GPIO Function Select 0 32 R/W
#define GPFSEL1_PA_REG		(GPIO_PA_BASE+GPFSEL1_OFFSET)	//	0x20200004	GPIO Function Select 1 32 R/W
#define GPFSEL2_PA_REG		(GPIO_PA_BASE+GPFSEL2_OFFSET)	//	0x20200008	GPIO Function Select 2 32 R/W
#define GPFSEL3_PA_REG		(GPIO_PA_BASE+GPFSEL3_OFFSET)	//	0x2020000C	GPIO Function Select 3 32 R/W
#define GPFSEL4_PA_REG		(GPIO_PA_BASE+GPFSEL4_OFFSET)	//	0x20200010	GPIO Function Select 4 32 R/W
#define GPFSEL5_PA_REG		(GPIO_PA_BASE+GPFSEL5_OFFSET)	//	0x20200014	GPIO Function Select 5 32 R/W

#define GPSET0_PA_REG		(GPIO_PA_BASE+GPSET0_OFFSET)	//	0x2020001C	GPIO Pin Output Set 0 32 W
#define GPSET1_PA_REG		(GPIO_PA_BASE+GPSET1_OFFSET)	//	0x20200020	GPIO Pin Output Set 1 32 W

#define GPCLR0_PA_REG		(GPIO_PA_BASE+GPCLR0_OFFSET)	//	0x20200028	GPIO Pin Output Clear 0 32 W
#define GPCLR1_PA_REG		(GPIO_PA_BASE+GPCLR1_OFFSET)	//	0x2020002C	GPIO Pin Output Clear 1 32 W

#define GPLEV0_PA_REG		(GPIO_PA_BASE+GPLEV0_OFFSET)	//	0x20200034	GPIO Pin Level 0 32 R
#define GPLEV1_PA_REG		(GPIO_PA_BASE+GPLEV1_OFFSET)	//	0x20200038	GPIO Pin Level 1 32 R

#define GPEDS0_PA_REG		(GPIO_PA_BASE+GPEDS0_OFFSET)	//	0x20200040	GPIO Pin Event Detect Status 0 32 R/W
#define GPEDS1_PA_REG		(GPIO_PA_BASE+GPEDS1_OFFSET)	//	0x20200044	GPIO Pin Event Detect Status 1 32 R/W

#define GPREN0_PA_REG		(GPIO_PA_BASE+GPREN0_OFFSET)	//	0x2020004C	GPIO Pin Rising Edge Detect Enable 0 32 R/W
#define GPREN1_PA_REG		(GPIO_PA_BASE+GPREN1_OFFSET)	//	0x20200050	GPIO Pin Rising Edge Detect Enable 1 32 R/W

#define GPFEN0_PA_REG		(GPIO_PA_BASE+GPFEN0_OFFSET)	//	0x20200058	GPIO Pin Falling Edge Detect Enable 0 32 R/W
#define GPFEN1_PA_REG		(GPIO_PA_BASE+GPFEN1_OFFSET)	//	0x2020005C	GPIO Pin Falling Edge Detect Enable 1 32 R/W
			

#define GPHEN0_PA_REG		(GPIO_PA_BASE+GPHEN0_OFFSET)	//	0x20200064	GPIO Pin High Detect Enable 0 32 R/W
#define GPHEN1_PA_REG		(GPIO_PA_BASE+GPHEN1_OFFSET)	//	0x20200068	GPIO Pin High Detect Enable 1 32 R/W

#define GPLEN0_PA_REG		(GPIO_PA_BASE+GPLEN0_OFFSET)	//	0x20200070	GPIO Pin Low Detect Enable 0 32 R/W
#define GPLEN1_PA_REG		(GPIO_PA_BASE+GPLEN1_OFFSET)	//	0x20200074	GPIO Pin Low Detect Enable 1 32 R/W

#define GPAREN0_PA_REG		(GPIO_PA_BASE+GPAREN0_OFFSET)	//	0x2020007C	GPIO Pin Async. Rising Edge Detect 0 32 R/W
#define GPAREN1_PA_REG		(GPIO_PA_BASE+GPAREN1_OFFSET)	//	0x20200080	GPIO Pin Async. Rising Edge Detect 1 32 R/W

#define GPAFEN0_PA_REG		(GPIO_PA_BASE+GPAFEN0_OFFSET)	//	0x20200088	GPIO Pin Async. Falling Edge Detect 0 32 R/W
#define GPAFEN1_PA_REG		(GPIO_PA_BASE+GPAFEN1_OFFSET)	//	0x2020008C	GPIO Pin Async. Falling Edge Detect 1 32 R/W

#define GPPUD_PA_REG		(GPIO_PA_BASE+GPPUD_OFFSET)		//	0x20200094	GPIO Pin Pull-up/down Enable 32 R/W
#define GPPUDCLK0_PA_REG	(GPIO_PA_BASE+GPPUDCLK0_OFFSET)	//	0x20200098	GPIO Pin Pull-up/down Enable Clock 0 32 R/W
#define GPPUDCLK1__PA_REG	(GPIO_PA_BASE+GPPUDCLK1_OFFSET)	//	0x2020009C	GPIO Pin Pull-up/down Enable Clock 1 32 R/W


// The following defines help set the function of each of the GPIO lines
// 
#define SETASINPUT		000
#define SETASOUTPUT		001
#define SETASALT0		100
#define SETASALT1		101
#define SETASALT2		110
#define SETASALT3		111
#define SETASALT4		011
#define SETASALT5		010

#define GPIOSEL_SHIFTx9		27
#define GPIOSEL_SHIFTx8		24
#define GPIOSEL_SHIFTx7		21
#define GPIOSEL_SHIFTx6		18
#define GPIOSEL_SHIFTx5		15
#define GPIOSEL_SHIFTx4		12
#define GPIOSEL_SHIFTx3		09
#define GPIOSEL_SHIFTx2		06
#define GPIOSEL_SHIFTx1		03
#define GPIOSEL_SHIFTx0		00

//#define GPIO_SETFUNC_BITS(a,b) (a << b)

#define GPIO09_R0_FN(a)		(a << GPIOSEL_SHIFTx9)
#define GPIO08_R0_FN(a)		(a << GPIOSEL_SHIFTx8)
#define GPIO07_R0_FN(a)		(a << GPIOSEL_SHIFTx7)
#define GPIO06_R0_FN(a)		(a << GPIOSEL_SHIFTx6)
#define GPIO05_R0_FN(a)		(a << GPIOSEL_SHIFTx5)
#define GPIO04_R0_FN(a)		(a << GPIOSEL_SHIFTx4)
#define GPIO03_R0_FN(a)		(a << GPIOSEL_SHIFTx3)
#define GPIO02_R0_FN(a)		(a << GPIOSEL_SHIFTx2)
#define GPIO01_R0_FN(a)		(a << GPIOSEL_SHIFTx1)
#define GPIO00_R0_FN(a)		(a << GPIOSEL_SHIFTx0)

#define GPIO19_R1_FN(a)		(a << GPIOSEL_SHIFTx9)
#define GPIO18_R1_FN(a)		(a << GPIOSEL_SHIFTx8)
#define GPIO17_R1_FN(a)		(a << GPIOSEL_SHIFTx7)
#define GPIO16_R1_FN(a)		(a << GPIOSEL_SHIFTx6)
#define GPIO15_R1_FN(a)		(a << GPIOSEL_SHIFTx5)
#define GPIO14_R1_FN(a)		(a << GPIOSEL_SHIFTx4)
#define GPIO13_R1_FN(a)		(a << GPIOSEL_SHIFTx3)
#define GPIO12_R1_FN(a)		(a << GPIOSEL_SHIFTx2)
#define GPIO11_R1_FN(a)		(a << GPIOSEL_SHIFTx1)
#define GPIO10_R1_FN(a)		(a << GPIOSEL_SHIFTx0)

#define GPIO29_R2_FN(a)		(a << GPIOSEL_SHIFTx9)
#define GPIO28_R2_FN(a)		(a << GPIOSEL_SHIFTx8)
#define GPIO27_R2_FN(a)		(a << GPIOSEL_SHIFTx7)
#define GPIO26_R2_FN(a)		(a << GPIOSEL_SHIFTx6)
#define GPIO25_R2_FN(a)		(a << GPIOSEL_SHIFTx5)
#define GPIO24_R2_FN(a)		(a << GPIOSEL_SHIFTx4)
#define GPIO23_R2_FN(a)		(a << GPIOSEL_SHIFTx3)
#define GPIO22_R2_FN(a)		(a << GPIOSEL_SHIFTx2)
#define GPIO21_R2_FN(a)		(a << GPIOSEL_SHIFTx1)
#define GPIO20_R2_FN(a)		(a << GPIOSEL_SHIFTx0)

#define GPIO39_R3_FN(a)		(a << GPIOSEL_SHIFTx9)
#define GPIO38_R3_FN(a)		(a << GPIOSEL_SHIFTx8)
#define GPIO37_R3_FN(a)		(a << GPIOSEL_SHIFTx7)
#define GPIO36_R3_FN(a)		(a << GPIOSEL_SHIFTx6)
#define GPIO35_R3_FN(a)		(a << GPIOSEL_SHIFTx5)
#define GPIO34_R3_FN(a)		(a << GPIOSEL_SHIFTx4)
#define GPIO33_R3_FN(a)		(a << GPIOSEL_SHIFTx3)
#define GPIO32_R3_FN(a)		(a << GPIOSEL_SHIFTx2)
#define GPIO31_R3_FN(a)		(a << GPIOSEL_SHIFTx1)
#define GPIO30_R3_FN(a)		(a << GPIOSEL_SHIFTx0)

#define GPIO49_R4_FN(a)		(a << GPIOSEL_SHIFTx9)
#define GPIO48_R4_FN(a)		(a << GPIOSEL_SHIFTx8)
#define GPIO47_R4_FN(a)		(a << GPIOSEL_SHIFTx7)
#define GPIO46_R4_FN(a)		(a << GPIOSEL_SHIFTx6)
#define GPIO45_R4_FN(a)		(a << GPIOSEL_SHIFTx5)
#define GPIO44_R4_FN(a)		(a << GPIOSEL_SHIFTx4)
#define GPIO43_R4_FN(a)		(a << GPIOSEL_SHIFTx3)
#define GPIO42_R4_FN(a)		(a << GPIOSEL_SHIFTx2)
#define GPIO41_R4_FN(a)		(a << GPIOSEL_SHIFTx1)
#define GPIO40_R4_FN(a)		(a << GPIOSEL_SHIFTx0)

#define GPIO53_R5_FN(a)		(a << GPIOSEL_SHIFTx3)
#define GPIO52_R5_FN(a)		(a << GPIOSEL_SHIFTx2)
#define GPIO51_R5_FN(a)		(a << GPIOSEL_SHIFTx1)
#define GPIO50_R5_FN(a)		(a << GPIOSEL_SHIFTx0)

// GPIO bit defines for the Set and Clear regs
#define GPIO00_R0_BIT		(1 << 0)
#define GPIO01_R0_BIT		(1 << 1)
#define GPIO02_R0_BIT		(1 << 2)
#define GPIO03_R0_BIT		(1 << 3)
#define GPIO04_R0_BIT		(1 << 4)
#define GPIO05_R0_BIT		(1 << 5)
#define GPIO06_R0_BIT		(1 << 6)
#define GPIO07_R0_BIT		(1 << 7)
#define GPIO08_R0_BIT		(1 << 8)
#define GPIO09_R0_BIT		(1 << 9)
#define GPIO10_R0_BIT		(1 << 10)
#define GPIO11_R0_BIT		(1 << 11)
#define GPIO12_R0_BIT		(1 << 12)
#define GPIO13_R0_BIT		(1 << 13)
#define GPIO14_R0_BIT		(1 << 14)
#define GPIO15_R0_BIT		(1 << 15)
#define GPIO16_R0_BIT		(1 << 16)
#define GPIO17_R0_BIT		(1 << 17)
#define GPIO18_R0_BIT		(1 << 18)
#define GPIO19_R0_BIT		(1 << 19)
#define GPIO20_R0_BIT		(1 << 20)
#define GPIO21_R0_BIT		(1 << 21)
#define GPIO22_R0_BIT		(1 << 22)
#define GPIO23_R0_BIT		(1 << 23)
#define GPIO24_R0_BIT		(1 << 24)
#define GPIO25_R0_BIT		(1 << 25)
#define GPIO26_R0_BIT		(1 << 26)
#define GPIO27_R0_BIT		(1 << 27)
#define GPIO28_R0_BIT		(1 << 28)
#define GPIO29_R0_BIT		(1 << 29)
#define GPIO30_R0_BIT		(1 << 30)
#define GPIO31_R0_BIT		(1 << 31)

#define GPIO32_R1_BIT		(1 << 0)			
#define GPIO33_R1_BIT		(1 << 1)			
#define GPIO34_R1_BIT		(1 << 2)			
#define GPIO35_R1_BIT		(1 << 3)			
#define GPIO36_R1_BIT		(1 << 4)			
#define GPIO37_R1_BIT		(1 << 5)			
#define GPIO38_R1_BIT		(1 << 6)			
#define GPIO39_R1_BIT		(1 << 7)			
#define GPIO40_R1_BIT		(1 << 8)			
#define GPIO41_R1_BIT		(1 << 9)			
#define GPIO42_R1_BIT		(1 << 10)			
#define GPIO43_R1_BIT		(1 << 11)			
#define GPIO44_R1_BIT		(1 << 12)			
#define GPIO45_R1_BIT		(1 << 13)			
#define GPIO46_R1_BIT		(1 << 14)			
#define GPIO47_R1_BIT		(1 << 15)			
#define GPIO48_R1_BIT		(1 << 16)			
#define GPIO49_R1_BIT		(1 << 17)			
#define GPIO50_R1_BIT		(1 << 18)			
#define GPIO51_R1_BIT		(1 << 19)			
#define GPIO52_R1_BIT		(1 << 20)			
#define GPIO53_R1_BIT		(1 << 21)			

//----------------------------------------------------------------------
// Primary UART
//
// Note: This is the UART that comes to GPIO pins 14&15. For Raspberry Pi, this is the UART on P1.
//
//----------------------------------------------------------------------
#define UART0_BASEOFF		0x00201000	

#define UART0_OFF_DR		(0x0000)  // Data Register
#define UART0_OFF_RSRECR	(0x0004)  // Receive Status Reg
#define UART0_OFF_FR		(0x0018)  // Flag Register
//#define UART0_OFF_ILPR		(0x0020)  // not in use (Disabled IrDA reg)
#define UART0_OFF_IBRD		(0x0024)  // Integer Baud rate divisor 
#define UART0_OFF_BBRD		(0x0028)  // Fractional Baud rate divisor
#define UART0_OFF_LCRH		(0x002C)  // Line Control register 
#define UART0_OFF_CR  		(0x0030)  // Control register 
#define UART0_OFF_IFLS		(0x0034)  // Interupt FIFO Level Select Register
#define UART0_OFF_IMSC		(0x0038)  // Interupt Mask Set Clear Register (Setting a bit enables that interrupt)
#define UART0_OFF_RIS 		(0x003C)  // Raw Interupt Status Register 
#define UART0_OFF_MIS 		(0x0040)  // Masked Interupt Status Register 
#define UART0_OFF_ICR 		(0x0044)  // Interupt Clear Register 
#define UART0_OFF_DMACR		(0x0048)  // DMA Control Register 
#define UART0_OFF_ITCR 		(0x0080)  // Test Control register 
#define UART0_OFF_ITIP		(0x0084)  // Integration test input reg 
#define UART0_OFF_ITOP		(0x0088)  // Integration test output reg 
#define UART0_OFF_TDR 		(0x008C)  // Test Data reg 


#define UART0_PA_BASE		(PERIPHERAL_PA_BASE + UART0_BASEOFF)		// 0x2020100 BUGBUG:: this is missing a digit 

#define UART0_PA_DR			(UART0_PA_BASE+UART0_OFF_DR	)  // Data Register
#define UART0_PA_RSRECR		(UART0_PA_BASE+UART0_OFF_RSRECR)  // Receive Status Reg
#define UART0_PA_FR			(UART0_PA_BASE+UART0_OFF_FR	)  // Flag Register
//#define UART0_PA_ILPR		(UART0_PA_BASE+UART0_OFF_ILPR	)  // not in use (Disabled IrDA reg)
#define UART0_PA_IBRD		(UART0_PA_BASE+UART0_OFF_IBRD	)  // Integer Baud rate divisor 
#define UART0_PA_BBRD		(UART0_PA_BASE+UART0_OFF_BBRD	)  // Fractional Baud rate divisor
#define UART0_PA_LCRH		(UART0_PA_BASE+UART0_OFF_LCRH	)  // Line Control register 
#define UART0_PA_CR  		(UART0_PA_BASE+UART0_OFF_CR  	)  // Control register 
#define UART0_PA_IFLS		(UART0_PA_BASE+UART0_OFF_IFLS	)  // Interupt FIFO Level Select Register
#define UART0_PA_IMSC		(UART0_PA_BASE+UART0_OFF_IMSC	)  // Interupt Mask Set Clear Register 
#define UART0_PA_RIS 		(UART0_PA_BASE+UART0_OFF_RIS 	)  // Raw Interupt Status Register 
#define UART0_PA_MIS 		(UART0_PA_BASE+UART0_OFF_MIS 	)  // Masked Interupt Status Register 
#define UART0_PA_ICR 		(UART0_PA_BASE+UART0_OFF_ICR 	)  // Interupt Clear Register 
#define UART0_PA_DMACR		(UART0_PA_BASE+UART0_OFF_DMACR	)  // DMA Control Register 
#define UART0_PA_ITCR 		(UART0_PA_BASE+UART0_OFF_ITCR 	)  // Test Control register 
#define UART0_PA_ITIP		(UART0_PA_BASE+UART0_OFF_ITIP	)  // Integration test input reg 
#define UART0_PA_ITOP		(UART0_PA_BASE+UART0_OFF_ITOP	)  // Integration test output reg 
#define UART0_PA_TDR 		(UART0_PA_BASE+UART0_OFF_TDR 	)  // Test Data reg 

// Data register (DR) mapping
#define DR_DATAMASK             0x000000FF				// Bottom byte is data
#define DR_OE					0x00000800				// Signal Overrun Error
#define DR_BE					0x00000400				// Signal Break Error
#define DR_PE					0x00000200				// Signal Parity Error
#define DR_FE					0x00000100				// Signal Framing Error

// Receive Status Register (RSRECR) mapping
#define RSRECR_OE				0x00000008				// Overrun Error
#define RSRECR_BE				0x00000004				// Break Error
#define RSRECR_PE				0x00000002				// Parity Error
#define RSRECR_FE				0x00000001				// Framing Error

// Flag Register
#define FR_TXFE  				0x00000080				// Transmit FIFO empty
#define FR_RXFF  				0x00000040				// Receive FIFO full
#define FR_TXFF  				0x00000020				// Transmit FIFO full
#define FR_RXFE  				0x00000010				// Receive FIFO empty
#define FR_BUSY  				0x00000008				// Busy
//#define FR_DCD_UNSUPPORTED	0x00000004				// Reserved
//#define FR_DSR_UNSUPPORTED	0x00000002				// Reserved
#define FR_CTS_NOT 				0x00000001				// Clear to Send (Line will be compliment of bit state)

//Integer Baud Rate divisor reg
#define IBRD_MASK				0x0000FFFF				// Lower 16 bits used

//Fractional Baud Rate divisor reg
#define BBRD_MASK				0x0000003F				// Lower 6 bits used

//Line Control Register (LCRH)
#define LCRH_SPS				0x00000080				// Stick Parity 0-disabled
#define LCRH_WLEN_MASK			0x00000060				// Word Length
#define LCRH_WLEN_8BITS			0x00000060				// 
#define LCRH_WLEN_7BITS			0x00000040				// 
#define LCRH_WLEN_6BITS			0x00000020				// 
#define LCRH_WLEN_5BITS			0x00000000				// 
#define LCRH_FEN				0x00000010				// FIFO Enable 1-Enabled
#define LCRH_STP2				0x00000008				// Two Stop Bits 1-Enabled
#define LCRH_EPS				0x00000004				// Even Parity 0-Odd, 1-Even
#define LCRH_PEN				0x00000002				// Parity Enable 1-Enable Parity tmit/chk
#define LCRH_BRK				0x00000001				// Send Break

//Control Register (CR)
//NOTE:
//To enable transmission, the TXE bit and UARTEN bit must be set to 1.
//Similarly, to enable reception, the RXE bit and UARTEN bit, must be set to 1.
//NOTE:
//Program the control registers as follows:
// 1. Disable the UART.
// 2. Wait for the end of transmission or reception of the current character.
// 3. Flush the transmit FIFO by setting the FEN bit to 0 in the Line Control
//    Register, UART_LCRH.
// 4. Reprogram the Control Register, UART_CR.
// 5. Enable the UART.
#define CR_CTSEN				0x00008000				//CTS hardware flow enable. 1-enable h/w flow
#define CR_RTSEN				0x00004000				//RTS hardware flow enable. 1-enable h/w flow
#define CR_RTS_NOT				0x00000800				//sets RTS line to compliment of bit state
#define CR_RXE					0x00000200				//Enables receive section of UART
#define CR_TXE					0x00000100				//Enables transmit section of UART
#define CR_LBE					0x00000080				//Enables Loopback
#define CR_UARTEN				0x00000001				//UART Enable 1-Enable UART

//Fifo level set reg (IFLS)
#define IFLS_RX_18FULL			(0 << 3)				// Recv Fifo Irq at 1/8 full
#define IFLS_RX_14FULL			(1 << 3)				// Recv Fifo Irq at 1/4 full
#define IFLS_RX_12FULL			(2 << 3)				// Recv Fifo Irq at 1/2 full
#define IFLS_RX_34FULL			(3 << 3)				// Recv Fifo Irq at 3/4 full
#define IFLS_RX_78FULL			(4 << 3)				// Recv Fifo Irq at 7/8 full

#define IFLS_TX_18FULL			(0 << 0)				// Xmit Fifo Irq at 1/8 full
#define IFLS_TX_14FULL			(1 << 0)				// Xmit Fifo Irq at 1/4 full
#define IFLS_TX_12FULL			(2 << 0)				// Xmit Fifo Irq at 1/2 full
#define IFLS_TX_34FULL			(3 << 0)				// Xmit Fifo Irq at 3/4 full
#define IFLS_TX_78FULL			(4 << 0)				// Xmit Fifo Irq at 7/8 full

//The following bits work on the mask, raw irq and masked irq regs

#define IRQ_OVRR_BIT			(1 << 10)				// Overrun error irq 
#define IRQ_BRK_BIT				(1 << 9)				// break irq 
#define IRQ_PARE_BIT			(1 << 8)				// Parity error irq 
#define IRQ_FRME_BIT			(1 << 7)				// Frame error irq 
#define IRQ_RCVTO_BIT			(1 << 6)				// Recv timeout error irq 
#define IRQ_TX_BIT				(1 << 5)				// Transmit irq 
#define IRQ_RX_BIT				(1 << 4)				// Receive irq 
// bits 2 and 3 reserved
#define IRQ_MODEM_BIT			(1 << 1)				// Modem error irq mask
// bit 1 reserved

// All the irq bits ORed together
#define IRQ_ALL_BITS		(IRQ_OVRR_BIT | IRQ_BRK_BIT	| IRQ_PARE_BIT | \
							 IRQ_FRME_BIT | IRQ_RCVTO_BIT | IRQ_TX_BIT | \
							 IRQ_RX_BIT	| IRQ_MODEM_BIT)



//----------------------------------------------------------------------
// PCM Audio
//----------------------------------------------------------------------
#define PCM_AUDIO_PA_BASE	(PERIPHERAL_PA_BASE + 0x00203000)   // 0x20203000



//----------------------------------------------------------------------
// Broadcomm Serial Controller - These are the I2C/SPI masters
//----------------------------------------------------------------------
#define BSC0_PA_BASE		(PERIPHERAL_PA_BASE + 0x00205000)	// 0x20205000
#define BSC1_PA_BASE		(PERIPHERAL_PA_BASE + 0x00804000)	// 0x20205000
#define BSC2_PA_BASE		(PERIPHERAL_PA_BASE + 0x00805000)	// 0x20205000


#define BSC0_PA_CONTROL		(PERIPHERAL_PA_BASE + 0x00205000)	// 0x20205000

//----------------------------------------------------------------------
// Broadcomm Serial Controller/SPI Slave
//----------------------------------------------------------------------
#define BSC0_SPI_PA_CONTROL		(BSC0_PA_BASE+0x000)	    // 0x20205000	Control 
#define BSC0_SPI_PA_STATUS		(BSC0_PA_BASE+0x004)	    // 0x20205004	Status 
#define BSC0_SPI_PA_DATALEN		(BSC0_PA_BASE+0x008)	    // 0x20205008	Data Length 
#define BSC0_SPI_PA_SLAVEADDR	(BSC0_PA_BASE+0x00C)	    // 0x2020500C	Slave Address 
#define BSC0_SPI_PA_FIFO		(BSC0_PA_BASE+0x010)	    // 0x20205010	Data FIFO 
#define BSC0_SPI_PA_CLKDIV		(BSC0_PA_BASE+0x014)	    // 0x20205014	Clock Divider
#define BSC0_SPI_PA_DATADELAY	(BSC0_PA_BASE+0x018)	    // 0x20205018	Data Delay 


#define SPI_CONTROL_I2CEN		0x00010000					// 1 = I2C enabled
#define SPI_CONTROL_INTR		0x00000400					// Interrupt on RX
#define SPI_CONTROL_INTT		0x00000200					// Interrupt on TX
#define SPI_CONTROL_INTD		0x00000100					// Interrupt on Done
#define SPI_CONTROL_ST			0x00000080					// Start Transfer
#define SPI_CONTROL_CLEARMASK	0x00000030					// Two clear bits
#define SPI_CONTROL_CLEARFIFO	0x00000020					// 
#define SPI_CONTROL_CLEARFIFO1	0x00000010					// 
#define SPI_CONTROL_READ		0x00000001					// 0 = Write transfwer, 1 = read transfer

#define SPI_STATUS_CLKT			0x00000200					// 1 = Slave has held the SCL signal low 
															// (clock stretching) for longer and that specified in the I2CCLKT register
#define SPI_STATUS_ERR			0x00000100					// 1 = Slave has not acknowledged its address.
#define SPI_STATUS_RXF			0x00000080					// 1 = FIFO is full.
#define SPI_STATUS_TXE			0x00000040					// 1 = FIFO is empty
#define SPI_STATUS_RXD			0x00000020					// 1 = FIFO contains at least 1 byte.
#define SPI_STATUS_TXD			0x00000010					// 1 = FIFO has space for at least 1 byte.
#define SPI_STATUS_RXR			0x00000008					// 1 = FIFO is or more full and a read is underway.
#define SPI_STATUS_TXW			0x00000004					// 1 = FIFO is less then full and a write is underway.
#define SPI_STATUS_DONE			0x00000002					// 1 = Transfer complete.
#define SPI_STATUS_TA			0x00000001					// 1 = Transfer active.


//----------------------------------------------------------------------
// AUX Control
//----------------------------------------------------------------------
#define AUX_PERIF_PA_BASE       (PERIPHERAL_PA_BASE + 0x00215000) //

#define AUX_IRQ					(AUX_PERIF_PA_BASE+0x0000)	// 0x20215000 Auxiliary Interrupt status 3
#define AUX_ENABLES				(AUX_PERIF_PA_BASE+0x0004)	// 0x20215004 Auxiliary enables 3

#define AUX_MU_PA_BASE 			(AUX_PERIF_PA_BASE+0x0040)	

#define AUX_MU_IO_REG			(AUX_MU_PA_BASE+0x0000)		// 0x20215040 Mini Uart I/O Data 8
#define AUX_MU_IER_REG			(AUX_MU_PA_BASE+0x0004)		// 0x20215044 Mini Uart Interrupt Enable 8
#define AUX_MU_IIR_REG			(AUX_MU_PA_BASE+0x0008)		// 0x20215048 Mini Uart Interrupt Identify 8
#define AUX_MU_LCR_REG			(AUX_MU_PA_BASE+0x000C)		// 0x2021504C Mini Uart Line Control 8
#define AUX_MU_MCR_REG			(AUX_MU_PA_BASE+0x0010)		// 0x20215050 Mini Uart Modem Control 8
#define AUX_MU_LSR_REG			(AUX_MU_PA_BASE+0x0014)		// 0x20215054 Mini Uart Line Status 8
#define AUX_MU_MSR_REG			(AUX_MU_PA_BASE+0x0018)		// 0x20215058 Mini Uart Modem Status 8
#define AUX_MU_SCRATCH			(AUX_MU_PA_BASE+0x001C)		// 0x2021505C Mini Uart Scratch 8
#define AUX_MU_CNTL_REG			(AUX_MU_PA_BASE+0x0020)		// 0x20215060 Mini Uart Extra Control 8
#define AUX_MU_STAT_REG			(AUX_MU_PA_BASE+0x0024)		// 0x20215064 Mini Uart Extra Status 32
#define AUX_MU_BAUD_REG			(AUX_MU_PA_BASE+0x0028)		// 0x20215068 Mini Uart Baudrate 16

#define AUX_SPI0_PA_BASE 		(AUX_PERIF_PA_BASE+0x0080)	

#define AUX_SPI0_CNTL0_REG		(AUX_SPI0_PA_BASE+0x0000)	// 0x20215080 SPI 1 Control register 0 32
#define AUX_SPI0_CNTL1_REG		(AUX_SPI0_PA_BASE+0x0004)	// 0x20215084 SPI 1 Control register 1 8
#define AUX_SPI0_STAT_REG		(AUX_SPI0_PA_BASE+0x0008)	// 0x20215088 SPI 1 Status 32
#define AUX_SPI0_IO_REG			(AUX_SPI0_PA_BASE+0x0010)	// 0x20215090 SPI 1 Data 32
#define AUX_SPI0_PEEK_REG		(AUX_SPI0_PA_BASE+0x0014)	// 0x20215094 SPI 1 Peek 16

#define AUX_SPI1_CNTL0_REG		// 0x202150C0 SPI 2 Control register 0 32
#define AUX_SPI1_CNTL1_REG		// 0x202150C4 SPI 2 Control register 1 8

//----------------------------------------------------------------------
// Mass Media Controller
#define EX_MASSMEDIA_PA_BASE    (PERIPHERAL_PA_BASE + 0x00300000)// 0x20300000

//----------------------------------------------------------------------
// USB controller
#define USB_BLOCK_OFFSET       (0x00980000) // 0x20980000			
#define USB_BLOCK_PA_BASE      (PERIPHERAL_PA_BASE + USB_BLOCK_OFFSET) // 0x20980000			
#define USB_BLOCK_SIZE         0x1000							 // 4K
//----------------------------------------------------------------------

#endif    // __BCM2835_H