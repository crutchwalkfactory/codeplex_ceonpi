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
#ifndef __BSP_ARGS_H
#define __BSP_ARGS_H

//------------------------------------------------------------------------------
//
// File:        bsp_args.h
//
// Description: This header file defines device structures and constant related
//              to boot configuration. BOOT_CFG structure defines layout of
//              persistent device information. It is used to control boot
//              process. BSP_ARGS structure defines information passed from
//              boot loader to kernel HAL/OAL. Each structure has version
//              field which should be updated each time when structure layout
//              change.
//
//------------------------------------------------------------------------------

#include "oal_args.h"
#include "oal_kitl.h"

#define BSP_ARGS_VERSION    1

#define BSP_ARGS_QUERY_HIVECLEAN        (BSP_ARGS_QUERY)        // Query hive clean flag.
#define BSP_ARGS_QUERY_CLEANBOOT        (BSP_ARGS_QUERY+1)        // Query clean boot flag.
#define BSP_ARGS_QUERY_FORMATPART    (BSP_ARGS_QUERY+2)        // Query format partition flag.

typedef struct
{
    OAL_ARGS_HEADER    header;
    UINT8                deviceId[16];            // Device identification
    OAL_KITL_ARGS        kitl;
    UINT8                uuid[16];
    BOOL                bUpdateMode;            // TRUE = Enter update mode on reboot.
    BOOL                bHiveCleanFlag;            // TRUE = Clean hive at boot
    BOOL                bCleanBootFlag;            // TRUE = Clear RAM, user objectstore at boot
    BOOL                bFormatPartFlag;        // TRUE = Format partion when mounted at boot
    DWORD                nfsblk;                    // for NAND Lock Tight
} BSP_ARGS;

//------------------------------------------------------------------------------
//
//  Function:  OALArgsInit
//
//  This function is called by other OAL modules to intialize value of argument
//  structure.
//
VOID OALArgsInit(BSP_ARGS *pBSPArgs);

//------------------------------------------------------------------------------

#endif    // __BSP_ARGS_H