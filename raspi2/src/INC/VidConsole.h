//
// Copyright (c) Douglas Boling
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

#ifndef __VIDCONDBG_H
#define __VIDCONDBG_H


#if __cplusplus
extern "C" {
#endif

//#include <windows.h>
//#include <bcm2835.h>
//#include <nkintr.h>


int VidCON_InitDebugSerial (BOOL fVirtualMem);

int VidCON_DisableDebugSerial (BOOL fVirtualMem);

void VidCON_WriteDebugByteExtern(BYTE ucChar);

void VidCON_WriteDebugString(LPWSTR pBuffer);

#if __cplusplus
}
#endif

#endif //__VIDCONDBG_H