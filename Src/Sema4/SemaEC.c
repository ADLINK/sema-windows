#include "SemaEC.h"
#include "SemaEC.tmh"


#define IBF 1
#define OBF 0


int ReadEc_(UINT16 RegionIndex, UINT16 offset, unsigned char *data)
{
	volatile int retry, limit = 100;
	volatile unsigned char EC_SC_D, EC_DA_D;
	
	PUCHAR EC_SC = (PUCHAR)(RegionIndex & 0xFF);
	PUCHAR EC_DATA = (PUCHAR)((RegionIndex >> 8) & 0xFF);

	for (retry = 0; retry < limit; retry++)
	{
		EC_SC_D = READ_PORT_UCHAR(EC_SC);
		if (!!(EC_SC_D & (1 << IBF)) == 0)
		{
			break;
		}
		KeStallExecutionProcessor(10);
	}

	if (limit == retry)
	{
		return -1;
	}

	WRITE_PORT_UCHAR(EC_SC, EC_SC_RD_CMD);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "1st IBF check\n");

	for (retry = 0; retry < limit; retry++)
	{
		EC_SC_D = READ_PORT_UCHAR(EC_SC);
		if (!!(EC_SC_D & (1 << IBF)) == 0)
		{
			break;
		}
		KeStallExecutionProcessor(10);
	}

	if (limit == retry)
	{
		return -1;
	}

	WRITE_PORT_UCHAR(EC_DATA, (UCHAR)offset);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "After address written\n");

	for (retry = 0; retry < limit; retry++)
	{
		EC_SC_D = READ_PORT_UCHAR(EC_SC);
		if (!!(EC_SC_D & (1 << IBF)) == 0)
		{
			break;
		}
		KeStallExecutionProcessor(10);
	}

	if (limit == retry)
	{
		return -1;
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "Before OBF check\n");

	for (retry = 0; retry < limit; retry++)
	{
		EC_SC_D = READ_PORT_UCHAR(EC_SC);
		if (!!(EC_SC_D & (1 << OBF)) == 1)
		{
			break;
		}
		KeStallExecutionProcessor(10);
	}

	if (limit == retry)
	{
		return -1;
	}

	EC_DA_D = READ_PORT_UCHAR(EC_DATA);

	*data = EC_DA_D;

	return 0;
}

int ReadEc(UINT16 RegionIndex, UINT16 offset, unsigned char *data)
{
	volatile int i, result = -1, retry = 100;
	for (i = 0; i < retry; i++)
	{
		if ((result = ReadEc_(RegionIndex, offset, data)) == 0)
		{
			break;
		}
		KeStallExecutionProcessor(10);
	}
	return result;
}

int WriteEc_(UINT16 RegionIndex, UINT16 offset, unsigned char data)
{
	volatile int retry, limit = 100;
	volatile unsigned char EC_SC_D;

	PUCHAR EC_SC = (PUCHAR)(RegionIndex & 0xFF);
	PUCHAR EC_DATA = (PUCHAR)((RegionIndex >> 8) & 0xFF);

	for (retry = 0; retry < limit; retry++)
	{
		EC_SC_D = READ_PORT_UCHAR(EC_SC);
		if (!!(EC_SC_D & (1 << IBF)) == 0)
		{
			break;
		}
		KeStallExecutionProcessor(10);
	}

	if (limit == retry)
	{
		return -1;
	}

	WRITE_PORT_UCHAR(EC_SC, EC_SC_WR_CMD);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "1st IBF check\n");

	for (retry = 0; retry < limit; retry++)
	{
		EC_SC_D = READ_PORT_UCHAR(EC_SC);
		if (!!(EC_SC_D & (1 << IBF)) == 0)
		{
			break;
		}
		KeStallExecutionProcessor(10);
	}

	if (limit == retry)
	{
		return -1;
	}

	WRITE_PORT_UCHAR(EC_DATA, (UCHAR)offset);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "After address written\n");

	for (retry = 0; retry < limit; retry++)
	{
		EC_SC_D = READ_PORT_UCHAR(EC_SC);
		if (!!(EC_SC_D & (1 << IBF)) == 0)
		{
			break;
		}
		KeStallExecutionProcessor(10);
	}

	if (limit == retry)
	{
		return -1;
	}

	WRITE_PORT_UCHAR(EC_DATA, data);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "After address written\n");

	for (retry = 0; retry < limit; retry++)
	{
		EC_SC_D = READ_PORT_UCHAR(EC_SC);
		if (!!(EC_SC_D & (1 << IBF)) == 0)
		{
			break;
		}
		KeStallExecutionProcessor(10);
	}

	return 0;
}


int WriteEc(UINT16 RegionIndex, UINT16 offset, unsigned char data)
{
	volatile int i, result = -1, retry = 100;
	for (i = 0; i < retry; i++)
	{
		if ((result = WriteEc_(RegionIndex, offset, data)) == 0)
		{
			break;
		}
		KeStallExecutionProcessor(10);
	}
	return result;
}


NTSTATUS EC_Read_Transfer(WDFDEVICE pDevice, PEC_COMMAND pCmd, BYTE* Buffer, UINT32 nDataOut, ULONG* pulRet)
{
	UNREFERENCED_PARAMETER(pDevice);
	volatile unsigned short i;

	if (Buffer == NULL)
	{
		return STATUS_INVALID_ADDRESS;
	}

	if (nDataOut < pCmd->bLength)
	{
		return STATUS_INVALID_BUFFER_SIZE;
	}

	for (i = 0; i < pCmd->bLength; i++)
	{
		if (ReadEc(pCmd->RegionIndex, pCmd->bAddr + i, &(Buffer[i])) != 0)
		{
			*pulRet = 0;
			return STATUS_ABANDONED;
		}
	}

	*pulRet = i;

	return STATUS_SUCCESS;
}



NTSTATUS EC_Write_Transfer(WDFDEVICE pDevice, PEC_COMMAND pCmd, BYTE* Buffer, UINT32 nDataOut, ULONG* pulRet)
{
	UNREFERENCED_PARAMETER(pDevice);
	volatile unsigned short i;

	if (Buffer == NULL)
	{
		return STATUS_INVALID_ADDRESS;
	}

	if (nDataOut < pCmd->bLength)
	{
		return STATUS_INVALID_BUFFER_SIZE;
	}

	for (i = 0; i < pCmd->bLength; i++)
	{
		if (WriteEc(pCmd->RegionIndex, pCmd->bAddr + i, Buffer[i]) != 0)
		{
			*pulRet = 0;
			return STATUS_ABANDONED;
		}
	}

	*pulRet = i;
	return STATUS_SUCCESS;
}

int Adl_PortRead(unsigned short in_ucPortAddr, unsigned char *in_pucData)
{
	PUCHAR PortAddr = (PUCHAR)(in_ucPortAddr & 0xFF);
	UCHAR ucData = 0;
	ucData = READ_PORT_UCHAR(PortAddr);
	*in_pucData = ucData;
	KeStallExecutionProcessor(10);

	return 0;
}

int Adl_PortWrite(unsigned short in_ucPortAddr, unsigned char in_ucData)
{
	PUCHAR PortAddr = (PUCHAR)(in_ucPortAddr & 0xFF);
	WRITE_PORT_UCHAR(PortAddr, in_ucData);
	KeStallExecutionProcessor(10);

	return 0;
}

int Adl_EcFirmwareVersion(char *in_cpBMCVersion, unsigned short in_usBuffSize)
{
	unsigned short iIndex = 0;
	int iRetVal = 0;
	unsigned char usAddr = 0;
	char cBuff[64] = { 0 };
	unsigned short uiLength = 0;

	/* Get Module name */
	usAddr = 0xA0;
	uiLength = 16;
	memset(cBuff, 0, sizeof(cBuff));
	
	for (iIndex = 0; iIndex < uiLength; iIndex++)
	{
		if (ReadEc(EC_REGION_1, usAddr + iIndex, (unsigned char*)&(cBuff[iIndex])) != 0)
		{
			return STATUS_ABANDONED;
		}
	}
	
	strncpy_s(in_cpBMCVersion, in_usBuffSize, cBuff, strlen(cBuff) + 1);
	strcat_s(in_cpBMCVersion, in_usBuffSize, " ");
	
	/* Get Version and company name */
	usAddr = 0xF0;
	uiLength = 16;
	memset(cBuff, 0, sizeof(cBuff));
	
	for (iIndex = 0; iIndex < uiLength; iIndex++)
	{
		if (ReadEc(EC_REGION_1, usAddr + iIndex, (unsigned char*)&(cBuff[iIndex])) != 0)
		{
			return -1;
		}
	}
	
	strcat_s(in_cpBMCVersion, in_usBuffSize, cBuff);
	strcat_s(in_cpBMCVersion, in_usBuffSize, " ");
		
	return iRetVal;
}
