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
#include "binfilesystem.h"
#include "mailbox.h"
#include "Debugfuncs.h"
#include "video.h"

#define dim(a) (sizeof(a)/sizeof(a[0]))

typedef struct {
	DWORD dwTagID;
	DWORD dwValBuffSize;
	DWORD dwReqRespFlag;
} TAGSTRUCT, *PTAGSTRUCT;

typedef struct {
	DWORD dwSize;
	DWORD dwRequestCode;
	TAGSTRUCT tsTags[1];
} TAGHEADERSTRUCT, *PTAGHEADERSTRUCT;

#define PROPERTY_REQUEST       0x00000000
#define PROPERTY_RESPONSE      0x80000000
#define PROPERTY_RESPONSEERR   0x00000001


#define MAILBOX_BUFF_PA_BASE   0x00001f00

#define TAG_A2V_GETFIRMWAREVERSION       0x00000001
#define TAG_A2V_GETFIRMWAREVERSION_SIZE  4

#define TAG_A2V_GETBOARDMODEL            0x00010001
#define TAG_A2V_GETBOARDMODEL_SIZE       4
#define TAG_A2V_GETBOARDREVISION         0x00010002
#define TAG_A2V_GETBOARDREVISION_SIZE    4
#define TAG_A2V_GETMACADDRESS            0x00010003
#define TAG_A2V_GETMACADDRESS_SIZE       6
#define TAG_A2V_GETBOARDSERIALNUM        0x00010004
#define TAG_A2V_GETBOARDSERIALNUM_SIZE   8
#define TAG_A2V_GETARMMEMORYSIZE         0x00010005
#define TAG_A2V_GETARMMEMORYSIZE_SIZE    8
#define TAG_A2V_GETVCMEMORYSIZE          0x00010006
#define TAG_A2V_GETVCMEMORYSIZE_SIZE     8

#define TAG_A2V_GETCMDLINE               0x00050001
#define TAG_A2V_GETCMDLINE_SIZE          512

#define TAG_A2V_GETDMACHANS              0x00060001
#define TAG_A2V_GETDMACHANS_SIZE         4

#define TAG_A2V_GETVIDDIM                0x00040003
#define TAG_A2V_GETVIDDIM_SIZE           8 

#define TAG_A2V_GETVIDVIRTDIM            0x00040004
#define TAG_A2V_GETVIDVIRTDIM_SIZE       8  

#define TAG_A2V_GETVIDDEPTH              0x00040005
#define TAG_A2V_GETVIDDEPTH_SIZE         4  

int SendTag (DWORD dwChan, PBYTE pData);
typedef struct {
	DWORD dwTag;
	DWORD dwDataSize;
	TCHAR *szTagName;
} PROPTAGIDARRAY, *PPROPTAGIDARRAY;

#define  RASPI_PROP_FIRMWAREVERSION     0
#define  RASPI_PROP_GETBOARDMODEL       1
#define  RASPI_PROP_GETBOARDREVISION    2
#define  RASPI_PROP_GETMACADDRESS       3
#define  RASPI_PROP_GETBOARDSERIALNUM   4
#define  RASPI_PROP_GETARMMEMORYSIZE    5
#define  RASPI_PROP_GETVCMEMORYSIZE     6
#define  RASPI_PROP_GETCMDLINE          7
#define  RASPI_PROP_GETDMACHANS         8
#define  RASPI_PROP_GETPHYSDISPDIM      9
#define  RASPI_PROP_GETVIDVIRTDIM       10
#define  RASPI_PROP_GETVIDDEPTH         11

#define  RASPI_PROP_MAX                 11

PROPTAGIDARRAY TagArray[] = {
	{TAG_A2V_GETFIRMWAREVERSION, TAG_A2V_GETFIRMWAREVERSION_SIZE, L"Firmware Ver"},
	{TAG_A2V_GETBOARDMODEL     , TAG_A2V_GETBOARDMODEL_SIZE, L"Board Model"},
	{TAG_A2V_GETBOARDREVISION  , TAG_A2V_GETBOARDREVISION_SIZE, L"Board Revision"},
	{TAG_A2V_GETMACADDRESS     , TAG_A2V_GETMACADDRESS_SIZE, L"MAC Address"},
	{TAG_A2V_GETBOARDSERIALNUM , TAG_A2V_GETBOARDSERIALNUM_SIZE, L"Board Serial Num"},
	{TAG_A2V_GETARMMEMORYSIZE  , TAG_A2V_GETARMMEMORYSIZE_SIZE, L"ARM Memory"},
	{TAG_A2V_GETVCMEMORYSIZE   , TAG_A2V_GETVCMEMORYSIZE_SIZE, L"VidCon Memory"},
	{TAG_A2V_GETCMDLINE, TAG_A2V_GETCMDLINE_SIZE, L"Command line"},
	{TAG_A2V_GETDMACHANS, TAG_A2V_GETDMACHANS_SIZE, L"DMA Channels"},
	{TAG_A2V_GETVIDDIM, TAG_A2V_GETVIDDIM_SIZE, L"Video width/height"},
	{TAG_A2V_GETVIDVIRTDIM, TAG_A2V_GETVIDVIRTDIM_SIZE, L"Video virtual width/height"},
	{TAG_A2V_GETVIDDEPTH       , TAG_A2V_GETVIDDEPTH_SIZE, L"Video pixel depth"},

};

//int MailboxGetMailboxBuffer (BOOL fVirtualMem, DWORD *pdwBuff, DWORD *pdwBuffSize)

#pragma optimize( "", off )

//----------------------------------------------------------------------------------
// GetRASPIProperty - Queries the Video CPU to get property data.
//
int GetRASPIProperty (int idProperty, PBYTE pDataBuff, DWORD dwBuffSize, PDWORD pdwBytesReturned)
{
	PTAGHEADERSTRUCT pTagHeader;
	PBYTE pTagData;
	DWORD dwSize;
	int rc = 0;
	
	// Verify the ID is valid
	if ((idProperty < 0) || (idProperty > RASPI_PROP_MAX))
		return -1;

	// See if they just want the necessary size of the buffer.
	if ((dwBuffSize == 0) && (pdwBytesReturned != 0))
	{
		*pdwBytesReturned = TagArray[idProperty].dwDataSize;
		return 0;
	}

	// Verify we have a data buffer.
	if (pDataBuff == 0) 
		return -2;

	// Get the mailbox buff
	if (MailboxGetMailboxBuffer (FALSE, (DWORD *)&pTagHeader, &dwSize) != 0)
	{
		Debug (TEXT("PropTest fail. Can't get mailbox buffer address.\r\n"));
		return -1;
	}
	pTagData = (PBYTE)&pTagHeader[1];
	memset (pTagData, 0, sizeof (TAGHEADERSTRUCT) + TagArray[idProperty].dwDataSize + 40); 

	pTagHeader->dwRequestCode = PROPERTY_REQUEST;
	pTagHeader->tsTags[0].dwTagID = TagArray[idProperty].dwTag;
	pTagHeader->tsTags[0].dwValBuffSize = TagArray[idProperty].dwDataSize;
	pTagHeader->tsTags[0].dwReqRespFlag = PROPERTY_REQUEST;
	pTagHeader->dwSize = sizeof (TAGHEADERSTRUCT) + pTagHeader->tsTags[0].dwValBuffSize + 40;
	pTagData = (PBYTE)pTagHeader + sizeof (TAGHEADERSTRUCT);

	rc = SendTag (MAILBOX_CHAN_PROPARM2VC, (PBYTE)pTagHeader);
	if (rc != 0)
	{
		Debug (TEXT("Error sending tag rc=%d\r\n"), rc);
	}
	else
	{
		int cnt = pTagHeader->tsTags[0].dwReqRespFlag & 0xffff;
		memcpy (pDataBuff, pTagData, cnt);

		if (pdwBytesReturned != 0)
			*pdwBytesReturned = cnt;
	}
	return rc;
}
//----------------------------------------------------------------------------------
// PropTest
//
int DumpAllPropsTest()
{
	int i, rc = -1;
	DWORD dwSize;
	BYTE bBuff[256];

	for (i = 0; i < dim (TagArray); i++)
	{
		rc = GetRASPIProperty (i, bBuff, sizeof(bBuff), &dwSize);
		if (dwSize <= 4)
			Debug (TEXT(">%S<  Size: %d    Data %0x (%d)\r\n"), TagArray[i].szTagName, dwSize, *(DWORD *)bBuff, *(DWORD *)bBuff);
		if (dwSize <= 8)
			Debug (TEXT(">%S<  Size: %d    Data %08x-%08x\r\n"), TagArray[i].szTagName, dwSize, *(DWORD *)bBuff, (DWORD *)&bBuff[4]);
		else
		{
			Debug (TEXT(">%S<  Size: %d    \r\n"), TagArray[i].szTagName, dwSize);
			PBYTE pTagData = bBuff;
			for (int j = 0; j < (int)dwSize; j++)
			{
				if (j == 0)
					Debug (TEXT("%02x "), *pTagData++);
				else if (j % 16 == 0)
					Debug (TEXT("%02x\r\n"), *pTagData++);
				else if (j % 7 == 0)
					Debug (TEXT("%02x-"), *pTagData++);
				else
					Debug (TEXT("%02x "), *pTagData++);
			}
		}
	}
	return rc;
}
//----------------------------------------------------------------------------------
// PropTest
//
int PropTest()
{
	int nRetryCnt, rc = -1;
	PTAGHEADERSTRUCT pTagHeader;
	TAGSTRUCT pTag;
	DWORD dwSize;
	PBYTE pTagData;

	Debug (TEXT("PropTest++\r\n"));

	// Init structure. I tried putting this buff on the stack (with alignment) but
	// the call failed.
	if (MailboxGetMailboxBuffer (FALSE, (DWORD *)&pTagHeader, &dwSize) != 0)
	{
		Debug (TEXT("PropTest fail. Can't get mailbox buffer address.\r\n"));
		return rc;
	}
	Debug (TEXT("PropTest Mailbox buffer at %8x.\r\n"), pTagHeader);
	pTagData = (PBYTE)&pTagHeader[1];

	int i, j;
	for (i = 0; i < dim (TagArray); i++)
	{
		memset (pTagData, 0, sizeof (TAGHEADERSTRUCT) + TagArray[i].dwDataSize + 40); 

		pTagHeader->dwRequestCode = PROPERTY_REQUEST;
		pTagHeader->tsTags[0].dwTagID = TagArray[i].dwTag;
		pTagHeader->tsTags[0].dwValBuffSize = TagArray[i].dwDataSize;
		pTagHeader->tsTags[0].dwReqRespFlag = PROPERTY_REQUEST;
		pTagHeader->dwSize = sizeof (TAGHEADERSTRUCT) + pTagHeader->tsTags[0].dwValBuffSize + 40;
		pTagData = (PBYTE)pTagHeader + sizeof (TAGHEADERSTRUCT);
	
		rc = SendTag (MAILBOX_CHAN_PROPARM2VC, (PBYTE)pTagHeader);
		if (rc != 0)
		{
			Debug (TEXT("Error sending tag rc=%d\r\n"), rc);
			break;
		}

		Debug (TEXT("\r\n%d. %s\r\n"), i, TagArray[i].szTagName);
		Debug (TEXT("  pTagHeader.dwRequestCode = %x\r\n"), pTagHeader->dwRequestCode);
		Debug (TEXT("  pTagHeader.tsTags[0].dwReqRespFlag = %x\r\n"), pTagHeader->tsTags[0].dwReqRespFlag);

		int cnt = pTagHeader->tsTags[0].dwReqRespFlag & 0xffff;
		if (cnt <= 4)
			Debug (TEXT("  pTagData = %08x\r\n"), *(DWORD *)pTagData);
		else if (cnt <= 8)
			Debug (TEXT("  pTagData = %08x %08x\r\n"), *(DWORD *)pTagData, *(DWORD *)(pTagData+4));
		else
		{
			for (j = 0; j < cnt; j++)
			{
				if (j == 0)
					Debug (TEXT("%02x "), *pTagData++);
				else if (j % 16 == 0)
					Debug (TEXT("%02x\r\n"), *pTagData++);
				else if (j % 7 == 0)
					Debug (TEXT("%02x-"), *pTagData++);
				else
					Debug (TEXT("%02x "), *pTagData++);
			}
		}
	}
	Debug (TEXT("\r\nDone\r\n"));


	i = 7;
	memset (pTagData, 0, sizeof (TAGHEADERSTRUCT) + TagArray[i].dwDataSize + 40); 

	pTagHeader->dwRequestCode = PROPERTY_REQUEST;
	pTagHeader->tsTags[0].dwTagID = TagArray[i].dwTag;
	pTagHeader->tsTags[0].dwValBuffSize = TagArray[i].dwDataSize;
	pTagHeader->tsTags[0].dwReqRespFlag = PROPERTY_REQUEST;
	pTagHeader->dwSize = sizeof (TAGHEADERSTRUCT) + pTagHeader->tsTags[0].dwValBuffSize + 40;
	pTagData = (PBYTE)pTagHeader + sizeof (TAGHEADERSTRUCT);

	rc = SendTag (MAILBOX_CHAN_PROPARM2VC, (PBYTE)pTagHeader);
	if (rc != 0)
	{
		Debug (TEXT("Error sending tag rc=%d\r\n"), rc);
	}

	Debug (TEXT("\r\n%d. %s\r\n"), i, TagArray[i].szTagName);
	Debug (TEXT("  pTagHeader.dwRequestCode = %x\r\n"), pTagHeader->dwRequestCode);
	Debug (TEXT("  pTagHeader.tsTags[0].dwReqRespFlag = %x\r\n"), pTagHeader->tsTags[0].dwReqRespFlag);
	Debug (TEXT("  pTagData = %S\r\n"), pTagData);



	//nRetryCnt = 200;
	//while (nRetryCnt > 0)
	//{
	//	// Get Version
	//	pTagHeader->dwRequestCode = PROPERTY_REQUEST;
	//	pTagHeader->tsTags[0].dwTagID = TAG_A2V_GETFIRMWAREVERSION;
	//	pTagHeader->tsTags[0].dwValBuffSize = TAG_A2V_GETFIRMWAREVERSION_SIZE;
	//	pTagHeader->tsTags[0].dwReqRespFlag = PROPERTY_REQUEST;
	//	//The size field must include 4 bytes for the NULL end tag
	//	//pTagHeader->dwSize = sizeof (TAGHEADERSTRUCT) + pTagHeader->tsTags[0].dwValBuffSize + 4;
	//	pTagHeader->dwSize = sizeof (TAGHEADERSTRUCT) + pTagHeader->tsTags[0].dwValBuffSize + 40;

	//	pTagData = (PBYTE)pTagHeader + sizeof (TAGHEADERSTRUCT);
	//	memset (pTagData, 0, pTagHeader->tsTags[0].dwValBuffSize + 40); //The 4 bytes are the NULL end tag
	//
	//	rc = SendTag (MAILBOX_CHAN_PROPARM2VC, (PBYTE)pTagHeader);
	//	if (rc != 0)
	//	{
	//		Debug (TEXT("Error sending tag rc=%d\r\n"), rc);
	//		break;
	//	}
	//	if (pTagHeader->tsTags[0].dwReqRespFlag != 0)
	//		break;
	//	nRetryCnt--;
	//}
	//Debug (TEXT("GetBoardFirmware data at %x.  retry %d\r\n"), pTagHeader, nRetryCnt);
	//Debug (TEXT("  pTagHeader.dwSize = %x\r\n"), pTagHeader->dwSize);
	//Debug (TEXT("  pTagHeader.dwRequestCode = %x\r\n"), pTagHeader->dwRequestCode);
	//Debug (TEXT("  pTagHeader.tsTags[0].dwTagID = TAG = %x\r\n"), pTagHeader->tsTags[0].dwTagID);
	//Debug (TEXT("  pTagHeader.tsTags[0].dwValBuffSize = %x\r\n"), pTagHeader->tsTags[0].dwValBuffSize);
	//Debug (TEXT("  pTagHeader.tsTags[0].dwReqRespFlag = %x\r\n"), pTagHeader->tsTags[0].dwReqRespFlag);
	//Debug (TEXT("  pTagData = %08x\r\n"), *(DWORD *)pTagData);
	//Debug (TEXT("  pTagData = %02x %02x %02x %02x\r\n"), *pTagData, *(pTagData+1), *(pTagData+2), *(pTagData+3));

	//nRetryCnt = 200;
	//while (nRetryCnt > 0)
	//{
	//	// Get Version
	//	pTagHeader->dwRequestCode = PROPERTY_REQUEST;
	//	pTagHeader->tsTags[0].dwTagID = TAG_A2V_GETBOARDMODEL;
	//	pTagHeader->tsTags[0].dwValBuffSize = TAG_A2V_GETBOARDMODEL_SIZE;
	//	pTagHeader->tsTags[0].dwReqRespFlag = PROPERTY_REQUEST;
	//	//The size field must include 4 bytes for the NULL end tag
	//	//pTagHeader->dwSize = sizeof (TAGHEADERSTRUCT) + pTagHeader->tsTags[0].dwValBuffSize + 4;
	//	pTagHeader->dwSize = sizeof (TAGHEADERSTRUCT) + pTagHeader->tsTags[0].dwValBuffSize + 40;

	//	pTagData = (PBYTE)pTagHeader + sizeof (TAGHEADERSTRUCT);
	//	memset (pTagData, 0, pTagHeader->tsTags[0].dwValBuffSize + 40); //The 4 bytes are the NULL end tag
	//
	//	rc = SendTag (MAILBOX_CHAN_PROPARM2VC, (PBYTE)pTagHeader);
	//	if (rc != 0)
	//	{
	//		Debug (TEXT("Error sending tag rc=%d\r\n"), rc);
	//		break;
	//	}
	//	if (pTagHeader->tsTags[0].dwReqRespFlag != 0)
	//		break;
	//	nRetryCnt--;
	//}
	//Debug (TEXT("\r\nGetBoardModel data at %x.  retry %d\r\n"), pTagHeader, nRetryCnt);
	//Debug (TEXT("  pTagHeader.dwSize = %x\r\n"), pTagHeader->dwSize);
	//Debug (TEXT("  pTagHeader.dwRequestCode = %x\r\n"), pTagHeader->dwRequestCode);
	//Debug (TEXT("  pTagHeader.tsTags[0].dwTagID       = %x\r\n"), pTagHeader->tsTags[0].dwTagID);
	//Debug (TEXT("  pTagHeader.tsTags[0].dwValBuffSize = %x\r\n"), pTagHeader->tsTags[0].dwValBuffSize);
	//Debug (TEXT("  pTagHeader.tsTags[0].dwReqRespFlag = %x\r\n"), pTagHeader->tsTags[0].dwReqRespFlag);
	//Debug (TEXT("  pTagData = %08x\r\n"), *(DWORD *)pTagData);
	//Debug (TEXT("  pTagData = %02x %02x %02x %02x\r\n"), *pTagData, *(pTagData+1), *(pTagData+2), *(pTagData+3));

	//nRetryCnt = 200;
	//while (nRetryCnt > 0)
	//{
	//	// Get Version
	//	pTagHeader->dwRequestCode = PROPERTY_REQUEST;
	//	pTagHeader->tsTags[0].dwTagID = TAG_A2V_GETBOARDREVISION;
	//	pTagHeader->tsTags[0].dwValBuffSize = TAG_A2V_GETBOARDREVISION_SIZE;
	//	pTagHeader->tsTags[0].dwReqRespFlag = PROPERTY_REQUEST;
	//	//The size field must include 4 bytes for the NULL end tag
	//	//pTagHeader->dwSize = sizeof (TAGHEADERSTRUCT) + pTagHeader->tsTags[0].dwValBuffSize + 4;
	//	pTagHeader->dwSize = sizeof (TAGHEADERSTRUCT) + pTagHeader->tsTags[0].dwValBuffSize + 40;

	//	pTagData = (PBYTE)pTagHeader + sizeof (TAGHEADERSTRUCT);
	//	memset (pTagData, 0, pTagHeader->tsTags[0].dwValBuffSize + 40); //The 4 bytes are the NULL end tag
	//
	//	rc = SendTag (MAILBOX_CHAN_PROPARM2VC, (PBYTE)pTagHeader);
	//	if (rc != 0)
	//	{
	//		Debug (TEXT("Error sending tag rc=%d\r\n"), rc);
	//		break;
	//	}
	//	if (pTagHeader->tsTags[0].dwReqRespFlag != 0)
	//		break;
	//	nRetryCnt--;
	//}
	//Debug (TEXT("\r\nGetBoardVersion data at %x.  retry %d\r\n"), pTagHeader, nRetryCnt);
	//Debug (TEXT("  pTagHeader.dwSize = %x\r\n"), pTagHeader->dwSize);
	//Debug (TEXT("  pTagHeader.dwRequestCode = %x\r\n"), pTagHeader->dwRequestCode);
	//Debug (TEXT("  pTagHeader.tsTags[0].dwTagID = TAG = %x\r\n"), pTagHeader->tsTags[0].dwTagID);
	//Debug (TEXT("  pTagHeader.tsTags[0].dwValBuffSize = %x\r\n"), pTagHeader->tsTags[0].dwValBuffSize);
	//Debug (TEXT("  pTagHeader.tsTags[0].dwReqRespFlag = %x\r\n"), pTagHeader->tsTags[0].dwReqRespFlag);
	//Debug (TEXT("  pTagData = %08x\r\n"), *(DWORD *)pTagData);
	//Debug (TEXT("  pTagData = %02x %02x %02x %02x\r\n"), *pTagData, *(pTagData+1), *(pTagData+2), *(pTagData+3));



	Debug (TEXT("PropTest-- rc=%d\r\n"), rc);
	return rc;
}

//----------------------------------------------------------------------------------
// SendTag 
//
int SendTag (DWORD dwChan, PBYTE pData)
{
	DWORD dwData;
	int rc = 0;

	// Check for properly aligned pointer
	if ((DWORD)pData % 16)
	{
		Debug (TEXT(" ***Error*** Mailbox request pointer not 16 byte aligned. p = %x\r\n"), pData);
		return -2;
	}

	// Send the info to the video subsystem
	rc = MailboxSendMail (0, dwChan, (DWORD)pData >> 4);
	if (rc == 0)
	{
		// Read back ack from the video subsystem. The data is updated in the existing structure.  
		// The value read is simply a return code.
		rc = MailboxGetMail (0, dwChan, &dwData);
		if (rc != 0)
		{
			Debug (TEXT("Read failed\r\n"));
		}
	}
	else
	{
		Debug (TEXT("Write failed\r\n"));
	}

	return rc;
}
