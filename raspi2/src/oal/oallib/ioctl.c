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
#include <oal.h>
#include <nkintr.h>
#include <bsp_cfg.h>
#include <image_cfg.h>
#include <video.h>
//------------------------------------------------------------------------------
// Externs
//
extern PBYTE g_p2835Regs;

static struct {
    BOOL postInit;
    CRITICAL_SECTION cs;
} g_ioctlState = { FALSE };

BOOL OALIoCtlHalSetKitlSerBuffs(UINT32 code, VOID* pInpBuffer, UINT32 inpSize, 
							    VOID* pOutBuffer,UINT32 outSize, UINT32 *pOutSize); 

BOOL OALIoCtlHalGetHardwarePtrs(UINT32 code, VOID *pInpBuffer, UINT32 inpSize, 
								 VOID *pOutBuffer, UINT32 outSize, UINT32 *pOutSize);

// Function prototype for our IOCTL_HAL_DDI handler function, implemented in ddi.c
BOOL OALIoctlHalDdi(UINT32 code, VOID *pInpBuffer, UINT32 inpSize, 
					VOID *pOutBuffer, UINT32 outSize, UINT32 *pOutSize);

//------------------------------------------------------------------------------
//
//  Global: g_MyoalIoCtlTable[]
//
//  IOCTL handler table. This table includes the IOCTL code/handler pairs
//  defined in the IOCTL configuration file. This global array is exported
//  via oal_ioctl.h and is used by the OAL IOCTL component.
//
const OAL_IOCTL_HANDLER g_MyoalIoCtlTable[] =
{

// The common OAL IOCTL table
//{ IOCTL_HAL_TRANSLATE_IRQ,                  0,  OALIntrTranslateIrq         },
{ IOCTL_HAL_REQUEST_SYSINTR,                0,  OALIoCtlHalRequestSysIntr   },
//{ IOCTL_HAL_RELEASE_SYSINTR,                0,  OALIoCtlHalReleaseSysIntr   },
//{ IOCTL_HAL_REQUEST_IRQ,                    0,  OALIoCtlHalRequestIrq       },
//
{ IOCTL_HAL_INITREGISTRY,                   0,  OALIoCtlHalInitRegistry     },
//{ IOCTL_HAL_INIT_RTC,                       0,  OALIoCtlHalInitRTC          },
//{ IOCTL_HAL_REBOOT,                         0,  OALIoCtlHalReboot           },
//
//{ IOCTL_HAL_DDK_CALL,                       0,  OALIoCtlHalDdkCall          },
//
//{ IOCTL_HAL_DISABLE_WAKE,                   0,  OALIoCtlHalDisableWake      },
//{ IOCTL_HAL_ENABLE_WAKE,                    0,  OALIoCtlHalEnableWake       },
//{ IOCTL_HAL_GET_WAKE_SOURCE,                0,  OALIoCtlHalGetWakeSource    },
//
//{ IOCTL_HAL_GET_CACHE_INFO,                 0,  OALIoCtlHalGetCacheInfo     },
//{ IOCTL_HAL_GET_DEVICEID,                   0,  OALIoCtlHalGetDeviceId      },
//{ IOCTL_HAL_GET_DEVICE_INFO,                0,  OALIoCtlHalGetDeviceInfo    },
//{ IOCTL_HAL_GET_UUID,                       0,  OALIoCtlHalGetUUID          },
//{ IOCTL_HAL_GET_RANDOM_SEED,                0,  OALIoCtlHalGetRandomSeed    },
//{ IOCTL_PROCESSOR_INFORMATION,              0,  OALIoCtlProcessorInfo       },
//{ IOCTL_HAL_UPDATE_MODE,                    0,  OALIoCtlHalUpdateMode       },


// Platform specific OAL IOCTLs
//{ IOCTL_HAL_GET_HWENTROPY,                  0,    OALIoCtlHalGetHWEntropy           },
//{ IOCTL_HAL_GET_HIVE_CLEAN_FLAG,            0,    OALIoCtlHalGetHiveCleanFlag       },
//{ IOCTL_HAL_QUERY_FORMAT_PARTITION,         0,    OALIoCtlHalQueryFormatPartition   },
//{ IOCTL_HAL_QUERY_DISPLAYSETTINGS,          0,    OALIoCtlHalQueryDisplaySettings   },
//{ IOCTL_HAL_GET_CPUID,                      0,    OALIoCtlHalGetCPUID               },
//{ IOCTL_HAL_SET_SYSTEM_LEVEL,               0,    OALIoCtlHalSetSystemLevel         },
//{ IOCTL_HAL_PROFILE_DVS,                    0,    OALIoCtlHalProfileDVS             },
//{ IOCTL_HAL_CLOCK_INFO,                     0,    OALIoCtlHalClockInfo              },
//{ IOCTL_HAL_GET_POWER_DISPOSITION,          0,    OALIoCtlHalGetPowerDisposition    },

//
// (db) Added these for the RASPI port
{ IOCTL_HAL_SET_KITL_SERIAL_BUFFERS,          0,  OALIoCtlHalSetKitlSerBuffs     },
{ IOCTL_HAL_MY_GET_HARDWARE_POINTERS,         0,  OALIoCtlHalGetHardwarePtrs     },
{ IOCTL_HAL_DDI,                              0,  OALIoctlHalDdi   },

// Required Termination
{ 0,                                        0,    NULL    }

};


__declspec(noinline) BOOL DoOEMIoControlWithCS (
    DWORD dwIndex,
    DWORD code, VOID *pInBuffer, DWORD inSize, VOID *pOutBuffer, DWORD outSize,
    DWORD *pOutSize
) {
    BOOL rc = FALSE;
    // Take critical section            
    EnterCriticalSection(&g_ioctlState.cs);
    __try {
        // Execute the handler
        rc = g_MyoalIoCtlTable[dwIndex].pfnHandler(
            code, pInBuffer, inSize, pOutBuffer, outSize, (UINT32 *)pOutSize
        );
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        rc = FALSE;
        NKSetLastError(ERROR_INVALID_PARAMETER);
    }
    // Release critical section            
    LeaveCriticalSection(&g_ioctlState.cs);
    return rc;

}


// ---------------------------------------------------------------------------
// OEMIoControl: REQUIRED
//
// This function provides a generic I/O control code (IOCTL) for
// OEM-supplied information.
//
BOOL OEMIoControl(DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, PVOID lpOutBuf,
                  DWORD nOutBufSize, LPDWORD lpBytesReturned)
{
    BOOL rc = FALSE;
    UINT32 dwIndex;

#ifdef DEBUG
	NKDbgPrintfW(L"OEMIoControl: Code 0x%x Fn:%d\r\n", dwIoControlCode, (dwIoControlCode >> 2) & 0x0fff);
#endif

    OALMSG(OAL_IOCTL&&OAL_FUNC, (L"+OEMIoControl(0x%x, 0x%x, %d, 0x%x, %d, 0x%x)\r\n", 
        dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpBytesReturned));

    //OALLogSerial((L"OEMIoControl: Code 0x%x - device 0x%04x func %d\r\n", 
    //        dwIoControlCode, dwIoControlCode >> 16, (dwIoControlCode >> 2)&0x0FFF));

	
    //Initialize g_ioctlState.cs when IOCTL_HAL_POSTINIT is called. By this time, 
    //the kernel is up and ready to handle the critical section initialization.
    if (!g_ioctlState.postInit && dwIoControlCode == IOCTL_HAL_POSTINIT) 
	{
        // Initialize critical section
        InitializeCriticalSection(&g_ioctlState.cs);
        g_ioctlState.postInit = TRUE;
    } 

    // Search the IOCTL table for the requested code.
    for (dwIndex = 0; g_MyoalIoCtlTable[dwIndex].pfnHandler != NULL; dwIndex++) 
	{
        if (g_MyoalIoCtlTable[dwIndex].code == dwIoControlCode) break;
    }

    // Indicate unsupported code
    if (g_MyoalIoCtlTable[dwIndex].pfnHandler == NULL)	
	{
        NKSetLastError(ERROR_NOT_SUPPORTED);
        OALMSG(OAL_IOCTL, (L"OEMIoControl: Unsupported Code 0x%x - device 0x%04x func %d\r\n", 
            dwIoControlCode, dwIoControlCode >> 16, (dwIoControlCode >> 2)&0x0FFF));
        goto cleanUp;
    }        

    // Take critical section if required (after postinit & no flag)
    if (g_ioctlState.postInit && (g_MyoalIoCtlTable[dwIndex].flags & OAL_IOCTL_FLAG_NOCS) == 0) 
	{
        rc = DoOEMIoControlWithCS(dwIndex, dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpBytesReturned);
		//// Take critical section            
		//EnterCriticalSection(&g_ioctlState.cs);
		//__try 
		//{
		//	// Execute the handler
		//	rc = g_MyoalIoCtlTable[dwIndex].pfnHandler(dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpBytesReturned);
		//}
		//__except(EXCEPTION_EXECUTE_HANDLER) 
		//{
		//	rc = FALSE;
		//	NKSetLastError(ERROR_INVALID_PARAMETER);
		//}
		//// Release critical section            
		//LeaveCriticalSection(&g_ioctlState.cs);
    } 
    else 
	{
        rc = g_MyoalIoCtlTable[dwIndex].pfnHandler(dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpBytesReturned);
    }

cleanUp:
    OALMSG(OAL_IOCTL&&OAL_FUNC, (L"-OEMIoControl(rc = %d)\r\n", rc ));
    return rc;


  return TRUE;
}

// ---------------------------------------------------------------------------
// OEMKDIoctl: OPTIONAL
//
// This function supports requests from the kernel debugger.
//
BOOL OEMKDIoctl(DWORD dwCode, LPVOID pBuf, DWORD cbSize)
{
  // Fill in IOCTL code here.

  return TRUE;
}

// ---------------------------------------------------------------------------
// OEMIsProcessorFeaturePresent: OPTIONAL
//
// This function provides information about processor features and support.
//
BOOL OEMIsProcessorFeaturePresent(DWORD dwProcessorFeature)
{
  // Fill in processor feature code here.

  return TRUE;
}



#define DBG_FEATURE_QUERY        (FALSE)

//------------------------------------------------------------------------------
//
//  Function:  OALIsProcessorFeaturePresent
//
//  Called to determine the processor's supported feature set.
//
BOOL OALIsProcessorFeaturePresent(DWORD feature)
{
    // We Do Not check CPU ID Code
    // Already Knows Feature of S3C6410

    BOOL bRet = FALSE;

    switch(feature)
    {
    case PF_ARM_V4:
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_V4) = %d\r\n", bRet));
        break;
    case PF_ARM_V5:
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_V5) = %d\r\n", bRet));
        break;
    case PF_ARM_V6:
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_V6) = %d\r\n", bRet));
        break;
    case PF_ARM_THUMB:
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_THUMB) = %d\r\n", bRet));
        break;
    case PF_ARM_WRITE_BUFFER:
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_WRITE_BUFFER) = %d\r\n", bRet));
        break;
    case PF_ARM_JAZELLE:
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_JAZELLE) = %d\r\n", bRet));
        break;
    case PF_ARM_DSP:        //(Deprecated)
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_DSP) = %d\r\n", bRet));
        break;
    case PF_ARM_MOVE_CP:
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_MOVE_CP) = %d\r\n", bRet));
        break;
    case PF_ARM_PHYSICALLY_TAGGED_CACHE:
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_PHYSICALLY_TAGGED_CACHE) = %d\r\n", bRet));
        break;
    case PF_ARM_VFP_DOUBLE_PRECISION:    // VFPv2 (VFP11)
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_VFP_DOUBLE_PRECISION) = %d\r\n", bRet));
        break;
    case PF_ARM_ITCM:
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_ITCM) = %d\r\n", bRet));
        break;
    case PF_ARM_DTCM:
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_DTCM) = %d\r\n", bRet));
        break;
    case PF_ARM_WRITE_BACK_CACHE:
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_WRITE_BACK_CACHE) = %d\r\n", bRet));
        break;
    case PF_ARM_CACHE_CAN_BE_LOCKED_DOWN:
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_CACHE_CAN_BE_LOCKED_DOWN) = %d\r\n", bRet));
        break;
    case PF_ARM_UNALIGNED_ACCESS:
        bRet = TRUE;            // TRUE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_UNALIGNED_ACCESS) = %d\r\n", bRet));
        break;


    case PF_ARM_VFP10:        //(Deprecated)
        bRet = FALSE;            // FALSE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_VFP10) = %d\r\n", bRet));
        break;
    case PF_ARM_MPU:
        bRet = FALSE;            // FALSE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_MPU) = %d\r\n", bRet));
        break;
    case PF_ARM_MBX:
        bRet = FALSE;            // FALSE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_MBX) = %d\r\n", bRet));
        break;
    case PF_ARM_L2CACHE:
        bRet = FALSE;            // FALSE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_L2CACHE) = %d\r\n", bRet));
        break;
    case PF_ARM_VFP_SINGLE_PRECISION:
        bRet = FALSE;            // FALSE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_VFP_SINGLE_PRECISION) = %d\r\n", bRet));
        break;
    case PF_ARM_UNIFIED_CACHE:
        bRet = FALSE;            // FALSE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_UNIFIED_CACHE) = %d\r\n", bRet));
        break;
    case PF_ARM_L2CACHE_MEMORY_MAPPED:
        bRet = FALSE;            // FALSE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_L2CACHE_MEMORY_MAPPED) = %d\r\n", bRet));
        break;
    case PF_ARM_L2CACHE_COPROC:
        bRet = FALSE;            // FALSE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_L2CACHE_COPROC) = %d\r\n", bRet));
        break;
    case PF_ARM_THUMB2:
        bRet = FALSE;            // FALSE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_THUMB2) = %d\r\n", bRet));
        break;
    case PF_ARM_T2EE:        // (Thumb-2 Execution Environment)
        bRet = FALSE;            // FALSE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_T2EE) = %d\r\n", bRet));
        break;
    case PF_ARM_VFP3:
        bRet = FALSE;            // FALSE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_VFP3) = %d\r\n", bRet));
        break;
    case PF_ARM_V7:
        bRet = FALSE;            // FALSE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_V7) = %d\r\n", bRet));
        break;
    case PF_ARM_NEON:
        bRet = FALSE;            // FALSE
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(PF_ARM_NEON) = %d\r\n", bRet));
        break;
    case PF_ARM_INTEL_XSCALE:    // Not Supported
    case PF_ARM_INTEL_PMU    :    // Not Supported
    case PF_ARM_INTEL_WMMX:    // Not Supported
        bRet = FALSE;
        break;
    default:
        bRet = FALSE;
        OALMSG(DBG_FEATURE_QUERY, (L"OALIsProcessorFeaturePresent(Unknown:0x%08x) = %d\r\n", feature, bRet));
        break;
    }

    return bRet;
}
//======================================================================
//======================================================================
//
// Interrupt related IOCTLs
//
//======================================================================
//======================================================================
//static UINT32 g_oalSysIntr2Irq[SYSINTR_MAXIMUM];
//static UINT32 g_oalIrq2SysIntr[OAL_INTR_IRQ_MAXIMUM];

//------------------------------------------------------------------------------
//
//  Function:  OALIoCtlHalRequestSysIntr
//
//  This function return existing SysIntr for non-shareable IRQs and create
//  new Irq -> SysIntr mapping for shareable.
//
BOOL OALIoCtlHalRequestSysIntr(UINT32 code, VOID* pInpBuffer, UINT32 inpSize, 
							   VOID* pOutBuffer,UINT32 outSize, UINT32 *pOutSize) 
{
    BOOL rc = FALSE;
	DWORD dwIrq;
	DWORD dwSysIntr = 0xffffffff;
	DWORD *pIn = (DWORD *)pInpBuffer;

	if ((pInpBuffer == 0) || (inpSize < sizeof (DWORD)) || (pOutBuffer == 0) || (outSize < sizeof (DWORD)))
	{
		return FALSE;
	}

	// MS changed this interface a while back so grab the proper irq val this way if needed
	if ((*pIn == 0xffffffff) && (inpSize >= sizeof (DWORD) * 3))
	{
		dwIrq = pIn[2];
	}
	else
	{
		dwIrq = *pIn;
	}


	// right now I'm just hardcoding this
	if (dwIrq == RASPI_PHYSIRQ_UART0)
		dwSysIntr = SYSINTR_UART0;


	if (dwSysIntr != 0xffffffff)
	{
		*(DWORD *)pOutBuffer = dwSysIntr;
		if (pOutSize != 0)
			*pOutSize = sizeof (DWORD);

		rc = TRUE;
	}
	return rc;
}
//------------------------------------------------------------------------------
//
//  Function:  OALIoCtlHalInitRegistry
//
//  Implements the IOCTL_HAL_INITREGISTRY handler.

BOOL OALIoCtlHalInitRegistry(UINT32 code, VOID *pInpBuffer, UINT32 inpSize, VOID *pOutBuffer, 
                             UINT32 outSize, UINT32 *pOutSize)
{
    // Default implementation doesn't do anything

    // KITL registry initialization is done in kitl.dll's IOCTL_HAL_INITREGISTRY handler
    
    UNREFERENCED_PARAMETER(code);    
    UNREFERENCED_PARAMETER(pInpBuffer);    
    UNREFERENCED_PARAMETER(inpSize);    
    UNREFERENCED_PARAMETER(pOutBuffer);    
    UNREFERENCED_PARAMETER(outSize);    
    UNREFERENCED_PARAMETER(pOutSize);    

    return TRUE;
}

BOOL OALIoCtlHalGetHardwarePtrs(UINT32 code, VOID *pInpBuffer, UINT32 inpSize, 
								 VOID *pOutBuffer, UINT32 outSize, UINT32 *pOutSize);
//------------------------------------------------------------------------------
//
//  Function:  OALIoCtlHalGetHardwarePtrs
//
//  Implements the IOCTL_HAL_MY_GET_HARDWARE_POINTERS handler.  This is called by
//  drivers to query various hardware pointers.

BOOL OALIoCtlHalGetHardwarePtrs(UINT32 code, VOID *pInpBuffer, UINT32 inpSize, 
								 VOID *pOutBuffer, UINT32 outSize, UINT32 *pOutSize)
{
    BOOL fRC = FALSE;
	MYDISPLAYSETTTNGS DSData;
	PMYHARDWAREPTRSTRUCT pOut = (PMYHARDWAREPTRSTRUCT)pOutBuffer;

	//NKDbgPrintfW(L"OALIoCtlHalGetHardwarePtrs++\r\n");

    UNREFERENCED_PARAMETER(code);    
    UNREFERENCED_PARAMETER(pInpBuffer);    
    UNREFERENCED_PARAMETER(inpSize);    

	if (pOutBuffer && (outSize == sizeof (MYHARDWAREPTRSTRUCT)))
	{
		__try 
		{
			// This routine is located in video.c
			fRC = GetVideoSettings (&DSData);
			if (fRC)
			{
				pOut->dwPhysSOCRegs = 0;
				pOut->pSOCRegs = g_p2835Regs;
				pOut->dwPhysFrameBuffer  = DSData.dwPhysFrameBuffer;
				pOut->pFrameBuffer = DSData.pFrameBuffer; // Frame buffer physical address

				*pOutSize = sizeof (MYHARDWAREPTRSTRUCT);
			}
			else
				*pOutSize = 0;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
//			SetLastError(ERROR_INVALID_PARAMETER);
		}
	}
	//NKDbgPrintfW(L"OALIoCtlHalGetHardwarePtrs-- rc %d\r\n", fRC);
    return fRC;
}
