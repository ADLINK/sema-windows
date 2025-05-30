#include "chipinfo.h"
#include "chipinfo.tmh"

#include "SemaSMB.h"
#include "SemaEC.h"


#define NUM_SMBUS_CHIPSETS	20

tSMBusInfo SMBUS_ChipsetInfo[NUM_SMBUS_CHIPSETS] =
{											//##
	SEMA_ICH10_Info,						//00
	SEMA_ICH9_Info,							//01
	SEMA_ICH7_Info,							//02
	SEMA_PCH_Info,							//03
	SEMA_S6_Info,							//04
	SEMA_S7_Info,							//05
	SEMA_FCH_Info,							//06
	SEMA_HL_Info,							//07
	SEMA_CHL_Info,							//08
	SEMA_BT_Info,							//09
	SEMA_BL_Info,							//10
	SEMA_BE_Info,							//11
	SEMA_SL_Info,							//12
	SEMA_BW_Info,							//13
	SEMA_cSL_Info,							//14
	SEMA_AL_Info,							//15
	SEMA_CF_Info,							//16
	SEMA_DN7_Info,							//17
	SEMA_WL_Info,							//18
	SEMA_CL_Info
};

long pci_read(unsigned char bus, unsigned char device, unsigned char function, unsigned short reg, unsigned char len)
{
#define INDEX_PCI 0xCF8
	long value = 0;

	WRITE_PORT_ULONG((PULONG)(INDEX_PCI), (ULONG)((8 << 28 | bus << 16 | device << 11 | function << 8) + reg));

	switch (len)
	{
	case 1: value = (long)READ_PORT_UCHAR((PUCHAR)(0xCFC + (reg & 3))); break;
	case 2: value = (long)READ_PORT_USHORT((PUSHORT)(0xCFC + (reg & 2))); break;
	case 4: value = (long)READ_PORT_ULONG((PULONG)(0xCFC)); break;
	}

	return value;
}

long pci_write(unsigned char bus, unsigned char device, unsigned char function, unsigned short reg, unsigned char len, long Value)
{
#define INDEX_PCI 0xCF8

	WRITE_PORT_ULONG((PULONG)(INDEX_PCI), (ULONG)((8 << 28 | bus << 16 | device << 11 | function << 8) + reg));

	switch (len)
	{
	case 1: WRITE_PORT_UCHAR((PUCHAR)(0xCFC + (reg & 3)), Value & 0xFF); break;
	case 2: WRITE_PORT_USHORT((PUSHORT)(0xCFC + (reg & 2)), Value & 0xFFFF); break;
	case 4: WRITE_PORT_ULONG((PULONG)(0xCFC), Value); break;
	}

	return pci_read(bus, device, function, reg, len);
}

UINT8 IsSMBDeviceExist(WDFDEVICE pDevice)
{
	SEMA_DEVICE_EXT* pDevExt = NULL;

	volatile int node;

	unsigned int VenderDeviceID;
	unsigned char BusNo, DevNo, FuncNo;

	pDevExt = GetSEMADevExt(pDevice);

	for (node = 0; node < NUM_SMBUS_CHIPSETS; node++)
	{
		BusNo = SMBUS_ChipsetInfo[node].BuDeFu >> 16 & 0xFF;
		DevNo = SMBUS_ChipsetInfo[node].BuDeFu >> 8 & 0xFF;
		FuncNo = SMBUS_ChipsetInfo[node].BuDeFu & 0xFF;
		VenderDeviceID = pci_read(BusNo, DevNo, FuncNo, 0x0, 4);

		if (((VenderDeviceID & 0xFFFF) == SMBUS_ChipsetInfo[node].VID) && (((VenderDeviceID >> 16) & 0xFFFF) == SMBUS_ChipsetInfo[node].DID))
		{
			pDevExt->Chipset = SMBUS_ChipsetInfo[node];
			TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "SMBus Device Found\n");
			return 1;
		}
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "SMBus Device Not Found\n");
	return 0;
}

unsigned long ReadsupportedExistingDevicePortBase(WDFDEVICE pDevice)
{
	SEMA_DEVICE_EXT* pDevExt = NULL;
	unsigned char BusNo, DevNo, FuncNo;

	pDevExt = GetSEMADevExt(pDevice);

	BusNo = pDevExt->Chipset.BuDeFu >> 16 & 0xFF;
	DevNo = pDevExt->Chipset.BuDeFu >> 8 & 0xFF;
	FuncNo = pDevExt->Chipset.BuDeFu & 0xFF;

	return pci_read(BusNo, DevNo, FuncNo, 0x20, 4) & 0xFFFFFFF0;
}


unsigned long ReadsupportedExistingDeviceMMIOBase(WDFDEVICE pDevice)
{
	SEMA_DEVICE_EXT* pDevExt = NULL;
	unsigned char BusNo, DevNo, FuncNo;

	pDevExt = GetSEMADevExt(pDevice);

	BusNo = pDevExt->Chipset.BuDeFu >> 16 & 0xFF;
	DevNo = pDevExt->Chipset.BuDeFu >> 8 & 0xFF;
	FuncNo = pDevExt->Chipset.BuDeFu & 0xFF;

	return pci_read(BusNo, DevNo, FuncNo, 0x10, 4) & 0xFFFFFFF0;
}

unsigned long ReadsupportedExistingDevice4GDecoding(WDFDEVICE pDevice)
{
	SEMA_DEVICE_EXT* pDevExt = NULL;
	unsigned char BusNo, DevNo, FuncNo;

	pDevExt = GetSEMADevExt(pDevice);

	BusNo = pDevExt->Chipset.BuDeFu >> 16 & 0xFF;
	DevNo = pDevExt->Chipset.BuDeFu >> 8 & 0xFF;
	FuncNo = pDevExt->Chipset.BuDeFu & 0xFF;

	return pci_read(BusNo, DevNo, FuncNo, 0x14, 4) & 0xFFFFFFF0;
}
UINT8 IsECDeviceExist(WDFDEVICE pDevice)
{
	char buffer[16] = { 0 };
	char status;
	ULONG ulRet;
	EC_COMMAND  BufCmdInEC; NTSTATUS eRet;

	BufCmdInEC.bAddr = 0xF0;
	BufCmdInEC.RegionIndex = ((0x68 << 8) | 0x6C);
	BufCmdInEC.bLength = 16;
	if ((eRet = EC_Read_Transfer(pDevice, &BufCmdInEC, (BYTE*)buffer, (UINT32)16, &ulRet)) != STATUS_SUCCESS)
	{
		BufCmdInEC.RegionIndex = ((0x62 << 8) | 0x66);
		Ec_Region = BufCmdInEC.RegionIndex;
		if ((eRet = EC_Read_Transfer(pDevice, &BufCmdInEC, (BYTE*)buffer, (UINT32)16, &ulRet)) == STATUS_SUCCESS)
		{
			TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "EC_Region1 selected = %s\n", buffer);
			Ec_Region = BufCmdInEC.RegionIndex;
		}
	}
	else
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "EC_Region2 selected = %s\n",buffer);
		Ec_Region = BufCmdInEC.RegionIndex;
	}

	if (strstr(buffer, "ADLINK") != NULL)
	{
		return 1;
	}
	else
	{
		/*Reading the address 0xEA*/
		BufCmdInEC.bAddr = 0xEA;
		BufCmdInEC.RegionIndex = ((0x62 << 8) | 0x66);
		BufCmdInEC.bLength = 1;
		if ((eRet = EC_Read_Transfer(pDevice, &BufCmdInEC, (BYTE*)&status, (UINT32)1, &ulRet)) == STATUS_SUCCESS)
		{
			/*Checking the address 0xEA for value 0x34*/
			if (status != 0x34)
			{
				return 0;
			}
			else
			{
				return 1;
			}
		}
	}
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "EC Device Not Found\n");
	return 0;
}
