#include "stdafx.h"
#include "SMBusFunct.h"


EERROR CSMBusFunct::GetSmartFanSupport(uint32_t FanId)
{
	unsigned int fanid_list[] = { SEMA_CAP_FAN_CPU, SEMA_CAP_FAN_1, \
		SEMA_CAP_FAN_2, SEMA_CAP_FAN_3};
	
	if ((!!(m_nSemaCaps & fanid_list[FanId])) != 1)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}
	return EAPI_STATUS_SUCCESS;
}

EERROR CSMBusFunct::GetSmartFanTempPoints(uint32_t nFanNr, char* pData, uint32_t nSize)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;
	uint8_t bCmd;

	ZeroMemory(pDataRet, MAX_PATH);

	switch (nFanNr)
	{
	case SEMA_FAN_CPU:
		bCmd = SEMA_CMD_SFC_CPU_TMP_SET_PTS;
		break;
	case SEMA_FAN_SYSTEM_1:
		bCmd = SEMA_CMD_SFC_SYS_1_TMP_SET_PTS; 
		break;
	case SEMA_FAN_SYSTEM_2: 
		bCmd = SEMA_CMD_SFC_SYS_2_TMP_SET_PTS;
		break;
	case SEMA_FAN_SYSTEM_3:
		bCmd = SEMA_CMD_SFC_SYS_3_TMP_SET_PTS;
		break;
	}

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, bCmd, NULL, \
		0x00, pDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nRet <= nSize)
		{
			memcpy_s(pData, nSize, pDataRet, nRet);
			return EAPI_STATUS_SUCCESS;
		}
		else
		{
			memcpy_s(pData, nSize, pDataRet, nSize);
			return EAPI_STATUS_MORE_DATA;
		}
	}

	return eRet;
}

EERROR CSMBusFunct::GetSmartFanPWMPoints(uint32_t nFanNr, char* pData, uint32_t nSize)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;
	uint8_t bCmd;

	ZeroMemory(pDataRet, MAX_PATH);

	switch (nFanNr)
	{
	case SEMA_FAN_CPU:
		bCmd = SEMA_CMD_SFC_CPU_PWM_SET_PTS;
		break;
	case SEMA_FAN_SYSTEM_1:
		bCmd = SEMA_CMD_SFC_SYS_1_PWM_SET_PTS;
		break;
	case SEMA_FAN_SYSTEM_2:
		bCmd = SEMA_CMD_SFC_SYS_2_PWM_SET_PTS; 
		break;
	case SEMA_FAN_SYSTEM_3: 
		bCmd = SEMA_CMD_SFC_SYS_3_PWM_SET_PTS; 
		break;
	default:
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, bCmd, NULL, \
		0x00, pDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nSize >= nRet)
		{
			memcpy_s(pData, nSize, pDataRet, nRet);
			return EAPI_STATUS_SUCCESS;
		}
		else
		{
			memcpy_s(pData, nSize, pDataRet, nSize);
			return EAPI_STATUS_MORE_DATA;
		}
	}

	return eRet;
}

EERROR CSMBusFunct::GetSmartFanMode(uint32_t nFanNr, uint8_t* pData)
{
	EERROR eRet;
	uint32_t nCtrlReg = 0x00;

	if ((eRet = GetSysCtrlReg(&nCtrlReg)) == EAPI_STATUS_SUCCESS)
	{
		switch (nFanNr)
		{
		case SEMA_FAN_CPU:
			nCtrlReg = (nCtrlReg >> 0x09) & 0x03; 
			break;
		case SEMA_FAN_SYSTEM_1:
			nCtrlReg = (nCtrlReg >> 0x0B) & 0x03; 
			break;
		case SEMA_FAN_SYSTEM_2:	
			nCtrlReg = (nCtrlReg >> 0x11) & 0x03;
			break;
		case SEMA_FAN_SYSTEM_3:	
			nCtrlReg = (nCtrlReg >> 0x14) & 0x03;
			break;
		default: eRet = EAPI_STATUS_ERROR;
		}

		if (eRet == EAPI_STATUS_SUCCESS)
		{
			if (nCtrlReg <= 0x03)
			{
				*pData = (uint8_t)nCtrlReg;
				return EAPI_STATUS_SUCCESS;
			}
			else
			{
				return EAPI_STATUS_ERROR;
			}
		}
	}

	return eRet;
}

EERROR CSMBusFunct::GetSmartFanTempSrc(uint32_t nFanNr, uint8_t* pData)
{
	EERROR eRet;
	uint32_t nCtrlReg = 0x00;

	if ((eRet = GetSysCtrlReg(&nCtrlReg)) == EAPI_STATUS_SUCCESS)
	{
		switch (nFanNr)
		{
		case SEMA_FAN_CPU:		
			*pData = (nCtrlReg >> 0x08) & 0x01 ? SEMA_FAN_TEMP_SYS : SEMA_FAN_TEMP_CPU; 
			break;
		case SEMA_FAN_SYSTEM_1:	
			*pData = (nCtrlReg >> 0x0D) & 0x01 ? SEMA_FAN_TEMP_SYS : SEMA_FAN_TEMP_CPU; 
			break;
		case SEMA_FAN_SYSTEM_2:	
			*pData = (nCtrlReg >> 0x10) & 0x01 ? SEMA_FAN_TEMP_SYS : SEMA_FAN_TEMP_CPU;
			break;
		case SEMA_FAN_SYSTEM_3:	
			*pData = (nCtrlReg >> 0x17) & 0x01 ? SEMA_FAN_TEMP_SYS : SEMA_FAN_TEMP_CPU; 
			break;
		default: eRet = EAPI_STATUS_ERROR;
		}
	}

	return eRet;
}

EERROR CSMBusFunct::SetSmartFanTempPoints(uint32_t nFanNr, char* pData, uint32_t nSize)
{
	uint8_t bCmd = SEMA_CMD_SFC_CPU_TMP_SET_PTS;
	
	switch (nFanNr)
	{
	case SEMA_FAN_CPU: 
		bCmd = SEMA_CMD_SFC_CPU_TMP_SET_PTS;
		break;
	case SEMA_FAN_SYSTEM_1: 
		bCmd = SEMA_CMD_SFC_SYS_1_TMP_SET_PTS; 
		break;
	case SEMA_FAN_SYSTEM_2: 
		bCmd = SEMA_CMD_SFC_SYS_2_TMP_SET_PTS; 
		break;
	case SEMA_FAN_SYSTEM_3: 
		bCmd = SEMA_CMD_SFC_SYS_3_TMP_SET_PTS;
		break;
	default:
		return EAPI_STATUS_ERROR;
	}
	
	return m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, bCmd, (uint8_t*)pData, 0x04);
}

EERROR CSMBusFunct::SetSmartFanPWMPoints(uint32_t nFanNr, char* pData, uint32_t nSize)
{
	EERROR eRet;
	uint8_t bCmd = 0;

	switch (nFanNr)
	{
	case SEMA_FAN_SYSTEM_1: bCmd = SEMA_CMD_SFC_SYS_1_PWM_SET_PTS; break;
	case SEMA_FAN_SYSTEM_2: bCmd = SEMA_CMD_SFC_SYS_2_PWM_SET_PTS; break;
	case SEMA_FAN_SYSTEM_3: bCmd = SEMA_CMD_SFC_SYS_3_PWM_SET_PTS; break;
	case SEMA_FAN_CPU: bCmd = SEMA_CMD_SFC_CPU_PWM_SET_PTS; break;
	default:
		return EAPI_STATUS_ERROR;
	}

	if (nSize >= 0x04)
	{
		eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, bCmd, (uint8_t*)pData, 0x04);
	}
	else
	{
		eRet = EAPI_STATUS_WRITE_ERROR;
	}

	return eRet;
}

EERROR CSMBusFunct::SetSmartFanMode(uint32_t nFanNr, uint8_t bMode)
{
	uint32_t nCtrlReg = 0x00;
	uint32_t nDataSet = 0x00;

	EERROR eRet = GetSysCtrlReg(&nCtrlReg);

	if (eRet != EAPI_STATUS_SUCCESS)
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
	case SEMA_FAN_SYSTEM_2:	
		nCtrlReg &= 0xFFF9FFFF; 
		nDataSet = 0x11;
		break;
	case SEMA_FAN_SYSTEM_3:	
		nCtrlReg &= 0xFFCFFFFF; 
		nDataSet = 0x14;
		break;
	default: eRet = EAPI_STATUS_ERROR;
	}

	if (eRet == EAPI_STATUS_SUCCESS)
	{
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
			if(FindCapExt(SEMA_CAP_SOFT_FAN) == 0)
			{
				return EAPI_STATUS_UNSUPPORTED;
			}
			nCtrlReg |= 3 << nDataSet;
		}

		eRet = SetSysCtrlReg(nCtrlReg);
	}
	return eRet;
}

EERROR CSMBusFunct::SetSmartFanTempSrc(uint32_t nFanNr, uint8_t bSrc)
{
	uint32_t nCtrlReg = 0x00, nDataSet = 0x00;

	EERROR eRet;

	if ((eRet = GetSysCtrlReg(&nCtrlReg)) != EAPI_STATUS_SUCCESS)
	{
		return eRet;
	}

	switch (nFanNr)
	{
	case SEMA_FAN_CPU:	
		nDataSet = 0x08; 
		break;
	case SEMA_FAN_SYSTEM_1:
		nDataSet = 0x0D; 
		break;
	case SEMA_FAN_SYSTEM_2:
		nDataSet = 0x10; 
		break;
	case SEMA_FAN_SYSTEM_3:
		nDataSet = 0x17; 
		break;
	default:
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (bSrc == SEMA_FAN_TEMP_CPU)
	{
		nCtrlReg &= ~(1 << nDataSet);
	}

	if (bSrc == SEMA_FAN_TEMP_SYS)
	{
		nCtrlReg |= 1 << nDataSet;
	}

	return  SetSysCtrlReg(nCtrlReg);
}


EERROR CSMBusFunct::GetFanSpeed(uint32_t nFanNr, uint16_t* pushSpeed)
{
	uint8_t bCmd = SEMA_CMD_RD_CPU_FAN;
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nDataRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);

	if ((nFanNr >= SEMA_FAN_SYSTEM_1) && (nFanNr <= SEMA_FAN_SYSTEM_3))
	{
		bCmd = SEMA_CMD_RD_SYSTEM_FAN_1 + (nFanNr - 0x01);
	}

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, bCmd, NULL,\
		0x00, (uint8_t*)pDataRet, nDataRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nDataRet == 2)
		{
			*pushSpeed = (pDataRet[0x00] << 0x08) + pDataRet[0x01];
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}

	return eRet;
}