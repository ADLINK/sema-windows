#include "stdafx.h"
#include <tchar.h>
#include "ECFunct.h"


EERROR CECFunct::SetWatchdog(uint32_t nValue)
{
	uint8_t pDataIn[] = { (uint8_t)(nValue), (uint8_t)(nValue >> 0x08) };

	if (m_clsCmn.SaveLocalWDT(nValue) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (m_clsECTrans.ECWrite(EC_WO_ADDR_RUN_WD_MIRR, EC_REGION_1, pDataIn, 0x02) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (m_clsECTrans.ECWrite(EC_WO_ADDR_RUN_WD, EC_REGION_1, pDataIn, 0x02) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	return EAPI_STATUS_SUCCESS;
}

EERROR CECFunct::TriggerWatchdog()
{
	unsigned int timeout;

	if (GetLastWDogSetValue(&timeout) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if(SetWatchdog(timeout) == EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}

EERROR CECFunct::GetLastWDogSetValue(uint32_t *nValue)
{
	if (m_clsCmn.ShowLocalWDT(nValue) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	return EAPI_STATUS_SUCCESS;
}

EERROR CECFunct::IsWDogRunning(void)
{
	EERROR result;
	uint32_t nValue;
	unsigned char buf[MAX_PATH];

	result=m_clsECTrans.ECRead(EC_WO_ADDR_RUN_WD, EC_REGION_1, buf, 0x02);

	if (buf[0x00] != 0x00)
	{
		return EAPI_STATUS_RUNNING;
	}
	if (GetLastWDogSetValue(&nValue) == EAPI_STATUS_ERROR)
	{
		return EAPI_STATUS_ERROR;
	}

	if (nValue == 0x00)
	{
		return EAPI_STATUS_ERROR;
	}

	return EAPI_STATUS_RUNNING;
}

EERROR CECFunct::IsPwrUpWDogRunning(void)
{
	uint32_t nValue;
	uint8_t ConfigData[4] = { 0 };

	if (m_clsCmn.ShowLocalPwrWDT(&nValue) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}
	
	if (m_clsECTrans.ECRead(EC_RW_ADDR_BMC_CONFIG, EC_REGION_1, ConfigData, 0x04) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (nValue == 0 && (!!(ConfigData[3] & (1 << 5)) == 0))
	{
		return EAPI_STATUS_ERROR;
	}

	return EAPI_STATUS_RUNNING;
}

EERROR CECFunct::SetPwrUpWatchdog(uint32_t nValue)
{
	uint8_t pDataIn[] = { (uint8_t)(nValue & 0xFF), (uint8_t)((nValue & 0xFF00) >> 8) }, ConfigData[4] = { 0 };

	if (m_clsCmn.SaveLocalPwrWDT(nValue) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (m_clsECTrans.ECRead(EC_RW_ADDR_BMC_CONFIG, EC_REGION_1, ConfigData, 0x04) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (m_clsECTrans.ECWrite(EC_WO_ADDR_PWRUP_WD, EC_REGION_1, pDataIn, 0x02) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (nValue == 0)
	{
		ConfigData[3] = (ConfigData[3] & ~(1 << 5));
	}
	else
	{
		ConfigData[3] = (ConfigData[3] | (1 << 5));
	}

	if (m_clsECTrans.ECWrite(EC_RW_ADDR_BMC_CONFIG, EC_REGION_1, ConfigData, 0x04) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}
	
	return EAPI_STATUS_SUCCESS;
}
