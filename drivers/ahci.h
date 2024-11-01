#include<stdlib.h>
#include<stdio.h>
/*
    Referred from: https://wiki.osdev.org/AHCI
    FRAME INFORMATION Structure(FIS) -

        a data set containing commands to transfer data
        bet host and device


*/
/*
    1. The following are FIS types specified in intel serial ata 3.0 
*/
typedef enum{
    FIS_TYPE_REG_H2D=0x27, //register fis- host to device
    FIS_TYPE_REG_D2H=0x34, //REGISTER FIS - device to host
    FIS_TYPE_DMA_ACT=0x39, //dma activate fsi -device to host
    FIS_TYPE_DMA_SETUP=0x41, //dma setup FIS - bidirectional
    FIS_TYPE_DATA=0x46, //DATA FIS -birtectional
    FIS_TYPE_BIST=0x58, //bist activate fis - bidirectionl 
    FIS_TYPE_PIO_SETUP=0x5F,//pio setup device -device to host
    FIS_TYPE_DEV_BITS=0xA1,//set devices bits -device to host
}FIS_TYPE;


/*

    2. REGISTER FIS - Host to Device

    A host to device register FIS is used by the host to send command or control 
    to a device. As illustrated in the following data structure, it contains the
    IDE registers such as command, LBA, device, feature, count and control. An ATA 
    command is constructed in this structure and issued to the device. 
    All reserved fields in an FIS should be cleared to zero. 
*/

typedef unsigned char one;
typedef unsigned short two;
typedef unsigned int four;
typedef unsigned long eight;


typedef struct tagFIS_REG_H2D
{
	// DWORD 0
	one  fis_type;	// FIS_TYPE_REG_H2D

	one  pmport:4;	// Port multiplier
	one  rsv0:3;		// Reserved
	one  c:1;		// 1: Command, 0: Control

	one  command;	// Command register
	one  featurel;	// Feature register, 7:0
	
	// DWORD 1
	one  lba0;		// LBA low register, 7:0
	one  lba1;		// LBA mid register, 15:8
	one  lba2;		// LBA high register, 23:16
	one  device;		// Device register

	// DWORD 2
	one  lba3;		// LBA register, 31:24
	one  lba4;		// LBA register, 39:32
	one  lba5;		// LBA register, 47:40
	one  featureh;	// Feature register, 15:8

	// DWORD 3
	one  countl;		// Count register, 7:0
	one  counth;		// Count register, 15:8
	one  icc;		// interface command completion
	one  control;	// Control register

	// DWORD 4
	one  rsv1[4];	// Reserved
}FIS_REG_H2D;

/*
   3.  Register FIS – Device to Host

A device to host register FIS is used by the device to notify the host that some ATA register has changed. It contains the updated task files 
such as status, error and other registers. 

*/

typedef struct tagFIS_REG_D2H
{
	// DWORD 0
	one  fis_type;    // FIS_TYPE_REG_D2H

	one  pmport:4;    // Port multiplier
	one  rsv0:2;      // Reserved
	one  i:1;         // Interrupt bit
	one  rsv1:1;      // Reserved

	one  status;      // Status register
	one  error;       // Error register
	
	// DWORD 1
	one  lba0;        // LBA low register, 7:0
	one  lba1;        // LBA mid register, 15:8
	one  lba2;        // LBA high register, 23:16
	one  device;      // Device register

	// DWORD 2
	one  lba3;        // LBA register, 31:24
	one  lba4;        // LBA register, 39:32
	one  lba5;        // LBA register, 47:40
	one  rsv2;        // Reserved

	// DWORD 3
	one  countl;      // Count register, 7:0
	one  counth;      // Count register, 15:8
	one  rsv3[2];     // Reserved

	// DWORD 4
	one  rsv4[4];     // Reserved
} FIS_REG_D2H;

/*

    4.Data FIS – Bidirectional

This FIS is used by the host or device to send data payload. 
The data size can be varied. 
*/

typedef struct tagFIS_DATA
{
	// DWORD 0
	one fis_type;	// FIS_TYPE_DATA

	one pmport:4;	// Port multiplier
	one rsv0:4;		// Reserved

	one rsv1[2];	// Reserved

	// DWORD 1 ~ N
	four data[1];	// Payload
} FIS_DATA;


/*

5.
PIO Setup – Device to Host

This FIS is used by the device to tell the host that it’s about 
to send or ready to receive a PIO data payload. 

PIO stands for Programmed Input/Output. It is a method for 
data transfer between a computer's main memory (host) and a 
peripheral device (like a hard drive or other storage device) 
where the CPU is responsible for the data transfer, handling each 
byte or word directly. In this mode, data moves in small chunks
 under direct CPU control
 without using specialized hardware (like DMA).
*/

typedef struct tagFIS_PIO_SETUP
{
	// DWORD 0
	one  fis_type;	// FIS_TYPE_PIO_SETUP

	one  pmport:4;	// Port multiplier
	one  rsv0:1;		// Reserved
	one  d:1;		// Data transfer direction, 1 - device to host
	one  i:1;		// Interrupt bit
	one  rsv1:1;

	one  status;		// Status register
	one  error;		// Error register

	// DWORD 1
	one  lba0;		// LBA low register, 7:0
	one  lba1;		// LBA mid register, 15:8
	one  lba2;		// LBA high register, 23:16
	one  device;		// Device register

	// DWORD 2
	one  lba3;		// LBA register, 31:24
	one  lba4;		// LBA register, 39:32
	one  lba5;		// LBA register, 47:40
	one  rsv2;		// Reserved

	// DWORD 3
	one  countl;		// Count register, 7:0
	one  counth;		// Count register, 15:8
	one  rsv3;		// Reserved
	one  e_status;	// New value of status register

	// DWORD 4
	two tc;		// Transfer count
	one  rsv4[2];	// Reserved
} FIS_PIO_SETUP;

/*
    6. DMA Setup – Device to Host

*/
typedef struct tagFIS_DMA_SETUP
{
	// DWORD 0
	one  fis_type;	// FIS_TYPE_DMA_SETUP

	one  pmport:4;	// Port multiplier
	one  rsv0:1;		// Reserved
	one  d:1;		// Data transfer direction, 1 - device to host
	one  i:1;		// Interrupt bit
	one  a:1;            // Auto-activate. Specifies if DMA Activate FIS is needed

        one  rsved[2];       // Reserved

	//DWORD 1&2

        eight DMAbufferID;    // DMA Buffer Identifier. Used to Identify DMA buffer in host memory.
                                 // SATA Spec says host specific and not in Spec. Trying AHCI spec might work.

        //DWORD 3
        four rsvd;           //More reserved

        //DWORD 4
        four DMAbufOffset;   //Byte offset into buffer. First 2 bits must be 0

        //DWORD 5
        four TransferCount;  //Number of bytes to transfer. Bit 0 must be 0

        //DWORD 6
        four resvd;          //Reserved
        
} FIS_DMA_SETUP;

/*
	HBA memory registers are of two types
	1. Generic Host Control Registers(Control behaviour of whole controller)
	2. Port Control Registers(Each port has its own port control registers)
*/
/*
	7. HBA memory ports
*/
typedef volatile struct tagHBA_PORT
{
	four clb;		// 0x00, command list base address, 1K-byte aligned
	four clbu;		// 0x04, command list base address upper 32 bits
	four fb;		// 0x08, FIS base address, 256-byte aligned
	four fbu;		// 0x0C, FIS base address upper 32 bits
	four is;		// 0x10, interrupt status
	four ie;		// 0x14, interrupt enable
	four cmd;		// 0x18, command and status
	four rsv0;		// 0x1C, Reserved
	four tfd;		// 0x20, task file data
	four sig;		// 0x24, signature
	four ssts;		// 0x28, SATA status (SCR0:SStatus)
	four sctl;		// 0x2C, SATA control (SCR2:SControl)
	four serr;		// 0x30, SATA error (SCR1:SError)
	four sact;		// 0x34, SATA active (SCR3:SActive)
	four ci;		// 0x38, command issue
	four sntf;		// 0x3C, SATA notification (SCR4:SNotification)
	four fbs;		// 0x40, FIS-based switch control
	four rsv1[11];	// 0x44 ~ 0x6F, Reserved
	four vendor[4];	// 0x70 ~ 0x7F, vendor specific
} HBA_PORT;


/*
	8. HBA memory registers

	Pg -23 sec 3.1
*/
typedef volatile struct tagHBA_MEM
{
	// 0x00 - 0x2B, Generic Host Control
	four cap;		// 0x00, Host capability
	four ghc;		// 0x04, Global host control
	//when 31st bit of ghc is set communication to HBA shall be via AHCI else via legacy mechanisms
	four is;		// 0x08, Interrupt status
	four pi;		// 0x0C, Port implemented
	//each bit for a single port . IF 1 : port available , 0: not available
	four vs;		// 0x10, Version
	four ccc_ctl;	// 0x14, Command completion coalescing control
	four ccc_pts;	// 0x18, Command completion coalescing ports
	four em_loc;		// 0x1C, Enclosure management location
	four em_ctl;		// 0x20, Enclosure management control
	four cap2;		// 0x24, Host capabilities extended
	four bohc;		// 0x28, BIOS/OS handoff control and status

	// 0x2C - 0x9F, Reserved
	one  rsv[0xA0-0x2C];

	// 0xA0 - 0xFF, Vendor specific registers
	one  vendor[0x100-0xA0];

	// 0x100 - 0x10FF, Port control registers
	HBA_PORT	ports[1];	// 1 ~ 32
} HBA_MEM;


/*
	WHY volatile ?
	ans:
	Volatile says that you have some hardware registers which may change without code
	and these must be informed to compiler .
	This memory area should be configured as uncacheable as they are 
	memory mapped hardware registers, not normal prefetchable RAM. For the same reason, the data structures are declared as "volatile"
	 to prevent the compiler from over optimizing the code. 
*/


/*----------------------------------------------------------------------------------

	MOST IMP part
	 
	-> Port Received FIS and Command List Memory

1. 	Each port can attach a single SATA device. 
2. 	Host sends commands to the device using Command List and 
	device delivers information to the host using Received FIS structure.

3.	They are located at HBA_PORT.clb/clbu, and HBA_PORT.fb/fbu. The most 
	important part of AHCI initialization is to set correctly 
	these two pointers and the data structures they point to. 


*/

/*

	Received FIS :
	 Received FIS

There are four kinds of FIS which may be sent to the host by the device as 
indicated in the following structure declaration. 
When an FIS has been copied into the host specified memory, an according bit will 
be set in the Port Interrupt Status register (HBA_PORT.is).

Data FIS – Device to Host is not copied to this structure. 
Data payload is sent and received through PRDT (Physical Region Descriptor Table) 
in Command List, as will be introduced later. 
*/
	typedef volatile struct tagHBA_FIS
	{
		// 0x00
		FIS_DMA_SETUP	dsfis;		// DMA Setup FIS
		one         pad0[4];

		// 0x20
		FIS_PIO_SETUP	psfis;		// PIO Setup FIS
		one         pad1[12];

		// 0x40
		FIS_REG_D2H	rfis;		// Register – Device to Host FIS
		one         pad2[4];

		// 0x58
		int	sdbfis;		// Set Device Bit FIS
		
		// 0x60
		one         ufis[64];

		// 0xA0
		one   	rsv[0x100-0xA0];
	} HBA_FIS;

/*

	Command List and Command HEADER

	Host sends commands to the device through Command List.
	 Command List consists of 1 to 32 command headers, each one is called a slot. 
	Each command header describes an ATA or ATAPI command.
*/

typedef struct tagHBA_CMD_HEADER
{
	// DW0
	one  cfl:5;		// Command FIS length in DWORDS, 2 ~ 16
	one  a:1;		// ATAPI
	one  w:1;		// Write, 1: H2D, 0: D2H
	one  p:1;		// Prefetchable

	one  r:1;		// Reset
	one  b:1;		// BIST
	one  c:1;		// Clear busy upon R_OK
	one  rsv0:1;		// Reserved
	one  pmp:4;		// Port multiplier port

	two prdtl;		// Physical region descriptor table length in entries

	// DW1
	volatile
	four prdbc;		// Physical region descriptor byte count transferred

	// DW2, 3
	four ctba;		// Command table descriptor base address
	four ctbau;		// Command table descriptor base address upper 32 bits

	// DW4 - 7
	four rsv1[4];	// Reserved
} HBA_CMD_HEADER;


typedef struct tagHBA_PRDT_ENTRY
{
	four dba;		// Data base address
	four dbau;		// Data base address upper 32 bits
	four rsv0;		// Reserved

	// DW3
	four dbc:22;		// Byte count, 4M max
	four rsv1:9;		// Reserved
	four i:1;		// Interrupt on completion
} HBA_PRDT_ENTRY;

typedef struct tagHBA_CMD_TBL
{
	// 0x00
	one  cfis[64];	// Command FIS

	// 0x40
	one  acmd[16];	// ATAPI command, 12 or 16 bytes

	// 0x50
	one  rsv[48];	// Reserved

	// 0x80
	HBA_PRDT_ENTRY	prdt_entry[1];	// Physical region descriptor table entries, 0 ~ 65535
} HBA_CMD_TBL;
