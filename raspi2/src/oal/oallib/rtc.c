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

// ---------------------------------------------------------------------------
// OEMInitRTC: OPTIONAL
//
// This function initializes the RTC 
//
void InitRTC(void)
{
  

  return;
}

// ---------------------------------------------------------------------------
// OEMGetRealTime: REQUIRED
//
// This function is called by the kernel to retrieve the time from the
// real-time clock.
//
BOOL OEMGetRealTime(LPSYSTEMTIME lpst)
{
	BOOL rc = FALSE;
    OALMSG(OAL_RTC&&OAL_FUNC, (L"+OEMGetRealTime(pTime = 0x%x)\r\n", lpst));

	if (lpst == NULL) goto cleanUp;

	lpst->wYear        = 2012;
	lpst->wMonth       = 9;
	lpst->wDay         = 19;
	lpst->wDayOfWeek   = 3;
	lpst->wHour        = 0;
	lpst->wMinute      = 0;
	lpst->wSecond      = 0;
	lpst->wMilliseconds= 0;

	// Done
    rc = TRUE;
cleanUp:

    OALMSG(OAL_RTC&&OAL_FUNC, (L"-OEMGetRealTime(rc = %d)\r\n", rc));

	return rc;
}

// ---------------------------------------------------------------------------
// OEMSetRealTime: REQUIRED
//
// This function is called by the kernel to set the real-time clock.
//
BOOL OEMSetRealTime(LPSYSTEMTIME lpst)
{
  // Fill in timer code here.

  return TRUE;
}

// ---------------------------------------------------------------------------
// OEMSetAlarmTime: REQUIRED
//
// This function is called by the kernel to set the real-time clock alarm.
//
BOOL OEMSetAlarmTime(LPSYSTEMTIME lpst)
{
  // Fill in timer code here.

  return TRUE;
}

