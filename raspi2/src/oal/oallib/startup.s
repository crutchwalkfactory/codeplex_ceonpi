;
; Copyright (c) Microsoft Corporation.  All rights reserved.
;
;
; Use of this sample source code is subject to the terms of the Microsoft
; license agreement under which you licensed this sample source code. If
; you did not accept the terms of the license agreement, you are not
; authorized to use this sample source code. For the terms of the license,
; please see the license agreement between you and Microsoft or, if applicable,
; see the LICENSE.RTF on your install media or the root of your tools installation.
; THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
;
;
;------------------------------------------------------------------------------
;
;   File:  startup.s
;
;   Hardware startup routine for Raspberry Pi
;
;------------------------------------------------------------------------------

    OPT 2                                       ; disable listing
    INCLUDE     kxarm.h
    INCLUDE     bcm2835.inc
    INCLUDE     image_cfg.inc
    OPT 1                                       ; reenable listing

;------------------------------------------------------------------------------
;    Macro For VFP
;------------------------------------------------------------------------------
VFPEnable                           EQU        (0x40000000)

	IMPORT		KernelStart

    STARTUPTEXT

;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
;    StartUp Entry
;
;    Main entry point for CPU initialization.
;------------------------------------------------------------------------------
;------------------------------------------------------------------------------

    LEAF_ENTRY    StartUp

        b        ResetHandler
        b        .                ; HandlerUndef    (0x00000004)
        b        .                ; HandlerSWI        (0x00000008)
        b        .                ; HandlerPabort    (0x0000000C)
        b        .                ; HandlerDabort    (0x00000010)
        b        .                ; HandlerReserved    (0x00000014)
        b        .                ; HandlerIRQ        (0x00000018)
        b        .                ; HandlerFIQ        (0x0000001C)

;------------------------------------------------------------------------------
;    ResetHandler Function
;
;    Reset Exception Handler
;------------------------------------------------------------------------------

ResetHandler
	ldr     sp, =0x1000						; IRQStack
	
;------------------------------
;    Enable Instruction Cache
;------------------------------

;	mov        r0, #0
;	mcr        p15, 0, r0, c7, c7, 0		; Invalidate Entire I&D Cache

;	mrc        p15, 0, r0, c1, c0, 0		; Enable I Cache
;	orr        r0, r0, #R1_I
;	mcr        p15, 0, r0, c1, c0, 0

;------------------------------------
; Enable VFP via Coprocessor Access Cotrol Register
;------------------------------------
;       mrc        p15, 0, r0, c1, c0, 2
;        orr        r0, r0, #0x00F00000
;        mcr        p15, 0, r0, c1, c0, 2
;------------------------------------
; Add following: SISO added
; Enable FPEXC enable bit to enable VFP
;------------------------------------
;        MOV        r1, #0
;        MCR        p15, 0, r1, c7, c5, 4
;        MOV        r0,#VFPEnable
;        FMXR       FPEXC, r0       ; FPEXC = r0
;        nop
        nop
        nop
        nop
        nop


;------------------------------
; Setup GPIOs for use.  
;
; Current use:
;
;   GPIO 14 == UART0 TX  (Alt 0 func)
;   GPIO 15 == UART0 RX  (Alt 0 func)
;   GPIO 16 == Output	(OK LED)
;   GPIO 17 == UART0 RTS (Alt 3 func) (db 9-21-12)
;
;------------------------------

;
; Select Reg 0 controls GPIOs 0-9
; This was to set GPIO 6 to ensure that the LAN/USB hub chip was enabled.
; Looks like the init code already does this.
;

	ldr		r0, =GPFSEL0_PA_REG		; GPIO Select reg 1
	ldr		r1,[r0]					; Read reg

	ldr		r2, =(SETASMASK<<GPIO06_R0_SHFT)
	ldr		r3, =(SETASOUTPUT<<GPIO06_R0_SHFT)

	bic		r1, r1, r2				; clear out the old assignments
	orr		r1, r1, r3				; Set new GPIO assignments
	str		r1,[r0]					; Write reg

; Now set GPIO 6 (Lan 
	ldr		r0, =GPSET0_OFFSET		; GPIO Set reg 0
	ldr		r1, =(1<<6)             ; Set GPIO 6
	str		r1,[r0]					; Write reg


;
; Select Reg 1 controls GPIOs 10-19
;
	ldr		r0, =GPFSEL1_PA_REG		; GPIO Select reg 1
	ldr		r1,[r0]					; Read reg

	ldr		r2, =((SETASMASK<<GPIO14_R1_SHFT) :OR: \
			      (SETASMASK<<GPIO15_R1_SHFT) :OR: \
			      (SETASMASK<<GPIO16_R1_SHFT) :OR: \
			      (SETASMASK<<GPIO17_R1_SHFT))

	ldr		r3, =((SETASALT0<<GPIO14_R1_SHFT) :OR: \
			      (SETASALT0<<GPIO15_R1_SHFT) :OR: \
			      (SETASOUTPUT<<GPIO16_R1_SHFT) :OR: \
			      (SETASALT3<<GPIO17_R1_SHFT))

	bic		r1, r1, r2				; clear out the old assignments
	orr		r1, r1, r3				; Set new GPIO assignments
	str		r1,[r0]					; Write reg


;------------------------------
; Setup UART0 
; Baud rate at 115.2 == integer divisor 1, fractional divisor 40
;------------------------------

; Set integer divisor
	ldr		r0, =UART0_PA_IBRD		; UART0 Integer Baud rate divisor 
	ldr		r1, =0x01				; Integer divisor = 1
	str		r1,[r0]					; Set Reg

; Set fractional divisor
	ldr		r0, =UART0_PA_BBRD		; UART0 Fractional Baud rate divisor
	ldr		r1, =40					; Fractional divisor = 40
	str		r1,[r0]					; Set Reg

; Set line control register (No parity 8bits 1 stop bit)
	ldr		r0, =UART0_PA_LCRH		; UART0 line control register
	ldr		r1, =(LCRH_WLEN_8BITS :OR: LCRH_FEN)
	str		r1,[r0]					; Set Reg

; Set control register (Rx enable, Tx enable, UART enable)
	ldr		r0, =UART0_PA_CR		; UART0 control register
	ldr		r1, =(CR_RXE :OR: CR_TXE :OR: CR_UARTEN :OR: CR_RTSEN) ;Hardware recv flow control enabled
	str		r1,[r0]					; Set Reg

;------------------------------
; Write something.  This code is kept in to help debug startup problems.  It needs to be 
; commented out to enable KITL over serial.
;------------------------------
	
;	ldr		r0, =0x0d
;	bl		QuickDebugOut
;	ldr		r0, =0x0a
;	bl		QuickDebugOut
;	ldr		r0, ='R'			
;	bl		QuickDebugOut
;	ldr		r0, ='A'			
;	bl		QuickDebugOut
;	ldr		r0, ='S'
;	bl		QuickDebugOut
;	ldr		r0, ='P'
;	bl		QuickDebugOut
;	ldr		r0, ='I'
;	bl		QuickDebugOut
;	ldr		r0, =0x0d
;	bl		QuickDebugOut
;	ldr		r0, =0x0a
;	bl		QuickDebugOut

	bl		SetOKLED

;------------------------------------
;    Jump to the kernel
;------------------------------------

	add     r0, pc, #g_oalAddressTable - (. + 8)
	bl      KernelStart
	b         .            ; Should not be here...
;------------------------------------
;    Include OemAddressTable here.
;------------------------------------
        INCLUDE    OEMAddrTable.inc        


        ENTRY_END
;-----------------------------------------------------------------------
;    End of StartUp
;-----------------------------------------------------------------------

;-----------------------------------------------------------------------
;    SetOKLED
;-----------------------------------------------------------------------

	LEAF_ENTRY SetOKLED

	ldr		r0, =GPCLR0_PA_REG		; GPIO Set reg 0 (bits 0-31)
	ldr		r1, =GPIO16_R0_BIT		; Get GPIO16 bit
	str		r1,[r0]					; Clear GPIO16
        
	mov     pc, lr					; return
	ENTRY_END

;-----------------------------------------------------------------------
;    QuickDebugOut
; R0 = char to write
;-----------------------------------------------------------------------

	LEAF_ENTRY QuickDebugOut
QuickDebugOut_1
	ldr		r1, =UART0_PA_FR		; UART0 flag register
	ldr		r2,[r1]
	tst		r2, #FR_TXFF
	bne		QuickDebugOut_1

	ldr		r1, =UART0_PA_DR		; UART0 data register
	str		r0,[r1]					; Set Reg
        
	mov     pc, lr					; return
	ENTRY_END


        END

