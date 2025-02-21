#include "stdafx.h"
#include "stdio.h"
#include <string>

#include "SMBusFunct.h"

CSMBusFunct::CSMBusFunct(Ccommon_bmc &cmn)
{
	m_bBMCAdr = 0x50;
	m_clsCmn = cmn;

	m_nSemaCaps = SEMA_CAP_NONE;
	m_nSemaCapsEx = SEMA_CAP_NONE;
	m_n_GPIO_Cur_Id = -1;

	m_bDTSTemp = FALSE;
	m_bInit = FALSE;

	m_nTotalChannel = 0x00;
	m_nTotalScale = 0x00;

	ZeroMemory(m_tbScale, sizeof(uint16_t) * MAX_DESC_NR_CHANNEL);
}

CSMBusFunct::~CSMBusFunct()
{
	m_nSemaCaps = SEMA_CAP_NONE;
	m_nSemaCapsEx = SEMA_CAP_NONE;

	m_clsCmn.Close();
}
void GPIO_Registry_read(DWORD *value, int GPIO_Dir_Level);

EERROR CSMBusFunct::Init()
{
	char szVersion[MAX_PATH];
	uint8_t pDataRet[MAX_PATH];
	uint32_t nRet = MAX_PATH, nCount = 0x00;
	uint8_t bSelChannel = 0;

	ZeroMemory(pDataRet, MAX_PATH);

	if (m_clsSemaTrans.Init(m_clsCmn.m_hDrv) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_NOT_FOUND;
	}

	if (GetVersion(szVersion, MAX_PATH) == EAPI_STATUS_SUCCESS)
	{
		m_bInit = TRUE;
		if (strstr(szVersion, "LiPPERT") || strstr(szVersion, "ADLINK"))
		{
			while (tbBoard[nCount] != NULL)
			{
				if (strstr(szVersion, tbBoard[nCount]))
				{
					break;
				}
				nCount++;
			}

			if (m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_CAPABILITIES, NULL, 0x00, pDataRet, nRet) == EAPI_STATUS_SUCCESS)
			{
				switch (nRet)
				{
				case 0x02:	m_nSemaCaps = (pDataRet[0x00] << 0x08) + pDataRet[0x01]; break;
				case 0x04:  m_nSemaCaps = (pDataRet[0x00] << 0x18) + (pDataRet[0x01] << 0x10) + (pDataRet[0x02] << 0x08) + pDataRet[0x03]; break;
				case 0x05:
					m_nSemaCapsEx = pDataRet[0x00];
					m_nSemaCaps = (pDataRet[0x01] << 0x18) + (pDataRet[0x02] << 0x10) + (pDataRet[0x03] << 0x08) + pDataRet[0x04];
					break;

				case 0x08:
					m_nSemaCapsEx = (pDataRet[0x00] << 0x18) + (pDataRet[0x01] << 0x10) + (pDataRet[0x02] << 0x08) + pDataRet[0x03];
					m_nSemaCaps = (pDataRet[0x04] << 0x18) + (pDataRet[0x05] << 0x10) + (pDataRet[0x06] << 0x08) + pDataRet[0x07];
					break;

				default:
					m_nSemaCapsEx = -1;
					m_nSemaCaps = -1;
				}
			}
			else
			{
				return EAPI_STATUS_ERROR;
			}

			if (m_nSemaCaps & SEMA_CAP_TEMPERATURES)
			{
				uint8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;
				ZeroMemory(pDataRet, MAX_PATH);
				if (m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_CPUTEMPCUR, NULL, \
					0x00, pDataRet, nRet) == EAPI_STATUS_SUCCESS)
				{
					m_bDTSTemp = (nRet == 0x02) ? FALSE : TRUE;
				}
			}

			if (m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_GET_EXT_VOLT_DESCR, \
				&bSelChannel, 0x01) == EAPI_STATUS_SUCCESS)
			{
				while (m_nTotalChannel < MAX_DESC_NR_CHANNEL)
				{
					nRet = MAX_PATH;
					ZeroMemory(pDataRet, MAX_PATH);
					if (m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, \
						SEMA_CMD_GET_EXT_VOLT_DESCR, NULL, 0x00, pDataRet, nRet) == EAPI_STATUS_SUCCESS)
					{
						if (m_nTotalChannel > 0x00)
						{
							if (strstr(m_tbDesc[0x00], (char*)pDataRet) == m_tbDesc[0x00])
							{
								break;
							}
						}
						memcpy_s(m_tbDesc[m_nTotalChannel], MAX_DESC_LEN, pDataRet, nRet > (MAX_DESC_LEN - 0x01) ? \
							(MAX_DESC_LEN - 0x01) : nRet);
					}
					m_nTotalChannel++;
				}
			}

			
			if (m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_GET_ADC_SCALE, \
				NULL, 0x00, pDataRet, nRet) == EAPI_STATUS_SUCCESS)
			{
				uint8_t bStep = 0x00;

				while (bStep < nRet)
				{
					m_tbScale[bStep / 0x02] = (pDataRet[bStep] << 0x08) + pDataRet[bStep + 0x01];
					bStep += 0x02;
				}
				
				m_nTotalScale = (nRet / 0x02) - 0x01;
			}
			

			if (strstr(szVersion, "cExpress-SL/KL"))
			{
				DWORD gpio=0;
				DWORD bitmask=0;
				GPIO_Registry_read(&gpio, 0);
				SetGPIODir(gpio,bitmask);
				GPIO_Registry_read(&gpio, 1);
				SetGPIO(gpio,bitmask);
			}
			return EAPI_STATUS_SUCCESS;
		}
	}

	return EAPI_STATUS_NOT_FOUND;
}


void GPIO_Registry_read(DWORD *value, int GPIO_Dir_Level)
{
	DWORD dwRet, cbData;
	HKEY hkey;
	BYTE *byte_value;

	dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		L"Software\\SEMA_TOOL",
		0,
		KEY_READ,
		&hkey);

	cbData = sizeof(BYTE) * 4;
	byte_value = (BYTE*)malloc(sizeof(BYTE) * 4);

	if (GPIO_Dir_Level == 0)
		dwRet = RegGetValueA(hkey, NULL, "PCA9535_GPIO0_DIR", RRF_RT_DWORD, NULL,(PVOID) byte_value, &cbData);
	else if (GPIO_Dir_Level == 1)
		dwRet = RegGetValueA(hkey, NULL, "PCA9535_GPIO0_Value", RRF_RT_DWORD, NULL,(PVOID) byte_value, &cbData);

	*value= byte_value[0];

	RegCloseKey(hkey);

}
bool CSMBusFunct::IsInitialiezed()
{
	return m_bInit;
}

uint32_t CSMBusFunct::GetCaps()
{
	return m_nSemaCaps;
}

uint32_t CSMBusFunct::GetCapsExt()
{
	return m_nSemaCapsEx;
}


uint32_t CSMBusFunct::FindCap(uint32_t cap)
{
	return !!(m_nSemaCaps & cap);
}

uint32_t CSMBusFunct::FindCapExt(uint32_t cap)
{
	return !!(m_nSemaCapsEx & cap);
}


EERROR CSMBusFunct::GetVersion(char* szVersion, uint32_t nLen)
{
	uint32_t nDataRet = MAX_PATH;
	char szDataRet[MAX_PATH];
	uint32_t nDataRet2;

	ZeroMemory(szVersion, nLen);
	ZeroMemory(szDataRet, MAX_PATH);
	
	if (m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_VERSION1, \
		NULL, 0x00, (uint8_t*)szDataRet, nDataRet) == EAPI_STATUS_SUCCESS)
	{
		nDataRet2 = MAX_PATH - nDataRet;
		if (m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_VERSION2, \
			NULL, 0x00, (uint8_t*)szDataRet + nDataRet, nDataRet2) == EAPI_STATUS_SUCCESS)
		{
			nDataRet += nDataRet2;
			memcpy_s(szVersion, nLen, szDataRet, nDataRet > nLen ? nLen : nDataRet);
			return (nDataRet > nLen ? EAPI_STATUS_ERROR : EAPI_STATUS_SUCCESS);
		}
	}

	return EAPI_STATUS_ERROR;
}

EERROR CSMBusFunct::GetBootVersion(char* szVersion, uint32_t nLen)
{
	EERROR eRet;
	char szDataRet[MAX_PATH]; uint32_t nDataRet = MAX_PATH;

	ZeroMemory(szDataRet, MAX_PATH);
	ZeroMemory(szVersion, nLen);

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_BLVERSION, NULL,\
		0x00, (uint8_t*)szDataRet, nDataRet)) == EAPI_STATUS_SUCCESS)
	{
		if (szDataRet[0x00] != 0xF0)
		{
			eRet = (nLen - 0x01) > nDataRet ? EAPI_STATUS_SUCCESS : EAPI_STATUS_MORE_DATA;
			if (eRet == EAPI_STATUS_SUCCESS)
			{
				memcpy_s(szVersion, nLen, szDataRet, (nLen - 0x01) > nDataRet ? nDataRet : (nLen - 0x01));
			}
		}
	}
	return eRet;
}

EERROR CSMBusFunct::GetBootVersionExt(char* szVersion, uint32_t nLen)
{
	EERROR eRet = EAPI_STATUS_ERROR; 
	uint32_t nDataRet;
	uint32_t nRetSize;
	char* szVerRet;

	ZeroMemory(szVersion, nLen);

	char szVer1[MAX_PATH], szVer2[MAX_PATH];
	if (((eRet = GetBootVersion(szVer1, MAX_PATH)) == EAPI_STATUS_SUCCESS) \
		&& ((eRet = GetBootVersion(szVer2, MAX_PATH)) == EAPI_STATUS_SUCCESS))
	{
		switch (strcmp(szVer1, szVer2))
		{
		case 0x00:
			
			nDataRet = (uint32_t)strlen(szVer1);
			eRet = (nLen - 0x01) > nDataRet ? EAPI_STATUS_SUCCESS : EAPI_STATUS_MORE_DATA;
			memcpy_s(szVersion, nLen, szVer1, (nLen - 0x01) > nDataRet ? nDataRet : (nLen - 0x01));
			
			break;

		default:
			
			nRetSize = (uint32_t)(strlen(szVer1) + strlen(szVer1) + 0x01);
			szVerRet = new char[nRetSize];
		
			ZeroMemory(szVerRet, nRetSize);

			strcpy_s(szVerRet, nRetSize, szVer1);
			strcat_s(szVerRet, nRetSize, szVer2);

			eRet = (nLen - 0x01) > nRetSize ? EAPI_STATUS_SUCCESS : EAPI_STATUS_MORE_DATA;


			memcpy_s(szVersion, nLen, szVerRet, (nLen - 0x01) > nRetSize ? nRetSize : (nLen - 0x01));
			delete[] szVerRet;
		}
	}

	return eRet;
}

EERROR CSMBusFunct::GetManufData(uint32_t nDataInfo, uint8_t* pData, uint32_t nLen)
{
	uint32_t nDataRet = nLen;
	
	ZeroMemory(pData, nLen);
	
	if (nDataInfo > SEMA_MANU_DATA_MAX)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (nDataInfo == SEMA_MANU_DATA_MACID_2)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_MF_DATA + nDataInfo, \
		NULL, 0x00, pData, nDataRet) == EAPI_STATUS_SUCCESS)
	{
		if (!(((nDataRet == 0x01) && (pData[0x00] == 0xF0)) || (pData[0x00] == 0xFF) || \
			(pData[0x00] == ' ')))
		{
			return EAPI_STATUS_SUCCESS;
		}
		ZeroMemory(pData, nLen);
	}

	return EAPI_STATUS_ERROR;
}

EERROR CSMBusFunct::SetManufData(uint32_t nDataInfo, uint8_t* pData, uint32_t nLen)
{
	uint8_t pData_[10] = { 0 }, to_send[0x10] = { 0 };
	
	memcpy(to_send, pData, nLen);

	if (nDataInfo < 0 || nDataInfo > 6)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (nLen <= 0 || pData == NULL)
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}

	if (nLen > 0x10)
	{
		return EAPI_STATUS_MORE_DATA;
	}

	pData_[0] = 0x04;
	pData_[1] = 0x10 * nDataInfo;
	pData_[2] = 0x10;

	if(m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_SET_ADDRESS, pData_, 3) == EAPI_STATUS_SUCCESS)
	{
		if (m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_WRITE_DATA, to_send, 0x10) != EAPI_STATUS_SUCCESS)
		{
			return EAPI_STATUS_ERROR;
		}

	}

	return EAPI_STATUS_SUCCESS;
}

EERROR CSMBusFunct::GetTotalUpTime(uint32_t* pnTotalUpTime)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nDataRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_TOM, NULL, \
		0x00, (uint8_t*)pDataRet, nDataRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nDataRet == 4)
		{
			*pnTotalUpTime = (pDataRet[0x00] << 0x18) + (pDataRet[0x01] << 0x10) + \
				(pDataRet[0x02] << 0x08) + pDataRet[0x03];
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}

	return eRet;
}

EERROR CSMBusFunct::GetPwrUpTime(uint32_t* pnPwrUpTime)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nDataRet = MAX_PATH;
			
	ZeroMemory(pDataRet, MAX_PATH);

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_PWRUPSECS, \
		NULL, 0x00, (uint8_t*)pDataRet, nDataRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nDataRet == 4)
		{
			*pnPwrUpTime = (pDataRet[0x00] << 0x18) + (pDataRet[0x01] << 0x10)\
				+ (pDataRet[0x02] << 0x08) + pDataRet[0x03];
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}
	return eRet;
}

EERROR CSMBusFunct::GetPwrCycles(uint32_t* pnPwrCycle)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nDataRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_PWRCYCLES, \
		NULL, 0x00, (uint8_t*)pDataRet, nDataRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nDataRet == 4)
		{
			*pnPwrCycle = (pDataRet[0x00] << 0x18) + (pDataRet[0x01] << 0x10) + \
				(pDataRet[0x02] << 0x08) + pDataRet[0x03];
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}
	return eRet;
}

EERROR CSMBusFunct::GetBootCount(uint32_t* pnBootCount)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nDataRet = MAX_PATH;
	
	ZeroMemory(pDataRet, MAX_PATH);

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_BOOTCOUNT, \
		NULL, 0x00, (uint8_t*)pDataRet, nDataRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nDataRet == 4)
		{
			*pnBootCount = (pDataRet[0x00] << 0x18) + (pDataRet[0x01] << 0x10) + \
				(pDataRet[0x02] << 0x08) + pDataRet[0x03];
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}
	return eRet;
}

EERROR CSMBusFunct::GetBMCFlags(uint8_t* pBMCFlag)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nDataRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_BMC_FLAGS, \
		NULL, 0x00, (uint8_t*)pDataRet, nDataRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nDataRet == 1)
		{
			*pBMCFlag = pDataRet[0x00];
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}
	return eRet;
}

EERROR CSMBusFunct::GetBMCStatus(uint8_t* pBMCStatus)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nDataRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_BMC_STATUS, \
		NULL, 0x00, (uint8_t*)pDataRet, nDataRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nDataRet == 1)
		{
			*pBMCStatus = pDataRet[0x00];
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}
	return eRet;
}

EERROR CSMBusFunct::GetRestartEvent(uint8_t* pEvent)
{
	uint8_t pDataRet[MAX_PATH]; uint32_t nDataRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);

	if (m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_RESTARTEVT, \
		NULL, 0x00, (uint8_t*)pDataRet, nDataRet) == EAPI_STATUS_SUCCESS)
	{
		if (nDataRet == 1)
		{
			*pEvent = pDataRet[0x00];
			return EAPI_STATUS_SUCCESS;
		}
	}

	return EAPI_STATUS_ERROR;
}

EERROR CSMBusFunct::GetRestartEventText(char* szVersion, uint32_t nSize)
{
	EERROR eRet;
	char* szRet = "";
	uint8_t bEvent = 0x00;

	if ((eRet = GetRestartEvent(&bEvent)) == EAPI_STATUS_SUCCESS)
	{
		switch (bEvent)
		{
		case SEMA_SRE_UNKNOWN:		szRet = "Unknown"; break;
		case SEMA_SRE_SW_RESET:		szRet = "Software Reset"; break;
		case SEMA_SRE_HW_RESET:		szRet = "Hardware Reset"; break;
		case SEMA_SRE_WATCHDOG:		szRet = "Watchdog"; break;
		case SEMA_SRE_BIOS_FAULT:	szRet = "BIOS fault"; break;
		case SEMA_SRE_PWR_DOWN:		szRet = "Power down"; break;
		case SEMA_SRE_PWR_LOSS:		szRet = "Power loss"; break;
		case SEMA_SRE_PWR_CYCLE:	szRet = "Power cycle"; break;
		case SEMA_SRE_VIN_DROP:		szRet = "Drop of main input voltage"; break;
		case SEMA_SRE_PWR_FAIL:		szRet = "Power fail"; break;
		case SEMA_SRE_CRIT_TEMP:	szRet = "Critical CPU shutdown"; break;
		case SEMA_SRE_WAKEUP:		szRet = "Wake up"; break;
		default:					szRet = "Invalid value"; break;
		}
		memset(szVersion, 0, nSize);
		uint32_t nLen = (uint32_t)strlen(szRet);
		eRet = nLen > (nSize - 0x01) ? EAPI_STATUS_MORE_DATA : EAPI_STATUS_SUCCESS;
		memcpy_s(szVersion, nSize, szRet, nLen > (nSize - 0x01) ? (nSize - 0x01) : nLen);
	}

	return eRet;
}


EERROR CSMBusFunct::GetSysCtrlReg(uint32_t* pnCtrlReg)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;

	*pnCtrlReg = 0x00;
	ZeroMemory(pDataRet, MAX_PATH);
	
	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_SYSCFG, NULL,\
		0x00, pDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		switch (nRet)
		{
		case 0x00:	*pnCtrlReg = 0x00;
			break;
		case 0x02:	*pnCtrlReg = pDataRet[0x00] + (pDataRet[0x01] << 0x08); 
			break;
		case 0x03:	*pnCtrlReg = pDataRet[0x00] + (pDataRet[0x01] << 0x08)\
			+ (pDataRet[0x02] << 0x10); 
			break;
		case 0x04:	*pnCtrlReg = pDataRet[0x00] + (pDataRet[0x01] << 0x08)\
			+ (pDataRet[0x02] << 0x10)	+ (pDataRet[0x03] << 0x18); 
			break;
		default: eRet = EAPI_STATUS_ERROR;
		}
	}
	return eRet;
}


EERROR CSMBusFunct::GetScaleFactor(uint8_t bChannel, uint16_t* pushScale)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);
	*pushScale = 0x00;

	if (m_nTotalScale != 0x00 && bChannel <= m_nTotalScale)
	{
		*pushScale = m_tbScale[bChannel];
		eRet = EAPI_STATUS_SUCCESS;
	}
	else
	{
		eRet = EAPI_STATUS_UNSUPPORTED;
	}

	return eRet;
}

EERROR CSMBusFunct::GetVolt(uint8_t bChannel, float* pfVoltage)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;
	uint16_t usScale = 0x00, ushValue;

	ZeroMemory(pDataRet, MAX_PATH);
	*pfVoltage = 0.0f;

	if (bChannel > m_nTotalScale)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, (bChannel >= 0x08) ? \
		(SEMA_CMD_RD_AIN8 + (bChannel - 0x08)) : (SEMA_CMD_RD_AIN0 + bChannel), NULL, \
		0x00, pDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nRet == 0x02)
		{
			if ((eRet = GetScaleFactor(bChannel, &usScale)) == EAPI_STATUS_SUCCESS)
			{
				ushValue = (pDataRet[0x00] << 0x08) + pDataRet[0x01];
				*pfVoltage = ((float)3.3 / 1024 / 1000) * usScale * ushValue;
			}
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}

	return eRet;
}


EERROR CSMBusFunct::GetVoltDescEx(uint8_t bChannel, char* pData)
{
	EERROR eRet;
	uint8_t bCount = 0x00;
	bool bStop = false;
	uint8_t bSelChannel = 0x00;
	uint8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);

	if (bChannel > m_nTotalChannel)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_GET_EXT_VOLT_DESCR, \
		&bSelChannel, 0x01)) == EAPI_STATUS_SUCCESS)
	{
		while (bCount < m_nTotalChannel)
		{
			if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_GET_EXT_VOLT_DESCR, \
				NULL, 0x00, pDataRet, nRet)) == EAPI_STATUS_SUCCESS)
			{
				if (bChannel == bCount)
				{
					memcpy_s(pData, 20, pDataRet, nRet);
				}
			}
			else
			{
				break;
			}
			bCount++;
		}
	}

	return eRet;
}

EERROR CSMBusFunct::GetVoltDescExt(uint8_t bChannel, char* szDesc, uint32_t nSize, bool bTruncate)
{
	EERROR eRet;
	uint8_t bSelChannel = 0x00;
	ZeroMemory(szDesc, nSize);

	if (bChannel > m_nTotalChannel)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (m_nTotalChannel > 0x00)
	{
		if (bChannel < m_nTotalChannel)
		{
			uint32_t nDataLen = (uint32_t)strlen(m_tbDesc[bChannel]);
			if (bTruncate)
			{
				nDataLen -= 0x04;
			}

			if (nDataLen > 0x00)
			{
				eRet = nDataLen > nSize ? EAPI_STATUS_MORE_DATA : EAPI_STATUS_SUCCESS;
				memcpy_s(szDesc, nSize, bTruncate ? &m_tbDesc[bChannel][0x04] : m_tbDesc[bChannel], \
					nDataLen > nSize ? nSize : nDataLen);
				return eRet;
			}
		}
		else
		{
			eRet = EAPI_STATUS_UNSUPPORTED;
		}
	}

	return EAPI_STATUS_ERROR;
}

EERROR CSMBusFunct::GetCurrentChannel(uint8_t* pChannel)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;
	uint8_t bStep = 0x00; uint32_t nValue = 0x00;

	ZeroMemory(pDataRet, MAX_PATH);
	
	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_GET_VOLT_DESCR,\
		NULL, 0x00, pDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		while (bStep < nRet)
		{
			nValue = (pDataRet[bStep] << 0x08) + pDataRet[bStep + 0x01];
			if (nValue == 0x0F)
			{
				*pChannel = bStep / 0x02;
				return EAPI_STATUS_SUCCESS;
			}
			bStep += 0x02;
		}
	}

	return EAPI_STATUS_ERROR;
}

EERROR CSMBusFunct::GetMainPowerCurrent(uint16_t* pushPower)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_MPCURRENT,\
		NULL, 0x00, pDataRet, nRet)) == EAPI_STATUS_SUCCESS) 
	{
		uint8_t bChannel = -1;
		if ((eRet = GetCurrentChannel(&bChannel)) == EAPI_STATUS_SUCCESS)
		{
			uint16_t ushScale = 0x00;
			if ((eRet = GetScaleFactor(bChannel, &ushScale)) == EAPI_STATUS_SUCCESS)
			{
				double dbMux = ushScale;
				switch (nRet)
				{
				case 0x02:
				{
					*pushPower = (unsigned short)((dbMux * ((((unsigned short)pDataRet[0]) << 0x08) \
						+ pDataRet[0x01])) / 10) * 10;
				}break;

				case 0x08:
				{
					double	dbTmp = 0x00, dbMax = 0x00, dbBuf = 0x00;
					for (uint32_t nCount = 0; nCount < 4; nCount++)
					{
						// Main power current = buf[0]<<8+buf[1]*4.03mA
						// reduce resolution to 10mA increments
						dbTmp = ((((unsigned short)pDataRet[nCount * 2]) << 8) + pDataRet[(nCount * 2) + 1]);
						dbBuf += dbTmp;							// Add value to buffer

						if (dbMax < dbTmp)
						{
							dbMax = dbTmp;						// Save highest value so far
						}
					}
					// calculate average over lower three values
					dbBuf -= dbMax;							// Subtract highest value and ...
					*pushPower = (unsigned short)((((unsigned short)(dbBuf / 3))) *(3.3 / 1024) * (dbMux));

				}break;

				default: 
					eRet = EAPI_STATUS_ERROR;
				}
			}
		}
	}
	return eRet;
}

EERROR CSMBusFunct::GetBacklightPWM(uint8_t* pPWM)
{
	uint8_t pDataRet[MAX_PATH] = { 0 };
	uint32_t nDataRet = MAX_PATH;

	if (m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_BL_GETPWM, NULL,\
		0x00, (uint8_t*)pDataRet, nDataRet) == EAPI_STATUS_SUCCESS)
	{
		if (nDataRet == 1)
		{
			*pPWM = pDataRet[0x00];
			return EAPI_STATUS_SUCCESS;
		}
		else
		{
			return EAPI_STATUS_MORE_DATA;
		}
	}

	return EAPI_STATUS_ERROR;
}

EERROR CSMBusFunct::SetBacklightPWM(uint8_t bPWM)
{
	return m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_BL_SETPWM, &bPWM, 0x01);
}

EERROR CSMBusFunct::GetCurrentCPUTemp(int32_t* pfTemp)
{
	EERROR eRet;
	signed char buf[33] = { 0 };
	bool result = false; char szChipSet[MAX_PATH];
	uint32_t nTemp = 0x00;
	uint32_t nEax = 0x00, nEdx = 0x00;
	uint32_t nEax1 = 0x00, nEdx1 = 0x00;
	uint8_t pDataRet[0x02]; uint32_t nRet = 0x02;
	float pfTempL = 0;

	ZeroMemory(szChipSet, MAX_PATH);
	
	if ((eRet = m_clsCmn.GetChipSet(szChipSet, MAX_PATH)) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}
	
	if (strstr(szChipSet, "Intel") == NULL)
	{
		return EAPI_STATUS_ERROR;
	}
	
	if (m_bDTSTemp)
	{
		if ((eRet = m_clsCmn.Rdmsr(0x1a2, &nEax, &nEdx)) == EAPI_STATUS_SUCCESS)
		{
			nTemp = (nEax >> 0x10) & 0x7F;

			if ((eRet = m_clsCmn.Rdmsr(0x19c, &nEax1, &nEdx1)) == EAPI_STATUS_SUCCESS)
			{
				if ((nEax1 >> 4) & 0x1)
					buf[0] = (nTemp + ((nEax1 >> 0x10) & 0xFF));
				else
					buf[0] = (nTemp - ((nEax1 >> 0x10) & 0xFF));

				pfTempL = (float)buf[0];
				*pfTemp = EAPI_ENCODE_CELCIUS(pfTempL);

				return EAPI_STATUS_SUCCESS;
			
			}
		}

		return EAPI_STATUS_ERROR;
	}
	else
	{
		if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_CPUTEMPCUR, \
			NULL, 0x00, pDataRet, nRet)) == EAPI_STATUS_SUCCESS)
		{
			pfTempL = ((float)(pDataRet[0x00])) + ((pDataRet[0x00] & 0x80) ? \
				-((float)pDataRet[0x01] / 10) : (float)pDataRet[0x01] / 10);
			
			*pfTemp = EAPI_ENCODE_CELCIUS(pfTempL);
		}
	}

	return eRet;
}

EERROR CSMBusFunct::GetCurrentBoardTemp(int32_t* pchTemp)
{
	EERROR eRet;
	
	uint8_t pDataRet[MAX_PATH];
	uint32_t nDataRet = MAX_PATH;
	float pchTempL = 0;

	ZeroMemory(pDataRet, MAX_PATH);

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_BRDTEMPCUR, \
		NULL, 0x00, (uint8_t*)pDataRet, nDataRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nDataRet == 1)
		{
			*pchTemp = pDataRet[0x00];
			pchTempL = (float)(*pchTemp);
			*pchTemp = EAPI_ENCODE_CELCIUS(pchTempL);
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}
	return eRet;
}


EERROR CSMBusFunct::GetBRDMinMaxTemp(int* pchMinBoard, int* pchMaxBoard)
{
	EERROR eRet;
	int8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);

	*pchMinBoard = *pchMaxBoard = 0x00;

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_MINMAXTEMP,\
		NULL, 0x00, (uint8_t*)pDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nRet >= 0x04)
		{
			*pchMinBoard = (pDataRet[0x03] * 10) + 2731;
			*pchMaxBoard = (pDataRet[0x02] * 10) + 2731;
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}
	return eRet;
}

EERROR CSMBusFunct::GetCPUMinMaxTemp(int* pchMinCPU, int* pchMaxCPU)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);

	*pchMinCPU = *pchMaxCPU = 0x00;

	if (m_bDTSTemp == TRUE)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_MINMAXTEMP,\
		NULL, 0x00, pDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nRet >= 0x04)
		{
			*pchMinCPU = (pDataRet[0x01] * 10) + 2731;
			*pchMaxCPU = (pDataRet[0x00] * 10) + 2731;
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}
	return eRet;
}

EERROR CSMBusFunct::GetStartupTemp(int* pchStartBoard)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);

	*pchStartBoard = 0x00;

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_STARTTEMP,\
		NULL, 0x00, pDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nRet >= 0x02)
		{
			*pchStartBoard = (pDataRet[0x01] * 10) + 2731;
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}

	return eRet;
}

EERROR CSMBusFunct::GetCPUStartupTemp(int* pchStartCPU)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;
	
	ZeroMemory(pDataRet, MAX_PATH);

	*pchStartCPU = 0x00;

	if (m_bDTSTemp == TRUE)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_STARTTEMP, \
		NULL, 0x00, pDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nRet >= 0x02)
		{
			*pchStartCPU = (pDataRet[0x00] * 10) + 2731;
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}
	return eRet;
}

EERROR CSMBusFunct::GetCurrentBoardTemp_1(int* pchBoardTemp)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nDataRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_2ND_BRDTEMPCUR, \
		NULL, 0x00, (uint8_t*)pDataRet, nDataRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nDataRet == 1)
		{
			*pchBoardTemp = (pDataRet[0x00] * 10) + 2731;
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}
	
	return eRet;
}

EERROR CSMBusFunct::GetMinMaxTemp_1(int* pchMinCPU, int* pchMaxCPU, int* pchMinBoard, int* pchMaxBoard)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);


	*pchMinCPU = *pchMaxCPU = *pchMinBoard = *pchMaxBoard = 0x00;
	
	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_MINMAXTEMP, \
		NULL, 0x00, pDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nRet >= 0x04)
		{
			*pchMaxBoard = (pDataRet[0x02] * 10) + 2731;
			*pchMinBoard = (pDataRet[0x03] * 10) + 2731;
			
			if (m_bDTSTemp)
			{
				*pchMaxCPU = SCHAR_MAX;	
				*pchMinCPU = SCHAR_MIN;
			}
			else
			{
				*pchMinCPU = pDataRet[0x01];	
				*pchMaxCPU = pDataRet[0x00];
			}
			*pchMinCPU = (*pchMinCPU * 10) + 2731;
			*pchMaxCPU = (*pchMaxCPU * 10) + 2731;
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}
	
	return eRet;
}

EERROR CSMBusFunct::GetStartupTemp_1(int* pchStartCPU, int* pchStartBoard)
{
	EERROR eRet;
	uint8_t pDataRet[MAX_PATH]; uint32_t nRet = MAX_PATH;

	ZeroMemory(pDataRet, MAX_PATH);

	*pchStartCPU = 0x00;
	*pchStartBoard = 0x00;

	if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_RD_2ND_STARTTEMP, \
		NULL, 0x00, pDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		if (nRet >= 0x02)
		{
			*pchStartCPU = (pDataRet[0x00] * 10) + 2731;
			*pchStartBoard = (pDataRet[0x01] * 10) + 2731;
		}
		else
		{
			eRet = EAPI_STATUS_MORE_DATA;
		}
	}
	return EAPI_STATUS_SUCCESS;
}

EERROR CSMBusFunct::SetSysCtrlReg(uint32_t nValue)
{
	uint8_t pDataIn[] = { (uint8_t)nValue,  (uint8_t)(nValue >> 0x08),  \
		(uint8_t)(nValue >> 0x10), (uint8_t)(nValue >> 0x18) };

	return m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_SYSCFG, pDataIn, \
		(m_nSemaCaps & SEMA_CAP_BMC_TIVA) ? 0x04 : 0x02);
}

EERROR CSMBusFunct::GetErrorLog(uint32_t nPos, uint8_t* pData, uint32_t* pnDataSize, bool bSetPos)
{
	uint8_t bPos = (uint8_t)nPos;

	if (bSetPos)
	{
		if (m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_GET_BOARDERRLOG, \
			&bPos, 0x01) != EAPI_STATUS_SUCCESS)
		{
			return EAPI_STATUS_ERROR;
		}
	}

	return m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_GET_BOARDERRLOG, \
		NULL, 0x00, pData, *pnDataSize);
}

EERROR CSMBusFunct::GetErrorNumDescription(uint32_t ErrorNum, uint8_t* ErrorNumDes)
{
	uint32_t count, pnDataSize = 0x20;
	unsigned char buf[32] = { 0 };
	uint32_t ErrorNumRec; 
	uint32_t DataSize = 0x20;

	for (count = 0; count < 32; count++)
	{
		if(GetErrorLog(count, buf, &pnDataSize, !count) == EAPI_STATUS_SUCCESS)
		{
			ErrorNumRec = (uint32_t)buf[0] | ((uint32_t)buf[1] << 8);
			if (ErrorNum == ErrorNumRec)
			{
				char Errorcode = buf[2] & 0x0f;

				return GetErrorDesc(Errorcode, ErrorNumDes, &DataSize);
			}
		}
	}

	return EAPI_STATUS_ERROR;
}


EERROR CSMBusFunct::I2CRead(uint16_t nAdr, uint8_t cmd, uint8_t* pData, uint32_t &nSize)
{
	return m_clsSemaTrans.BlockI2CTrans(m_bBMCAdr, TT_I2C_RBL, cmd, NULL, 0, pData, nSize);
}

EERROR CSMBusFunct::I2CWrite(uint16_t nAdr, uint8_t cmd, uint8_t* pData, uint32_t nSize)
{
	uint32_t &dummy = nSize;
	
	return m_clsSemaTrans.BlockI2CTrans(m_bBMCAdr, TT_I2C_WBL, cmd, pData, nSize, NULL, dummy);
}

EERROR CSMBusFunct::GetErrorDesc(uint32_t nErrorCode, uint8_t* pData, uint32_t* pnDataSize)
{
	uint8_t bPos[5];
	bPos[0] = (uint8_t)nErrorCode;

	if (m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_EXC_CODE_TABLE,\
		bPos, 0x01) == EAPI_STATUS_SUCCESS)
	{
		return m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_EXC_CODE_TABLE,\
			NULL, 0x00, pData, *pnDataSize);
	}

	return EAPI_STATUS_ERROR;
}

EERROR CSMBusFunct::GetRealChannel(char** tbString, uint8_t& bChannelRet)
{
	char szRet[MAX_PATH];
	uint8_t bChannel = 0x00;
	uint32_t nCount = 0x00;

	while (bChannel < m_nTotalChannel)
	{
		ZeroMemory(szRet, MAX_PATH);

		if (GetVoltDescExt(bChannel, szRet, MAX_PATH, true) == EAPI_STATUS_SUCCESS)
		{
			nCount = 0;

			std::string strRet = "";
			while (szRet[nCount] != 0x00)
			{
				if (szRet[nCount] != ' ')
				{
					strRet += szRet[nCount];
				}
				nCount++;
			}
			
			nCount = 0x00;

			while (tbString[nCount] != NULL)
			{
				if (strRet == tbString[nCount])
				{
					std::string strRealChannel = "";
					
					strRealChannel += m_tbDesc[bChannel][0x01];
					strRealChannel += m_tbDesc[bChannel][0x02];

					bChannelRet = atoi(strRealChannel.c_str());

					return EAPI_STATUS_SUCCESS;
				}
				nCount++;
			}
		}
		bChannel++;
	}

	return EAPI_STATUS_UNSUPPORTED;
}

EERROR CSMBusFunct::GetHWMonitor(char** tbString, uint32_t* pnValue)
{
	EERROR eRet;
	uint8_t bRealChannel = -1;
	float flVolt = 0.00;

	if ((eRet = GetRealChannel(tbString, bRealChannel)) != EAPI_STATUS_SUCCESS)
	{
		return eRet;
	}

	if ((eRet = GetVolt(bRealChannel, &flVolt)) == EAPI_STATUS_SUCCESS)
	{
		*pnValue = (uint32_t)(flVolt * 1000);
	}

	return eRet;
}

EERROR CSMBusFunct::GetBIOSSource(uint8_t* pIndex)
{
	EERROR eRet;
	if (m_nSemaCaps & SEMA_CAP_FAILSAFEBIOS)
	{
		eRet = EAPI_STATUS_ERROR;       // ER_INVALID_POINTER;
		if (pIndex != NULL)
		{
			uint8_t pDataRet[MAX_PATH]; uint32_t nDataRet = MAX_PATH;
			ZeroMemory(pDataRet, MAX_PATH);
			//eRet = ER_EXEC;
			if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_SET_SYSCTRLREG, NULL, 0x00, (uint8_t*)pDataRet, nDataRet)) == EAPI_STATUS_SUCCESS)
			{
				eRet = EAPI_STATUS_MORE_DATA;
				if (nDataRet == 1)
				{
					eRet = EAPI_STATUS_SUCCESS;
					*pIndex = pDataRet[0x00];
					if ((*pIndex) > MAX_BIOS_INDEX)
					{
						if ((eRet = GetBMCFlags(pIndex)) == EAPI_STATUS_SUCCESS)
						{
							*pIndex = ((*pIndex) & 0x80) >> 0x07;
						}
					}
				}
			}
		}
	}
	return eRet;
}


EERROR CSMBusFunct::SetBIOSSource(uint8_t bIndex)
{
	EERROR eRet = EAPI_STATUS_UNSUPPORTED;
	if (m_nSemaCaps & SEMA_CAP_FAILSAFEBIOS)
	{
		
		bIndex |= 0x80;
		eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, SEMA_CMD_SET_SYSCTRLREG, &bIndex, 0x01);
	}
	return eRet;
}