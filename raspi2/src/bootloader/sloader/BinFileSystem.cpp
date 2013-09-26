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

#include <nkintr.h>
#include <pehdr.h>
#include <romldr.h>
#include <binfilesystem.h>

extern "C" {
void QuickDebugOut(char ch);
void DumpDWORD(DWORD dwLbl, DWORD dwPattern);
void Debug(LPCTSTR szFormat, ...);
}


// This is a really trivial file system that understands how to parse the
// Table of Contents of a BIN file.  It can return the location of file
// data for a file. 


// ---------------------------------------------------------------------------
// BinFileSystemInit
//
DWORD BinFileSystemInit (ROMHDR *pTOC)
{
	
  
	return (DWORD)pTOC;
}

// ---------------------------------------------------------------------------
// BinFindFileData - Returns a pointer to the file data as well as the size
// of the file
//
int BinFindFileData(DWORD dwHdl, char *szName, const unsigned char **ppcData, DWORD *pdwSize)
{
	int i, rc = -1;
	ROMHDR *pTOC = (ROMHDR *)dwHdl;
	LPFILESentry lpFiles = 0;

	// Init things;
	*ppcData = 0;
	*pdwSize = 0;

	//Debug (TEXT("3.1\r\n"));

	// Trival validation of pTOC
	if (pTOC == 0)
	{
		return rc;
	}
	//Debug (TEXT("3.2\r\n"));

	// Set up ptr to files array. The files array follows the Modules array which 
	// immediately follows the ROMHDR structure.
	lpFiles = (LPFILESentry)((DWORD)pTOC + sizeof(ROMHDR) + (pTOC->nummods * sizeof (TOCentry)));

	//Debug (TEXT("3.3 pFiles=%x nummods %d numfiles %d\r\n"), lpFiles, pTOC->nummods, pTOC->numfiles);

	// Loop through file list.  We're only looking at the files, not the module
	// list.
	for (i = 0; i < (int)pTOC->numfiles;i++)
	{
		Debug (TEXT("3.4 i=%d pFile=%x >%S<\r\n"), i, lpFiles[i].ulLoadOffset, lpFiles[i].lpszFileName);
		// See if names match.  The filesnames are ASCII
		if (strcmp (lpFiles[i].lpszFileName, szName) == 0)
		{
			*ppcData = (const unsigned char *)lpFiles[i].ulLoadOffset;
			*pdwSize = lpFiles[i].nCompFileSize; // The caller gets the raw (perhaps compressed) data.
			rc = 0;
			Debug (TEXT("3.5  rc=%d\r\n"), rc);
			break;
		}
	}
	//Debug (TEXT("3.6  rc=%d\r\n"), rc);

	return rc;
}


// ---------------------------------------------------------------------------
// BinFileSystemDeinit - Free file sysetm resources
//
int BinFileSystemDeinit(DWORD dwHdl)
{
	int rc = 0;

	return rc;
}
