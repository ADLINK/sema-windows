#include "stdafx.h"
#include <tchar.h>
#include "I2CFunct.h"


EERROR CI2CFunct::SetWatchdog(uint32_t nValue)
{
#if 0
	uint8_t pDataIn[] = { (uint8_t)(nValue >> 0x08), (uint8_t)nValue };

	if (m_clsCmn.SaveLocalWDT(nValue) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_SET_WD, pDataIn, 0x02) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}
#endif
	return EAPI_STATUS_SUCCESS;
}

EERROR CI2CFunct::TriggerWatchdog()
{
#if 0
	unsigned __int32 u32Buff = 0;

	if (m_clsCmn.ShowLocalWDT(&u32Buff) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (SetWatchdog(u32Buff) == EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_SUCCESS;
	}
#endif
	return EAPI_STATUS_ERROR;
}

EERROR CI2CFunct::GetLastWDogSetValue(uint32_t *nValue)
{
#if 0
	if (m_clsCmn.ShowLocalWDT(nValue) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}
#endif
	return EAPI_STATUS_SUCCESS;
}

EERROR CI2CFunct::IsWDogRunning(void)
{
#if 0
	uint32_t lastValue = 0;

	if (GetLastWDogSetValue(&lastValue) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (lastValue == 0x00)
	{
		return EAPI_STATUS_ERROR;
	}
#endif
	return EAPI_STATUS_RUNNING;
}

EERROR CI2CFunct::IsPwrUpWDogRunning(void)
{
#if 0
	unsigned __int32 nValue = 0;

	if (m_clsCmn.ShowLocalPwrWDT(&nValue) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (nValue == 0)
	{
		return EAPI_STATUS_ERROR;
	}
#endif
	return EAPI_STATUS_RUNNING;
}

EERROR CI2CFunct::SetPwrUpWatchdog(uint32_t nValue)
{
#if 0
	uint8_t pDataIn[] = { (uint8_t)(nValue >> 0x08), (uint8_t)nValue };

	if (m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_SET_PWRUPWD, pDataIn, 0x02) == EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_SUCCESS;
	}
#endif
	return EAPI_STATUS_ERROR;
}