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

#include <windows.h>
//#include <bcm2835.h>
//#include <nkintr.h>


// This is a really trivial file system that understands how to parse the
// Table of Contents of a BIN file.  It can return the location of file
// data for a file. 


// ---------------------------------------------------------------------------
// BinFileSystemInit
//
DWORD BinFileSystemInit (ROMHDR *pTOC);

// ---------------------------------------------------------------------------
// BinFindFileData - Returns a pointer to the file data as well as the size
// of the file
//
int BinFindFileData(DWORD dwHdl, char *szName, const unsigned char **ppcData, DWORD *pdwSize);


// ---------------------------------------------------------------------------
// BinFileSystemDeinit - Free file sysetm resources
//
int BinFileSystemDeinit(DWORD dwHdl);
