#include "stdafx.h"
#include "EApi.h"
#include "SemaFunct.h"
#include "ECFunct.h"
#include "SMBusFunct.h"

extern IBMCFunc* pBMCFunct;
extern Ccommon_bmc Ccmn;

uint32_t SemaEApiGetBIOSSource(uint8_t* bit0, uint8_t* bit1)
{
	EERROR eRet;
	uint8_t State;
	if (pBMCFunct->IsInitialiezed())
	{
		if (bit0 != NULL && bit1 != NULL)
		{
			if ((eRet = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
			{
				Ccmn.unlock();
				return eRet;
			}
			eRet = pBMCFunct->GetBIOSSource(&State);
			Ccmn.unlock();
			*bit0 = (State & 0x1);
			*bit1 = !!(State & 0x2);
			return eRet;
		}
		eRet = EAPI_STATUS_MORE_DATA;
	}
	return eRet;
}

uint32_t SemaEApiGetBIOSSourceSts(uint8_t* bit0, uint8_t* bit1, uint8_t* bit2)
{
	EERROR eRet;
	uint8_t State;
	bmc_type BoardType;

	if (pBMCFunct->IsInitialiezed())
	{
		BoardType = Ccmn.Init();
		if (BoardType == EC)
		{
			if (bit0 != NULL && bit1 != NULL)
			{
				if ((eRet = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
				{
					Ccmn.unlock();
					return eRet;
				}
				eRet = pBMCFunct->GetBIOSSource(&State);
				Ccmn.unlock();
				*bit0 = !!(State & (1 << 2));
				*bit1 = !!(State & (1 << 3));
				*bit2 = !!(State & (1 << 4));
				return eRet;
			}
			eRet = EAPI_STATUS_MORE_DATA;
		}
		else
			eRet = EAPI_STATUS_UNSUPPORTED;
	}
	return eRet;
}

uint32_t SemaEApiSetBIOSSource(uint8_t bit0, uint8_t bit1)
{
	EERROR eRet;
	uint8_t State;

	if ((bit0 | bit1) > 1)
	{
		return EAPI_STATUS_MORE_DATA;
	}

	State = (bit1 << 1) | bit0;

	if (pBMCFunct->IsInitialiezed())
	{
		if ((eRet = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
		{
			Ccmn.unlock();
			return eRet;
		}
		eRet = pBMCFunct->SetBIOSSource(State);
		Ccmn.unlock();
	}
	return eRet;
}

uint32_t SemaEApiBoardGetVoltageMonitor(uint32_t Id, uint32_t *pValue, char *pBuffer)
{
	char pdata[30] = { 0 };
	uint32_t eRet;
	uint32_t nSize = 20, i, j;

	float volt = 0.0;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pValue == NULL || pBuffer == NULL)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((eRet = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	if (((eRet = pBMCFunct->GetVolt(Id, &volt)) != EAPI_STATUS_SUCCESS) ||\
		((eRet = pBMCFunct->GetVoltDescEx(Id, pdata)) != EAPI_STATUS_SUCCESS))
	{
		goto out;
	}

	*pValue = (uint32_t)(volt * 1000);

	for (i = 0; pdata[i] != '\0'; i++)
	{
		if (pdata[i] == '=')
		{
			memcpy_s(pBuffer, 30, &pdata[i + 1], strlen(&pdata[i + 1]));
			break;
		}
	}

	for (i = 0; i < strlen(pBuffer); i++)
	{
		for (j = i; j < strlen(pBuffer); j++)
		{
			if (pBuffer[j] != ' ')
				break;
		}
		if (j == strlen(pBuffer))
		{
			pBuffer[i] = 0;
			break;
		}
	}
out:
	Ccmn.unlock();
	return eRet;
}

uint32_t SemaEApiBoardGetErrorLog(uint32_t  position, uint32_t *ErrorNumber, uint8_t  *Flags, uint8_t  *RestartEvent, uint32_t *PwrCycles, uint32_t *Bootcount, uint32_t *Time,\
	uint32_t *TotalOnTime, uint8_t  *BiosSel, uint16_t *Status, signed char *CPUtemp, signed char *Boardtemp)
{
	uint8_t pData[0x25] = { 0 };
	uint32_t DataSize = 0x20;
	uint32_t sts;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->FindCapExt(SEMA_CAP_ERROR_LOG) == 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((Boardtemp == NULL) || (CPUtemp == NULL) || (Status == NULL) || (Time == NULL)\
		|| (Bootcount == NULL) || (PwrCycles == NULL) || (RestartEvent == NULL) || (Flags == NULL) || \
		(ErrorNumber == NULL) || (TotalOnTime == NULL) || (BiosSel == NULL) || (position < 0x00))
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}


	if ((sts = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	if ((sts = pBMCFunct->GetErrorLog(position, pData, &DataSize, true)) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	*ErrorNumber = (uint32_t)pData[0] | ((uint32_t)pData[1] << 8);
	*Flags = pData[2];
	*RestartEvent = pData[3];
	*PwrCycles = (uint32_t)pData[4] | ((uint32_t)pData[5] << 8) | ((uint32_t)pData[6] << 16) |\
		((uint32_t)pData[7] << 24);
	*Bootcount = (uint32_t)pData[8] | ((uint32_t)pData[9] << 8) | ((uint32_t)pData[10] << 16) |\
		((uint32_t)pData[11] << 24);
	*Time = (uint32_t)pData[12] | ((uint32_t)pData[13] << 8) | ((uint32_t)pData[14] << 16) |\
		((uint32_t)pData[15] << 24);
	*Status = (uint16_t)pData[16] | ((uint16_t)pData[17] << 8);
	*CPUtemp = pData[18];
	*Boardtemp = pData[19];
	*TotalOnTime = pData[20] | (((uint32_t)pData[21]) << 8) | (((uint32_t)pData[22]) << 16) |\
		(((uint32_t)pData[23]) << 24);
	*BiosSel = pData[24];
out:
	Ccmn.unlock();
	return sts;
}

uint32_t SemaEApiBoardGetCurErrorLog(uint32_t *ErrorNumber, uint8_t  *Flags, uint8_t  *RestartEvent, uint32_t *PwrCycles, uint32_t *Bootcount, uint32_t *Time,\
	uint32_t *TotalOnTime, uint8_t  *BiosSel, uint16_t *Status, signed char *CPUtemp, signed char *Boardtemp)
{
	uint8_t pData[33] = { 0 };
	uint32_t DataSize = 0x20, sts;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->FindCapExt(SEMA_CAP_ERROR_LOG) == 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((Boardtemp == NULL) || (CPUtemp == NULL) || (Status == NULL) || (Time == NULL)\
		|| (Bootcount == NULL) || (PwrCycles == NULL) || (RestartEvent == NULL) || (Flags == NULL) || \
		(ErrorNumber == NULL) || (TotalOnTime == NULL) || (BiosSel == NULL))
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((sts = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	if ((sts = pBMCFunct->GetErrorLog(0/*Invalid*/, pData, &DataSize, false)) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	*ErrorNumber = (uint32_t)pData[0] | ((uint32_t)pData[1] << 8);
	*Flags = pData[2];
	*RestartEvent = pData[3];
	*PwrCycles = (uint32_t)pData[4] | ((uint32_t)pData[5] << 8) | ((uint32_t)pData[6] << 16) |\
		((uint32_t)pData[7] << 24);
	*Bootcount = (uint32_t)pData[8] | ((uint32_t)pData[9] << 8) | ((uint32_t)pData[10] << 16) |\
		((uint32_t)pData[11] << 24);
	*Time = (uint32_t)pData[12] | ((uint32_t)pData[13] << 8) | ((uint32_t)pData[14] << 16) |\
		((uint32_t)pData[15] << 24);
	*Status = (uint16_t)pData[16] | ((uint16_t)pData[17] << 8);
	*CPUtemp = pData[18];
	*Boardtemp = pData[19];
	*TotalOnTime = pData[20] | (((uint32_t)pData[21]) << 8) | (((uint32_t)pData[22]) << 16) | \
		(((uint32_t)pData[23]) << 24);
	*BiosSel = pData[24];
out:
	Ccmn.unlock();
	return sts;
}

uint32_t SemaEApiBoardGetErrorNumberDescription(uint32_t ErrorNum, uint8_t* pBuffer)
{
	uint32_t Status;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->FindCapExt(SEMA_CAP_ERROR_LOG) == 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((pBuffer == NULL) || (ErrorNum < 0x01))
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status = pBMCFunct->GetErrorNumDescription(ErrorNum, pBuffer);

out:
	Ccmn.unlock();
	return Status;
}

uint32_t SemaEApiBoardGetExceptionDescription(uint32_t ErrorCode, uint8_t* pBuffer)
{
	uint32_t pDataSize = 0x20, Status;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBuffer == NULL || ErrorCode > 0x22)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status = pBMCFunct->GetErrorDesc(ErrorCode, pBuffer, &pDataSize);

out:

	Ccmn.unlock();
	return Status;
}

uint32_t SemaEApiSmartFanSetTempSetpoints(uint32_t FanID, uint32_t Level1, uint32_t Level2, uint32_t Level3, uint32_t Level4)
{
	char buf[SEMA_FAN_LEVEL_MAX] = { 0 };
	uint32_t nSize = SEMA_FAN_ID_MAX, Status;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->GetSmartFanSupport(FanID) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	buf[0] = (uint8_t)Level1;
	buf[1] = (uint8_t)Level2;
	buf[2] = (uint8_t)Level3;
	buf[3] = (uint8_t)Level4;

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status = pBMCFunct->SetSmartFanTempPoints(FanID, buf, nSize);

out:
	Ccmn.unlock();
	return Status;
}

uint32_t SemaEApiSmartFanGetTempSetpoints(uint32_t FanId, uint32_t* pLevel1, uint32_t* pLevel2, uint32_t* pLevel3, uint32_t* pLevel4)
{
	uint32_t eRet;
	char buf[SEMA_FAN_LEVEL_MAX] = { 0 };
	uint32_t nSize = SEMA_FAN_ID_MAX;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->GetSmartFanSupport(FanId) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (pLevel1 == NULL || pLevel2 == NULL || pLevel3 == NULL || pLevel4 == NULL)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((eRet = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	if ((eRet = pBMCFunct->GetSmartFanTempPoints(FanId, buf, nSize)) == EAPI_STATUS_SUCCESS)
	{
		*pLevel1 = buf[0];
		*pLevel2 = buf[1];
		*pLevel3 = buf[2];
		*pLevel4 = buf[3];
	}
out:

	Ccmn.unlock();
	return eRet;
}

uint32_t SemaEApiSmartFanSetPWMSetpoints(uint32_t FanID, uint32_t Level1, uint32_t Level2, uint32_t Level3, uint32_t Level4)
{
	char buf[SEMA_FAN_LEVEL_MAX] = { 0 };
	uint32_t nSize = SEMA_FAN_ID_MAX, Status;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->GetSmartFanSupport(FanID) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	buf[0] = (uint8_t)Level1;
	buf[1] = (uint8_t)Level2;
	buf[2] = (uint8_t)Level3;
	buf[3] = (uint8_t)Level4;

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status = pBMCFunct->SetSmartFanPWMPoints(FanID, buf, nSize);
out:
	Ccmn.unlock();
	return Status;
}


uint32_t SemaEApiSmartFanGetPwmSetpoints(uint32_t FanId, uint32_t* pLevel1, uint32_t* pLevel2, uint32_t* pLevel3, uint32_t* pLevel4)
{
	uint32_t eRet;
	char buf[SEMA_FAN_LEVEL_MAX] = { 0 };
	uint32_t nSize = SEMA_FAN_ID_MAX;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->GetSmartFanSupport(FanId) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (pLevel1 == NULL || pLevel2 == NULL || pLevel3 == NULL || pLevel4 == NULL)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((eRet = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	if ((eRet = pBMCFunct->GetSmartFanPWMPoints(FanId, buf, nSize)) == EAPI_STATUS_SUCCESS)
	{
		*pLevel1 = buf[0];
		*pLevel2 = buf[1];
		*pLevel3 = buf[2];
		*pLevel4 = buf[3];
	}

out:
	Ccmn.unlock();
	return eRet;
}

uint32_t SemaEApiSmartFanGetTempSrc(uint32_t FanId, uint8_t* pTempSrc)
{
	uint32_t Status;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->GetSmartFanSupport(FanId) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (pTempSrc == NULL)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}
	
	Status = pBMCFunct->GetSmartFanTempSrc(FanId, pTempSrc);

out:
	Ccmn.unlock();
	return Status;
}

uint32_t SemaEApiSmartFanSetTempSrc(uint32_t FanId, uint8_t pTempSrc)
{
	uint32_t Status;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->GetSmartFanSupport(FanId) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status = pBMCFunct->SetSmartFanTempSrc(FanId, pTempSrc);

out:
	
	Ccmn.unlock();
	return Status;
}

uint32_t SemaEApiSmartFanGetMode(uint32_t FanId, uint8_t* pFanMode)
{
	uint32_t Status;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->GetSmartFanSupport(FanId) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (pFanMode == NULL)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status = pBMCFunct->GetSmartFanMode(FanId, pFanMode);

out:

	Ccmn.unlock();
	return Status;
}

uint32_t SemaEApiSmartFanSetMode(uint32_t FanId, uint8_t pFanMode)
{
	uint32_t Status;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->GetSmartFanSupport(FanId) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status = pBMCFunct->SetSmartFanMode(FanId, pFanMode);

out:

	Ccmn.unlock();
	return Status;
}

uint32_t SemaEApiI2CGetBusSts(uint32_t Id, uint32_t *pStatus)
{
	uint32_t MaxBlkSize, Status;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (EApiI2CGetBusCap(Id, &MaxBlkSize) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (pStatus == NULL)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status = pBMCFunct->I2CGetStatus(pStatus);

out:
	Ccmn.unlock();
	return Status;
}


uint32_t SemaEApiBoardRWI2C(uint16_t Address, uint8_t cmd, void* pBuffer, uint32_t nByteCnt, uint8_t OpType)
{
	uint32_t Status;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBuffer == NULL)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}
	
	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status = EAPI_STATUS_UNSUPPORTED;

	if (OpType == 1)
	{
		return pBMCFunct->I2CRead(Address, cmd, (uint8_t*)pBuffer, nByteCnt);
	}
	else
	{
		return pBMCFunct->I2CWrite(Address, cmd, (uint8_t*)pBuffer, nByteCnt);
	}

out:
	Ccmn.unlock();
	return Status;
}

uint32_t SemaEApiStorageClear(uint32_t Id)
{
	uint32_t Status;

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (Id != EAPI_ID_STORAGE_STD && Id != EAPI_ID_STORAGE_SCR && Id != EAPI_ID_STORAGE_ODM) {
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status = pBMCFunct->ClearMem(Id);

out:
	Ccmn.unlock();
	return Status;
}


uint32_t SemaEApiStorageLock(uint32_t Id)
{
	uint32_t Status;

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (Id != EAPI_ID_STORAGE_SCR && Id != EAPI_ID_STORAGE_ODM) {
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status =  pBMCFunct->LockMem(Id);

out:
	Ccmn.unlock();
	return Status;
}

uint32_t SemaEApiStorageUnlock(uint32_t Id, uint32_t Permission, char *Password)
{
	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (Id != EAPI_ID_STORAGE_SCR && Id != EAPI_ID_STORAGE_ODM) {
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	return pBMCFunct->UnlockMem(Id, Permission, Password);
}

uint32_t SemaEApiLibInitialize(void)
{
	return EApiLibInitialize();
}

uint32_t SemaEApiUnInitialize(void)
{
	return EApiLibUnInitialize();
}

uint32_t SemaEApiWDogGetLastValue(uint32_t *LastValue)
{
	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (LastValue == NULL) {
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if (pBMCFunct->FindCap(SEMA_CAP_WATCHDOG) == 0) {
		return EAPI_STATUS_UNSUPPORTED;
	}

	return pBMCFunct->GetLastWDogSetValue(LastValue);
}

uint32_t SemaEApiWDogGetCap(uint32_t *pMaxDelay, uint32_t *pMaxEventTimeout, uint32_t* Resetvalue)
{
	return EApiWDogGetCap(pMaxDelay, pMaxEventTimeout, Resetvalue);
}

uint32_t SemaEApiWDogStart(uint32_t delay, uint32_t EventTimeout, uint32_t ResetTime)
{
	return EApiWDogStart(delay, EventTimeout, ResetTime);
}

uint32_t SemaEApiWDogTrigger()
{
	return EApiWDogTrigger();
}

uint32_t SemaEApiWDogStop()
{
	return EApiWDogStop();
}

uint32_t SemaEApiPwrUpWDogStart(uint32_t ResetTime)
{
	return EApiPwrUpWDogStart(ResetTime);
}

uint32_t SemaEApiPwrUpWDogStop()
{
	return EApiPwrUpWDogStop();
}

uint32_t SemaEApiStorageAreaRead(uint32_t Id, uint32_t nOffset, void* pBuffer, uint32_t nBufLen, uint32_t nByteCnt)
{
	return EApiStorageAreaRead(Id, nOffset, pBuffer, nBufLen, nByteCnt);
}

uint32_t SemaEApiStorageAreaWrite(uint32_t Id, uint32_t nOffset, void* pBuffer, uint32_t  nByteCnt)
{
	return EApiStorageAreaWrite(Id, nOffset, pBuffer, nByteCnt);
}

uint32_t SemaEApiStorageCap(uint32_t Id, uint32_t* pStorageSize, uint32_t* pBlockLength)
{
	return EApiStorageCap(Id,  pStorageSize, pBlockLength);
}

uint32_t SemaEApiBoardGetValue(uint32_t Id, uint32_t* pData)
{
	return EApiBoardGetValue(Id,  pData);
}

uint32_t SemaEApiBoardGetStringA(uint32_t Id, uint8_t* pData, uint32_t *pBufLen)
{
	return EApiBoardGetStringA(Id, pData, pBufLen);
}

uint32_t SemaEApiGPIOGetDirectionCaps(uint8_t Id, uint32_t* pnCapsIn, uint32_t* pnCapsOut)
{
	return EApiGPIOGetDirectionCaps(Id, pnCapsIn, pnCapsOut);
}

uint32_t SemaEApiGPIOGetDirection(uint8_t GpioId, uint32_t Bitmask, uint32_t* pDirection)
{
	return EApiGPIOGetDirection(GpioId, Bitmask, pDirection);
}

uint32_t SemaEApiGPIOSetDirection(uint8_t GpioId, uint32_t Bitmask, uint32_t pDirection)
{
	return EApiGPIOSetDirection(GpioId, Bitmask, pDirection);
}

uint32_t SemaEApiGPIOGetLevel(uint8_t GpioId, uint32_t Bitmask, uint32_t* plevel)
{
	return EApiGPIOGetLevel(GpioId, Bitmask, plevel);
}

uint32_t SemaEApiGPIOSetLevel(uint8_t GpioId, uint32_t Bitmask, uint32_t plevel)
{
	return EApiGPIOSetLevel(GpioId, Bitmask, plevel);
}

uint32_t SemaEApiVgaGetBacklightBrightness(uint32_t PanelId, uint8_t* PWMLevel)
{
	return EApiVgaGetBacklightBrightness(PanelId, PWMLevel);
}

uint32_t SemaEApiVgaGetBacklightEnable(uint32_t PanelId, uint8_t* PWMLevel)
{
	return EApiVgaGetBacklightEnable(PanelId, PWMLevel);
}

uint32_t SemaEApiVgaSetBacklightEnable(uint32_t PanelId, uint8_t PWMLevel)
{
	return EApiVgaSetBacklightEnable(PanelId, PWMLevel);
}

uint32_t SemaEApiVgaSetBacklightBrightness(uint32_t PanelId, uint8_t PWMLevel)
{
	return EApiVgaSetBacklightBrightness(PanelId, PWMLevel);
}

uint32_t SemaEApiI2CWriteReadRaw(uint32_t Id, uint32_t Addr, void *pWBuffer, uint32_t WriteBCnt, void *pRBuffer, uint32_t RBufLen, uint32_t ReadBCnt)
{
	return EApiI2CWriteReadRaw(Id, Addr, pWBuffer, WriteBCnt, pRBuffer, RBufLen, ReadBCnt);
}

uint32_t SemaEApiI2CReadTransfer(uint32_t Id, uint16_t Addr, uint32_t Cmd, void *pBuffer, uint32_t BufLen, uint32_t ByteCnt)
{
	return EApiI2CReadTransfer(Id, Addr, Cmd, pBuffer, BufLen, ByteCnt);
}

uint32_t SemaEApiI2CProbeDevice(uint8_t Id, uint32_t Addr)
{
	return EApiI2CProbeDevice(Id, Addr);
}

uint32_t SemaEApiI2CWriteTransfer(uint32_t Id, uint32_t Addr, uint32_t Cmd, void *pBuffer, uint32_t ByteCnt)
{
	return EApiI2CWriteTransfer(Id, Addr, Cmd, pBuffer, ByteCnt);
}

uint32_t SemaEApiI2CGetBusCap(uint32_t Id, uint32_t *pMaxBlkLen)
{
	return EApiI2CGetBusCap(Id, pMaxBlkLen);
}
