#pragma once

#include "Driver.h"


#define FALSE   0
#define TRUE    1

//Macro definitions
#define EC_SC_WR_CMD			(0x81)
#define EC_SC_RD_CMD			(0x80)

#define EC_SC_IBF			(0x02)
#define EC_SC_OBF			(0x01)

#define EC_SC_IBFOROBF		(0x03)
#define EC_REGION_1   ((0x62 << 8) | 0x66)
#define EC_REGION_2   ((0x68 << 8) | 0x6C)

unsigned short Ec_Region;

NTSTATUS EC_Read_Transfer(WDFDEVICE pDevice, PEC_COMMAND pCmd, BYTE* Buffer, UINT32 nDataOut, ULONG* pulRet);
NTSTATUS EC_Write_Transfer(WDFDEVICE pDevice, PEC_COMMAND pCmd, BYTE* Buffer, UINT32 nDataOut, ULONG* pulRet);
int Adl_PortRead(unsigned short in_ucPortAddr, unsigned char *in_pucData);
int Adl_PortWrite(unsigned short in_ucPortAddr, unsigned char in_ucData);
int Adl_EcFirmwareVersion(char *, unsigned short);