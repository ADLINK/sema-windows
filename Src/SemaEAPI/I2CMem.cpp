#include "stdafx.h"
#include "I2cFunct.h"

#define MAX_BYTES_PER_TRXN 0x20
#define SMC_FLASH_ALIGNMENT 0x04

static void nsleep(unsigned long msecs)
{
	LARGE_INTEGER frequency;
	LARGE_INTEGER t1, t2;
	double elapsedTime;
	QueryPerformanceFrequency(&frequency);
	elapsedTime = 0x00;
	QueryPerformanceCounter(&t1);
	while (elapsedTime < msecs)
	{
		QueryPerformanceCounter(&t2);
		elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
	}

}

#define	SleepMS(msecs)	 nsleep(msecs)

#define EEPROM_SIZE 1024
#define _EEPROM_SIZE 512
#define BLOCK_SIZE 4

EERROR CI2CFunct::StorageCap(uint8_t Region, uint32_t *nSize, uint32_t* nBlk)
{

	if (Region != EAPI_ID_STORAGE_STD)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (FindCap(SEMA_CAP_1KB_FLASH) == 1)
	{
		*nSize = EEPROM_SIZE;
	}
	else
	{
		*nSize = _EEPROM_SIZE;
	}

	*nBlk = BLOCK_SIZE;
	return EAPI_STATUS_SUCCESS;
}

EERROR CI2CFunct::ReadMemL(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize)
{
#if 0
	uint32_t nRet = nSize;
	uint8_t pDataIn[] = { (uint8_t)(nAdr >> 0x08), (uint8_t)nAdr, (uint8_t)nSize };

	if (m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_SET_ADDRESS, pDataIn, 0x03) ==\
									EAPI_STATUS_SUCCESS) {
		ZeroMemory(pData, nSize);
		if (m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_READ_DATA, NULL, 0x00,\
									pData, nRet) == EAPI_STATUS_SUCCESS) {
			return ((nRet == nSize) ? EAPI_STATUS_SUCCESS : EAPI_STATUS_MORE_DATA);
		}
	}
#endif
	return EAPI_STATUS_ERROR;
}

EERROR CI2CFunct::WriteMemL(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize)
{
#if 0
	uint32_t nRet = nSize;
	uint8_t pDataIn[] = { (uint8_t)(nAdr >> 0x08), (uint8_t)(nAdr & 0x00FF), (uint8_t)nSize };

	if (m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_SET_ADDRESS, pDataIn, \
								0x03) == EAPI_STATUS_SUCCESS) {
		SleepMS(3);
		if (m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_WRITE_DATA, pData,\
			nSize) == EAPI_STATUS_SUCCESS)		{
			SleepMS(30);
			return EAPI_STATUS_SUCCESS;
		}
	}
#endif
	return EAPI_STATUS_ERROR;
}

EERROR CI2CFunct::WriteMem(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize)
{
	EERROR Status;
	int end = nAdr + nSize, start = nAdr, nSizeL;

	if (Region != EAPI_ID_STORAGE_STD)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	while (start < end)
	{
		nSizeL = ((end - start) < MAX_BYTES_PER_TRXN) ? (end - start) : MAX_BYTES_PER_TRXN;
		if ((Status = WriteMemL(Region + 1, start, (((uint8_t*)pData) + start - nAdr), nSizeL)) != EAPI_STATUS_SUCCESS)
		{
			break;
		}
		start += nSizeL;
	}

	return Status;
}

EERROR CI2CFunct::ReadMem(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize)
{
	EERROR Status;
	int end = nAdr + nSize, start = nAdr, nSizeL;

	ZeroMemory(pData, nSize);

	if (Region != EAPI_ID_STORAGE_STD)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	while (start < end)
	{
		nSizeL = ((end - start) < MAX_BYTES_PER_TRXN) ? (end - start) : MAX_BYTES_PER_TRXN;
		if ((Status = ReadMemL(Region + 1, start, (((uint8_t*)pData) + start - nAdr), \
			nSizeL)) != EAPI_STATUS_SUCCESS)
		{
			break;
		}
		start += nSizeL;
	}

	return Status;
}

EERROR CI2CFunct::ClearMem(uint8_t Region)
{
#if 0
	uint8_t pData[] = { 0x0AE, 0xCD };

	EERROR eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_CLEAR_DATA, pData, 0x02);
	Sleep(400);
	return eRet;
#endif
	return EAPI_STATUS_SUCCESS;
}


EERROR CI2CFunct::UnlockMem(uint8_t Region, uint8_t Permission, char* passcode)
{
#if 0
	uint8_t pData_[10] = { 0 };
	uint32_t nDataRet;

	if (Region != EAPI_ID_STORAGE_ODM)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (Permission != 2)
	{
		return EAPI_STATUS_SUCCESS;
	}

	if (!((_stricmp(passcode, "AECD") == 0) || \
		(_stricmp(passcode, "0xAECD") == 0) || \
		(_stricmp(passcode, "0xAE0xCD") == 0)))
	{
		return EAPI_STATUS_WRITE_ERROR;
	}

	pData_[0] = 0xAE;
	pData_[1] = 0xCD;
	pData_[2] = 0x17;
	pData_[3] = 0x51;

	if (m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_ENTERADMIN, pData_, 4) == EAPI_STATUS_SUCCESS)
	{
		if (m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_ENTERADMIN, \
			NULL, 0x00, pData_, nDataRet) == EAPI_STATUS_SUCCESS)
		{
			if (nDataRet == 1 && pData_[0] == 1)
			{
				return EAPI_STATUS_SUCCESS;
			}
		}
	}
#endif
	return EAPI_STATUS_ERROR;
}

EERROR CI2CFunct::LockMem(uint8_t Region)
{
#if 0
	uint8_t pData_[10] = { 0 };
	uint32_t nDataRet;

	if (Region != EAPI_ID_STORAGE_ODM)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	pData_[0] = 0xAE;
	pData_[1] = 0xCD;
	pData_[2] = 0x17;
	pData_[3] = 0x51;

	if (m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_ENTERADMIN, pData_, 4) == EAPI_STATUS_SUCCESS)
	{
		m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_ENTERADMIN, \
			NULL, 0x00, pData_, nDataRet);
	}

	if (m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_LEAVEADMIN, \
		NULL, 0x00, pData_, nDataRet) == EAPI_STATUS_SUCCESS)
	{
		m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_ENTERADMIN, \
			NULL, 0x00, pData_, nDataRet);
		if (nDataRet == 1 && pData_[0] == 0)
		{
			return EAPI_STATUS_SUCCESS;
		}
	}
#endif
	return EAPI_STATUS_ERROR;
}