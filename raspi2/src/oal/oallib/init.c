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
#include <winnt.h>
#include <pkfuncs.h>
#include <oemglobal.h>

#include <bsp.h>
#include <image_cfg.h>
#include <vfpSupport.h>
#include <mailbox.h>

// Init.c
// The comments in this file will vary from OS version to version.
// Look up OEMGlobal on MSDN for a full reference of OAL functions,
// or see public\common\oak\inc\oemglobal.h
//
// All OAL functions in the template BSP fall into one of three categories:
// REQUIRED - you must implement this function for kernel functionality
// OPTIONAL - you may implement this function to enable specific functionality
// CUSTOM   - this function is a helper function specific to this BSP
//
// OEMGlobal is initialized by the kernel to use default function names
// for all required functions.  See the comments in
// public\common\oak\inc\oemglobal.h for a concise listing of the default
// function names along with their required/optional status.
//
// In this BSP all of the functions are implemented using the default
// function names.  However, with the exception of the OEMInitDebugSerial
// and OEMInit functions, you can change these function names from the
// defaults by pointing the kernel calls to use a different name - see
// RemapOALGlobalFunctions for an example of how to do this.  
//


// custom BSP functions
void RemapOALGlobalFunctions(void);
void SetOALGlobalVariables(void);
void InitRTC(void);

BOOL OEMSetMemoryAttributes (
    LPVOID pVirtAddr,       // Virtual address of region
    LPVOID pPhysAddrShifted,// PhysicalAddress >> 8 (to support up to 40 bit address)
    DWORD  cbSize,          // Size of the region
    DWORD  dwAttributes     // attributes to be set
    );

// forward declarations of optional OAL functions
//void OEMWriteDebugByte(BYTE bChar);
//void OEMWriteDebugString(LPCWSTR pszStr);
//int OEMReadDebugByte(void);
//void OEMWriteDebugLED(WORD wIndex, DWORD dwPattern);
//void OEMInitClock(void);
//BOOL OEMQueryPerformanceCounter(LARGE_INTEGER* lpPerformanceCount);
//BOOL OEMQueryPerformanceFrequency(LARGE_INTEGER* lpFrequency);
//void OEMNotifyThreadExit(DWORD dwThrdId, DWORD dwExitCode);
//void OEMNotifyReschedule(DWORD dwThrdId, DWORD dwPrio, DWORD dwQuantum, DWORD dwFlags);
//DWORD OEMNotifyIntrOccurs(DWORD dwSysIntr);
//VOID OEMUpdateReschedTime(DWORD dwTick);
//DWORD OEMEnumExtensionDRAM(PMEMORY_SECTION pMemSections, DWORD dwMemSections);
//DWORD OEMCalcFSPages(DWORD dwMemPages, DWORD dwDefaultFSPages);
//void OEMInitCoProcRegs(LPBYTE pArea);
//void OEMSaveCoProcRegs(LPBYTE pArea);
//void OEMRestoreCoProcRegs(LPBYTE pArea);
//DWORD OEMReadRegistry(DWORD dwFlags, LPBYTE pBuf, DWORD len);
//BOOL OEMWriteRegistry (DWORD dwFlags, LPBYTE pBuf, DWORD len);
//void OEMProfilerTimerEnable(DWORD dwUSec);
//void OEMProfilerTimerDisable(void);
//BOOL OEMIsRom(DWORD dwShiftedPhysAddr);
//void OEMMapW32Priority(int nPrios, LPBYTE pPrioMap);
//BOOL OEMMpStartAllCPUs(PLONG pnCpus, FARPROC pfnContinue);
//DWORD OEMMpPerCPUInit(void);
//BOOL OEMMpCpuPowerFunc(DWORD dwProcessor, BOOL fOnOff, DWORD dwHint);
//void OEMIpiHandler(DWORD dwCommand, DWORD dwData);
//BOOL OEMSendIPI(DWORD dwType, DWORD dwTarget);
//void OEMIdleEx(LARGE_INTEGER *pliIdleTime);
//void OEMInitInterlockedFunctions(void);
//#if defined (ARM)
//void OEMSaveVFPCtrlRegs(LPDWORD lpExtra, int nMaxRegs);
//void OEMRestoreVFPCtrlRegs(LPDWORD lpExtra, int nMaxRegs);
//BOOL OEMHandleVFPException(ULONG fpexc, EXCEPTION_RECORD* pExr, CONTEXT* pContext, DWORD* pdwReserved, BOOL fInKMode);
//BOOL OEMIsVFPFeaturePresent(DWORD dwProcessorFeature);
//BOOL OEMVFPCoprocControl(DWORD dwCommand);
//void OEMPTEUpdateBarrier(LPVOID pte, DWORD cbSize);
//#endif

//------------------------------------------------------------------------------
// Externs
//

// Declaration of KITL entry point
extern BOOL WINAPI KitlDllMain (HINSTANCE DllInstance, DWORD dwReason, LPVOID Reserved);

// Declaration of kernel security cookie for linking purposes
extern DWORD_PTR __security_cookie;
extern DWORD_PTR __security_cookie_complement;

// Declaration of kernel page pool parameters for linking purposes
extern NKPagePoolParameters pagePoolParams;


//------------------------------------------------------------------------------
// Global Variables
//
extern PBYTE g_p2835Regs;


void DumpGPIOSettings ()
{
	
	// Save ptrs to the regs we need
	PDWORD pGPIO;
	DWORD dwData, dwData1;
	BYTE bCfg[10];
	int i, j;
		
	OALLogSerial(L"DumpGPIOSettings++  p2835Regs = %08x\r\n", g_p2835Regs);

	if (g_p2835Regs == 0)
		return;


	// Save ptrs to the regs we need
	pGPIO = (PDWORD) (g_p2835Regs + GPIO_BASEOFF + GPFSEL0_OFFSET);
	for (i = 0; i < 6; i++)
	{
		dwData = pGPIO[i];
		dwData1 = dwData;
		for (j = 0; j < 10; j++)
		{
			bCfg[j] = dwData & 0x00000007;
			dwData = dwData >> 3;
		}
		OALLogSerial(L"GPIOCfg[%d] %08x 0:%02x 1:%02x 2:%02x 3:%02x 4:%02x - 5:%02x 6:%02x 7:%02x 8:%02x 9:%02x\r\n",
					 i, dwData1, bCfg[0], bCfg[1], bCfg[2], bCfg[3], bCfg[4], bCfg[5], bCfg[6], bCfg[7], bCfg[8], bCfg[9]);
	}

	pGPIO = (PDWORD) (g_p2835Regs + GPIO_BASEOFF + GPSET0_OFFSET);
	OALLogSerial(L"GPIOSet[0] %08x %08x\r\n", &pGPIO[0], pGPIO[0]);
	OALLogSerial(L"GPIOSet[1]          %08x\r\n", pGPIO[1]);

	pGPIO = (PDWORD) (g_p2835Regs + GPIO_BASEOFF + GPCLR0_OFFSET);
	OALLogSerial(L"GPIOClr[0] %08x %08x\r\n", &pGPIO[0], pGPIO[0]);
	OALLogSerial(L"GPIOClr[1]          %08x\r\n", pGPIO[1]);

	pGPIO = (PDWORD) (g_p2835Regs + GPIO_BASEOFF + GPLEV0_OFFSET);
	OALLogSerial(L"GPIOLev[0] %08x %08x\r\n", &pGPIO[0], pGPIO[0]);
	OALLogSerial(L"GPIOLev[1]          %08x\r\n", pGPIO[1]);

	pGPIO = (PDWORD) (g_p2835Regs + GPIO_BASEOFF + GPEDS0_OFFSET);
	OALLogSerial(L"GPIOEvnt[0] %08x %08x\r\n", &pGPIO[0], pGPIO[0]);
	OALLogSerial(L"GPIOEvnt[1]         %08x\r\n", pGPIO[1]);

	pGPIO = (PDWORD) (g_p2835Regs + GPIO_BASEOFF + GPREN0_OFFSET);
	OALLogSerial(L"GPIOUpEg[0] %08x %08x\r\n", &pGPIO[0], pGPIO[0]);
	OALLogSerial(L"GPIOUpEg[1]          %08x\r\n", pGPIO[1]);

	pGPIO = (PDWORD) (g_p2835Regs + GPIO_BASEOFF + GPFEN0_OFFSET);
	OALLogSerial(L"GPIODnEg[0] %08x %08x\r\n", &pGPIO[0], pGPIO[0]);
	OALLogSerial(L"GPIODnEg[1]          %08x\r\n", pGPIO[1]);

	pGPIO = (PDWORD) (g_p2835Regs + GPIO_BASEOFF + GPHEN0_OFFSET);
	OALLogSerial(L"GPIOHiLv[0] %08x %08x\r\n", &pGPIO[0], pGPIO[0]);
	OALLogSerial(L"GPIOHiLv[1]          %08x\r\n", pGPIO[1]);

	pGPIO = (PDWORD) (g_p2835Regs + GPIO_BASEOFF + GPLEN0_OFFSET);
	OALLogSerial(L"GPIOLoLv[0] %08x %08x\r\n", &pGPIO[0], pGPIO[0]);
	OALLogSerial(L"GPIOLoLv[1]          %08x\r\n", pGPIO[1]);

	pGPIO = (PDWORD) (g_p2835Regs + GPIO_BASEOFF + GPAREN0_OFFSET);
	OALLogSerial(L"GPIOAsUp[0] %08x %08x\r\n", &pGPIO[0], pGPIO[0]);
	OALLogSerial(L"GPIOAsUp[1]          %08x\r\n", pGPIO[1]);

	pGPIO = (PDWORD) (g_p2835Regs + GPIO_BASEOFF + GPAFEN0_OFFSET);
	OALLogSerial(L"GPIOAFal[0] %08x %08x\r\n", &pGPIO[0], pGPIO[0]);
	OALLogSerial(L"GPIOAFal[1]          %08x\r\n", pGPIO[1]);

	pGPIO = (PDWORD) (g_p2835Regs + GPIO_BASEOFF + GPPUD_OFFSET);
	OALLogSerial(L"GPIOPull[0] %08x %08x\r\n", &pGPIO[0], pGPIO[0]);
	OALLogSerial(L"GPIOPclk[0]          %08x\r\n", pGPIO[1]);
	OALLogSerial(L"GPIOPclk[1]          %08x\r\n", pGPIO[2]);

	return;
}

// ---------------------------------------------------------------------------
// OEMInit: REQUIRED
//
// This function intializes device hardware as well as initiates the KITL
// debug transport.
//
// This is the second OAL function that the kernel calls.
//
// When the kernel calls OEMInit, interrupts are disabled and the kernel is
// unable to handle exceptions. The only kernel service available to this
// function is HookInterrupt.
//
void OEMInit(void)
{
    //BOOL *bCleanBootFlag;
	int rc;
	DWORD dw;

	OEMWriteDebugString (TEXT("\r\nOEMInit++\r\n"));

	// Optionally customize function names for kernel entry points.
	RemapOALGlobalFunctions();

	// Optionally initialize OAL variables
	SetOALGlobalVariables();

    
    //----------------------------------------------------------------------
    // set values of globals used in IOCTL_HAL_GET_DEVICE_INFO handler
    //----------------------------------------------------------------------
    //g_oalIoCtlPlatformManufacturer  = IOCTL_PLATFORM_MANUFACTURER;
    //g_oalIoCtlPlatformName          = IOCTL_PLATFORM_NAME;
    CEProcessorType = PROCESSOR_ARM11;
    CEInstructionSet = PROCESSOR_ARM_V6FP_INSTRUCTION;

    // Set memory size for DrWatson kernel support
    //
    dwNKDrWatsonSize = 128 * 1024;

    // Intialize optional kernel functions. (Processor Extended Feature)
    //
    g_pOemGlobal->pfnIsProcessorFeaturePresent = (PFN_IsProcessorFeaturePresent)OALIsProcessorFeaturePresent;

    // Set OEMSetMemoryAttributes function
    g_pOemGlobal->pfnSetMemoryAttributes = (PFN_SetMemoryAttributes)OEMSetMemoryAttributes;


    // Initialize VFP support
    //
    VfpOemInit(g_pOemGlobal, VFP_AUTO_DETECT_FPSID);

    //// Initialize GPIO
    ////
    //InitializeGPIO();

    // Initialize BCD registers in RTC to known values
    //
	InitRTC();


    // Initilize cache globals
    //
    OALCacheGlobalsInit();

    //OALLogSerial(L"DCache: %d sets, %d ways, %d line size, %d size...\r\n",
    //            g_oalCacheInfo.L1DSetsPerWay, g_oalCacheInfo.L1DNumWays,
    //            g_oalCacheInfo.L1DLineSize, g_oalCacheInfo.L1DSize);

    //OALLogSerial(L"ICache: %d sets, %d ways, %d line size, %d size\r\n",
    //            g_oalCacheInfo.L1ISetsPerWay, g_oalCacheInfo.L1INumWays,
    //            g_oalCacheInfo.L1ILineSize, g_oalCacheInfo.L1ISize);

    //OALLogSerial(L"L2 DCache: %d sets, %d ways, %d line size, %d size\r\n",
    //            g_oalCacheInfo.L2DSetsPerWay, g_oalCacheInfo.L2DNumWays,
    //            g_oalCacheInfo.L2DLineSize, g_oalCacheInfo.L2DSize);

    //OALLogSerial(L"L2 ICache: %d sets, %d ways, %d line size, %d size\r\n",
    //            g_oalCacheInfo.L2ISetsPerWay, g_oalCacheInfo.L2INumWays,
    //            g_oalCacheInfo.L2ILineSize, g_oalCacheInfo.L2ISize);
    
    // Check clean boot flag in BSP Args area
    //
	NKForceCleanBoot();
    //}

	//OALLogSerial (L"pa %08x  va: %08x\r\n", PERIPHERAL_PA_BASE, g_p2835Regs);
	//OALLogSerial (L"pa %08x  OALpa2va: %08x\r\n", PERIPHERAL_PA_BASE, OALPAtoVA(PERIPHERAL_PA_BASE, FALSE));
	//OALLogSerial (L"va %08x  OALva2pa: %08x\r\n", g_p2835Regs, OALVAtoPA(g_p2835Regs));

    // Initialize Interrupts
    //
    if (!OALIntrInit())
    {
        OALMSG(OAL_ERROR, (L"[OAL:ERR] OEMInit() : failed to initialize interrupts\r\n"));
    }

    // Initialize System Clock
    //
    OALTimerInit (RESCHED_PERIOD, OEM_COUNT_1MS, 0);

    // Make high-res Monte Carlo profiling available to the kernel
    //
    //g_pOemGlobal->pfnProfileTimerEnable = OEMProfileTimerEnable;
    //g_pOemGlobal->pfnProfileTimerDisable = OEMProfileTimerDisable;

    // Initialize the KITL connection if required
    //
	//OEMWriteDebugString (TEXT("Calling KITLIoctl (IOCTL_KITL_STARTUP)\r\n"));
    KITLIoctl(IOCTL_KITL_STARTUP, NULL, 0, NULL, 0, NULL);
	//OEMWriteDebugString (TEXT("Return from KITLIoctl (IOCTL_KITL_STARTUP)\r\n"));

	g_pOemGlobal->dwMainMemoryEndAddress = 0x80000000 + (128 * 1024 * 1024);

	OALLogSerial(TEXT("dwMainMemoryEndAddress set to %x\r\n"), g_pOemGlobal->dwMainMemoryEndAddress);

	//OEMWriteDebugString (TEXT("HALT in OEMInit\r\n"));
	//while(1);

	// Power on USB
	rc = MailboxSendMail (0, MAILBOX_CHAN_POWER, 0x08);
	dw = 0;
	if (rc == 0)
	{
		// Get back status
		rc = MailboxGetMail (0, MAILBOX_CHAN_POWER, &dw);
		OALLogSerial(TEXT("[OAL] PowerUp USB status %x rc %d\r\n"), dw, rc);
	}
	else
		OALLogSerial(TEXT("OAL] ***ERROR*** PowerUp USB returned %d\r\n"), rc);

	// Uncomment below to see the current GPIO settings at kernel start.
	//DumpGPIOSettings ();

	OEMWriteDebugString (TEXT("OEMInit--\r\n"));

  return;
}

// ---------------------------------------------------------------------------
// RemapOALGlobalFunctions: CUSTOM
//
// The kernel calls OAL functions through a table of function pointers called
// OEMGlobal (see oemglobal.h for the full listing).  If you want to use a
// different function name for a required function, or use an optional
// function, simply change the pointer here.
//
void RemapOALGlobalFunctions(void)
{
  // Example remapping of a required function from default to a custom name:
  //g_pOemGlobal->pfnWriteDebugByte = CustomWriteDebugByte;

  // Example mappings of optional functions.  Uncomment any line to map
  // (use) the optional function.
  //g_pOemGlobal->pfnWriteDebugByte = OEMWriteDebugByte;
  //g_pOemGlobal->pfnWriteDebugString = OEMWriteDebugString;
  //g_pOemGlobal->pfnReadDebugByte = OEMReadDebugByte;
  //g_pOemGlobal->pfnWriteDebugLED = OEMWriteDebugLED;
  //g_pOemGlobal->pfnInitClock = OEMInitClock;
  //g_pOemGlobal->pfnQueryPerfCounter = OEMQueryPerformanceCounter;
  //g_pOemGlobal->pfnQueryPerfFreq = OEMQueryPerformanceFrequency;
  //g_pOemGlobal->pfnNotifyThreadExit = OEMNotifyThreadExit;
  //g_pOemGlobal->pfnNotifyReschedule = OEMNotifyReschedule;
  //g_pOemGlobal->pfnNotifyIntrOccurs = OEMNotifyIntrOccurs;
  //g_pOemGlobal->pfnUpdateReschedTime = OEMUpdateReschedTime;
  //g_pOemGlobal->pfnEnumExtensionDRAM = OEMEnumExtensionDRAM;
  //g_pOemGlobal->pfnCalcFSPages = OEMCalcFSPages;
  //g_pOemGlobal->pfnInitCoProcRegs = OEMInitCoProcRegs;
  //g_pOemGlobal->pfnSaveCoProcRegs = OEMSaveCoProcRegs;
  //g_pOemGlobal->pfnRestoreCoProcRegs = OEMRestoreCoProcRegs;
  //g_pOemGlobal->pfnReadRegistry = OEMReadRegistry;
  //g_pOemGlobal->pfnWriteRegistry = OEMWriteRegistry;
  //g_pOemGlobal->pfnProfileTimerEnable= OEMProfilerTimerEnable;
  //g_pOemGlobal->pfnProfileTimerDisable = OEMProfilerTimerDisable;
  //g_pOemGlobal->pfnIsRom = OEMIsRom;
  //g_pOemGlobal->pfnMapW32Priority = OEMMapW32Priority;
  //g_pOemGlobal->pfnStartAllCpus = OEMMpStartAllCPUs;
  //g_pOemGlobal->pfnMpPerCPUInit = OEMMpPerCPUInit;
  //g_pOemGlobal->pfnMpCpuPowerFunc = OEMMpCpuPowerFunc;
  //g_pOemGlobal->pfnIpiHandler = OEMIpiHandler;
  //g_pOemGlobal->pfnSendIpi = OEMSendIPI;
  //g_pOemGlobal->pfnIdleEx = OEMIdleEx;
  //g_pOemGlobal->pfnInitInterlockedFunc = OEMInitInterlockedFunctions;

  // Set the KITL entry point.  This is already set by default to
  // KitlDllMain, but if we want to change it we can do so here.
  //g_pOemGlobal->pfnKITLGlobalInit = KitlDllMain;

  // The following optional functions are processor-specific, so they will
  // only exist when compiling for a certain cpu type
#if defined (ARM)
  //g_pOemGlobal->pfnSaveVFPCtrlRegs = OEMSaveVFPCtrlRegs;
  //g_pOemGlobal->pfnRestoreVFPCtrlRegs = OEMRestoreVFPCtrlRegs;
  //g_pOemGlobal->pfnHandleVFPExcp = OEMHandleVFPException;
  //g_pOemGlobal->pfnIsVFPFeaturePresent = OEMIsVFPFeaturePresent;
  //g_pOemGlobal->pfnVFPCoprocControl = OEMVFPCoprocControl;
  //g_pOemGlobal->pfnPTEUpdateBarrier = OEMPTEUpdateBarrier;
#endif

  return;
}

// ---------------------------------------------------------------------------
// SetOALGlobalVariables: CUSTOM
//
// The kernel uses several OAL global variables for configuration purposes.
// Any changes to these variables should generally be separated into more
// logical groups during OEMInit.
//
// Changing the variables is optional since they will all have benign
// default values (listed below).  This function is designed only to
// illustrate how you would override the defaults.
//
 void SetOALGlobalVariables(void)
{
  // Specifies the thread quantum for OS (scheduler period).
  g_pOemGlobal->dwDefaultThreadQuantum = DEFAULT_THREAD_QUANTUM;

  // Specifies the next available address following the first available
  // contiguous block of memory.  The kernel sets this variable before
  // OEMInit is called.  This should only be touched when extending
  // the contiguous memory range available the to kernel.
  //g_pOemGlobal->dwMainMemoryEndAddress = 0;

  // Specifies the size of the memory allocation, in bytes, needed for the
  // OEM to save or restore coprocessor registers. This variable is only
  // used if the particular platform or CPU has coprocessor registers that
  // must be saved or restored during context switches.
  g_pOemGlobal->cbCoProcRegSize = 0;

  // Controls whether the save/restore of coprocessor registers is
  // performed.
  g_pOemGlobal->fSaveCoProcReg = FALSE;

  // Specifies the watchdog period, in milliseconds, where the hardware
  // watchdog must be refreshed before system reset. The default value,
  // 0, indicates that the watchdog timer does not exist.
  g_pOemGlobal->dwWatchDogPeriod = 0;

  // Specifies the kernel watchdog thread priority.
  g_pOemGlobal->dwWatchDogThreadPriority = DEFAULT_WATCHDOG_PRIORITY;

  // Specifies the amount of memory for the error reporting dump area
  g_pOemGlobal->cbErrReportSize = 0;

  // Specifies XIP region(s)
  g_pOemGlobal->pROMChain = NULL;

  // Platform-specific information passed from the OAL to KITL
  g_pOemGlobal->pKitlInfo = NULL;

  // Specifies compiler /GS flag security cookies
  g_pOemGlobal->p__security_cookie = &__security_cookie;
  g_pOemGlobal->p__security_cookie_complement = &__security_cookie_complement;

  // Specifies the alarm resolution in ms
  g_pOemGlobal->dwAlarmResolution = DEFAULT_ALARMRESOLUTION_MSEC;

  // Specifies number of years for RTC rollover
  g_pOemGlobal->dwYearsRTCRollover  = 0;

  // Specifies platform capabilities such as Vector Floating Point support
  g_pOemGlobal->dwPlatformFlags = 0;

  // Set to the location of the kernel's page pool parameters, which are
  // used for controlling the size and behavior of the kernel page pool.
  g_pOemGlobal->pPagePoolParam = &pagePoolParams;

  // Try to keep 16 megabytes' of memory available for executable code
  pagePoolParams.Loader.Target = 16 * 1024 * 1024;

  // Try to keep 4 megabyte of memory available for memory-mapped files
  pagePoolParams.File.Target = 4 * 1024 * 1024;

  // Start a thread freeing pages when we dip below 1 megabyte's worth of pages
  g_pOemGlobal->cpPageOutLow = 1 * 1024 * 1024 / VM_PAGE_SIZE;

  // Stop the thread freeing pages when we rise above 3 megabytes' worth of pages
  g_pOemGlobal->cpPageOutHigh = 16 * 1024 * 1024 / VM_PAGE_SIZE;

  // Specifies whether multiprocessor support is enabled
  g_pOemGlobal->fMPEnable = FALSE;

#ifdef DEBUG
  // A reference to the OAL's debug zone settings DBGPARAM structure
  // This only exists in debug builds, thus the #ifdef.
  g_pOemGlobal->pdpCurSettings = &dpCurSettings;
#else
  g_pOemGlobal->pdpCurSettings = NULL;
#endif

  // The following variables are processor-specific, so they will
  // only exist when compiling for a certain cpu type
#if defined (ARM)
  //// Specifies extra 1st level page table bits
  //g_pOemGlobal->dwARM1stLvlBits = 0;

  //// Specifies C and B bits (used to build the ARM CPU page tables).
  //g_pOemGlobal->dwARMCacheMode = 0;

  //// Indicates Instruction-Cache is Virtually Indexed Virtually
  //// Tagged ASID-tagged - for ARMv6 only.
  //g_pOemGlobal->f_V6_VIVT_ICache = 0;

  //// Specifies cache bits for page table access (inner/outer)
  //g_pOemGlobal->dwPageTableCacheBits = 0;

  //// Specifies cache bits for page tables in Translation Table Base
  //// Register (TTBR) if page table is cacheable (inner/outer)
  //g_pOemGlobal->dwTTBRCacheBits = 0;
#endif

  return;
}


