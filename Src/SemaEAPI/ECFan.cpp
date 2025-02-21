#include "stdafx.h"
#include "ECFunct.h"

#define TMP_LVL_1_INDEX 0
#define PWM_LVL_1_INDEX 1

#define TMP_LVL_2_INDEX 2
#define PWM_LVL_2_INDEX 3

#define TMP_LVL_3_INDEX 4
#define PWM_LVL_3_INDEX 5

#define TMP_LVL_4_INDEX 6
#define PWM_LVL_4_INDEX 7

EERROR CECFunct::GetSmartFanSupport(uint32_t FanId)
{
	unsigned int fanid_list[] = { SEMA_CAP_FAN_CPU, SEMA_CAP_FAN_1, 0, 0};

	if ((!!(m_nSemaCaps & fanid_list[FanId])) != 1)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	return EAPI_STATUS_SUCCESS;
}

EERROR CECFunct::GetSmartFanTempPoints(uint32_t nFanNr, char* pData, uint32_t nSize)
{
	EERROR eRet;
	uint8_t	bCmd;
	uint8_t pDataRet[9] = { 0 };

	switch (nFanNr)
	{
	case SEMA_FAN_CPU:
		bCmd = EC_RW_CPU_TMP_REG;
		break;
	case SEMA_FAN_SYSTEM_1:
		bCmd = EC_RW_SYS_TMP_REG;
		break;
	default:
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((eRet = m_clsECTrans.ECRead(bCmd, EC_REGION_1, pDataRet, 8)) == EAPI_STATUS_SUCCESS)
	{
		pData[0] = pDataRet[TMP_LVL_1_INDEX];
		pData[1] = pDataRet[TMP_LVL_2_INDEX];
		pData[2] = pDataRet[TMP_LVL_3_INDEX];
		pData[3] = pDataRet[TMP_LVL_4_INDEX];
	}

	return eRet;
}

EERROR CECFunct::GetSmartFanPWMPoints(uint32_t nFanNr, char* pData, uint32_t nSize)
{
	EERROR eRet;
	uint8_t	bCmd;
	uint8_t pDataRet[9] = { 0 };

	switch (nFanNr)
	{
	case SEMA_FAN_CPU:
		bCmd = EC_RW_CPU_TMP_REG;
		break;
	case SEMA_FAN_SYSTEM_1:
		bCmd = EC_RW_SYS_TMP_REG;
		break;
	default:
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((eRet = m_clsECTrans.ECRead(bCmd, EC_REGION_1, pDataRet, 8)) == EAPI_STATUS_SUCCESS)
	{
		pData[0] = pDataRet[PWM_LVL_1_INDEX];
		pData[1] = pDataRet[PWM_LVL_2_INDEX];
		pData[2] = pDataRet[PWM_LVL_3_INDEX];
		pData[3] = pDataRet[PWM_LVL_4_INDEX];
	}

	return eRet;
}

EERROR CECFunct::GetSmartFanMode(uint32_t nFanNr, uint8_t* pData)
{
	EERROR eRet;
	uint32_t nCtrlReg = 0x00;

	if ((eRet = GetSysCtrlReg(&nCtrlReg)) == EAPI_STATUS_SUCCESS)
	{
		switch (nFanNr)
		{
		case SEMA_FAN_CPU:
			nCtrlReg = (nCtrlReg & (0x03 << 9)) >> 9;
			break;
		case SEMA_FAN_SYSTEM_1:
			nCtrlReg = (nCtrlReg & (0x03 << 11)) >> 11;
			break;
		default:
			return EAPI_STATUS_UNSUPPORTED;
		}
		*pData = (uint8_t)nCtrlReg;
	}

	return eRet;
}

EERROR CECFunct::GetSmartFanTempSrc(uint32_t nFanNr, uint8_t* pData)
{
	EERROR eRet;
	uint32_t nCtrlReg = 0x00;

	if ((eRet = GetSysCtrlReg(&nCtrlReg)) == EAPI_STATUS_SUCCESS)
	{
		switch (nFanNr)
		{
		case SEMA_FAN_CPU:
			*pData = (nCtrlReg >> 8) & 0x01 ? SEMA_FAN_TEMP_SYS : SEMA_FAN_TEMP_CPU;
			break;
		case SEMA_FAN_SYSTEM_1:
			*pData = (nCtrlReg >> 13) & 0x01 ? SEMA_FAN_TEMP_SYS : SEMA_FAN_TEMP_CPU;
			break;
		default:
			return EAPI_STATUS_UNSUPPORTED;
		}
	}

	return eRet;
}


EERROR CECFunct::SetSmartFanTempPoints(uint32_t nFanNr, char* pData, uint32_t nSize)
{
	uint8_t bCmd;

	switch (nFanNr)
	{
	case SEMA_FAN_CPU:
		bCmd = EC_RW_CPU_TMP_REG;
		break;
	case SEMA_FAN_SYSTEM_1:
		bCmd = EC_RW_SYS_TMP_REG;
		break;
	default:
		return EAPI_STATUS_ERROR;
	}

	m_clsECTrans.ECWrite(bCmd + TMP_LVL_1_INDEX, EC_REGION_1, (uint8_t*)&(pData[0]), 1);
	m_clsECTrans.ECWrite(bCmd + TMP_LVL_2_INDEX, EC_REGION_1, (uint8_t*)&(pData[1]), 1);
	m_clsECTrans.ECWrite(bCmd + TMP_LVL_3_INDEX, EC_REGION_1, (uint8_t*)&(pData[2]), 1);
	m_clsECTrans.ECWrite(bCmd + TMP_LVL_4_INDEX, EC_REGION_1, (uint8_t*)&(pData[3]), 1);

	return EAPI_STATUS_SUCCESS;
}

EERROR CECFunct::SetSmartFanPWMPoints(uint32_t nFanNr, char* pData, uint32_t nSize)
{
	uint8_t bCmd;

	switch (nFanNr)
	{
	case SEMA_FAN_CPU:
		bCmd = EC_RW_CPU_TMP_REG;
		break;
	case SEMA_FAN_SYSTEM_1:
		bCmd = EC_RW_SYS_TMP_REG;
		break;
	default:
		return EAPI_STATUS_ERROR;
	}

	if (nSize >= 0x04)
	{
		m_clsECTrans.ECWrite(bCmd + PWM_LVL_1_INDEX, EC_REGION_1, (uint8_t*)&(pData[0]), 1);
		m_clsECTrans.ECWrite(bCmd + PWM_LVL_2_INDEX, EC_REGION_1, (uint8_t*)&(pData[1]), 1);
		m_clsECTrans.ECWrite(bCmd + PWM_LVL_3_INDEX, EC_REGION_1, (uint8_t*)&(pData[2]), 1);
		m_clsECTrans.ECWrite(bCmd + PWM_LVL_4_INDEX, EC_REGION_1, (uint8_t*)&(pData[3]), 1);
		return  EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}

EERROR CECFunct::SetSmartFanMode(uint32_t nFanNr, uint8_t bMode)
{
	EERROR eRet;
	uint32_t nCtrlReg = 0x00, nDataSet = 0x00;

	if ((eRet = GetSysCtrlReg(&nCtrlReg)) != EAPI_STATUS_SUCCESS)
	{
		return eRet;
	}

	switch (nFanNr)
	{
	case SEMA_FAN_CPU:
		nCtrlReg &= 0xFFFFF9FF;
		nDataSet = 0x09;
		break;
	case SEMA_FAN_SYSTEM_1:
		nCtrlReg &= 0xFFFFE7FF;
		nDataSet = 0x0B;
		break;
	default:
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (bMode == SEMA_FAN_MODE_OFF)
	{
		nCtrlReg |= 1 << nDataSet;
	}

	if (bMode == SEMA_FAN_MODE_ON)
	{
		nCtrlReg |= 2 << nDataSet;
	}

	if (bMode == SEMA_FAN_MODE_SOFT_FAN)
	{
		if (FindCapExt(SEMA_CAP_SOFT_FAN) == 0)
		{
			return EAPI_STATUS_UNSUPPORTED;
		}
		nCtrlReg |= 3 << nDataSet;
	}

	return SetSysCtrlReg(nCtrlReg);
}

EERROR CECFunct::SetSmartFanTempSrc(uint32_t nFanNr, uint8_t bSrc)
{
	EERROR eRet;
	uint32_t nCtrlReg = 0x00, nDataSet = 0x00;

	if ((eRet = GetSysCtrlReg(&nCtrlReg)) != EAPI_STATUS_SUCCESS)
	{
		return eRet;
	}

	switch (nFanNr)
	{
	case SEMA_FAN_CPU:
		nDataSet = 8;
		break;
	case SEMA_FAN_SYSTEM_1:
		nDataSet = 13;
		break;
	default:
		return EAPI_STATUS_ERROR;
	}

	if (bSrc == SEMA_FAN_TEMP_CPU)
	{
		nCtrlReg &= ~(1 << nDataSet);
	}

	if (bSrc == SEMA_FAN_TEMP_SYS)
	{
		nCtrlReg |= 1 << nDataSet;
	}

	return SetSysCtrlReg(nCtrlReg);
}

EERROR CECFunct::GetFanSpeed(uint32_t nFanNr, uint16_t* pushSpeed)
{
	EERROR eRet;
	uint16_t fan_addr;

	fan_addr = (nFanNr * 2) + EC_RW_FAN_CUR_SPEED;

	eRet = m_clsECTrans.ECRead(fan_addr, EC_REGION_1, (uint8_t*)pushSpeed, 2);

	return eRet;
}