#include "iostream"
#include "stdafx.h"
#include "EApi.h"
#include "SemaFunct.h"
#include "ECFunct.h"
#include "SMBusFunct.h"
#include "I2CFunct.h"
uint32_t EC_BITMASK = 0x000000FF;
uint32_t status = EAPI_STATUS_SUCCESS;
IBMCFunc* pBMCFunct = NULL;
Ccommon_bmc Ccmn;

uint32_t EApiLibInitialize(void)
{
	bmc_type BoardType;
	char buff[50] = { 0 };

	uint32_t Status;

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}
#if !EGW3200
	{
		
		BoardType = Ccmn.Init();
	}
#else
	BoardType = I2C;
#endif

	if (BoardType == SMB)
	{
		pBMCFunct = new CSMBusFunct(Ccmn);
	}
	else if(BoardType == EC)
	{
		pBMCFunct = new CECFunct(Ccmn);
	}
	else if (BoardType == I2C)
	{
		pBMCFunct = new CI2CFunct(Ccmn);
	}
	else
	{
		
		Status = EAPI_STATUS_NOT_FOUND;
		goto out;
	}
	
	Status = EAPI_STATUS_NOT_INITIALIZED;

	if (pBMCFunct->Init() == EAPI_STATUS_SUCCESS)
	{
		Status = EAPI_STATUS_INITIALIZED;
	}

out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiUnInitialize(void)
{
	Ccmn.Close();

	if (pBMCFunct != NULL)
	{
		delete pBMCFunct;
	}

	return EAPI_STATUS_SUCCESS;
}

uint32_t EApiVgaGetBacklightBrightness(uint32_t PanelId, uint8_t* PWMLevel)
{
	uint32_t Status;

	if (pBMCFunct->IsInitialiezed() != TRUE)
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (PanelId != EAPI_ID_BACKLIGHT_1 || pBMCFunct->FindCap(SEMA_CAP_DISPLAYCTRL) == 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (PWMLevel == NULL)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status =  pBMCFunct->GetBacklightPWM(PWMLevel);

out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiVgaGetBacklightEnable(uint32_t PanelId, uint8_t* PWMLevel)
{
	uint32_t Status;

	if ((Status = EApiVgaGetBacklightBrightness(PanelId, PWMLevel)) == EAPI_STATUS_SUCCESS)
	{
		*PWMLevel = !!(*PWMLevel);
	}
	
	return Status;
}

uint32_t EApiVgaSetBacklightBrightness(uint32_t PanelId, uint8_t PWMLevel)
{
	uint32_t Status;

	if (pBMCFunct->IsInitialiezed() != TRUE)
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (PanelId != EAPI_ID_BACKLIGHT_1 || pBMCFunct->FindCap(SEMA_CAP_DISPLAYCTRL) == 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (PWMLevel > EAPI_BACKLIGHT_SET_BRIGHTEST || PWMLevel < EAPI_BACKLIGHT_SET_DIMEST)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status = pBMCFunct->SetBacklightPWM(PWMLevel);

out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiVgaSetBacklightEnable(uint32_t PanelId, uint8_t PWMLevel)
{
	return EApiVgaSetBacklightBrightness(PanelId, !!PWMLevel * 200);
}

uint32_t EApiStorageCap(uint32_t Id, uint32_t* pStorageSize, uint32_t* pBlockLength)
{
	if (pBMCFunct->IsInitialiezed() != TRUE)
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if ((Id != EAPI_ID_STORAGE_STD && Id != EAPI_ID_STORAGE_SCR && Id != EAPI_ID_STORAGE_ODM) || pStorageSize == NULL || pBlockLength == NULL)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	return pBMCFunct->StorageCap(Id, pStorageSize, pBlockLength);
}

uint32_t EApiStorageAreaRead(uint32_t Id, uint32_t nOffset, void* pBuffer, uint32_t nBufLen, uint32_t nByteCnt)
{
	uint32_t StorageSize, BlockLength, Status;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if ((Id != EAPI_ID_STORAGE_STD && Id != EAPI_ID_STORAGE_SCR && Id != EAPI_ID_STORAGE_FP && Id != EAPI_ID_STORAGE_ODM) || pBuffer == NULL || nBufLen < nByteCnt || \
		nByteCnt < 0) {
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if (pBMCFunct->StorageCap(Id, &StorageSize, &BlockLength) != EAPI_STATUS_SUCCESS) {
		return EAPI_STATUS_UNSUPPORTED;
	}
	
	if (nOffset != 0 && (nOffset % BlockLength) != 0) {
		//return EAPI_STATUS_INVALID_BLOCK_ALIGNMENT;
	}

	if ((nByteCnt % BlockLength) != 0)
	{
		return EAPI_STATUS_INVALID_BLOCK_LENGTH;
	}

	if ((nByteCnt + nOffset) > StorageSize)
	{
		return EAPI_STATUS_MORE_DATA;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status = pBMCFunct->ReadMem(Id, nOffset, (uint8_t*)pBuffer, nByteCnt);

out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiStorageAreaWrite(uint32_t Id, uint32_t nOffset, void* pBuffer, uint32_t nByteCnt)
{
	uint32_t StorageSize, BlockLength, Status;

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if ((Id != EAPI_ID_STORAGE_STD && Id != EAPI_ID_STORAGE_SCR && Id !=  EAPI_ID_STORAGE_ODM) || pBuffer == NULL || nByteCnt < 0) {
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if (pBMCFunct->StorageCap(Id, &StorageSize, &BlockLength) != EAPI_STATUS_SUCCESS) {
		return EAPI_STATUS_UNSUPPORTED;
	}


	if (nOffset != 0 && (nOffset % BlockLength) != 0) {
		return EAPI_STATUS_INVALID_BLOCK_ALIGNMENT;
	}

	if (nByteCnt % BlockLength != 0) {
		return EAPI_STATUS_INVALID_BLOCK_LENGTH;
	}

	if ((nByteCnt + nOffset) > StorageSize)
	{
		return EAPI_STATUS_MORE_DATA;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status =  pBMCFunct->WriteMem(Id, nOffset, (uint8_t*)pBuffer, nByteCnt);

out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiStorageHexWrite(uint32_t Id, uint32_t nOffset, void* pBuffer, uint32_t nByteCnt)
{
	uint32_t i, j;
	uint32_t StorageSize, BlockLength, Status;
	char buf[2048];
	unsigned char result[1024], * hex_buf;

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if ((Id != EAPI_ID_STORAGE_STD && Id != EAPI_ID_STORAGE_SCR && Id != EAPI_ID_STORAGE_ODM) || pBuffer == NULL || nByteCnt < 0) {
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if (pBMCFunct->StorageCap(Id, &StorageSize, &BlockLength) != EAPI_STATUS_SUCCESS) {
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((nByteCnt + nOffset) > StorageSize)
	{
		return EAPI_STATUS_MORE_DATA;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	memcpy(buf, pBuffer, nByteCnt);

	for (i = 0; i < nByteCnt; i++)
	{
		if (buf[i] >= '0' && buf[i] <= '9')
		{
			buf[i] = buf[i] - '0';
		}
		else
		{
			if (buf[i] >= 'A' && buf[i] <= 'F')
			{
				buf[i] = buf[i] + 32;
			}
			buf[i] = buf[i] - 'a' + 10;
		}
	}
	for (i = 0, j = 0; i < nByteCnt; i += 2, j++)
	{
		result[j] = buf[i] * 16 + buf[i + 1];
	}

	hex_buf = result;

	Status = pBMCFunct->WriteMem(Id, nOffset, (uint8_t*)hex_buf, nByteCnt / 2);

out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiWDogGetCap(uint32_t *pMaxDelay, uint32_t *pMaxEventTimeout, uint32_t* Resetvalue)
{
	if (!pBMCFunct->IsInitialiezed() ) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pMaxDelay == NULL || pMaxEventTimeout == NULL || Resetvalue == NULL) {
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if (pBMCFunct->FindCap(SEMA_CAP_WATCHDOG) == 0)	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	*pMaxDelay = 0;
	*pMaxEventTimeout = 0;
	*Resetvalue = UINT16_MAX;

	return EAPI_STATUS_SUCCESS;
}

uint32_t EApiWDogStart(uint32_t delay, uint32_t EventTimeout,uint32_t ResetTime)
{
	uint32_t MaxDelay = 0;
	uint32_t MaxEventTimeout = 0;
	uint32_t Resetvalue = 0, Status = EAPI_STATUS_SUCCESS;

	if ((Status = EApiWDogGetCap(&MaxDelay, &MaxEventTimeout, &Resetvalue))\
		!= EAPI_STATUS_SUCCESS)	{
		return Status;
	}

	if (delay > MaxDelay || EventTimeout > MaxEventTimeout || ResetTime > Resetvalue\
		|| ResetTime <= 0)	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if (pBMCFunct->IsWDogRunning() == EAPI_STATUS_RUNNING)
	{
		return EAPI_STATUS_RUNNING;
	}


	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	if (pBMCFunct->SetWatchdog(ResetTime) != EAPI_STATUS_SUCCESS)
	{
		Status = EAPI_STATUS_ERROR;
	}


out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiWDogTrigger(void)
{
	uint32_t Status = EAPI_STATUS_SUCCESS;

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->FindCap(SEMA_CAP_WATCHDOG) == 0) {
		return EAPI_STATUS_UNSUPPORTED;
	}
	
	if (pBMCFunct->IsWDogRunning() != EAPI_STATUS_RUNNING)
	{
		return EAPI_STATUS_ERROR;
	} 
	
	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	if(pBMCFunct->TriggerWatchdog() != EAPI_STATUS_SUCCESS)
	{
		Status = EAPI_STATUS_ERROR;
	}

out:
	Ccmn.unlock();

	return Status;
}


uint32_t EApiWDogStop()
{
	uint32_t Status = EAPI_STATUS_SUCCESS;

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->FindCap(SEMA_CAP_WATCHDOG) == 0) {
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (pBMCFunct->IsWDogRunning() != EAPI_STATUS_RUNNING)
	{
		return EAPI_STATUS_ERROR;
	}


	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	if(pBMCFunct->SetWatchdog(0) != EAPI_STATUS_SUCCESS)
	{
		Status = EAPI_STATUS_ERROR;
	}

out:
	Ccmn.unlock();

	return Status;
}


uint32_t EApiPwrUpWDogStart(uint32_t ResetTime)
{
	uint32_t Status = EAPI_STATUS_SUCCESS;

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->FindCap(SEMA_CAP_PWRUP_WD) == 0) {
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (ResetTime < 60 || ResetTime > 65535)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	if(pBMCFunct->SetPwrUpWatchdog(ResetTime) != EAPI_STATUS_SUCCESS)
	{
		Status = EAPI_STATUS_ERROR;
	}

out:

	Ccmn.unlock();

	return Status;
}


uint32_t EApiPwrUpWDogStop()
{
	uint32_t Status;

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->FindCap(SEMA_CAP_PWRUP_WD) == 0) {
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (pBMCFunct->IsPwrUpWDogRunning() != EAPI_STATUS_RUNNING)
	{
		return EAPI_STATUS_WRITE_ERROR;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	if(pBMCFunct->SetPwrUpWatchdog(0) != EAPI_STATUS_SUCCESS)
	{
		Status = EAPI_STATUS_ERROR;
	}

out:
	Ccmn.unlock();

	return Status;
}


int StrToBuf(char* szInfo, uint8_t* pData, uint32_t* pRetSize)
{
	uint32_t nSizeInfo = (uint32_t)strlen(szInfo);
	if (*pRetSize < nSizeInfo)
	{
		memcpy_s(pData, *pRetSize, szInfo, *pRetSize);
		return EAPI_STATUS_MORE_DATA;
	}
	else
	{
		memcpy_s(pData, *pRetSize, szInfo, nSizeInfo);
		*pRetSize = nSizeInfo;
		return EAPI_STATUS_SUCCESS;
	}
}

uint32_t EApiBoardGetStringA(uint32_t Id, uint8_t* pData, uint32_t *pBufLen)
{
	uint32_t Status;
	char szTemp[MAX_PATH];

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBufLen == NULL || pBufLen == 0 || pData == NULL)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	switch (Id)
	{

	case EAPI_ID_BOARD_MANUFACTURER_STR:
	{
		Status = StrToBuf(EAPI_MANUFACTURE, pData, pBufLen);
	}break;

	case EAPI_ID_BOARD_NAME_STR:
	{
		if ((Status = pBMCFunct->GetVersion(szTemp, MAX_PATH)) == EAPI_STATUS_SUCCESS)
		{
			char *szStart = szTemp, *szName = NULL, *szNameNext = NULL;
			if ((szStart = strstr(szTemp, "BMC")) != NULL)
			{
				szStart += strlen("BMC") + 0x01;
			}
			szName = strtok_s(szStart, " ", &szNameNext);
			if (szName != NULL)
			{
				Status = StrToBuf(szName, pData, pBufLen);
			}
		}
	}break;

	case EAPI_ID_BOARD_SERIAL_STR:
	{
		if ((Status = pBMCFunct->GetManufData(SEMA_MANU_DATA_SR_NO, (uint8_t*)szTemp, \
			MAX_PATH)) == EAPI_STATUS_SUCCESS)
		{
			Status = StrToBuf(szTemp, pData, pBufLen);
		}
	}
	break;

	case EAPI_ID_BOARD_BIOS_REVISION_STR:
	{
		if ((Status = Ccmn.GetBIOSVersion(szTemp, MAX_PATH)) == EAPI_STATUS_SUCCESS)
		{
			Status = StrToBuf(szTemp, pData, pBufLen);
		}
	}
	break;

	case EAPI_ID_BOARD_HW_REVISION_STR:
	{
		if ((Status = pBMCFunct->GetManufData(SEMA_MANU_DATA_HW_REV, (uint8_t*)szTemp,\
			MAX_PATH)) == EAPI_STATUS_SUCCESS)
		{
			Status = StrToBuf(szTemp, pData, pBufLen);
		}
	}
	break;

	case EAPI_ID_BOARD_PLATFORM_TYPE_STR:
	{
		if ((Status = pBMCFunct->GetManufData(SEMA_MANU_DATA_PLAT_ID, (uint8_t*)szTemp,\
			MAX_PATH)) == EAPI_STATUS_SUCCESS)
		{
			Status = StrToBuf(szTemp, pData, pBufLen);
		}
	}
	break;

	case EAPI_SEMA_ID_BOARD_BOOT_VERSION_STR:
	{
		if ((Status = pBMCFunct->GetBootVersionExt(szTemp, MAX_PATH)) == EAPI_STATUS_SUCCESS)
		{
			Status = StrToBuf(szTemp, pData, pBufLen);
		}
	}
	break;

	case EAPI_SEMA_ID_BOARD_APPLICATION_VERSION_STR:
	{
		if ((Status = pBMCFunct->GetVersion(szTemp, MAX_PATH)) == EAPI_STATUS_SUCCESS)
		{
			char *szStart = szTemp;
			if ((szStart = strstr(szTemp, "(")) != NULL)
			{
				*(szStart - 0x01) = 0x00;
				Status = StrToBuf(szTemp, pData, pBufLen);
			}
		}
	}
	break;

	case EAPI_SEMA_ID_BOARD_RESTART_EVENT_STR:
	{
		if (pBMCFunct->FindCap(SEMA_CAP_RESTARTEVENT) == 0)
		{
			return EAPI_STATUS_UNSUPPORTED;
		}

		if ((Status = pBMCFunct->GetRestartEventText(szTemp, MAX_PATH)) == EAPI_STATUS_SUCCESS)
		{
			Status = StrToBuf(szTemp, pData, pBufLen);
		}
	}
	break;

	case EAPI_SEMA_ID_BOARD_REPAIR_DATE_STR:
	{
		if ((Status = pBMCFunct->GetManufData(SEMA_MANU_DATA_LR_DATE, (uint8_t*)szTemp,\
			MAX_PATH)) == EAPI_STATUS_SUCCESS)
		{
			Status = StrToBuf(szTemp, pData, pBufLen);
		}
	}
	break;

	case EAPI_SEMA_ID_BOARD_MANUFACTURE_DATE_STR:
	{
		if ((Status = pBMCFunct->GetManufData(SEMA_MANU_DATA_MF_DATE, (uint8_t*)szTemp,\
			MAX_PATH)) == EAPI_STATUS_SUCCESS)
		{
			Status = StrToBuf(szTemp, pData, pBufLen);
		}
	}
	break;

	case EAPI_SEMA_ID_BOARD_MAC_1_STRING:
	{
		if ((Status = pBMCFunct->GetManufData(SEMA_MANU_DATA_MACID_1, (uint8_t*)szTemp,\
			MAX_PATH)) == EAPI_STATUS_SUCCESS)
		{
			Status = StrToBuf(szTemp, pData, pBufLen);
		}
	}
	break;

	case EAPI_SEMA_ID_BOARD_MAC_2_STRING:
	{
		if ((Status = pBMCFunct->GetManufData(SEMA_MANU_DATA_MACID_2, (uint8_t*)szTemp, \
			MAX_PATH)) == EAPI_STATUS_SUCCESS)
		{
			Status = StrToBuf(szTemp, pData, pBufLen);
		}
	}
	break;

	case EAPI_SEMA_ID_BOARD_2ND_HW_REVISION_STR:
	{
		if ((Status = pBMCFunct->GetManufData(SEMA_MANU_DATA_2HW_REV, (uint8_t*)szTemp,\
			MAX_PATH)) == EAPI_STATUS_SUCCESS)
		{
			Status = StrToBuf(szTemp, pData, pBufLen);
		}
	}
	break;

	case EAPI_SEMA_ID_BOARD_2ND_SERIAL_STR:
	{
		if ((Status = pBMCFunct->GetManufData(SEMA_MANU_DATA_2SR_NO, (uint8_t*)szTemp,\
			MAX_PATH)) == EAPI_STATUS_SUCCESS)
		{
			Status = StrToBuf(szTemp, pData, pBufLen);
		}
	}
	break;
	default:
		Status = EAPI_STATUS_UNSUPPORTED;
	}

out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiBoardGetValue(uint32_t Id, uint32_t* pData)
{
	uint32_t Status;
	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pData == NULL)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	switch (Id)
	{

	case EAPI_ID_GET_EAPI_SPEC_VERSION:
	{
		*pData = EAPI_VERSION;
		Status = EAPI_STATUS_SUCCESS;
	}
	break;


	case EAPI_ID_BOARD_BOOT_COUNTER_VAL:
	{
		if (pBMCFunct->FindCap(SEMA_CAP_BOOTCOUNT) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}
		Status = pBMCFunct->GetBootCount(pData);
	}
	break;

	case EAPI_ID_BOARD_RUNNING_TIME_METER_VAL:
	{
		if (pBMCFunct->FindCap(SEMA_CAP_UPTIME) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
		}

		Status = pBMCFunct->GetTotalUpTime(pData);
	}
	break;

	case EAPI_ID_BOARD_LIB_VERSION_VAL:
	{
		*pData = (SEMA_VERSION_MAJOR << 0x18) + (SEMA_VERSION_MINOR << 0x10) + SEMA_VERSION_ADDON;
		Status = EAPI_STATUS_SUCCESS;
	}
	break;

	case EAPI_SEMA_ID_BOARD_POWER_UP_TIME:
	{
		if (pBMCFunct->FindCap(SEMA_CAP_UPTIME) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status = pBMCFunct->GetPwrUpTime(pData);
	}
	break;

	case EAPI_SEMA_ID_BOARD_RESTART_EVENT:
	{
		if (pBMCFunct->FindCap(SEMA_CAP_RESTARTEVENT) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		status = pBMCFunct->GetRestartEvent((uint8_t*)pData);
	}
	break;

	case EAPI_SEMA_ID_BOARD_CAPABILITIES:
	{
		*pData = pBMCFunct->GetCaps();
		Status = EAPI_STATUS_SUCCESS;
	}
	break;

	case EAPI_SEMA_ID_BOARD_CAPABILITIES_EX:
	{
		*pData = pBMCFunct->GetCapsExt();
		Status = EAPI_STATUS_SUCCESS;
	}
	break;

	case EAPI_SEMA_ID_BOARD_SYSTEM_MIN_TEMP:
	{
		int chMaxTempBoard = 0x00;

		if (pBMCFunct->FindCap(SEMA_CAP_TEMPERATURES) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status = pBMCFunct->GetBRDMinMaxTemp((int*)pData, &chMaxTempBoard);

	}
	break;

	case EAPI_SEMA_ID_BOARD_SYSTEM_MAX_TEMP:
	{
		int chMinTempBoard = 0x00;

		if (pBMCFunct->FindCap(SEMA_CAP_TEMPERATURES) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status = pBMCFunct->GetBRDMinMaxTemp(&chMinTempBoard, (int*)pData);
	}
	break;

	case EAPI_SEMA_ID_BOARD_SYSTEM_STARTUP_TEMP:
	{
		if (pBMCFunct->FindCap(SEMA_CAP_TEMPERATURES) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status =  pBMCFunct->GetStartupTemp((int*)pData);
	}
	break;

	case EAPI_SEMA_ID_BOARD_CPU_MIN_TEMP:
	{
		int chMaxTempCpu = 0x00;

		if (pBMCFunct->FindCap(SEMA_CAP_TEMPERATURES) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status = pBMCFunct->GetCPUMinMaxTemp((int*)pData, &chMaxTempCpu);
	}
	break;

	case EAPI_SEMA_ID_BOARD_CPU_MAX_TEMP:
	{
		int chMinTempCpu = 0x00;

		if (pBMCFunct->FindCap(SEMA_CAP_TEMPERATURES) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status = pBMCFunct->GetCPUMinMaxTemp(&chMinTempCpu, (int*)pData);
	}
	break;

	case EAPI_SEMA_ID_BOARD_CPU_STARTUP_TEMP:
	{
		if (pBMCFunct->FindCap(SEMA_CAP_TEMPERATURES) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status = pBMCFunct->GetCPUStartupTemp((int*)pData);
	}
	break;

	case EAPI_SEMA_ID_BOARD_MAIN_CURRENT:
	{
		if (pBMCFunct->FindCap(SEMA_CAP_CURRENTS) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status = pBMCFunct->GetMainPowerCurrent((uint16_t*)pData);
	}
	break;

	case EAPI_SEMA_ID_BOARD_2ND_SYSTEM_TEMP:
	{
		if (pBMCFunct->FindCapExt(SEMA_CAP_TEMPERATURES_1) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status = pBMCFunct->GetCurrentBoardTemp_1((int*)pData);
	}
	break;

	case EAPI_SEMA_ID_BOARD_2ND_SYSTEM_MIN_TEMP:
	{
		if (pBMCFunct->FindCapExt(SEMA_CAP_TEMPERATURES_1) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		int Dummy = 0x00;
		Status = pBMCFunct->GetMinMaxTemp_1(&Dummy, &Dummy, (int*)pData, &Dummy);
	}
	break;

	case EAPI_SEMA_ID_BOARD_2ND_SYSTEM_MAX_TEMP:
	{
		if (pBMCFunct->FindCapExt(SEMA_CAP_TEMPERATURES_1) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		int Dummy = 0x00;
		Status = pBMCFunct->GetMinMaxTemp_1(&Dummy, &Dummy, &Dummy, (int*)pData);
	}
	break;

	case EAPI_SEMA_ID_BOARD_2ND_SYSTEM_STARTUP_TEMP:
	{
		if (pBMCFunct->FindCapExt(SEMA_CAP_TEMPERATURES_1) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		int CPUDummy = 0x00;

		Status = pBMCFunct->GetStartupTemp_1(&CPUDummy, (int*)pData);
	}
	break;

	case EAPI_SEMA_ID_BOARD_POWER_CYCLE:
	{
		if (pBMCFunct->FindCap(SEMA_CAP_UPTIME) == 0)
		{
			Status = EAPI_STATUS_SUCCESS;
			goto out;
		}
		Status = pBMCFunct->GetPwrCycles(pData);
	}
	break;

	case EAPI_SEMA_ID_BOARD_BMC_FLAG:
	{
		Status = pBMCFunct->GetBMCFlags((uint8_t*)pData);
	}
	break;

	case EAPI_SEMA_ID_BOARD_BMC_STATUS:
	{
		Status = pBMCFunct->GetBMCStatus((uint8_t*)pData);
	}
	break;

	case EAPI_SEMA_ID_HWMON_VOLTAGE_GFX_VCORE:
	{
		char* tbData[] = { "GFX-Vcore", "VCC_GT", "VGFX", "VGG_S3", "VNN_S", NULL };

		Status = pBMCFunct->GetHWMonitor(tbData, pData);
	}
	break;

	case EAPI_SEMA_ID_HWMON_VOLTAGE_1V05:
	{
		char* tbData[] = { "1.050", "V1P05S", "V1P05_A", "1.05", "V1P05_S3", NULL };

		Status = pBMCFunct->GetHWMonitor(tbData, pData);
	}
	break;

	case EAPI_SEMA_ID_HWMON_VOLTAGE_VIN:
	{
		char* tbData[] = { "VIN(12V)", "VIN", "+VIN", "V12", "Q7", "VSMARC", "V12S", "V5_ATX",\
			"P_+5V_ATX", "+V12", NULL };

		Status = pBMCFunct->GetHWMonitor(tbData, pData);
	}
	break;

	case EAPI_SEMA_ID_HWMON_VOLTAGE_1V5:
	{
		char* tbData[] = { "1.500", "V1P5_S0", "V1P5S", "V1v5", "P_+1V5_S", NULL };

		Status = pBMCFunct->GetHWMonitor(tbData, pData);
	}
	break;

	case EAPI_ID_HWMON_VOLTAGE_VCORE:
	{
		char* tbData[] = { "CPU-Vcore", "VCC_CORE", "VCORE", "VCC_S", "P_+VCORE", "+VCORE", NULL };

		Status = pBMCFunct->GetHWMonitor(tbData, pData);
	}
	break;

	case EAPI_ID_HWMON_VOLTAGE_2V5:
	{
		char* tbData[] = { "2.500", "2V5_VPP", NULL };

		Status = pBMCFunct->GetHWMonitor(tbData, pData);
	}
	break;

	case EAPI_ID_HWMON_VOLTAGE_3V3:
	{
		char* tbData[] = { "3.300", "V3P3", "V3P3A", "V3P3_A", "3.3V", "P_+3V3_A", "+V3.3S", NULL };

		Status = pBMCFunct->GetHWMonitor(tbData, pData);
	}
	break;

	case EAPI_ID_HWMON_VOLTAGE_VBAT:
	{
		char* tbData[] = { "VRTC", "RTC", "+_VRTC", NULL };

		Status = pBMCFunct->GetHWMonitor(tbData, pData);
	}
	break;

	case EAPI_ID_HWMON_VOLTAGE_5V:
	{
		char* tbData[] = { "5V", "5.000", "V5_DUAL", "V5S", "V5Vin", "V5_S0", "5V_DUAL", "V5_S",\
			"P_+5V_ATX",  "V5", NULL };

		Status = pBMCFunct->GetHWMonitor(tbData, pData);
	}
	break;

	case EAPI_ID_HWMON_VOLTAGE_5VSB:
	{
		char* tbData[] = { "V5SBY", "V5_SBY", "V5VSB", "5VSB", "5VSBY", "P_+5V_ATX_SBY", "+V5VSB", NULL };
		Status = pBMCFunct->GetHWMonitor(tbData, pData);
	}
	break;

	case EAPI_ID_HWMON_VOLTAGE_12V:
	{
		char* tbData[] = { "VIN (12V)", "+VIN", "12.000", "V12", "V12S", "V12_V", "+V12", NULL };

		Status = pBMCFunct->GetHWMonitor(tbData, pData);
	}
	break;

	case EAPI_SEMA_ID_IO_CURRENT:
	{
		if (pBMCFunct->FindCap(SEMA_CAP_CURRENTS) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status = pBMCFunct->GetMainPowerCurrent((uint16_t*)pData);
	}
	break;

	case EAPI_ID_HWMON_FAN_CPU:
	{
		if (pBMCFunct->GetSmartFanSupport(SEMA_FAN_CPU) == EAPI_STATUS_UNSUPPORTED)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status = pBMCFunct->GetFanSpeed(SEMA_FAN_CPU, (uint16_t*)pData);
	}
	break;

	case EAPI_ID_HWMON_FAN_SYSTEM:
	{
		if (pBMCFunct->GetSmartFanSupport(SEMA_FAN_SYSTEM_1) == EAPI_STATUS_UNSUPPORTED)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status = pBMCFunct->GetFanSpeed(SEMA_FAN_SYSTEM_1, (uint16_t*)pData);
	}
	break;

	case EAPI_SEMA_ID_HWMON_FAN_SYSTEM_2:
	{
		if (pBMCFunct->GetSmartFanSupport(SEMA_FAN_SYSTEM_2) == EAPI_STATUS_UNSUPPORTED)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status = pBMCFunct->GetFanSpeed(SEMA_FAN_SYSTEM_2, (uint16_t*)pData);
	}
	break;

	case EAPI_SEMA_ID_HWMON_FAN_SYSTEM_3:
	{
		if (pBMCFunct->GetSmartFanSupport(SEMA_FAN_SYSTEM_3) == EAPI_STATUS_UNSUPPORTED)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status = pBMCFunct->GetFanSpeed(SEMA_FAN_SYSTEM_3, (uint16_t*)pData);
	}
	break;

	case EAPI_ID_HWMON_CPU_TEMP:
	{
		if (pBMCFunct->FindCap(SEMA_CAP_TEMPERATURES) == 0)
		{
			Status = EAPI_STATUS_UNSUPPORTED;
			goto out;
		}

		Status = pBMCFunct->GetCurrentCPUTemp((int*)pData);

	}
	break;

	case EAPI_ID_HWMON_SYSTEM_TEMP:
	{
		Status = pBMCFunct->GetCurrentBoardTemp((int*)pData);
	}
	break;
	}
out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiGPIOGetDirectionCaps(uint8_t GpioId, uint32_t* pnCapsIn, uint32_t* pnCapsOut)
{
	uint32_t Status;
	uint32_t Bitmask=1;
	uint8_t eRet;

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if ((pnCapsOut == NULL) || (pnCapsIn == NULL))
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}
	if (pBMCFunct->FindCap(SEMA_CAP_GPIOS) == 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}
	Bitmask = Bitmask << GpioId;

	if (pBMCFunct->ValidateBitMask(Bitmask) != EAPI_STATUS_SUCCESS)
	{
		Status = EAPI_STATUS_UNSUPPORTED;
		goto out;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	pBMCFunct->SetId(GpioId);

	Status = pBMCFunct->GetCapsGpioDir(pnCapsIn, pnCapsOut, GpioId);

out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiGPIOGetDirection(uint8_t GpioId, uint32_t Bitmask, uint32_t* pDirection)
{
	EERROR eRet;
	int i;
	uint32_t Bitmask_Ext;

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}
	if (pBMCFunct->FindCap(SEMA_CAP_GPIOS) == 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (pDirection == NULL || Bitmask == 0)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if (GpioId != 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}
	
	if ((eRet = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	pBMCFunct->SetId(GpioId);

	if (pBMCFunct->ValidateBitMask(Bitmask) != EAPI_STATUS_SUCCESS)
	{
		eRet = EAPI_STATUS_INVALID_BITMASK;
		goto out;
	}
	
	if ((eRet = pBMCFunct->GetGPIODir(pDirection,Bitmask)) == EAPI_STATUS_SUCCESS)
	{
		if (Bitmask > 0xFF)
		{
			Bitmask_Ext = Bitmask >> 8;
			*pDirection &= Bitmask_Ext;
		}
		else
		*pDirection &= Bitmask;
	}

	for (i = 0; !!(Bitmask & (1 << i)) != 1; i++);

	if (Bitmask > 0xFF)
		i = i - 8;

	*pDirection = (*pDirection >> i);

out:
	Ccmn.unlock();
	return eRet;
}

uint32_t EApiGPIOSetDirection(uint8_t GpioId, uint32_t Bitmask, uint32_t pDirection)
{
	EERROR eRet;
	uint32_t pDirectionLast, pDir, Level_last, i,Bitmask_Ext;

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}
	if (pBMCFunct->FindCap(SEMA_CAP_GPIOS) == 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}
	if (Bitmask == 0)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if (GpioId != 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((eRet = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	pBMCFunct->SetId(GpioId);

	if (pBMCFunct->ValidateBitMask(Bitmask) != EAPI_STATUS_SUCCESS)
	{
		eRet = EAPI_STATUS_INVALID_BITMASK;
		goto out;
	}

	if ((eRet = pBMCFunct->GetGPIODir(&pDirectionLast,Bitmask)) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}
	for (i = 0; !!(Bitmask & (1 << i)) != 1; i++);

	if (Bitmask > 0xFF)
		i = i - 8;

	if(Bitmask > 0xFF)
	{
		Bitmask_Ext = Bitmask >> 8;
		pDirectionLast |= Bitmask_Ext;
		pDirectionLast &= (~Bitmask_Ext | (pDirection << i));
	}
	else
	{
		pDirectionLast |= Bitmask;
		pDirectionLast &= (~Bitmask | (pDirection << i));
	}
	eRet = pBMCFunct->SetGPIODir(pDirectionLast,Bitmask);

out:
	Ccmn.unlock();

	return eRet;
}

uint32_t EApiGPIOGetLevel(uint8_t GpioId, uint32_t Bitmask, uint32_t* plevel)
{
	EERROR eRet,ret;
	uint32_t pDir,Bitmask_Ext;
	int i;

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}
	if (pBMCFunct->FindCap(SEMA_CAP_GPIOS) == 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}
	if (GpioId != 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (Bitmask == 0)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if ((eRet = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	pBMCFunct->SetId(GpioId);

	
	if (pBMCFunct->ValidateBitMask(Bitmask) != EAPI_STATUS_SUCCESS)
	{
		eRet = EAPI_STATUS_INVALID_BITMASK;
		goto out;
	}

	if ((pBMCFunct->GetGPIODir(&pDir , Bitmask)) == EAPI_STATUS_SUCCESS)
	{
		if (Bitmask > 0xFF)
		{
			Bitmask_Ext = Bitmask >> 8;
			pDir &= Bitmask_Ext;
		}
		else
		pDir &= Bitmask;
	}

	for (i = 0; !!(Bitmask & (1 << i)) != 1; i++);

	if (Bitmask > 0xFF)
		i = i - 8;

	pDir = (pDir >> i);

	/*Checking the GPIO Pin is input or output*/
	if (pDir != 0)
	{
		if ((eRet = pBMCFunct->GetGPIOIN(plevel, Bitmask)) == EAPI_STATUS_SUCCESS)
		{
			if (Bitmask > 0xFF)
			{
				Bitmask_Ext = Bitmask >> 8;
				*plevel &= Bitmask_Ext;
			}
			else
			*plevel &= Bitmask;
		}
	}
	if (pDir == 0)
	{
		if ((eRet = pBMCFunct->GetGPIOOUT(plevel, Bitmask)) == EAPI_STATUS_SUCCESS)
		{
			if (Bitmask > 0xFF)
			{
				Bitmask_Ext = Bitmask >> 8;
				*plevel &= Bitmask_Ext;
			}
			else
			*plevel &= Bitmask;
		}
	}
	/*Checking the GPIO Pin For SMBus*/ 
	if ((ret = pBMCFunct->GetGPIO(plevel)) == EAPI_STATUS_SUCCESS)
	{
		*plevel &= Bitmask;
		eRet = EAPI_STATUS_SUCCESS;
	}

	for (i = 0; !!(Bitmask & (1 << i)) != 1; i++);

	if (Bitmask > 0xFF)
		i = i - 8;

	*plevel = (*plevel >> i);
out:
	Ccmn.unlock();
	return eRet;
}

uint32_t EApiGPIOSetLevel(uint8_t GpioId, uint32_t Bitmask, uint32_t plevel)
{
	EERROR eRet;
	uint32_t Level_last;
	uint32_t dir, i , Bitmask_Ext;

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}
	if (pBMCFunct->FindCap(SEMA_CAP_GPIOS) == 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}
	if (Bitmask == 0)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if (GpioId != 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((eRet = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	pBMCFunct->SetId(GpioId);

	if (pBMCFunct->ValidateBitMask(Bitmask) != EAPI_STATUS_SUCCESS)
	{
		eRet = EAPI_STATUS_INVALID_BITMASK;
		goto out;
	}

	if (pBMCFunct->GetGPIODir(&dir , Bitmask) != EAPI_STATUS_SUCCESS)
	{
		eRet = EAPI_STATUS_ERROR;
		goto out;
	}

	if (Bitmask > 0xFF)
	{
		Bitmask_Ext = Bitmask >> 8;
		if ((dir & Bitmask_Ext) != 0)
		{
			eRet = EAPI_STATUS_INVALID_DIRECTION;
			goto out;
		}
	}
	else
	{
		if ((dir & Bitmask) != 0)
		{
			eRet = EAPI_STATUS_INVALID_DIRECTION;
			goto out;
		}
	}

	if ((eRet = pBMCFunct->GetGPIO(&Level_last)) == EAPI_STATUS_SUCCESS)
	{
		for (i = 0; !!(Bitmask & (1 << i)) != 1; i++);
		Level_last |= Bitmask;
		Level_last &= (~Bitmask | (plevel << i));
		eRet = pBMCFunct->SetGPIO(Level_last, Bitmask);
		goto out;
	}

	/*Reading the last status from 0x86 and 0x87 for Output direction pin*/
	if ((eRet = pBMCFunct->GetGPIOOUT(&Level_last, Bitmask)) != EAPI_STATUS_SUCCESS)
	{
		eRet = EAPI_STATUS_ERROR;
		goto out;
	}

	for (i = 0; !!(Bitmask & (1 << i)) != 1; i++);

	if (Bitmask > 0xFF)
		i = i - 8;

	if (Bitmask > 0xFF)
	{
		Level_last |= Bitmask_Ext;
		Level_last &= (~Bitmask_Ext | (plevel << i));
	}
	else
	{
		Level_last |= Bitmask;
		Level_last &= (~Bitmask | (plevel << i));
	}

	eRet = pBMCFunct->SetGPIO(Level_last, Bitmask);

out:
	Ccmn.unlock();
	return eRet;
}

uint32_t EApiI2CGetBusCap(uint32_t Id, uint32_t *pMaxBlkLen)
{
	if (!pBMCFunct->IsInitialiezed())
	{
		

		return EAPI_STATUS_NOT_INITIALIZED;
	}
	
	if (pMaxBlkLen == NULL)
	{
	
		return EAPI_STATUS_INVALID_PARAMETER;
	}
	
	if (pBMCFunct->I2CGetBusCap(Id, pMaxBlkLen) != EAPI_STATUS_SUCCESS)
	{
		
		return EAPI_STATUS_UNSUPPORTED;
	}
	
	return EAPI_STATUS_SUCCESS;
}

uint32_t EApiI2CProbeDevice(uint8_t Id, uint32_t Addr)
{
	uint32_t pMaxBlkLen, Status;

	Addr <<= 1; 

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (EApiI2CGetBusCap(Id, &pMaxBlkLen) != EAPI_STATUS_SUCCESS)
	{
		
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		
		goto out;
	}

	if (pBMCFunct->I2CWaitForFree() != EAPI_STATUS_SUCCESS)
	{
		
		Status = EAPI_STATUS_BUSY_COLLISION;
		goto out;
	}
	
	Status = pBMCFunct->I2CIsDeviceExist(Id, Addr);
	
out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiI2CReadTransfer(uint32_t Id, uint32_t Addr, uint32_t Cmd, void* pBuffer, \
	uint32_t BufLen, uint32_t ByteCnt)
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

	if (pBuffer == NULL || ByteCnt == 0 || BufLen == 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (ByteCnt > MaxBlkSize)
	{
		return EAPI_STATUS_INVALID_BLOCK_LENGTH;
	}

	if (ByteCnt > BufLen)
	{
		return EAPI_STATUS_MORE_DATA;
	}
	
	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Addr <<= 1;

	Status = pBMCFunct->I2CReadTransfer(Id, Addr, Cmd, pBuffer, BufLen, ByteCnt);

out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiI2CWriteTransfer(uint32_t Id, uint32_t Addr, uint32_t Cmd, void* pBuffer, \
	uint32_t ByteCnt)
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

	if (pBuffer == NULL || ByteCnt == 0)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (ByteCnt > MaxBlkSize)
	{
		return EAPI_STATUS_INVALID_BLOCK_LENGTH;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}
	Addr <<= 1;

	Status = pBMCFunct->I2CWriteTransfer(Id, Addr, Cmd, pBuffer, ByteCnt);

out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiI2CWriteReadRaw(uint32_t Id, uint32_t Addr, void *pWBuffer, uint32_t WriteBCnt, \
	void *pRBuffer, uint32_t RBufLen, uint32_t ReadBCnt)
{
	uint32_t pMaxBlkLen, Status;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if ((pWBuffer == NULL && WriteBCnt > 1) || (pRBuffer == NULL && ReadBCnt > 1) \
		|| (ReadBCnt > 1 && RBufLen == 0) || (WriteBCnt == 0 && ReadBCnt == 0))
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if (ReadBCnt > RBufLen)
	{
		return EAPI_STATUS_MORE_DATA;
	}

	if (EApiI2CGetBusCap(Id, &pMaxBlkLen) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (WriteBCnt > pMaxBlkLen || ReadBCnt > pMaxBlkLen)
	{
		return EAPI_STATUS_INVALID_BLOCK_LENGTH;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}
	
	Addr <<= 1;
	Status = pBMCFunct->I2CRawTransaction(Id, Addr, pWBuffer, WriteBCnt, pRBuffer, ReadBCnt);
out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiI2CWriteRaw(uint32_t Id, uint32_t Addr, void *pBuffer, uint32_t ByteCnt)
{
	return EApiI2CWriteReadRaw(Id, Addr, pBuffer, ByteCnt, NULL, 0, 0);
}

uint32_t EApiI2CReadRaw(uint32_t Id, uint32_t Addr, void* pBuffer, uint32_t  BufLen, uint32_t ByteCnt)
{
	return EApiI2CWriteReadRaw(Id, Addr, NULL, 0, pBuffer, BufLen, ByteCnt);
}

uint32_t EApiGUIDWrite(uint32_t Id, uint32_t nOffset, void* pBuffer, uint32_t nByteCnt) 
{

	uint32_t StorageSize, BlockLength, Status;

	if (!pBMCFunct->IsInitialiezed()) {
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (pBMCFunct->StorageCap(Id, &StorageSize, &BlockLength) != EAPI_STATUS_SUCCESS) {
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((nByteCnt + nOffset) > StorageSize)
	{
		return EAPI_STATUS_MORE_DATA;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status = pBMCFunct->WriteMem(Id, nOffset, (uint8_t*)pBuffer, nByteCnt / 2);

out:
	Ccmn.unlock();
	return Status;
}

uint32_t EApiStorageHexRead(uint32_t Id, uint32_t nOffset, void* pBuffer, uint32_t nBufLen, uint32_t nByteCnt)
{
	uint32_t StorageSize, BlockLength, Status;

	if (!pBMCFunct->IsInitialiezed())
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if ((Id != EAPI_ID_STORAGE_STD && Id != EAPI_ID_STORAGE_SCR && Id != EAPI_ID_STORAGE_FP && Id != EAPI_ID_STORAGE_ODM) || pBuffer == NULL || nBufLen < nByteCnt || \
		nByteCnt < 0) {
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if (pBMCFunct->StorageCap(Id, &StorageSize, &BlockLength) != EAPI_STATUS_SUCCESS) {
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((nByteCnt + nOffset) > StorageSize)
	{
		return EAPI_STATUS_MORE_DATA;
	}

	if ((Status = Ccmn.lock()) != EAPI_STATUS_SUCCESS)
	{
		goto out;
	}

	Status = pBMCFunct->ReadMem(Id, nOffset, (uint8_t*)pBuffer, nByteCnt);

out:
	Ccmn.unlock();
	return Status;
}