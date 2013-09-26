;
; Copyright (c) Boling Consulting Inc.  All rights reserved.
;

;
; This file has to be synced with image_cfg.h
;


;// DRAM Base Address
DRAM_BASE_PA_START      EQU     (0x00000000)
DRAM_BASE_VA_START      EQU     (0x80000000)

DRAM_SIZE               EQU     (0x10000000)	;256K


;//------------------------------------------------------------------------------

;// SLoader Area
SLOADER_IMAGE_PA_START  EQU    (0x00000000)
SLOADER_CODE_PA_START   EQU    (0x00008000)
SLOADER_SIZE            EQU    (0x00002000)

;//------------------------------------------------------------------------------


;//------------------------------------------------------------------------------

;// NK Area

;//------------------------------------------------------------------------------

;// BSP ARGs Area
IMAGE_SHARE_ARGS_OFFSET      EQU    (0x00020800)
IMAGE_SHARE_ARGS_PA_START    EQU    (DRAM_BASE_PA_START+IMAGE_SHARE_ARGS_OFFSET)
IMAGE_SHARE_ARGS_VA_START    EQU    (DRAM_BASE_VA_START+IMAGE_SHARE_ARGS_OFFSET)
IMAGE_SHARE_ARGS_SIZE        EQU    (0x00000800)

;//------------------------------------------------------------------------------

;------------------------------------------------------------------------------
;    OEM Stack Layout
;------------------------------------------------------------------------------

STACK_PA_TOP                 EQU    (DRAM_BASE_PA_START+SLOADER_CODE_PA_START)    ; Stack Top is in front of loader Image

;
;; Stack Size of Each Mode
FIQStackSize    EQU    256
IRQStackSize    EQU    256
AbortStackSize  EQU    256
UndefStackSize  EQU    256
SVCStackSize    EQU    1024
;;UserStackSize   EQU    2048
;
;; Stack Location of Each Mode (in Physical Address)
FIQStack_PA     EQU    (STACK_PA_TOP    - 0x0)
IRQStack_PA     EQU    (FIQStack_PA        - FIQStackSize)
AbortStack_PA   EQU    (IRQStack_PA        - IRQStackSize)
UndefStack_PA   EQU    (AbortStack_PA        - AbortStackSize)
SVCStack_PA     EQU    (UndefStack_PA        - UndefStackSize)
UserStack_PA    EQU    (SVCStack_PA        - SVCStackSize)

    END

