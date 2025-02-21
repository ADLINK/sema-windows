#include "stdafx.h"
#include <tchar.h>
#include "SMBusFunct.h"


EERROR CSMBusFunct::SetWatchdog(uint32_t nValue)
{
	uint8_t pDataIn[] = { (uint8_t)(nValue >> 0x08), (uint8_t)nValue };

	if (m_clsCmn.SaveLocalWDT(nValue) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_SET_WD, pDataIn, 0x02) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	return EAPI_STATUS_SUCCESS;
}

EERROR CSMBusFunct::TriggerWatchdog()
{
	unsigned __int32 u32Buff = 0;

	if (m_clsCmn.ShowLocalWDT(&u32Buff) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (SetWatchdog(u32Buff) == EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}

EERROR CSMBusFunct::GetLastWDogSetValue(uint32_t *nValue)
{
	if (m_clsCmn.ShowLocalWDT(nValue) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	return EAPI_STATUS_SUCCESS;
}

EERROR CSMBusFunct::IsWDogRunning(void)
{
	uint32_t lastValue = 0;
	EERROR result;
	uint32_t nRet = MAX_PATH;
	unsigned char buf[MAX_PATH];

	result = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x1, TT_RBL, SEMA_CMD_SET_WD, NULL, 0x00, buf, nRet);

	if (buf[0x01] != 0x00)
	{
		return EAPI_STATUS_RUNNING;
	}
	if (GetLastWDogSetValue(&lastValue) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}
	
	if (lastValue == 0x00)
	{
		return EAPI_STATUS_ERROR;
	}

	return EAPI_STATUS_RUNNING;
}

EERROR CSMBusFunct::IsPwrUpWDogRunning(void)
{
	unsigned __int32 nValue = 0;
	EERROR result;
	uint32_t nRet = MAX_PATH;
	unsigned char buf[MAX_PATH];

	result = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x1, TT_RBL, SEMA_CMD_SET_PWRUPWD, NULL, 0x00, buf, nRet);

	if (buf[0x01] != 0x00)
	{
		return EAPI_STATUS_RUNNING;
	}

	if (m_clsCmn.ShowLocalPwrWDT(&nValue) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (nValue == 0)
	{
		return EAPI_STATUS_ERROR;
	}
	return EAPI_STATUS_RUNNING;
}

EERROR CSMBusFunct::SetPwrUpWatchdog(uint32_t nValue)
{
	uint8_t pDataIn[] = { (uint8_t)(nValue >> 0x08), (uint8_t)nValue };
	/*Saving the Powerup Watchdog timeout value*/
	if (m_clsCmn.SaveLocalPwrWDT(nValue) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}
	
	if (m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_SET_PWRUPWD, pDataIn, 0x02) == EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}
