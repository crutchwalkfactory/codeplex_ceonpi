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
//
//
#ifndef __KITL_CFG_H
#define __KITL_CFG_H

//------------------------------------------------------------------------------
#include <bsp.h>
#include <oal_kitl.h>


//------------------------------------------------------------------------------
// KITL Devices
//------------------------------------------------------------------------------

//OAL_KITL_SERIAL_DRIVER g_kitlSerial  = OAL_KITL_SERIAL_DRIVER;
OAL_KITL_SERIAL_DRIVER g_kitlSerial = {0};


OAL_KITL_DEVICE g_kitlDevices[] = {
    { 
        L"RaspiSerial", Internal, 0x1234, 0, OAL_KITL_TYPE_SERIAL, 
        &g_kitlSerial
    },
    {
        NULL, 0, 0, 0, 0, NULL
    }
};  

#endif

