#include "stdafx.h"
#include "ECFunct.h"

#define EEPROM_USER_SIZE 1024
#define EEPROM_SCRE_SIZE 2048
#define EEPROM_ODM_SIZE  1024
#define EEPROM_FIXED_SIZE  0X200

#define BLOCK_SIZE 4

#define MAX_BYTES_PER_TRXN 0x80
#define SMC_FLASH_ALIGNMENT 0x04

EERROR CECFunct::StorageCap(uint8_t Region, uint32_t *nSize, uint32_t* nBlk)
{
	if (FindCap(SEMA_CAP_1KB_FLASH) == 1)
	{
		if (Region == 1)
		{
			*nSize = EEPROM_USER_SIZE;
			*nBlk = BLOCK_SIZE;
		}
		else if (Region == 2)
		{
			*nSize = EEPROM_SCRE_SIZE;
			*nBlk = BLOCK_SIZE;
		}
		else if (Region == 3)
		{
			*nSize = EEPROM_ODM_SIZE;
			*nBlk = BLOCK_SIZE;
		}
		else if (Region == 5)	//Fixed parameter
		{
			*nSize = EEPROM_FIXED_SIZE;
			*nBlk = BLOCK_SIZE;
		}
		else
		{
			return EAPI_STATUS_UNSUPPORTED;
		}
		return EAPI_STATUS_SUCCESS;
	}
	return EAPI_STATUS_UNSUPPORTED;
}

EERROR CECFunct::ReadMemL(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize)
{
	uint32_t nRet = nSize;
	unsigned char Status, i;
	
	uint8_t pDataIn[] = { 0x2, 0x1, (uint8_t)nSize, (uint8_t)(Region + 1), (uint8_t)(nAdr >> 0x08),\
							(uint8_t)(nAdr & 0x00FF) };

	for (i = 0; i < 10; i++)
	{
		if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, &Status, 1)\
			== EAPI_STATUS_SUCCESS)
		{
			if ((Status & 0x1) == 0)
				break;
		}
		if (i == 9)
			return EAPI_STATUS_TIMEOUT;
	}

	if (m_clsECTrans.ECWrite(EC_WO_ADDR_IIC_CMD_START, EC_REGION_2, pDataIn, 6) \
		== EAPI_STATUS_SUCCESS)
	{
		pDataIn[0] = 4;
		if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, pDataIn, \
			1) == EAPI_STATUS_SUCCESS)
		{
			for (i = 0; i < 10; i++)
			{
				if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, pDataIn, \
					1) == EAPI_STATUS_SUCCESS)
				{
					if (!!(pDataIn[0] & 0x04) == 0 && ((pDataIn[0] & 0x1) != 1) && (!!(pDataIn[0] & 0x8) != 1))
					{
						Sleep(50);
						if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BUFFER, EC_REGION_2, pData,\
							nSize) == EAPI_STATUS_SUCCESS)
						{
							return EAPI_STATUS_SUCCESS;
						}
					}							
				}
				Sleep(50);
			}
		}
	}

	return EAPI_STATUS_ERROR;
}


EERROR CECFunct::WriteMemL(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize)
{
	uint32_t nRet = nSize;
	unsigned char Status, i;
	uint8_t pDataIn[] = { 0x2, 0x2, (uint8_t)nSize, (uint8_t)(Region + 1), (uint8_t)(nAdr >> 0x08), \
							(uint8_t)(nAdr & 0x00FF) };

	for (i = 0; i < 10; i++)
	{
		if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, &Status, 1) \
			== EAPI_STATUS_SUCCESS)
		{
			if ((Status & 0x1) == 0)
				break;
		}
		if (i == 9)
			return EAPI_STATUS_TIMEOUT;
	}

	if (m_clsECTrans.ECWrite(EC_WO_ADDR_IIC_CMD_START, EC_REGION_2, pDataIn, 6) \
		== EAPI_STATUS_SUCCESS)
	{
		if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BUFFER, EC_REGION_2, pData, nSize) \
			== EAPI_STATUS_SUCCESS)
		{
			pDataIn[0] = 4;
			if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, pDataIn, 1)\
				== EAPI_STATUS_SUCCESS)
			{
				for (i = 0; i < 20; i++)
				{
					if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, pDataIn, 1) \
						== EAPI_STATUS_SUCCESS)
					{
						if (!!(pDataIn[0] & 0x04) == 0 && ((pDataIn[0] & 0x1) != 1) && (!!(pDataIn[0] & 0x8) != 1))
						{
							return EAPI_STATUS_SUCCESS;
						}
					}
					Sleep(300);
				}
			}
		}
	}
	return EAPI_STATUS_ERROR;
}




EERROR CECFunct::WriteMem(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize)
{
	EERROR Status;
	int end = nAdr + nSize, start = nAdr, nSizeL;

	if (Region == EAPI_ID_STORAGE_ODM)
	{
		nAdr = nAdr + 0x0C00;
		end = nAdr + 0x10;
		start = nAdr;
	}

	if (Region == EAPI_ID_STORAGE_SCR)
	{
		nAdr = nAdr + 0x6000;
		end = nAdr + nSize;
		start = nAdr;
	}

	while (start < end)
	{
		nSizeL = ((end - start) < MAX_BYTES_PER_TRXN) ? (end - start) : MAX_BYTES_PER_TRXN;
		if ((Status = WriteMemL(Region, start, (((uint8_t*)pData) + start - nAdr),\
			nSizeL)) != EAPI_STATUS_SUCCESS)
		{
			break;
		}
		start += nSizeL;
	}

	return Status;
}


EERROR CECFunct::ReadMem(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize)
{
	EERROR Status;
	int end = nAdr + nSize, start = nAdr, nSizeL;
	
	ZeroMemory(pData, nSize);

	if (Region == EAPI_ID_STORAGE_ODM)
	{
		nAdr = nAdr + 0x0C00;
		end = nAdr + 0x10;
		start = nAdr;
	}

	if (Region == EAPI_ID_STORAGE_SCR)
	{
		nAdr = nAdr + 0x6000;
		end = nAdr + nSize;
		start = nAdr;
	}

	if (Region == EAPI_ID_STORAGE_FP)
	{
		nAdr = nAdr + 0x1800;
		end = nAdr + nSize;
		start = nAdr;
	}

	while (start < end)
	{
		nSizeL = ((end - start) < MAX_BYTES_PER_TRXN) ? (end - start) : MAX_BYTES_PER_TRXN;
		if ((Status = ReadMemL(Region, start, (((uint8_t*)pData) + start - nAdr), \
			nSizeL)) != EAPI_STATUS_SUCCESS)
		{
			break;
		}
		start += nSizeL;
	}

	return Status;
}

EERROR CECFunct::ClearMem(uint8_t Region)
{
	return EAPI_STATUS_UNSUPPORTED;
}


EERROR CECFunct::GetManufData(uint32_t nDataInfo, uint8_t* pData, uint32_t nLen)
{
	int i, j;
	uint8_t Status;

	ZeroMemory(pData, nLen);

	if (nDataInfo > SEMA_MANU_DATA_MAX)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	for (i = 0; i < 10; i++)
	{
		if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, &Status, 1)\
			== EAPI_STATUS_SUCCESS)
		{
			if ((Status & 0x1) == 0)
				break;
		}
		if (i == 9)
			return EAPI_STATUS_TIMEOUT;
	}

	nDataInfo += 1;

	for (i = 0; i < 10; i++)
	{
		uint8_t pDataIn[] = { 0x2, 0x1, 16, 0x4, 0xC, (uint8_t)(nDataInfo * 16) };

		if (m_clsECTrans.ECWrite(EC_WO_ADDR_IIC_CMD_START, EC_REGION_2, pDataIn, 6)\
			== EAPI_STATUS_SUCCESS)
		{
			pDataIn[0] = 4;
			if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, pDataIn, 1)\
				== EAPI_STATUS_SUCCESS)
			{
				for (j = 0; j < 100; j++)
				{
					if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, pDataIn, 1)\
						== EAPI_STATUS_SUCCESS)
					{
						if (!!(pDataIn[0] & 0x04) == 0 && ((pDataIn[0] & 0x1) != 1) && (!!(pDataIn[0] & 0x8) != 1))
						{
							m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BUFFER, EC_REGION_2, pData, 16);
							return EAPI_STATUS_SUCCESS;
						}
					}
					Sleep(50);
				}
			}
		}
	}

	return EAPI_STATUS_ERROR;
}


EERROR CECFunct::LockMem(uint8_t Region)
{
	int i, j;
	uint8_t pDataIn_data[3] = { 0xAD, 0xEC, 0x0 };
	uint8_t Status;

	if (Region == EAPI_ID_STORAGE_SCR)
	{
		pDataIn_data[1] = 0xAC;
	}
	else if (Region == EAPI_ID_STORAGE_ODM)
	{
		pDataIn_data[1] = 0xEC;
	}
	else
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	for (i = 0; i < 10; i++)
	{
		if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, &Status, 1)\
			== EAPI_STATUS_SUCCESS)
		{
			if ((Status & 0x1) == 0)
				break;
		}
		if (i == 9)
			return EAPI_STATUS_TIMEOUT;
	}

	for (i = 0; i < 10; i++)
	{
		uint8_t pDataIn_Cmd[10] = { 0x2, 0x4, 3, (uint8_t)(Region + 1), 0, 0 };
		if (m_clsECTrans.ECWrite(EC_WO_ADDR_IIC_CMD_START, EC_REGION_2, pDataIn_Cmd, 6) \
			== EAPI_STATUS_SUCCESS)
		{
			if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BUFFER, EC_REGION_2, pDataIn_data, 3)\
				== EAPI_STATUS_SUCCESS)
			{
				pDataIn_Cmd[0] = 4;
				if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, pDataIn_Cmd, 1) \
					== EAPI_STATUS_SUCCESS)
				{
					for (j = 0; j < 10; j++)
					{
						if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, pDataIn_Cmd, 1)\
							== EAPI_STATUS_SUCCESS)
						{
							if (!!(pDataIn_Cmd[0] & 0x04) == 0 && ((pDataIn_Cmd[0] & 0x1) != 1) && (!!(pDataIn_Cmd[0] & 0x8) != 1))
							{
								return EAPI_STATUS_SUCCESS;
							}
						}
						Sleep(50);
					}
				}
			}
		}
	}

	return EAPI_STATUS_ERROR;
}

EERROR CECFunct::UnlockMem(uint8_t Region, uint8_t Permission, char* passcode)
{
	int i, j;

	uint8_t pDataIn_data[16] = { 0xAD, 0xEC, 0x7 }, Status;

	if (Region == EAPI_ID_STORAGE_SCR)
	{
		pDataIn_data[1] = 0xAC;
		if (Permission == 1)
			pDataIn_data[2] = 0x4;
		else if (Permission == 2)
			pDataIn_data[2] = 0x6;
		else
			return EAPI_STATUS_UNSUPPORTED;

		if (!((_stricmp(passcode, "ADAC") == 0) || \
			(_stricmp(passcode, "0xADAC") == 0) || \
			(_stricmp(passcode, "0xAD0xAC") == 0)))
		{
			return EAPI_STATUS_WRITE_ERROR;
		}
	}
	else if (Region == EAPI_ID_STORAGE_ODM)
	{
		if (Permission != 2)
			return EAPI_STATUS_UNSUPPORTED;
		if (!((_stricmp(passcode, "ADEC") == 0) || \
			(_stricmp(passcode, "0xADEC") == 0) || \
			(_stricmp(passcode, "0xAD0xEC") == 0)))
		{
			return EAPI_STATUS_WRITE_ERROR;
		}
	}
	else
	{
		return EAPI_STATUS_UNSUPPORTED;
	}
	
	for (i = 0; i < 10; i++)
	{
		if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, &Status, 1)\
			== EAPI_STATUS_SUCCESS)
		{
			if ((Status & 0x1) == 0)
				break;
		}
		if (i == 9)
			return EAPI_STATUS_TIMEOUT;
	}

	for (i = 0; i < 10; i++)
	{
		uint8_t pDataIn_[] = { 0x2, 0x4, 3, (uint8_t)(Region + 1), 0, 0 };
		if (m_clsECTrans.ECWrite(EC_WO_ADDR_IIC_CMD_START, EC_REGION_2, pDataIn_, 6) \
			== EAPI_STATUS_SUCCESS)
		{
			if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BUFFER, EC_REGION_2, pDataIn_data, 3)\
				== EAPI_STATUS_SUCCESS)
			{
				pDataIn_[0] = 4;
				if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, pDataIn_, 1)\
					== EAPI_STATUS_SUCCESS)
				{
					for (j = 0; j < 10; j++)
					{
						if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, pDataIn_, 1)\
							== EAPI_STATUS_SUCCESS)
						{
							if (!!(pDataIn_[0] & 0x04) == 0 && ((pDataIn_[0] & 0x1) != 1) && (!!(pDataIn_[0] & 0x8) != 1))
							{
								return EAPI_STATUS_SUCCESS;
							}
						}
						Sleep(50);
					}
				}
			}
		}
	}

	return EAPI_STATUS_ERROR;
}


EERROR CECFunct::SetManufData(uint32_t nDataInfo, uint8_t* pData, uint32_t nLen)
{
	uint8_t Status;
	uint8_t data_[18] = { 0 };
	int i, j;

	if (nDataInfo > SEMA_MANU_DATA_MAX)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	memcpy(data_, pData, nLen);

	for (i = 0; i < 10; i++)
	{
		if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, &Status, 1)\
			== EAPI_STATUS_SUCCESS)
		{
			if ((Status & 0x1) == 0)
				break;
		}
		if (i == 9)
			return EAPI_STATUS_TIMEOUT;
	}

	nDataInfo += 1;

	for (i = 0; i < 10; i++)
	{
		uint8_t pDataIn[] = { 0x2, 0x2, 16, 0x4, 0xC, (uint8_t)(nDataInfo * 16) };
		if (m_clsECTrans.ECWrite(EC_WO_ADDR_IIC_CMD_START, EC_REGION_2, pDataIn, 6) \
			== EAPI_STATUS_SUCCESS)
		{
			if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BUFFER, EC_REGION_2, data_, 16)\
				== EAPI_STATUS_SUCCESS)
			{
				pDataIn[0] = 4;
				if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, pDataIn, 1)\
					== EAPI_STATUS_SUCCESS)
				{
					for (j = 0; j < 100; j++)
					{
						if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, pDataIn, 1)\
							== EAPI_STATUS_SUCCESS)
						{
							if (!!(pDataIn[0] & 0x04) == 0 && ((pDataIn[0] & 0x1) != 1) && (!!(pDataIn[0] & 0x8) != 1))
							{
								return EAPI_STATUS_SUCCESS;
							}
						}
						Sleep(50);
					}
				}
			}
		}
	}
	return EAPI_STATUS_ERROR;
}

#pragma pack(1)

typedef struct {
	uint16_t Number;
	uint16_t Status;
	uint32_t PwrCycles;
	uint32_t Bootcount;
	uint32_t Time;
	uint32_t TotalOnTime;
	uint8_t Flags;
	uint8_t BIOS_Selected;
	signed char CPUtemp;
	signed char Boardtemp;
	uint8_t RestartEvent;
	uint8_t Reserved[7];
} ErrorLog;


typedef struct {
	uint16_t Number;
	uint8_t Flags;
	uint8_t RestartEvent;
	uint32_t PwrCycles;
	uint32_t Bootcount;
	uint32_t Time;
	uint16_t Status;
	signed char CPUtemp;
	signed char Boardtemp;
	uint32_t TotalOnTime;
	uint8_t BIOS_Selected;
} oldErrorLog;

EERROR CECFunct::GetErrorLog(uint32_t nPos, uint8_t* pData, uint32_t* pnDataSize, bool bSetPos)
{
	ErrorLog errorLog;
	oldErrorLog olderror;

	if (!bSetPos)
	{
		nPos = 0;
	}

	uint8_t pDataIn[] = { 0x2, 0x1, 0/*Don't used*/, 0x5, 0, (uint8_t)(nPos) };
	int i;
	if (m_clsECTrans.ECWrite(EC_WO_ADDR_IIC_CMD_START, EC_REGION_2, pDataIn, 6) \
		== EAPI_STATUS_SUCCESS)
	{

		pDataIn[0] = 4;
		if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, pDataIn, 1) != EAPI_STATUS_SUCCESS)
		{
			return EAPI_STATUS_ERROR;
		}

		for (i = 0; i < 1000; i++)
		{
			if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, pDataIn, 1)\
				== EAPI_STATUS_SUCCESS)
			{
				if (!!(pDataIn[0] & 0x04) == 0 && ((pDataIn[0] & 0x1) != 1))
				{
					break;
				}
			}
		}

		if (!!(pDataIn[0] & 0x8) == 1)
		{
			return EAPI_STATUS_ERROR;
		}

		if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BUFFER, EC_REGION_2, ((uint8_t *)&errorLog), *pnDataSize)\
			== EAPI_STATUS_SUCCESS)
		{

			olderror.Number = errorLog.Number;
			olderror.Flags = errorLog.Flags;
			olderror.RestartEvent = errorLog.RestartEvent;
			olderror.PwrCycles = errorLog.PwrCycles;
			olderror.Bootcount = errorLog.Bootcount;
			olderror.Time = errorLog.Time;
			olderror.Status = errorLog.Status;
			olderror.CPUtemp = errorLog.CPUtemp;
			olderror.Boardtemp = errorLog.Boardtemp;
			olderror.TotalOnTime = errorLog.TotalOnTime;
			olderror.BIOS_Selected = errorLog.BIOS_Selected;

			memcpy((void*)pData, (void*)&olderror, sizeof(oldErrorLog));
			
			return EAPI_STATUS_SUCCESS;
		}
	}

	return EAPI_STATUS_ERROR;
}


EERROR CECFunct::GetErrorNumDescription(uint32_t ErrorNum, uint8_t* ErrorNumDes)
{
	uint32_t count, pnDataSize = sizeof(oldErrorLog);
	unsigned char buf[32] = { 0 };
	uint32_t ErrorNumRec;
	uint32_t DataSize = 0x20;
	oldErrorLog errorLog;

	for (count = 0; count < 32; count++)
	{
		if (GetErrorLog(count, (uint8_t*)&errorLog, &pnDataSize, 1) == EAPI_STATUS_SUCCESS)
		{
			ErrorNumRec = errorLog.Number;
			if (ErrorNum == ErrorNumRec)
			{
				char Errorcode = errorLog.Flags & 0x0f;
				return GetErrorDesc(Errorcode, ErrorNumDes, &DataSize);
			}
		}
	}

	return EAPI_STATUS_ERROR;
}
