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
#include <windows.h>
#include <oemglobal.h>

#include <nkintr.h>
#include <oal.h>
#include <bsp.h>

//------------------------------------------------------------------------------
// Externs
//
extern PBYTE g_p2835Regs;

//------------------------------------------------------------------------------
// Local Variables
//
volatile PDWORD pTimerCtlStatReg = 0;
volatile PDWORD pTimerCntLowReg = 0;
volatile PDWORD pTimerCntHighReg = 0;
volatile PDWORD pTimerCompT0Reg = 0;
volatile PDWORD pTimerCompT1Reg = 0;
volatile PDWORD pTimerCompT2Reg = 0;
volatile PDWORD pTimerCompT3Reg = 0;

// ---------------------------------------------------------------------------
// ResetSchedulerTimer
//
void ResetSchedulerTimer(UINT32 countsPerMSec)
{
	// Set the compare reg value for Timer 3.  Timers run at a 1uS interval
	*pTimerCompT3Reg = *pTimerCntLowReg + countsPerMSec;

	// Clear match bit.
	*pTimerCtlStatReg = TIMER_CS_MATCH_3;  

}
// ---------------------------------------------------------------------------
// OALTimerInit
//
BOOL OALTimerInit(UINT32 msecPerSysTick, UINT32 countsPerMSec, UINT32 countsMargin)
{
	// We're going to use Timer3 for the scheduler.  This apparently
	// is the same one Linux uses so we won't conflict with other
	// code.

	BOOL rc = FALSE;

	if (g_p2835Regs == 0)
		return FALSE;

	// Save ptrs to the regs we need
	pTimerCtlStatReg = (PDWORD) (g_p2835Regs + SYSTEM_TIMER_BASEOFF + TIMER_CS_OFFSET);
	pTimerCntLowReg = (PDWORD) (g_p2835Regs + SYSTEM_TIMER_BASEOFF + TIMER_CHL_OFFSET);
	pTimerCntHighReg = (PDWORD) (g_p2835Regs + SYSTEM_TIMER_BASEOFF + TIMER_CHI_OFFSET);
	pTimerCompT0Reg = (PDWORD) (g_p2835Regs + SYSTEM_TIMER_BASEOFF + TIMER_C0_OFFSET);
	pTimerCompT1Reg = (PDWORD) (g_p2835Regs + SYSTEM_TIMER_BASEOFF + TIMER_C1_OFFSET);
	pTimerCompT2Reg = (PDWORD) (g_p2835Regs + SYSTEM_TIMER_BASEOFF + TIMER_C2_OFFSET);
	pTimerCompT3Reg = (PDWORD) (g_p2835Regs + SYSTEM_TIMER_BASEOFF + TIMER_C3_OFFSET);

    // Set Kernel Exported Globals to Initial values
    idleconv = countsPerMSec;
    curridlehigh = 0;
    curridlelow = 0;

	//NKDbgPrintfW (L"pTimerCtlStatReg:%x\r\n", *pTimerCtlStatReg);
	//NKDbgPrintfW (L"pTimerCntLowReg: %x\r\n", *pTimerCntLowReg);
	//NKDbgPrintfW (L"pTimerCntHighReg:%x\r\n", *pTimerCntHighReg);
	//NKDbgPrintfW (L"pTimerCompT0Reg: %x\r\n", *pTimerCompT0Reg);
	//NKDbgPrintfW (L"pTimerCompT1Reg: %x\r\n", *pTimerCompT1Reg);
	//NKDbgPrintfW (L"pTimerCompT2Reg: %x\r\n", *pTimerCompT2Reg);
	//NKDbgPrintfW (L"pTimerCompT3Reg: %x\r\n", *pTimerCompT3Reg);


	// Set the compare reg value for Timer 3.  Timers run at a 1uS interval
	ResetSchedulerTimer(OEM_COUNT_1MS);
	//{
	//	DWORD a,b,c,d;
	//	a = *pTimerCntLowReg;
	//	b = *pTimerCompT1Reg;
	//	c = *pTimerCtlStatReg;
	//	d = *pTimerCntLowReg;
	//
	//	NKDbgPrintfW (L"pTimerCntLowReg: %x\r\n", a);
	//	NKDbgPrintfW (L"pTimerCntLowReg: %x\r\n", b);
	//	NKDbgPrintfW (L"pTimerCntLowReg: %x\r\n", c);
	//	NKDbgPrintfW (L"pTimerCntLowReg: %x\r\n", d);
	//}

	//NKDbgPrintfW (L"pTimerCtlStatReg:%x\r\n", *pTimerCtlStatReg);
	//NKDbgPrintfW (L"pTimerCntLowReg: %x\r\n", *pTimerCntLowReg);
	//NKDbgPrintfW (L"pTimerCntHighReg:%x\r\n", *pTimerCntHighReg);
	//NKDbgPrintfW (L"pTimerCompT3Reg: %x\r\n", *pTimerCompT3Reg);


	// Enable the timer interrupt
	OEMInterruptEnable(SYSINTR_RESCHED, 0, 0);

	//NKDbgPrintfW (L"pTimerCtlStatReg:%x\r\n", *pTimerCtlStatReg);
	//NKDbgPrintfW (L"pTimerCntLowReg: %x\r\n", *pTimerCntLowReg);
	//NKDbgPrintfW (L"pTimerCntHighReg:%x\r\n", *pTimerCntHighReg);
	//NKDbgPrintfW (L"pTimerCompT3Reg: %x\r\n", *pTimerCompT3Reg);

	//{
	//	DWORD a,b,c,d;
	//	a = *pTimerCntLowReg;
	//	b = *pTimerCntLowReg;
	//	c = *pTimerCntLowReg;
	//	d = *pTimerCntLowReg;
	//
	//	NKDbgPrintfW (L"pTimerCntLowReg: %x\r\n", a);
	//	NKDbgPrintfW (L"pTimerCntLowReg: %x\r\n", b);
	//	NKDbgPrintfW (L"pTimerCntLowReg: %x\r\n", c);
	//	NKDbgPrintfW (L"pTimerCntLowReg: %x\r\n", d);
	//}

	return rc;
}

// ---------------------------------------------------------------------------
// OEMQueryPerformanceCounter: OPTIONAL
//
// This function retrieves the current value of the high-resolution
// performance counter.
//
// This function is optional.  Generally, it should be implemented for
// platforms that provide timer functions with higher granularity than
// OEMGetTickCount.
//
BOOL OEMQueryPerformanceCounter(LARGE_INTEGER* lpPerformanceCount)
{
  // Fill in high-resolution timer code here.

  return TRUE;
}

// ---------------------------------------------------------------------------
// OEMQueryPerformanceFrequency: OPTIONAL
//
// This function retrieves the frequency of the high-resolution
// performance counter.
//
// This function is optional.  Generally, it should be implemented for
// platforms that provide timer functions with higher granularity than
// OEMGetTickCount.
//
BOOL OEMQueryPerformanceFrequency(LARGE_INTEGER* lpFrequency)
{
  // Fill in high-resolution timer frequency code here.

  return TRUE;
}

// ---------------------------------------------------------------------------
// OEMGetTickCount: REQUIRED
//
// For Release configurations, this function returns the number of
// milliseconds since the device booted, excluding any time that the system
// was suspended. GetTickCount starts at zero on boot and then counts up from
// there.
//
// For debug configurations, 180 seconds is subtracted from the
// number of milliseconds since the device booted. This enables code that
// uses GetTickCount to be easily tested for correct overflow handling.
//
DWORD OEMGetTickCount(void)
{
  return CurMSec;
}

// ---------------------------------------------------------------------------
// OEMUpdateReschedTime: OPTIONAL
//
// This function is called by the kernel to set the next reschedule time.  It
// is used in variable-tick timer implementations.
//
// It must set the timer hardware to interrupt at dwTick time, or as soon as
// possible if dwTick has already passed.  It must save any information
// necessary to calculate the g_pNKGlobal->dwCurMSec variable correctly.
//
VOID OEMUpdateReschedTime(DWORD dwTick)
{
  // Fill in timer code here.

  return;
}

// ---------------------------------------------------------------------------
// OEMRefreshWatchDog: OPTIONAL
//
// This function is called by the kernel to refresh the hardware watchdog
// timer.
//
void OEMRefreshWatchDog(void)
{
  // Fill in watchdog code here.

  return;
}

// ---------------------------------------------------------------------------
// OEMProfilerTimerEnable: OPTIONAL
//
// This function enables an optional profiler interrupt.
//
void OEMProfilerTimerEnable(DWORD dwUSec)
{
  // Fill in profiler code here.

  return;
}

// ---------------------------------------------------------------------------
// OEMProfilerTimerDisable: OPTIONAL
//
// This function disables an optional profiler interrupt.
//
void OEMProfilerTimerDisable(void)
{
  // Fill in profiler code here.

  return;
}

