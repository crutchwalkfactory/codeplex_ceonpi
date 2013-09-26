//
// Copyright (c) Douglas Boling
//
// Module Name:
//     dwcregs.h
//
// Abstract: Provides interface to UHCI host controller
//
// Notes:
//

#ifndef __DWCREGS_H__
#define __DWCREGS_H__

//(db) Needed to track initial load
#define FirstLoadAfterBootKey TEXT("FirstLoadAfterBoot")



	//-----------------------------------------------------------------
	// Start of DWC Global Register defines
	//

	//------------------------------------
	// 0x00 OTG Control reg
	typedef struct {
		volatile DWORD SesReqScs:1;       // 00 Device only
		volatile DWORD SesReq:1;          // 01 Device only
		volatile DWORD VbvalidOvEn:1;     // 02
		volatile DWORD VbvalidOvVal:1;    // 03
		volatile DWORD AvalidOvEn:1;      // 04 
		volatile DWORD AvalidOvVal:1;     // 05 
		volatile DWORD BvalidOvEn:1;      // 06 Device only
		volatile DWORD BvalidOvVal:1;     // 07 Device only
		volatile DWORD HstNegScs:1;       // 08 Device only
		volatile DWORD HNPReq:1;          // 09 Device only
		volatile DWORD HstSetHNPEn:1;     // 10 
		volatile DWORD DevHNPEn:1;        // 11  Device only
		volatile DWORD Reserved1:4;       // 12-15 
		volatile DWORD ConIDSts:1;        // 16 
		volatile DWORD DbncTime:1;        // 17 
		volatile DWORD ASesVld:1;         // 18 
		volatile DWORD BSesVld:1;         // 19 
		volatile DWORD OTGVer:1;          // 20 
		volatile DWORD Reserved2:1;       // 21
		volatile DWORD MultValIdBC:5;     // 22-26 
		volatile DWORD ChirpEn:1;         // 27 
		volatile DWORD Reserved3:4;       // 28-31
	} GOTGCTLREG_Bit;
    typedef union {
        volatile GOTGCTLREG_Bit bit;
        volatile DWORD ul;
    } GOTGCTLREG;

	//------------------------------------
	// 0x08 Global AHB Configuration reg
	typedef struct {
		volatile DWORD GlblIntrMsk:1;  // 00 Global Int enable
#define BURST_4    0
#define BURST_3    1
#define BURST_2    2
#define BURST_1    3
		volatile DWORD AxiBstLen:2;       // 01-02 Burst length (BCM2835 specific)
		volatile DWORD Reserved1:1;       // 03
		volatile DWORD WaitForAxiWrt:1;   // 04 
		volatile DWORD DMAEn:1;           // 05 
		volatile DWORD Reserved2:1;       // 06 
		volatile DWORD NPTxFEmpLvl:1;     // 07 
		volatile DWORD PerTxFEmpLvl:1;    // 08 
		volatile DWORD Reserved3:12;      // 09-20 
		volatile DWORD RemMemSupp:1;      // 21 
		volatile DWORD NotiAllDmaWrit:1;  // 22 
#define AHBSINGLE_INCR    0
#define AHBSINGLE_SINGLE  1
		volatile DWORD AHBSingle:1;       // 23 
		volatile DWORD InvDescEndianness:1; // 24 
		volatile DWORD Reserved4:7;       // 25-31
	} GAHBCFGREG_Bit;
    typedef union {
        volatile GAHBCFGREG_Bit bit;
        volatile DWORD ul;
    } GAHBCFGREG;

	//------------------------------------
	// 0x0C Global USB Configuration reg
	typedef struct {
		volatile DWORD TOutCal:3;        // 00-02 Timeout Calibration
#define PYHIF_8BIT	0
#define PYHIF_16BIT	1
		volatile DWORD PHYIf:1;          // 03 Phys I/F cfg. 0=8 bit, 1=16 bit
#define SEL_UTMI   0
#define SEL_ULPI   1
		volatile DWORD ULPI_UTMI_Sel:1;  // 04 0=UTMI+, 1=ULPI
		volatile DWORD FSIntf:1;         // 05 Full Speed I/F sel
		volatile DWORD PHYSel:1;         // 06 
		volatile DWORD DDRSel:1;         // 07 
		volatile DWORD SRPCap:1;         // 08 
		volatile DWORD HNPCap:1;         // 09 
		volatile DWORD USBTrdTim:4;      // 10-13 USB Turnaround Time
		volatile DWORD Reserved1:1;      // 14 
		volatile DWORD PhyLPwrClkSel:1;  // 15 
		volatile DWORD OtgI2CSel:1;      // 16 
		volatile DWORD ULPIFsLs:1;       // 17 
		volatile DWORD ULPIAutoRes:1;    // 18 
		volatile DWORD ULPIClkSusM:1;    // 19 
		volatile DWORD ULPIExtVbusDrv:1; // 20 
		volatile DWORD ULPIExtVbusIndicator:1; // 21 
		volatile DWORD TsDlinePulseEn:1; // 22 
		volatile DWORD Complement:1;     // 23 
		volatile DWORD Indicator:1;      // 24 
		volatile DWORD ULPI:1;           // 25 
		volatile DWORD IC_USBCap:1;      // 26 
		volatile DWORD IC_USBTrafCtl:1;  // 27 
		volatile DWORD Reserved3:1;      // 28 
		volatile DWORD ForceHstMode:1;   // 29 Force Host mode
		volatile DWORD ForceDevMode:1;   // 30 Force Dev mode 
		volatile DWORD CorruptTxPkt:1;   // 31 
	} GUSBCFGREG_Bit;
    typedef union {
        volatile GUSBCFGREG_Bit bit;
        volatile DWORD ul;
    } GUSBCFGREG;

	//------------------------------------
	// 0x10 Global Reset reg
	typedef struct {
		volatile DWORD CSftRst:1;    // 00 Soft Reset
		volatile DWORD Reserved1:1;  // 01
		volatile DWORD FrmCntrRst:1; // 02 Frame Ctr reset
		volatile DWORD INTknQFlsh:1; // 03 Device only
		volatile DWORD RxFFlsh:1;    // 04 Flush Recv Fifo
		volatile DWORD TxFFlsh:1;    // 05 Flush Xmit Fifo
#define TXFIFOFLUSH_ASYNC	0
#define TXFIFOFLUSH_ALL  	0x10
		volatile DWORD TxFNum:5;     // 06-10 Indicates how to flush Tx Fifo
		volatile DWORD Reserved3:19; // 11-29
		volatile DWORD DmaReq:1;     // 30 DMA request in progress
		volatile DWORD AHBIdle:1;    // 31 AHB Idle
	} GRSTCTLREG_Bit;
    typedef union {
        volatile GRSTCTLREG_Bit bit;
        volatile DWORD ul;
    } GRSTCTLREG;

	//------------------------------------
	// 0x14 Global Interrupt Reg
	typedef struct {
		volatile DWORD CurMod:1;     // 00 Current mode (0-dev, 1-host)
		volatile DWORD ModeMis:1;    // 01 Mode mismatch
		volatile DWORD OTGInt:1;     // 02 On-the-go irq 
		volatile DWORD Sof:1;        // 03 Start of microframe/frame
		volatile DWORD RxFLvl:1;     // 04 Rect Fifo empty
		volatile DWORD NPTxFEmp:1;   // 05 Async TxFifo empty
		volatile DWORD Reserved1:10; // 06-15
		volatile DWORD RstrDoneInt:1; // 16 Restore done
		volatile DWORD Reserved2:4;  // 17-20
		volatile DWORD incomplP:1;   // 21 Incomplete xfer
		volatile DWORD Reserved3:2;  // 22-23
		volatile DWORD PrtInt:1;     // 24 Host port irq
		volatile DWORD HChInt:1;     // 25 Host channel irq
		volatile DWORD PTxFEmp:1;    // 26 Periodic TxFifo empty
		volatile DWORD LPM_Int:1;    // 27 LPM Transaction
		volatile DWORD ConIDStsChng:1; // 28 Connector ID Status change
		volatile DWORD DisconnInt:1; // 29 Disconnect detect
		volatile DWORD SessReqInt:1; // 30 Session request
		volatile DWORD WkUpInt:1;    // 31 Wake detect
	} GINTSTSREG_Bit;
    typedef union {
        volatile GINTSTSREG_Bit bit;
        volatile DWORD ul;
    } GINTSTSREG;

	// 0x18 Global Interrupt Mask Reg
	typedef struct {
		volatile DWORD Reserved1:1;     // 00 Current mode (0-dev, 1-host)
		volatile DWORD ModeMisMsk:1;    // 01 Mode mismatch
		volatile DWORD OTGIntMsk:1;     // 02 On the go 
		volatile DWORD SofMsk:1;        // 03 Start of microframe/frame
		volatile DWORD RxFLvlMsk:1;     // 04 Rect Fifo empty
		volatile DWORD NPTxFEmpMsk:1;   // 05 Async TxFifo empty
		volatile DWORD Reserved2:10;    // 06-15
		volatile DWORD RstrDoneIntMsk:1; // 16 Restore done
		volatile DWORD Reserved3:4;     // 17-20
		volatile DWORD incomplPMsk:1;   // 21 Incomplete xfer
		volatile DWORD Reserved4:2;     // 22-23
		volatile DWORD PrtIntMsk:1;     // 24 Host port 
		volatile DWORD HChIntMsk:1;     // 25 Host channel 
		volatile DWORD PTxFEmpMsk:1;    // 26 Periodic TxFifo empty
		volatile DWORD LPM_IntMsk:1;    // 27 LPM Transaction
		volatile DWORD ConIDStsChngMsk:1; // 28 Connector ID Status change
		volatile DWORD DisconnIntMsk:1; // 29 Disconnect detect
		volatile DWORD SessReqIntMsk:1; // 30 Session request
		volatile DWORD WkUpIntMsk:1;    // 31 Wake detect
	} GINTMSKREG_Bit;
    typedef union {
        volatile GINTMSKREG_Bit bit;
        volatile DWORD ul;
    } GINTMSKREG;

// Bits for the TxFifo flush field
#define TXFIFO_FLUSH_ASYNCFIFO  0
#define TXFIFO_FLUSH_SYNCFIFO   1

	//------------------------------------
	// 0x1C and 0x20 Global Receive Status Read Reg (Pop if read from offset 0x20)
	typedef struct {
		volatile DWORD ChNum:4;			// 00-03 Channel number for curr recvd packet
		volatile DWORD BCnt:11;			// 04-14 Byte count of recvd packet
		volatile DWORD DPID:2;			// 15-16 Data PID if recvd packet
		volatile DWORD PktSts:4;        // 17-20 Packet status
		volatile DWORD fn:4;			// 21-24 
		volatile DWORD Reserved1:7;     // 25-31
	} GRXSTSREG_Bit;
    typedef union {
        volatile GRXSTSREG_Bit bit;
        volatile DWORD ul;
    } GRXSTSREG;

#define TOTAL_FIFO_SIZE         0x0ff0  // Seen in HwCfgReg3
// These values are in units of 4-byte blocks.  Offsets for the addresses seem also to be in the same units
#define RECV_FIFO_SIZE          774     
#define ASYNC_XMIT_FIFO_SIZE    256
#define PERIODIC_XMIT_FIFO_SIZE 512

	//------------------------------------
	// 0x24 Receive Fifo Size Reg
	typedef struct {
		volatile WORD RxFDep;	       // 00-15 Receive Fifo size
		volatile WORD Reserved1;       // 16-31
	} GRXFSIZREG_Bit;
    typedef union {
        volatile GRXFSIZREG_Bit bit;
        volatile DWORD ul;
    } GRXFSIZREG;

	//------------------------------------
	// 0x28 Non-Periodic Transmit Fifo Size Reg
	typedef struct {
		volatile WORD NPTxFStAddr;      // 00-15 Async Tmit Fifo Base
		volatile WORD NPTxFDep;         // 16-31 Async Tmit Fifo Size
	} GATXFSIZREG_Bit;
    typedef union {
        volatile GATXFSIZREG_Bit bit;
        volatile DWORD ul;
    } GATXFSIZREG;

	//------------------------------------
	// 0x48 Hardware Config Reg 2
	typedef struct {
#define OTGM_HNPSRP_HOSTDEVICE   0
#define OTGM_SRP_HOSTDEVICE      1
#define OTGM_NOHNPSRP_HOSTDEVICE 2
#define OTGM_SRP_DEVICE          3
#define OTGM_NO_OTG              4
#define OTGM_SRP_HOST            5
#define OTGM_NO_OTGHOST          6
		volatile DWORD OtgMode:3;		// 00-02 Operating Mode

#define DMAA_SLAVE         0
#define DMAA_EXTERNDMA     1
#define DMAA_INTERNDMA     2
		volatile DWORD OtgArch:2;		// 03-04 
		volatile DWORD SingPnt:1;		// 05 

#define HSP_NOTSUPPORTED   0
#define HSP_UTMI           1
#define HSP_ULPI           2
#define HSP_UTMI_ULPI      3
		volatile DWORD HSPhyType:2;     // 06-07

#define FSP_NOTSUPPORTED   0
#define FSP_DEDICATEDFS    1
#define FSP_UTMI           2
#define FSP_ULPI           3
		volatile DWORD FSPhyType:2;     // 08-09
		volatile DWORD NumDevEps:4;		// 10-13 
		volatile DWORD NumHstChnl:4;    // 14-17
		volatile DWORD PerioSupport:1;  // 18
		volatile DWORD DynFifoSizing:1; // 19
		volatile DWORD MultiProcIntrpt:1; // 20
		volatile DWORD Reserved1:1;     // 21
		volatile DWORD NPTxQDepth:2;    // 22-23
		volatile DWORD PTxQDepth:2;     // 24-25
		volatile DWORD TknQDepth:5;     // 26-30
		volatile DWORD OTG_ENABLE_IC_USB:1; // 31
	} GHWCFG2REG_Bit;
    typedef union {
        volatile GHWCFG2REG_Bit bit;
        volatile DWORD ul;
    } GHWCFG2REG;

	//------------------------------------
	// 0x4C Hardware Config Reg 3
	typedef struct {
		volatile DWORD XferSizeWidth:4;	// 00-03 Operating Mode
		volatile DWORD PktSizeWidth:2;	// 04-06
		volatile DWORD OtgEn:1;		    // 07 
		volatile DWORD I2CIntSel:1;     // 08
		volatile DWORD VndctlSupt:1;    // 09
		volatile DWORD OptFeature:1;    // 10 
		volatile DWORD RstType:1;       // 11
		volatile DWORD ADPSupport:1;    // 12
		volatile DWORD HSICMode:1;      // 13
		volatile DWORD BCSupport:1;     // 14
		volatile DWORD LPMMode:1;       // 15
		volatile DWORD DfifoDepth:17;   // 16-31
	} GHWCFG3REG_Bit;
    typedef union {
        volatile GHWCFG3REG_Bit bit;
        volatile DWORD ul;
    } GHWCFG3REG;

#define GRXSTSREG_PAKSTAT_UPDT	0x02 // Data packet
#define GRXSTSREG_PAKSTAT_COMP	0x03 // Transfer complete

#define GRXSTSREG_PAKSTAT__GOUT_NAK 	0x01 // Global out NAK
#define GRXSTSREG_PAKSTAT_SETUP_CMP		0x04 // Setup Phase Complete
#define GRXSTSREG_PAKSTAT_SETUP_UPDT	0x06 // Setup packet

	//------------------------------------
	// 0x100 Periodic Transmit Fifo Size Reg
	typedef struct {
		volatile WORD PTxFStAddr;       // 00-15 Periodic Tmit Fifo Base
		volatile WORD PTxFSize;         // 16-31 Periodic Tmit Fifo Size
	} HPTXFSIZREG_Bit;
    typedef union {
        volatile HPTXFSIZREG_Bit bit;
        volatile DWORD ul;
    } HPTXFSIZREG;

	//-----------------------------------------------------------------
	// DWC Global Register set
	//
	typedef struct {
		volatile GOTGCTLREG GOtgCtlReg;   //0x00  Global control and status reg
		volatile DWORD GOtgIntReg;   //0x04  Global Interrupt reg
		volatile GAHBCFGREG GAhbCfgReg;   //0x08  Global AHB Config reg
		volatile GUSBCFGREG GUsbCfgReg;   //0x0c  Global USB Config reg
		volatile GRSTCTLREG GRstCtlReg;   //0x10  Global Reset reg
		volatile GINTSTSREG GIntStsReg;   //0x14  Global Interrupt Status reg
		volatile GINTMSKREG GIntMskReg;   //0x18  Global Interrupt Mask reg
		volatile GRXSTSREG GRxStsRdReg;   //0x1c  Global Recv Stat reg
		volatile GRXSTSREG GRxStsPopReg;  //0x20 
		volatile GRXFSIZREG GRxfSizReg;    //0x24  Global Recv Fifo Size reg
		volatile GATXFSIZREG GNptxfsizReg; //0x28  Global Async Xmit Fifo Size reg
		volatile DWORD GNptxStsReg;        //0x2c  Global Async Xmit Fifo Stat reg
		volatile DWORD Reserved1;    //0x30 GI2CCTL Not implemented on 2835   
		volatile DWORD Reserved2;    //0x34 GPvndCtlReg Not implemented on 2835
		volatile DWORD GGpioReg;     //0x38  Global  reg xxxx
		volatile DWORD GUIDReg;      //0x3c  Global  reg xxxx
		volatile DWORD GSnpSidReg;   //0x40  Global  reg xxxx
		volatile DWORD GHwCfg1Reg;   //0x44  Global HW Cfg reg
		volatile GHWCFG2REG GHwCfg2Reg;   //0x48  Global HW Cfg reg
		volatile GHWCFG3REG GHwCfg3Reg;   //0x4c  Global HW Cfg reg
		volatile DWORD GHwCfg4Reg;   //0x50  Global HW Cfg reg
		volatile DWORD GLpmCfgReg;   //0x54  Global  reg
		volatile DWORD GPwrDnReg;    //0x58  Global  reg
		volatile DWORD GDFifoCfgReg; //0x5c  Global  reg
		volatile DWORD GAdpctlReg;   //0x60  Global control and status reg
		DWORD Reserved3[0x07];
		// The following 3 regs are 2835 specific regs
		volatile DWORD GMdioCtlReg;  //0x80  Global MDIO Interface Ctl Reg
		volatile DWORD GMdioDataReg; //0x84  Global MDIO Data          Reg
		volatile DWORD GVBusDrvReg;  //0x88  Global MDIO VBus Drive Reg
		DWORD Reserved4[0x1d];
		volatile HPTXFSIZREG GPTxfSizReg;  //0x100  Host Periodic Xmit Fifo reg
	} DWCGLOBALREGS, *PDWCGLOBALREGS;

	//-----------------------------------------------------------------
	// Start of DWC Host Register defines
	//

	//------------------------------------
	// 0x400 Host Config
	typedef struct {
#define PHYCLK_30_60   0
#define PHYCLK_48      1
#define PHYCLK_6       2
		volatile DWORD FSLSPclkSel:2;  // 00-01 FS/LS Phy Clk Select
		volatile DWORD FSLSSupp:1;     // 02 Core enum speed
		volatile DWORD Reserved1:4;    // 03-06 
		volatile DWORD Ena32KHzS:1;    // 07 Enable 32 KHz Suspend mode
		volatile DWORD ResValid:8;     // 08-15 Resume Validation Period
		volatile DWORD Reserved2:7;    // 16-22 
		volatile DWORD DescDMA:1;      // 23 (Scatter/gather only)
		volatile DWORD FrListEn:2;     // 24-25 (Scatter/gather only)
		volatile DWORD PerSchedEna:1;    // 26 (Scatter/gather only)
		volatile DWORD Reserved6:4;    // 27-30
		volatile DWORD ModeChTimEn:1;  // 31 Mode Change Ready Timer Enable
	} HCFGREG_Bit;
    typedef union {
        volatile HCFGREG_Bit bit;
        volatile DWORD ul;
    } HCFGREG;

	//------------------------------------
	// 0x408 Frame Interval Register
	typedef struct {
		volatile DWORD FrInt:16;      // 00-15 Frame Interval
		volatile DWORD HFIRRldCtrl:1; // 16 1 - Frame interval can be reprogrammed during runtime
		volatile DWORD FrRem:15;      // 17-31 
	} HFIRREG_Bit;
    typedef union {
        volatile HFIRREG_Bit bit;
        volatile DWORD ul;
    } HFIRREG;

	//------------------------------------
	// 0x408 Frame Number/Frame Time remaining
	typedef struct {
		volatile DWORD uFrNum:3;      // 00-02 Micro Frame number
		volatile DWORD FrNum:13;     // 03-15 Frame number
		volatile DWORD FrRem:16;     // 16-31 Frame time remainng
	} HFNUMREG_Bit;
    typedef union {
        volatile HFNUMREG_Bit bit;
        volatile DWORD ul;
    } HFNUMREG;

	//------------------------------------
	// 0x414 Host All Channels Interrupt Reg
	typedef struct {
		volatile DWORD HAINT:16;       // 00-15 Bitmask for channels (NOTE: our implementation only has 8 channels)
		volatile DWORD Reserved:16;    // 16-31 
	} HAINTREG_Bit;
    typedef union {
        volatile HAINTREG_Bit bit;
        volatile DWORD ul;
    } HAINTREG;

	//------------------------------------
	// 0x418 Host All Channels Interrupt Mask Reg
	typedef struct {
		volatile DWORD HAINTMsk:16;    // 00-15 Bitmask for channels (NOTE: our implementation only has 8 channels)
		volatile DWORD Reserved:16;    // 16-31 
	} HAINTMSKREG_Bit;
    typedef union {
        volatile HAINTMSKREG_Bit bit;
        volatile DWORD ul;
    } HAINTMSKREG;


//#define PRTCTL_RESPECTBITS  0x1f140
#define PRTCTL_RESPECTBITS  0x00100 // remove tst stat and resume bits (always read/write 0)
	//------------------------------------
	// 0x440 Host Port Control and Status Reg 0x1f140
	// 
	// NOTE: Don't write this register directly.  Use inline function instead.
	//
	typedef struct {
		volatile DWORD PrtConnSts:1;   // 00 Connection Stat
		volatile DWORD PrtConnDet:1;   // 01 Connect Detected
		volatile DWORD PrtEna:1;       // 02 Port Enable. (Write 1 to clear (DISABLE) port)
		volatile DWORD PrtEnChng:1;    // 03 Port enabled changed

		volatile DWORD PrtOvrCurrAct:1;  // 04 Port overcurrent 
		volatile DWORD PrtOvrCurrChng:1; // 05 Port overccurrent stat changed
		volatile DWORD PrtRes:1;       // 06* Port resume
		volatile DWORD PrtSusp:1;      // 07 Port suspend

		volatile DWORD PrtRst:1;       // 08* Port reset
		volatile DWORD Reserved1:1;    // 09 
		volatile DWORD PrtLnSts:2;     // 10-11 Level of D+/D- (opposite of EHCI)

		volatile DWORD PrtPwr:1;       // 12* Port power
		volatile DWORD PrtTstCtl:4;    // 13-16* Port test control
#define PORT_SPEED_HS  0
#define PORT_SPEED_FS  1
#define PORT_SPEED_LS  2
		volatile DWORD PrtSpd:2;       // 17 Port speed 0-HS, 1-FS, 2-LS
		volatile DWORD Reserved2:13;   // 11-31
	} HPRTREG_Bit;
    typedef union {
        volatile HPRTREG_Bit bit;
        volatile DWORD ul;
    } HPRTREG;


	//------------------------------------
	// 0x500 Host Channel Characteristics Reg

	typedef struct {
		volatile DWORD MPS:11;         // 00-10 Max packet size
		volatile DWORD EPNum:4;        // 11-14 Endpoint number
#define CHAN_CHAR_EPDIR_OUT   0
#define CHAN_CHAR_EPDIR_IN    1
		volatile DWORD EPDir:1;        // 15 Endpoint direction 0=Out, 1=In
		volatile DWORD Reserved1:1;    // 16  
		volatile DWORD LSpdDev:1;      // 17 Low speed device attached
#define CHAN_CHAR_TYPE_CTRL   0
#define CHAN_CHAR_TYPE_ISOC   1
#define CHAN_CHAR_TYPE_BULK   2
#define CHAN_CHAR_TYPE_INTR   3
		volatile DWORD EPType:2;       // 18-19 Endpoint type
		volatile DWORD EC:2;           // 20-21 MultiCount/Error count
		volatile DWORD DevAddr:7;      // 22-28 Device Address
		volatile DWORD OddFrm:1;       // 29 Odd Frame
		volatile DWORD ChDis:1;        // 30 Channel disable 
		volatile DWORD ChEna:1;        // 31 Channel enable
	} HCCHARREG_Bit;
    typedef union {
        volatile HCCHARREG_Bit bit;
        volatile DWORD ul;
    } HCCHARREG;

	//------------------------------------
	// 0x504+(Cn*0x20) Host Channel Split Control Reg
	typedef struct {
		volatile DWORD PrtAddr:7;      // 00-06 Port Address
		volatile DWORD HubAddr:7;      // 07-13 Hub Address
#define XACTPOS_ALL    3 // Entire payload in this transaction
#define XACTPOS_BEGIN  2 // First part of payload in this transaction
#define XACTPOS_END    1 // Last part of payload in this transaction
#define XACTPOS_MID    0 // Not first/last part payload in this transaction
		volatile DWORD XactPos:2;      // 14-15 Transaction Position
		volatile DWORD CompSPl:1;      // 16 Do Complete Split
		volatile DWORD Reserved:14;    // 17-30 
		volatile DWORD SpltEna:1;      // 31 split enable
	} HCSPLTREG_Bit;
    typedef union {
        volatile HCSPLTREG_Bit bit;
        volatile DWORD ul;
    } HCSPLTREG;

	//------------------------------------
	// 0x508+(Cn*0x20) Host Channel Interrupt Reg
	typedef struct {
		volatile DWORD XferCompl:1;    // 00 Transfer Complete
		volatile DWORD ChHltd:1;       // 01 Chan Halted
		volatile DWORD AHBErr:1;       // 02 AHB Error (DMA Err)
		volatile DWORD STALL:1;        // 03 Chan Stall
		volatile DWORD NAK:1;          // 04 NAK
		volatile DWORD ACK:1;          // 05 ACK
		volatile DWORD NYET:1;         // 06 NYET Not yet
		volatile DWORD XactErr:1;      // 07 Transaction Error (CRC, timeout, bitstuff,False EOP
		volatile DWORD BblErr:1;       // 08 Babble
		volatile DWORD FrmOvrun:1;     // 09 Frame Overrun
		volatile DWORD DataTglErr:1;   // 10 Data Toggle Err
		volatile DWORD BNAIntr:1;      // 11 Buff not Available (ScatterGather only)
		volatile DWORD XCS_XACT_ERR:1; // 12 Excessive Transaction error (ScatterGather only)
		volatile DWORD DESC_LST_ROLLIntr:1; // 13 Descriptor rollover error (ScatterGather only)
		volatile DWORD Reserved:18;    // 14-31 
	} HCINTREG_Bit;
    typedef union {
        volatile HCINTREG_Bit bit;
        volatile DWORD ul;
    } HCINTREG;

	//------------------------------------
	// 0x50c+(Cn*0x20) Host Channel Interrupt Mask Reg
	typedef struct {
		volatile DWORD XferCompl:1;    // 00 Transfer Complete
		volatile DWORD ChHltd:1;       // 01 Chan Halted
		volatile DWORD AHBErr:1;       // 02 AHB Error (DMA Err)
		volatile DWORD STALL:1;        // 03 Chan Stall
		volatile DWORD NAK:1;          // 04 NAK
		volatile DWORD ACK:1;          // 05 ACK
		volatile DWORD NYET:1;         // 06 NYET
		volatile DWORD XactErr:1;      // 07 Transaction Error (CRC, timeout, bitstuff,False EOP
		volatile DWORD BblErr:1;       // 08 Babble
		volatile DWORD FrmOvrun:1;     // 09 Frame Overrun
		volatile DWORD DataTglErr:1;   // 10 Data Toggle Err
		volatile DWORD BNAIntr:1;      // 11 Buff not Available (ScatterGather only)
		volatile DWORD Reserved:1;     // 12 Excessive Transaction error (ScatterGather only)
		volatile DWORD DESC_LST_ROLLIntr:1; // 13 Descriptor rollover error (ScatterGather only)
		volatile DWORD Reserved1:18;   // 14-31 
	} HCINTMSKREG_Bit;
    typedef union {
        volatile HCINTMSKREG_Bit bit;
        volatile DWORD ul;
    } HCINTMSKREG;

	//------------------------------------
	// 0x510+(Cn*0x20) Host Channel Transfer Size Reg
	typedef struct {
		volatile DWORD XferSize:19;    // 00-18 Transfer Size
		volatile DWORD PktCnt:10;      // 19-28 Packet Count
#define DWCPID_DATA0           0
#define DWCPID_DATA2           1   //High Speed (480mbps)
#define DWCPID_DATA1           2
#define DWCPID_MDATA_SETUP     3   //High Speed (480mbps)
		volatile DWORD Pid:2;          // 29-30 PID
		volatile DWORD DoPng:1;        // 31 Do Ping
	} HCTSIZREG_Bit;
    typedef union {
        volatile HCTSIZREG_Bit bit;
        volatile DWORD ul;
    } HCTSIZREG;

	//------------------------------------
	// 0x514+(Cn*0x20) Host Channel DMA Address Reg
	typedef struct {
		volatile DWORD DMAAddr:32;     // 00-31 Format so reg consistent with other regs
	} HCDMAREG_Bit;
    typedef union {
        volatile HCDMAREG_Bit bit;
        volatile DWORD ul;
    } HCDMAREG;

	//-----------------------------------------------------------------
	// DWC Host Register set
	//
#define NUM_CHAN    8   //BCM2835 supports 8 channels

	typedef struct {
		volatile HCFGREG HCfgReg;         //0x400  Host Reg
		volatile DWORD HFirReg;           //0x404  Host Reg
		volatile HFNUMREG HFNumReg;       //0x408  Host Frame Number/Frame Time Reg
		volatile DWORD Reserved1;         //0x40c  Host Reg
		volatile DWORD HPtxStsReg;        //0x410  Host Reg
		volatile HAINTREG HAIntReg;       //0x414  Host Reg
		volatile HAINTMSKREG HAIntMskReg; //0x418  Host Reg
		volatile DWORD Reserved2;         //0x41c  Host Reg
		volatile DWORD Reserved3[8];      //0x420-43c
		volatile HPRTREG HPrtReg;         //0x440  Host Port control and status Reg
		volatile DWORD Reserved4[0x2f];   //0x444-4fc
		struct {
			volatile HCCHARREG HChCharReg;   //0x500+n*20 Host Channel Characteristics Reg
			volatile HCSPLTREG HChSpltReg;   //0x504+n*20 Host Channel Split Ctl Reg
			volatile HCINTREG HChIntReg;     //0x508+n*20 Host Channel Interrupt Reg
			volatile HCINTMSKREG HChIntMaskReg; //0x50c+n*20 Host Channel Interrupt Mask Reg
			volatile HCTSIZREG HChXfrSizeReg;   //0x510+n*20 Host Channel Transfer Size Reg
			volatile HCDMAREG HChDmaAddrReg;    //0x514+n*20 Host Channel DMA Address Reg
			volatile DWORD Reserved1;        //0x518  
			volatile DWORD Reserved2;        //0x51c  
		} Channel[NUM_CHAN];
	} DWCHOSTREGS, *PDWCHOSTREGS;




#define USBOFFSET_POWER   0x0e00
	//------------------------------------
	// 0xE00 Power and Clock Gating Reg
	typedef struct {
		volatile DWORD StopPclk:1;         // 00 
		volatile DWORD GateHclk:1;         // 01 
		volatile DWORD PwrClmp:1;          // 02 
		volatile DWORD RstPdwnModule:1;    // 03 
		volatile DWORD Reserved:1;         // 04 
		volatile DWORD Enbl_L1Gating:1;    // 05 Enable Sleep Gating
		volatile DWORD PhySleep:1;         // 06 
		volatile DWORD Suspended:1;        // 07 
		volatile DWORD ResetAfterSusp:1;   // 08 
		volatile DWORD RestoreMode:1;      // 09 
		volatile DWORD EnExtndedHibernation:1;      // 10 
		volatile DWORD ExtendedHibernationClamp:1;  // 11 
		volatile DWORD ExtendedHibernationSwitch:1; // 12 
		volatile DWORD EssRegRestored:1;   // 13 
#if 0
		volatile DWORD RestoreValue:18;     // 14-31 
#else
		volatile DWORD prt_clk_sel:2;       // 14-15
		volatile DWORD port_power:1;        // 16 
		volatile DWORD max_xcvrselect:2;    // 17-18
		volatile DWORD max_termsel:1;       // 19 
		volatile DWORD mac_dev_addr:7;      // 20-26 
		volatile DWORD p2hd_dev_enum_spd:2; // 27-28 
		volatile DWORD p2hd_prt_spd:2;      // 29-30 
		volatile DWORD if_dev_mode:1;       // 31 

#endif
	} POWERREG_Bit;
    typedef union {
        volatile POWERREG_Bit bit;
        volatile DWORD ul;
    } POWERREG;


#endif //__DWCREGS_H__

