;
; Copyright (c) Boling Consulting Inc.  All rights reserved.
;

;
; This file has to be synced with BCM2835.h
;

;-----------------------------------------------------------------------
; (db) These defines are copied from the SMDK6410 BSP since it uses the same 
; ARM core (ARM1176) as the BCM2835.
;-----------------------------------------------------------------------


;---------------------------
; CPSR Mode Bit Definition
;---------------------------
Mode_USR    EQU    (0x10)
Mode_FIQ    EQU    (0x11)
Mode_IRQ    EQU    (0x12)
Mode_SVC    EQU    (0x13)
Mode_ABT    EQU    (0x17)
Mode_UND    EQU    (0x1B)
Mode_SYS    EQU    (0x1F)
Mode_MASK    EQU    (0x1F)
NOINT        EQU    (0xC0)
I_Bit        EQU    (0x80)
F_Bit        EQU    (0x40)

;---------------------------
; CP15 Mode Bit Definition
;---------------------------
R1_iA        EQU    (1<<31)
R1_nF        EQU    (1<<30)
R1_VE        EQU    (1<<24)
R1_I        EQU    (1<<12)
R1_BP        EQU    (1<<11)    ; Z bit
R1_C        EQU    (1<<2)
R1_A        EQU    (1<<1)
R1_M        EQU    (1<<0)

;-----------------------------------------------------------------------
; (db) End of copy from s3c6410.inc from SMDK6410 BSP
;-----------------------------------------------------------------------


; Mapped to this address by BCM GPU loader
PERIPHERAL_PA_BASE	EQU    0x20000000


;----------------------------------------------------------------------
; GPIO Registers
;
GPIO_PA_BASE	EQU		(PERIPHERAL_PA_BASE + 0x00200000)	; 0x20200000

GPFSEL0_OFFSET	EQU		0x0000	;	GPIO Function Select 0 32 R/W
GPFSEL1_OFFSET	EQU		0x0004	;	GPIO Function Select 1 32 R/W
GPFSEL2_OFFSET	EQU		0x0008	;	GPIO Function Select 2 32 R/W
GPFSEL3_OFFSET	EQU		0x000C	;	GPIO Function Select 3 32 R/W
GPFSEL4_OFFSET	EQU		0x0010	;	GPIO Function Select 4 32 R/W
GPFSEL5_OFFSET	EQU		0x0014	;	GPIO Function Select 5 32 R/W

GPSET0_OFFSET	EQU		0x001C	;	GPIO Pin Output Set 0 32 W
GPSET1_OFFSET	EQU		0x0020	;	GPIO Pin Output Set 1 32 W

GPCLR0_OFFSET	EQU		0x0028	;	GPIO Pin Output Clear 0 32 W
GPCLR1_OFFSET	EQU		0x002C	;	GPIO Pin Output Clear 1 32 W

GPLEV0_OFFSET	EQU		0x0034	;	GPIO Pin Level 0 32 R
GPLEV1_OFFSET	EQU		0x0038	;	GPIO Pin Level 1 32 R

GPEDS0_OFFSET	EQU		0x0040	;	GPIO Pin Event Detect Status 0 32 R/W
GPEDS1_OFFSET	EQU		0x0044	;	GPIO Pin Event Detect Status 1 32 R/W

GPREN0_OFFSET	EQU		0x004C	;	GPIO Pin Rising Edge Detect Enable 0 32 R/W
GPREN1_OFFSET	EQU		0x0050	;	GPIO Pin Rising Edge Detect Enable 1 32 R/W

GPFEN0_OFFSET	EQU		0x0058	;	GPIO Pin Falling Edge Detect Enable 0 32 R/W
GPFEN1_OFFSET	EQU		0x005C	;	GPIO Pin Falling Edge Detect Enable 1 32 R/W
	

GPHEN0_OFFSET	EQU		0x0064	;	GPIO Pin High Detect Enable 0 32 R/W
GPHEN1_OFFSET	EQU		0x0068	;	GPIO Pin High Detect Enable 1 32 R/W

GPLEN0_OFFSET	EQU		0x0070	;	GPIO Pin Low Detect Enable 0 32 R/W
GPLEN1_OFFSET	EQU		0x0074	;	GPIO Pin Low Detect Enable 1 32 R/W

GPAREN0_OFFSET	EQU		0x007C	;	GPIO Pin Async. Rising Edge Detect 0 32 R/W
GPAREN1_OFFSET	EQU		0x0080	;	GPIO Pin Async. Rising Edge Detect 1 32 R/W

GPAFEN0_OFFSET	EQU		0x0088	;	GPIO Pin Async. Falling Edge Detect 0 32 R/W
GPAFEN1_OFFSET	EQU		0x008C	;	GPIO Pin Async. Falling Edge Detect 1 32 R/W

GPPUD_OFFSET	EQU	0x0094	;	GPIO Pin Pull-up/down Enable 32 R/W
GPPUDCLK0_OFFSET	EQU	0x0098	;	GPIO Pin Pull-up/down Enable Clock 0 32 R/W
GPPUDCLK1_OFFSET	EQU	0x009C	;	GPIO Pin Pull-up/down Enable Clock 1 32 R/W


; Physical Addresses
GPFSEL0_PA_REG	EQU		(GPIO_PA_BASE+GPFSEL0_OFFSET)	;	0x20200000	GPIO Function Select 0 32 R/W
GPFSEL1_PA_REG	EQU		(GPIO_PA_BASE+GPFSEL1_OFFSET)	;	0x20200004	GPIO Function Select 1 32 R/W
GPFSEL2_PA_REG	EQU		(GPIO_PA_BASE+GPFSEL2_OFFSET)	;	0x20200008	GPIO Function Select 2 32 R/W
GPFSEL3_PA_REG	EQU		(GPIO_PA_BASE+GPFSEL3_OFFSET)	;	0x2020000C	GPIO Function Select 3 32 R/W
GPFSEL4_PA_REG	EQU		(GPIO_PA_BASE+GPFSEL4_OFFSET)	;	0x20200010	GPIO Function Select 4 32 R/W
GPFSEL5_PA_REG	EQU		(GPIO_PA_BASE+GPFSEL5_OFFSET)	;	0x20200014	GPIO Function Select 5 32 R/W

GPSET0_PA_REG	EQU		(GPIO_PA_BASE+GPSET0_OFFSET)	;	0x2020001C	GPIO Pin Output Set 0 32 W
GPSET1_PA_REG	EQU		(GPIO_PA_BASE+GPSET1_OFFSET)	;	0x20200020	GPIO Pin Output Set 1 32 W

GPCLR0_PA_REG	EQU		(GPIO_PA_BASE+GPCLR0_OFFSET)	;	0x20200028	GPIO Pin Output Clear 0 32 W
GPCLR1_PA_REG	EQU		(GPIO_PA_BASE+GPCLR1_OFFSET)	;	0x2020002C	GPIO Pin Output Clear 1 32 W

GPLEV0_PA_REG	EQU		(GPIO_PA_BASE+GPLEV0_OFFSET)	;	0x20200034	GPIO Pin Level 0 32 R
GPLEV1_PA_REG	EQU		(GPIO_PA_BASE+GPLEV1_OFFSET)	;	0x20200038	GPIO Pin Level 1 32 R

GPEDS0_PA_REG	EQU		(GPIO_PA_BASE+GPEDS0_OFFSET)	;	0x20200040	GPIO Pin Event Detect Status 0 32 R/W
GPEDS1_PA_REG	EQU		(GPIO_PA_BASE+GPEDS1_OFFSET)	;	0x20200044	GPIO Pin Event Detect Status 1 32 R/W

GPREN0_PA_REG	EQU		(GPIO_PA_BASE+GPREN0_OFFSET)	;	0x2020004C	GPIO Pin Rising Edge Detect Enable 0 32 R/W
GPREN1_PA_REG	EQU		(GPIO_PA_BASE+GPREN1_OFFSET)	;	0x20200050	GPIO Pin Rising Edge Detect Enable 1 32 R/W

GPFEN0_PA_REG	EQU		(GPIO_PA_BASE+GPFEN0_OFFSET)	;	0x20200058	GPIO Pin Falling Edge Detect Enable 0 32 R/W
GPFEN1_PA_REG	EQU		(GPIO_PA_BASE+GPFEN1_OFFSET)	;	0x2020005C	GPIO Pin Falling Edge Detect Enable 1 32 R/W

GPHEN0_PA_REG	EQU		(GPIO_PA_BASE+GPHEN0_OFFSET)	;	0x20200064	GPIO Pin High Detect Enable 0 32 R/W
GPHEN1_PA_REG	EQU		(GPIO_PA_BASE+GPHEN1_OFFSET)	;	0x20200068	GPIO Pin High Detect Enable 1 32 R/W
			 	
GPLEN0_PA_REG	EQU		(GPIO_PA_BASE+GPLEN0_OFFSET)	;	0x20200070	GPIO Pin Low Detect Enable 0 32 R/W
GPLEN1_PA_REG	EQU		(GPIO_PA_BASE+GPLEN1_OFFSET)	;	0x20200074	GPIO Pin Low Detect Enable 1 32 R/W
			 	
GPAREN0_PA_REG	EQU		(GPIO_PA_BASE+GPAREN0_OFFSET)	;	0x2020007C	GPIO Pin Async. Rising Edge Detect 0 32 R/W
GPAREN1_PA_REG	EQU		(GPIO_PA_BASE+GPAREN1_OFFSET)	;	0x20200080	GPIO Pin Async. Rising Edge Detect 1 32 R/W
			  	
GPAFEN0_PA_REG	EQU		(GPIO_PA_BASE+GPAFEN0_OFFSET)	;	0x20200088	GPIO Pin Async. Falling Edge Detect 0 32 R/W
GPAFEN1_PA_REG	EQU		(GPIO_PA_BASE+GPAFEN1_OFFSET)	;	0x2020008C	GPIO Pin Async. Falling Edge Detect 1 32 R/W
			  	
GPPUD_PA_REG	EQU		(GPIO_PA_BASE+GPPUD_OFFSET)		;	0x20200094	GPIO Pin Pull-up/down Enable 32 R/W
GPPUDCLK0_PA_REG	EQU	(GPIO_PA_BASE+GPPUDCLK0_OFFSET)	;	0x20200098	GPIO Pin Pull-up/down Enable Clock 0 32 R/W
GPPUDCLK1__PA_REG	EQU	(GPIO_PA_BASE+GPPUDCLK1_OFFSET)	;	0x2020009C	GPIO Pin Pull-up/down Enable Clock 1 32 R/W


; The following defines help set the function of each of the GPIO lines
; 
SETASINPUT		EQU	0x00
SETASOUTPUT		EQU	0x01
SETASALT0		EQU	0x04
SETASALT1		EQU	0x05
SETASALT2		EQU	0x06
SETASALT3		EQU	0x07
SETASALT4		EQU 0x03
SETASALT5		EQU 0x02
SETASMASK		EQU 0x07
					
; shift values for the GPIO function registers
GPIOSEL_SHIFTx9		EQU	27
GPIOSEL_SHIFTx8		EQU	24
GPIOSEL_SHIFTx7		EQU	21
GPIOSEL_SHIFTx6		EQU	18
GPIOSEL_SHIFTx5		EQU	15
GPIOSEL_SHIFTx4		EQU	12
GPIOSEL_SHIFTx3		EQU	9
GPIOSEL_SHIFTx2		EQU	06
GPIOSEL_SHIFTx1		EQU	03
GPIOSEL_SHIFTx0		EQU	00

; GPIO bit defines for the Set and Clear regs
GPIO00_R0_BIT		EQU			(1 << 00)
GPIO01_R0_BIT		EQU			(1 << 01)
GPIO02_R0_BIT		EQU			(1 << 02)
GPIO03_R0_BIT		EQU			(1 << 03)
GPIO04_R0_BIT		EQU			(1 << 04)
GPIO05_R0_BIT		EQU			(1 << 05)
GPIO06_R0_BIT		EQU			(1 << 06)
GPIO07_R0_BIT		EQU			(1 << 07)
GPIO08_R0_BIT		EQU			(1 << 8)
GPIO09_R0_BIT		EQU			(1 << 9)
GPIO10_R0_BIT		EQU			(1 << 10)
GPIO11_R0_BIT		EQU			(1 << 11)
GPIO12_R0_BIT		EQU			(1 << 12)
GPIO13_R0_BIT		EQU			(1 << 13)
GPIO14_R0_BIT		EQU			(1 << 14)
GPIO15_R0_BIT		EQU			(1 << 15)
GPIO16_R0_BIT		EQU			(1 << 16)
GPIO17_R0_BIT		EQU			(1 << 17)
GPIO18_R0_BIT		EQU			(1 << 18)
GPIO19_R0_BIT		EQU			(1 << 19)
GPIO20_R0_BIT		EQU			(1 << 20)
GPIO21_R0_BIT		EQU			(1 << 21)
GPIO22_R0_BIT		EQU			(1 << 22)
GPIO23_R0_BIT		EQU			(1 << 23)
GPIO24_R0_BIT		EQU			(1 << 24)
GPIO25_R0_BIT		EQU			(1 << 25)
GPIO26_R0_BIT		EQU			(1 << 26)
GPIO27_R0_BIT		EQU			(1 << 27)
GPIO28_R0_BIT		EQU			(1 << 28)
GPIO29_R0_BIT		EQU			(1 << 29)
GPIO30_R0_BIT		EQU			(1 << 30)
GPIO31_R0_BIT		EQU			(1 << 31)

GPIO32_R1_BIT		EQU			(1 << 00)			
GPIO33_R1_BIT		EQU			(1 << 01)			
GPIO34_R1_BIT		EQU			(1 << 02)			
GPIO35_R1_BIT		EQU			(1 << 03)			
GPIO36_R1_BIT		EQU			(1 << 04)			
GPIO37_R1_BIT		EQU			(1 << 05)			
GPIO38_R1_BIT		EQU			(1 << 06)			
GPIO39_R1_BIT		EQU			(1 << 07)			
GPIO40_R1_BIT		EQU			(1 << 8)			
GPIO41_R1_BIT		EQU			(1 << 9)			
GPIO42_R1_BIT		EQU			(1 << 10)			
GPIO43_R1_BIT		EQU			(1 << 11)			
GPIO44_R1_BIT		EQU			(1 << 12)			
GPIO45_R1_BIT		EQU			(1 << 13)			
GPIO46_R1_BIT		EQU			(1 << 14)			
GPIO47_R1_BIT		EQU			(1 << 15)			
GPIO48_R1_BIT		EQU			(1 << 16)			
GPIO49_R1_BIT		EQU			(1 << 17)			
GPIO50_R1_BIT		EQU			(1 << 18)			
GPIO51_R1_BIT		EQU			(1 << 19)			
GPIO52_R1_BIT		EQU			(1 << 20)			
GPIO53_R1_BIT		EQU			(1 << 21)			


GPIO09_R0_SHFT		EQU			GPIOSEL_SHIFTx9
GPIO08_R0_SHFT		EQU			GPIOSEL_SHIFTx8
GPIO07_R0_SHFT		EQU			GPIOSEL_SHIFTx7
GPIO06_R0_SHFT		EQU			GPIOSEL_SHIFTx6
GPIO05_R0_SHFT		EQU			GPIOSEL_SHIFTx5
GPIO04_R0_SHFT		EQU			GPIOSEL_SHIFTx4
GPIO03_R0_SHFT		EQU			GPIOSEL_SHIFTx3
GPIO02_R0_SHFT		EQU			GPIOSEL_SHIFTx2
GPIO01_R0_SHFT		EQU			GPIOSEL_SHIFTx1
GPIO00_R0_SHFT		EQU			GPIOSEL_SHIFTx0

GPIO19_R1_SHFT		EQU			GPIOSEL_SHIFTx9
GPIO18_R1_SHFT		EQU			GPIOSEL_SHIFTx8
GPIO17_R1_SHFT		EQU			GPIOSEL_SHIFTx7
GPIO16_R1_SHFT		EQU			GPIOSEL_SHIFTx6
GPIO15_R1_SHFT		EQU			GPIOSEL_SHIFTx5
GPIO14_R1_SHFT		EQU			GPIOSEL_SHIFTx4
GPIO13_R1_SHFT		EQU			GPIOSEL_SHIFTx3
GPIO12_R1_SHFT		EQU			GPIOSEL_SHIFTx2
GPIO11_R1_SHFT		EQU			GPIOSEL_SHIFTx1
GPIO10_R1_SHFT		EQU			GPIOSEL_SHIFTx0


;----------------------------------------------------------------------
; Primary UART
;
; Note: This is the UART that comes to GPIO pins 14&15. For Raspberry Pi, this is the UART on P1.
;
;----------------------------------------------------------------------
UART0_PA_BASE	EQU	(PERIPHERAL_PA_BASE + 0x00201000)		

UART0_PA_DR		EQU				(UART0_PA_BASE+0x0000)  ; Data Register
UART0_PA_RSRECR	EQU				(UART0_PA_BASE+0x0004)  ; Receive Status Reg
UART0_PA_FR		EQU				(UART0_PA_BASE+0x0018)  ; Flag Register
UART0_PA_IBRD	EQU				(UART0_PA_BASE+0x0024)  ; Integer Baud rate divisor 
UART0_PA_BBRD	EQU				(UART0_PA_BASE+0x0028)  ; Fractional Baud rate divisor
UART0_PA_LCRH	EQU				(UART0_PA_BASE+0x002C)  ; Line Control register 
UART0_PA_CR  	EQU				(UART0_PA_BASE+0x0030)  ; Control register 
UART0_PA_IFLS	EQU				(UART0_PA_BASE+0x0034)  ; Interupt FIFO Level Select Register
UART0_PA_IMSC	EQU				(UART0_PA_BASE+0x0038)  ; Interupt Mask Set Clear Register 
UART0_PA_RIS 	EQU				(UART0_PA_BASE+0x003C)  ; Raw Interupt Status Register 
UART0_PA_MIS 	EQU				(UART0_PA_BASE+0x0040)  ; Masked Interupt Status Register 
UART0_PA_ICR 	EQU				(UART0_PA_BASE+0x0044)  ; Interupt Clear Register 
UART0_PA_DMACR	EQU				(UART0_PA_BASE+0x0048)  ; DMA Control Register 
UART0_PA_ITCR 	EQU				(UART0_PA_BASE+0x0080)  ; Test Control register 
UART0_PA_ITIP	EQU				(UART0_PA_BASE+0x0084)  ; Integration test input reg 
UART0_PA_ITOP	EQU				(UART0_PA_BASE+0x0088)  ; Integration test output reg 
UART0_PA_TDR 	EQU				(UART0_PA_BASE+0x008C)  ; Test Data reg 


;Line Control Register (LCRH)
LCRH_SPS		EQU			0x00000080				; Stick Parity 0-disabled
LCRH_WLEN_MASK	EQU			0x00000060				; Word Length
LCRH_WLEN_8BITS	EQU			0x00000060				; 
LCRH_WLEN_7BITS	EQU			0x00000040				; 
LCRH_WLEN_6BITS	EQU			0x00000020				; 
LCRH_WLEN_5BITS	EQU			0x00000000				; 
LCRH_FEN		EQU			0x00000010				; FIFO Enable 1-Enabled
LCRH_STP2		EQU			0x00000008				; Two Stop Bits 1-Enabled
LCRH_EPS		EQU			0x00000004				; Even Parity 0-Odd, 1-Even
LCRH_PEN		EQU			0x00000002				; Parity Enable 1-Enable Parity tmit/chk
LCRH_BRK		EQU			0x00000001				; Send Break

;Control Register (CR)
;NOTE:
;To enable transmission, the TXE bit and UARTEN bit must be set to 1.
;Similarly, to enable reception, the RXE bit and UARTEN bit, must be set to 1.
;NOTE:
;Program the control registers as follows:
; 1. Disable the UART.
; 2. Wait for the end of transmission or reception of the current character.
; 3. Flush the transmit FIFO by setting the FEN bit to 0 in the Line Control
;    Register, UART_LCRH.
; 4. Reprogram the Control Register, UART_CR.
; 5. Enable the UART.
CR_CTSEN		EQU			0x00008000				;CTS hardware flow enable. 1-enable h/w flow
CR_RTSEN		EQU			0x00004000				;RTS hardware flow enable. 1-enable h/w flow
CR_RTS_NOT		EQU			0x00000800				;sets RTS line to compliment of bit state
CR_RXE			EQU			0x00000200				;Enables receive section of UART
CR_TXE			EQU			0x00000100				;Enables transmit section of UART
CR_LBE			EQU			0x00000080				;Enables Loopback
CR_UARTEN		EQU			0x00000001				;UART Enable 1-Enable UART

; Flag Register
FR_TXFE			EQU			0x00000080				; Transmit FIFO empty
FR_RXFF  		EQU			0x00000040				; Receive FIFO full
FR_TXFF  		EQU			0x00000020				; Transmit FIFO full
FR_RXFE  		EQU			0x00000010				; Receive FIFO empty
FR_BUSY  		EQU			0x00000008				; Busy
FR_CTS_NOT 		EQU			0x00000001				; Clear to Send (Line will be compliment of bit state)






    END		  

