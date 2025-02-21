#pragma once

#include "Driver.h"
#include "Public.h"
#include "SemaSMB.h"

typedef struct
{
	UINT16			VID;				// PCI Vendor ID	
	UINT16			DID;				// PCI Device ID
	UINT32			BuDeFu;				// Bus: Device: Function
	UINT8			RessourceType;		// Uses ressource type mmio/portio/none
	UINT8			RessourceLength;	// Length of used ressouces
	const char* Description;		// Descriptive text
} tSMBusInfo;

typedef enum {
	SMB = 1,
	EC
} t_SmbOrEc;

typedef struct _SEMA_DEVICE_EXT
{
	WDFDEVICE		pDevice;
	INTEL_SMB_REGS* pSmbMemReg;
	ULONG			ulMemSize;
	UINT32			pSmbPortReg;
	WDFWAITLOCK		hSmbLock;
	t_SmbOrEc		SmbOrEc;
	tSMBusInfo		Chipset;
} SEMA_DEVICE_EXT, * PSEMA_DEVICE_EXT;
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(SEMA_DEVICE_EXT, GetSEMADevExt)

/* ------------------------------ Definitions ------------------------------- */
#define RESSOURTYPE_MMIO	0x01
#define RESSOURTYPE_PORT	0x02

#define SEMA_FCH_Info	{													\
							0x1022,											\
							0x1510,											\
							0x00000000,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"ATI Technologies Inc SBx00 SMBus Controller",	\
						}

// FCH SMBus controller (D20:F4:R20)
// Boards:
//	- EXPRESS-BE
#define SEMA_BE_Info	{													\
							0x1022,											\
							0x1422,											\
							0x00000000,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"AMD Bolton Fusion SMBus Controller",			\
						}


#define SEMA_ICH7_Info	{													\
							0x8086u,									    \
							0x27dau,										\
							0x00001F03,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R) ICH7 Family SMBus Controller - 27da",	\
						}

// ICH9 SMBus controller (D31:F3:R20)
// Boards:
//	- CXR-GS45
#define SEMA_ICH9_Info	{													\
							0x8086u,									    \
							0x2930u,										\
							0x00001F03,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R) ICH9 Family SMBus Controller - 2930", \
						}

// ICH10 SMBus controller (D31:F3:R20)
// Boards:
//	- Test
#define SEMA_ICH10_Info	{												\
							0x8086u,									    \
							0x3A30u,										\
							0x00001F03,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R) ICH10 Family SMBus Controller - 3A30",\
						}

// PCH (QM57) SMBus controller (D31:F3:R20)
// Boards:
//	- H-QM57
//	- T-QM57
#define SEMA_PCH_Info	{													\
							0x8086u,									    \
							0x3b30u,										\
							0x00001F03,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R) PCH Chipset SMBus Controller - 3b30", \
						}

// 6 Series Chipset Family SMBus controller (D31:F3:R20)
// Boards:
//	- NuPRO-A40H
#define SEMA_S6_Info	{													\
							0x8086u,									    \
							0x1c22u,										\
							0x00001F03,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R) Ser. 6 Chipset Family SMBus - 1c22",  \
						}

// 7 Series Chipset Family SMBus controller (D31:F3:R20)
// Boards:
//	- EXPRESS-IBE2
//	- Toucan-QM77
#define SEMA_S7_Info	{													\
							0x8086u,									    \
							0x1e22u,										\
							0x00001F03,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R) Ser. 7 Chipset Family SMBus - 1e22",  \
						}

// Lynx Point SMBus controller (D31:F3:R20)
// Boards:
//	- EXPRESS-HL / Express-BD7
#define SEMA_HL_Info	{													\
							0x8086u,									    \
							0x8c22u,										\
							0x00001F03,										\
							RESSOURTYPE_PORT,								\
							0,												\
							"Intel(R)Lynx Point SMBus Controller - 8c22",   \
						}

// Lynx Point SMBus controller (D31:F3:R20)
// Boards:
//	- cEXPRESS-HL
#define SEMA_CHL_Info	{													\
							0x8086u,									    \
							0x9c22u,										\
							0x00001F03,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R)Lynx Point SMBus Controller - 9c22",   \
						}

// Bay Trail SMBus controller (D31:F3:R20)
// Boards:
//	- cEXPRESS-BT
#define SEMA_BT_Info	{													\
							0x8086u,									    \
							0x0F12u,										\
							0x00001F03,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R)Bay Trail SMBus Controller - 0F12",    \
						}

// Board Well SMBus controller (D31:F3:R20)
// Boards:
//	- cEXPRESS-HL
#define SEMA_BL_Info	{													\
							0x8086u,									    \
							0x9CA2u,										\
							0x00001F03,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R)Board Well SMBus Controller - 9CA2",   \
						}	

// Braswell SMBus controller (D31:F3:R20)
// Boards:
//	- cEXPRESS-BW
#define SEMA_BW_Info	{													\
							0x8086u,									    \
							0x2292u,										\
							0x00001F03,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R)Braswell SMBus Controller - 2292",     \
						}

// Skylake SMBus controller (D31:F4:R20)
// Boards:
//	- EXPRESS-SL
#define SEMA_SL_Info	{													\
							0x8086u,									    \
							0xA123u,										\
							0x00001F04,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R)Skylake SMBus Controller - A123",      \
						}
//	- cEXPRESS-SL
#define SEMA_cSL_Info	{													\
							0x8086u,									    \
							0x9D23u,										\
							0x00001F04,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R)cSkylake SMBus Controller - 9D23",     \
}


// Broxton SMBus controller (D31:F1:R20)
// Boards:
//	- cEXPRESS-AL/EXPRESS-AL
#define SEMA_AL_Info	{													\
							0x8086u,									    \
							0x5AD4u,										\
							0x00001F01,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R)Broxton SMBus Controller - 5AD4",      \
						}
// Cannon Lake and Coffee Lake SMBus controller (D31:F4)
// Boards:
//	- EXPRESS-CF
#define SEMA_CF_Info	{													\
							0x8086u,									    \
							0xA323u,										\
							0x00001F04,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R)Cannon/Coffee Lake SMBus Controller - A323",\
						}

// Cormorant Lake / Denverton SMBus controller (D31:F4)
// Boards:
//	- EXPRESS-DN7
#define SEMA_DN7_Info	{													\
							0x8086u,									    \
							0x19DFu,										\
							0x00001F04,										\
							RESSOURTYPE_PORT,								\
							0,												\
							"Intel(R)Denverton SMBus Controller - 19DF",    \
						}

// Whiskey Lake SMBus controller (D31:F4)
// Boards:
//	- cEXPRESS-WL
#define SEMA_WL_Info	{													\
							0x8086u,									    \
							0x9DA3u,										\
							0x00001F04,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R) Whiskey Lake SMBus Controller - 9DA3",    \
						}

// Comet Lake SMBus controller (D31:F4)
// Boards:
//	- 
#define SEMA_CL_Info	{													\
							0x8086u,									    \
							0x06A3u,										\
							0x00001F04,										\
							RESSOURTYPE_MMIO,								\
							0,												\
							"Intel(R) Comet Lake SMBus Controller - 9DA3",    \
						}


UINT8 IsSMBDeviceExist(WDFDEVICE pDevice);
UINT8 IsECDeviceExist(WDFDEVICE pDevice);

unsigned long ReadsupportedExistingDevicePortBase(WDFDEVICE pDevice);
unsigned long ReadsupportedExistingDeviceMMIOBase(WDFDEVICE pDevice);
unsigned long ReadsupportedExistingDevice4GDecoding(WDFDEVICE pDevice);
long pci_write(unsigned char bus, unsigned char device, unsigned char function, unsigned short reg, unsigned char len, long Value);
long pci_read(unsigned char bus, unsigned char device, unsigned char function, unsigned short reg, unsigned char len);