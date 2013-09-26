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
//------------------------------------------------------------------------------
//
//  File: bsp.h
//
//  This header file is comprised of component header files that defines
//  the standard include hierarchy for the board support packege.
//
#ifndef __BSP_H
#define __BSP_H

//------------------------------------------------------------------------------

#if __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------

#include <windows.h>
#include <ceddk.h>
#include <nkintr.h>
#include <oal.h>

// BSP Configuration Files
#include "image_cfg.h"
//#include "bsp_base_reg_cfg.h"
//#include "ioctl_cfg.h"
//#include "image_cfg.h"
#include "bsp_args.h"

// Processor Specific Definitions
#include "bcm2835.h"

#include <nkintr.h>
#include <bsp_cfg.h>


//------------------------------------------------------------------------------

#if __cplusplus
}
#endif

#endif
