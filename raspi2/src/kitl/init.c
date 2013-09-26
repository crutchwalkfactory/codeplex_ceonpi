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


// Init.c
// The comments in this file will vary from OS version to version.
//
// All KITL functions in the template BSP fall into one of three categories:
// REQUIRED - you must implement this function for KITL functionality
// OPTIONAL - you may implement this function to enable specific functionality
// CUSTOM   - this function is a helper function specific to this BSP
//


#include <windows.h>
#include <winnt.h>
#include <bsp.h>
#include <ceddk.h>
#include <kitl.h>
#include <oal_kitl.h>

//#define USE_INTERRUPTS  1

//------------------------------------------------------------------------------
//  External functions
//
BOOL OALKitlSerialInit (LPSTR deviceId, OAL_KITL_DEVICE *pDevice, 
						OAL_KITL_ARGS *pArgs, KITLTRANSPORT *pKitl);

OAL_KITL_SERIAL_DRIVER *GetKitlSerialDriver (void);
VOID UART_EnableInt (void);
// Secret zones 
#define KITL_ZONE_NOSERIAL  0x10000     // We write debug messages out debug serial port
                                        // if can't send them over ether, unless this
                                        // flag is set.


// ---------------------------------------------------------------------------
// dpCurSettings: REQUIRED
//
// This variable defines debug zones usable by the kernel and this
// implementation.  This is the operating system's standard
// mechanism for debug zones.
//
DBGPARAM dpCurSettings = {
    TEXT("KITL"), {
    TEXT("Warning"),    TEXT("Init"),       TEXT("Frame Dump"),     TEXT("Timer"),
    TEXT("Send"),       TEXT("Receive"),    TEXT("Retransmit"),     TEXT("Command"),
    TEXT("Interrupt"),  TEXT("Adapter"),    TEXT("LED"),            TEXT("DHCP"),
    TEXT("OAL"),        TEXT("Ethernet"),   TEXT("Unused"),         TEXT("Error"), },
    ZONE_WARNING | ZONE_ERROR,
};

//------------------------------------------------------------------------------
//
// OEMKitlStartup: REQUIRED
//
// This function is the first OEM code that executes in kitl.dll.  It is called
// by the kernel after the BSP calls KITLIoctl( IOCTL_KITL_STARTUP, ... ) in
// OEMInit().  This function should set up internal state, read any boot
// arguments, and call the KitlInit function to initialize KITL in active
// (immediate load) or passive (delay load) mode.
//
BOOL OEMKitlStartup()
{
	BOOL mode = TRUE; // TRUE for active mode, FALSE for passive mode
	BOOL rc;

	// fill in startup code here

	//KITLSetDebug (0xffff);
	//KITLSetDebug (KITL_ZONE_NOSERIAL);
	//KITLSetDebug (DEFAULT_SETTINGS | KITL_ZONE_NOSERIAL);
	//KITLSetDebug (DEFAULT_SETTINGS | ZONE_FRAMEDUMP | ZONE_SEND);
	//KITLSetDebug (DEFAULT_SETTINGS | ZONE_TIMER);
	//KITLSetDebug (DEFAULT_SETTINGS | ZONE_RECV | ZONE_SEND);
	//KITLSetDebug (DEFAULT_SETTINGS | ZONE_RECV | ZONE_SEND | ZONE_FRAMEDUMP | ZONE_INTR);
	//KITLSetDebug (DEFAULT_SETTINGS | ZONE_FRAMEDUMP | ZONE_INTR);
	//KITLSetDebug (DEFAULT_SETTINGS);

	rc = KitlInit(mode);
	return rc;
}

// ---------------------------------------------------------------------------
// OEMKitlInit: REQUIRED
//
// This function is called from the kernel to initialize the KITL device and
// KITLTRANSPORT structure when it is time to load KITL.  If OEMKitlStartup
// selects active mode KITL, KitlInit will call this function during boot. If
// OEMKitlStartup selects passive mode KITL, this function will not be called
// until an event triggers KITL load.

// When this function returns, the KITLTRANSPORT structure must contain valid
// variable initializations including valid function pointers for each
// required KITL function.  The KITL transport hardware must also be fully
// initialized.
//
BOOL OEMKitlInit(PKITLTRANSPORT pKitl)
{
    BOOL rc = FALSE;
    UCHAR *szDeviceId,buffer[OAL_KITL_ID_SIZE]="\0";
	OAL_KITL_ARGS Args;
	OAL_KITL_DEVICE kitlDevice;

    KITL_RETAILMSG(ZONE_KITL_OAL, ("+OEMKitlInit(0x%x)\r\n", pKitl));

	szDeviceId = "Raspi";

	//------------------------
	// Set up the device location structure for the KITL, this will be used
	// in both the OAL_KITL_ARGS and OAL_KITL_DEVICE structures.
	//------------------------

	// Set up the interface type according to ceddk.h.  If the interface 
	// is set to InterfaceTypeUndefined, the platform\common code 
	// will use devLoc.Pin for the IRQ of the device.  
	// However this won't be looked at if OAL_KITL_FLAGS_POLL is set
	// in the Args.flags field.

	Args.devLoc.IfcType = InterfaceTypeUndefined;
	Args.devLoc.Pin = RASPI_PHYSIRQ_UART0;

	// Set up the location information for our KITL device.  This will be
	// passed to the KITL driver's Init function during OEMKitlInit, and
	// also to the OEM-specific OAL implementation of OALIntrRequestIrqs
	// during IOCTL_HAL_REQUEST_IRQ (unless the interface type is undefined).
	//
	// In this sample we'll assume the KITL device IRQ is known ahead of time,
	// so the device location information is not relevant.  For a device on
	// a bus this information is important.  See pkfuncs.h for a description
	// of what the values mean for a PCI bus.
	Args.devLoc.BusNumber = 0;
	Args.devLoc.LogicalLoc = 0x1234;
	Args.devLoc.PhysicalLoc = (PVOID)0x1234;

	Args.flags = 0;
#ifndef USE_INTERRUPTS
	Args.flags = OAL_KITL_FLAGS_POLL;
#endif // not USE_INTERRUPTS
	Args.baudRate = 115200;
	Args.dataBits = 8;
	Args.stopBits = 1;
	Args.parity = 0;

	//------------------------
	//------------------------
	// Initialize dwBootFlags, which describe the KITL services to start and
	// whether we will clean boot.  These will actually be overwritten by the
	// selections made by the desktop transport.
	pKitl->dwBootFlags = KITL_FL_DBGMSG | KITL_FL_PPSH | KITL_FL_KDBG
					 | KITL_FL_CLEANBOOT;

	// Initialize device name - this needs to be changed to match the name
	// from the bootloader, and is typically read from memory shared with
	// the bootloader.
	memset(pKitl->szName, 0, sizeof(pKitl->szName));

	// Initialize to polling mode (no interrupt functionality).  Interrupt
	// mode will give far better performance; to use it, change this value
	// and initialize pfnEnableInt.
#ifdef USE_INTERRUPTS
	pKitl->Interrupt = (UCHAR)SYSINTR_UART0;

	// Initialize to polling mode (no interrupt functionality).  Interrupt
	// mode will give far better performance.
	pKitl->pfnEnableInt = UART_EnableInt;
#else
	pKitl->Interrupt = (UCHAR)KITL_SYSINTR_NOINTR;

	// Initialize to polling mode (no interrupt functionality).  Interrupt
	// mode will give far better performance.
	pKitl->pfnEnableInt = NULL;
#endif
	// Legacy value, ignored by KITL
	pKitl->WindowSize = 0;

	// size of the OEM-specific frame header, typically used for checksum.
	// Needs to be updated to match the Ethernet frame header size.
	pKitl->FrmHdrSize = 0;

	// size of the OEM-specific frame tail.  For Ethernet, this is 0.
	pKitl->FrmTlrSize = 0;

	// Legacy value, ignored by KITL
	pKitl->dwPhysBuffer = 0;

	// Legacy value, ignored by KITL
	pKitl->dwPhysBufLen = 0;

	//------------------------
	// Initialize the OAL_KITL_DEVICE structure
	//------------------------

	// Set up the device name (unicode string).  This should match the
	// bootloader's name.
	kitlDevice.name = L"";

	// Initialize the interface name.  Currently unused by platform\common code.
	kitlDevice.ifcType = Args.devLoc.IfcType;  

	// Initialize the id of the KITL device.  This is used by the
	// platform\common code to match our KITL device we're using against the
	// list of supported OAL_KITL_DEVICEs.  In this sample we only have
	// one supported device but if we support several how we setup the id
	// will determine which device we use.
	//
	// For InterfaceTypeUndefined and Internal interface types, the id
	// should match the LogicalLoc of the device location.
	//
	// For PCIBus interface type, the id should match the PCI Vendor and Device
	// IDs of the device (bits 0-15 vendor ID, bits 16-31 device ID)
	kitlDevice.id = Args.devLoc.LogicalLoc;

	// Initialize the resource identifier.  Currently unused by platform\common
	// code.
	kitlDevice.resource = 0;

	// Initialize the driver type.  We're supporting the default Ethernet
	// KITL debug transport, so the type is OAL_KITL_TYPE_ETH.
	//kitlDevice.type = OAL_KITL_TYPE_ETH;

	// Set this for internal transport (serial)
	kitlDevice.type = Internal;

	// Initialize the driver pointer to our defined set of Ethernet KITL
	// functions.  These are prototyped in bsp_ethdrv.h and implemented in
	// the driver library.
	kitlDevice.pDriver = GetKitlSerialDriver ();

    rc = OALKitlSerialInit (szDeviceId, &kitlDevice, &Args, pKitl);

	//BUGBUG:: overriding the irq value set in the serialinit routine.
#ifdef USE_INTERRUPTS
	pKitl->Interrupt = (UCHAR)SYSINTR_UART0;
#else
	pKitl->Interrupt = (UCHAR)KITL_SYSINTR_NOINTR;
#endif


	//if (rc) 
	//{
	//	pKitl->pfnPowerOn  = OALKitlPowerOn;
	//	pKitl->pfnPowerOff = OALKitlPowerOff;
	//} 
	//else 
	{
		pKitl->pfnPowerOn  = NULL;
		pKitl->pfnPowerOff = NULL;
	}

    KITL_RETAILMSG(ZONE_KITL_OAL, ("-OEMKitlInit(rc = %d) irq=%d\r\n", rc, pKitl->Interrupt));
	KITLOutputDebugString ("-OEMKitlInit(rc = %d) irq=%d\r\n", rc, pKitl->Interrupt);

	KITLOutputDebugString ("\r\nKITL Starting. Waiting for Platform Builder connect.\r\n\r\n");
    return rc;
}
