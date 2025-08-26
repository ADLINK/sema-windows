#include "stdafx.h"
#include "ECFunct.h"
#include "stdio.h"
#include <string>

#include "sysinfoapi.h"

CECFunct::CECFunct(Ccommon_bmc &cmn)
{
	m_bBMCAdr = 0x50;
	m_clsCmn = cmn;

	m_nSemaCaps = SEMA_CAP_NONE;
	m_nSemaCapsEx = SEMA_CAP_NONE;
	m_n_GPIO_Cur_Id = -1;
	m_bDTSTemp = FALSE;
	m_bInit = FALSE;
	
	m_nTotalChannel = 0x00;

	ZeroMemory(m_tbScale, sizeof(uint16_t) * DESC_NR_CHANNEL);
}

#define EC_VER_SIZE 50

typedef struct Exception {
	char *ExceptionDesc;
	char location;
}ExcDesc_T;

EERROR CECFunct::Init()
{
	int i,j;
	unsigned char *ptr[2];
	char szVersion[EC_VER_SIZE] = { 0 };
	uint8_t pDataRet[MAX_PATH];

	ZeroMemory(pDataRet, MAX_PATH);

	if (m_clsECTrans.Init(m_clsCmn.m_hDrv) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_NOT_FOUND;
	}

	m_bInit = TRUE;

	ptr[0] = (unsigned char*)&m_nSemaCaps;
	ptr[1] = (unsigned char*)&m_nSemaCapsEx;
	
	
	/*Wait and Reading the capability */
	for (j = 0; j < 100; j++)
	{
		if (m_nSemaCaps == SEMA_CAP_NONE)
		{
			if (m_clsECTrans.ECRead(EC_RO_ADDR_CAPABILITIES, EC_REGION_1, pDataRet, 8) == EAPI_STATUS_SUCCESS)
			{

				for (i = 0; i < 8; i++)
				{
					ptr[i / 4][(i % 4)] = 0;
					ptr[i / 4][(i % 4)] = pDataRet[i];
				}
			}

		}
		else
			break;
	}
	
	//EC address =(0x10)EC_RO_ADDR_CAPABILITIES+5(15th EC address byte has "Hardware Monitor Input" string)
	//EC_REGION_2 is used only in mem and i2c. So using EC_REGION_1.
	unsigned char *pData;
	
	uint8_t hardware_monitor=0;
	if (m_clsECTrans.ECRead(EC_RO_ADDR_CAPABILITIES+0x05, EC_REGION_1, pDataRet, 1) == EAPI_STATUS_SUCCESS)
	{
		hardware_monitor = (pDataRet[0] & 0x08)?1:0 ; //To get 43rd bit
		
		if(hardware_monitor==1)
		{
			int i=0, BlockLength =4;
			for (i=0; i<8;i++)
			{
				pData = (unsigned char*)calloc(sizeof(unsigned char), 16 + 2 * BlockLength);

				if (EApiStorageAreaRead(5,  (i * 0x10), pData, 16 + 2 * BlockLength, 16) == EAPI_STATUS_SUCCESS)
				{
					DescriptorList[i] = (char *)pData;
				}
				else{		// returns error when failed to write SEMA_CMD_READ_DATA
					return EAPI_STATUS_ERROR;
				}

			}
		}
	}
	else		// returns error when failed to read EC_RO_ADDR_CAPABILITIES
		return EAPI_STATUS_ERROR;
	
	if (hardware_monitor != 1)
	{
		if (m_clsCmn.ainlist != NULL)
		{
			char *next;
			int i = 0;

			char *string = strtok_s(m_clsCmn.ainlist, "#", &next);

			while (string != NULL && i < (sizeof(DescriptorList) / sizeof(DescriptorList[0])))
			{
				DescriptorList[i] = string;
				string = strtok_s(NULL, "#", &next);
				i++;
			}
		}
	}
		
	if (m_clsCmn.exetbl != NULL)
	{
		char *next;
		int i = 0;

		char *string = strtok_s(m_clsCmn.exetbl, "#", &next);

		while (string != NULL && i < (sizeof(ExcepDescList) / sizeof(ExcepDescList[0])))
		{
			ExcepDescList[i] = string;
			string = strtok_s(NULL, "#", &next);
			i++;
		}
	}

	for (i = 0; i < (sizeof(DescriptorList) / sizeof(DescriptorList[0])) && DescriptorList[i] != NULL; i++)
	{
		memset(m_tbDesc[i], 0, 25);
		memcpy_s(m_tbDesc[i], 25, DescriptorList[i], strlen(DescriptorList[i]));
		m_nTotalChannel++;
	}

	return EAPI_STATUS_SUCCESS;
}

bool CECFunct::IsInitialiezed()
{
	 return m_bInit;
}

uint32_t CECFunct::GetCaps()
{
	
	return m_nSemaCaps;
}

uint32_t CECFunct::GetCapsExt()
{
	return m_nSemaCapsEx;
}

uint32_t CECFunct::FindCap(uint32_t cap)
{
	return !!(m_nSemaCaps & cap);
}

uint32_t CECFunct::FindCapExt(uint32_t cap)
{
	return !!(m_nSemaCapsEx & cap);
}

EERROR CECFunct::GetSysCtrlReg(uint32_t* pnCtrlReg)
{
	EERROR eRet;
	unsigned char bytes[4] = { 0 }, i;

	if ((eRet = m_clsECTrans.ECRead(EC_RW_ADDR_BMC_CONFIG, EC_REGION_1, bytes, 4)) == EAPI_STATUS_SUCCESS)
	{
		for (i = 0; i < 4; i++)
		{
			((unsigned char*)pnCtrlReg)[3 - i] = bytes[3 - i];
		}
	}

	return eRet;
}

EERROR CECFunct::SetSysCtrlReg(uint32_t nValue)
{
	unsigned char bytes[4] = { 0 }, i;

	for (i = 0; i < 4; i++)
	{
		bytes[3 - i] |= ((unsigned char*)&nValue)[3 - i];
	}

	return m_clsECTrans.ECWrite(EC_RW_ADDR_BMC_CONFIG, EC_REGION_1, bytes, 4);
}


EERROR CECFunct::GetVersion(char* szVersion, uint32_t nLen)
{
	uint8_t buffer[40] = { 0 };

	ZeroMemory(szVersion, nLen);

	if (m_clsECTrans.ECRead(EC_RO_ADDR_FW_VERSION, EC_REGION_1, buffer, 16) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	strcpy_s(szVersion, nLen, (char*)buffer);

	strcpy_s((szVersion + strlen(szVersion)), nLen, " (BMC ");

	return GetBoardnameFromEC((uint8_t*)(szVersion + strlen(szVersion)), 16);
}


EERROR CECFunct::GetPwrUpTime(uint32_t* pnPwrUpTime)
{
	return m_clsECTrans.ECRead(EC_RO_ADDR_TOT_ON_TIME_SEC, EC_REGION_1, (uint8_t*)pnPwrUpTime, 4);
}


EERROR CECFunct::GetBMCFlags(uint8_t* pBMCFlag)
{
	return m_clsECTrans.ECRead(EC_RO_ADDR_BMC_FLAGS, EC_REGION_1, pBMCFlag, 1);
}


EERROR CECFunct::GetRestartEventText(char* szVersion, uint32_t nSize)
{
	EERROR eRet;
	char* szRet = "";
	uint8_t bEvent = 0x00;

	ZeroMemory(szVersion, nSize);

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

		uint32_t nLen = (uint32_t)strlen(szRet);
		eRet = nLen > (nSize - 0x01) ? EAPI_STATUS_MORE_DATA : EAPI_STATUS_SUCCESS;
		memcpy_s(szVersion, nSize, szRet, nLen > (nSize - 0x01) ? (nSize - 0x01) : nLen);
	}

	return eRet;
}



EERROR CECFunct::GetCurrentCPUTemp(int32_t *pfTemp)
{
	EERROR eRet;
	bool result = false;
	uint8_t pDataRet;

	if ((eRet = m_clsECTrans.ECRead(EC_RO_ADDR_CPU_CUR_TEMP, EC_REGION_1, \
		&pDataRet, 1)) == EAPI_STATUS_SUCCESS)
	{
		*pfTemp = (int8_t)pDataRet;
		*pfTemp = EAPI_ENCODE_CELCIUS(*pfTemp);
	}

	return eRet;
}


EERROR CECFunct::GetCurrentBoardTemp(int32_t* pchTemp)
{
	EERROR eRet;
	float pchTempL = 0;
	
	if ((eRet = m_clsECTrans.ECRead(EC_RO_ADDR_BRD_CUR_TEMP, EC_REGION_1, \
		(uint8_t *)pchTemp, 1)) == EAPI_STATUS_SUCCESS)
	{
		pchTempL = (float)(*((int8_t*)pchTemp));
		*pchTemp = EAPI_ENCODE_CELCIUS((int8_t)pchTempL);
	}
	
	return eRet;
}


EERROR CECFunct::GetBRDMinMaxTemp(int* pchMinBoard, int* pchMaxBoard)
{
	EERROR eRet;
	int8_t pDataRet[4];

	ZeroMemory(pDataRet, 4);

	*pchMinBoard = *pchMaxBoard = 0x00;

	if ((eRet = m_clsECTrans.ECRead(EC_RO_ADDR_TEMP_LOG, EC_REGION_1, (uint8_t*)pDataRet, 4)) == EAPI_STATUS_SUCCESS)
	{
		*pchMinBoard = (pDataRet[0x03] * 10) + 2731;
		*pchMaxBoard = (pDataRet[0x02] * 10) + 2731;
	}
	
	return eRet;
}


EERROR CECFunct::GetCPUMinMaxTemp(int* pchMinCPU, int* pchMaxCPU)
{
	EERROR eRet;
	int8_t pDataRet[4];

	ZeroMemory(pDataRet, 4);

	*pchMinCPU = *pchMaxCPU = 0x00;

	if (m_bDTSTemp == TRUE)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((eRet = m_clsECTrans.ECRead(EC_RO_ADDR_TEMP_LOG, EC_REGION_1, (uint8_t *)pDataRet, 4)) == EAPI_STATUS_SUCCESS)
	{
		*pchMinCPU = (pDataRet[0x01] * 10) + 2731;
		*pchMaxCPU = (pDataRet[0x00] * 10) + 2731;
	}

	return eRet;
}


EERROR CECFunct::GetStartupTemp(int* pchStartBoard)
{
	EERROR eRet;
	uint8_t pDataRet[2];

	ZeroMemory(pDataRet, 2);

	*pchStartBoard = 0x00;

	if ((eRet = m_clsECTrans.ECRead(EC_RO_ADDR_STARTUP_TEMP, EC_REGION_1, pDataRet, 2)) == EAPI_STATUS_SUCCESS)
	{
		*pchStartBoard = (pDataRet[0x01] * 10) + 2731;
	}

	return eRet;
}


EERROR CECFunct::GetCPUStartupTemp(int* pchStartCPU)
{
	EERROR eRet;
	uint8_t pDataRet[2];

	ZeroMemory(pDataRet, 2);

	*pchStartCPU = 0x00;

	if (m_bDTSTemp == TRUE)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if ((eRet = m_clsECTrans.ECRead(EC_RO_ADDR_STARTUP_TEMP, EC_REGION_1, pDataRet, 2)) == EAPI_STATUS_SUCCESS)
	{
		*pchStartCPU = (pDataRet[0x00] * 10) + 2731;
	}

	return eRet;
}


EERROR CECFunct::GetVoltDescExt(uint8_t bChannel, char* szDesc, uint32_t nSize, bool bTruncate)
{
	EERROR eRet;
	if (bChannel > DESC_NR_CHANNEL)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (m_nTotalChannel == 0x00)
	{
		return EAPI_STATUS_ERROR;
	}

	if (bChannel < m_nTotalChannel)
	{
		uint32_t nDataLen = (uint32_t)strlen(m_tbDesc[bChannel]);
		if (nDataLen > 0x00)
		{
			eRet = nDataLen > nSize ? EAPI_STATUS_MORE_DATA : EAPI_STATUS_SUCCESS;
			memcpy_s(szDesc, nSize, m_tbDesc[bChannel], nDataLen > nSize ? nSize : nDataLen);
			return eRet;
		}
	}

	return EAPI_STATUS_ERROR;
}

EERROR CECFunct::GetRealChannel(char** tbString, uint8_t& bChannelRet)
{
	char szRet[MAX_PATH];
	uint8_t bChannel = 0x00;
	uint32_t nCount;

	while (bChannel < DESC_NR_CHANNEL)
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
					bChannelRet = bChannel;
					return EAPI_STATUS_SUCCESS;
				}
				nCount++;
			}
		}
		else
		{
			return EAPI_STATUS_ERROR;
		}
		bChannel++;
	}

	return EAPI_STATUS_UNSUPPORTED;
}


EERROR CECFunct::GetHWMonitor(char** tbString, uint32_t* pnValue)
{
	uint8_t bRealChannel = -1;
	EERROR eRet = GetRealChannel(tbString, bRealChannel);
	if (eRet == EAPI_STATUS_SUCCESS)
	{
		float flVolt = 0.00;
		if ((eRet = GetVolt(bRealChannel, &flVolt)) == EAPI_STATUS_SUCCESS)
		{
			*pnValue = (uint32_t)(flVolt * 1000);
		}
	}

	return eRet;
}

EERROR CECFunct::GetVolt(uint8_t bChannel, float* pfVoltage)
{
	EERROR eRet;
	uint16_t chnl_addr;
	uint16_t ushValue;

	if (bChannel > DESC_NR_CHANNEL)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	*pfVoltage = 0;

	chnl_addr = (bChannel * 2) + EC_RO_ADDR_HW_MON_IN;

	if ((eRet = m_clsECTrans.ECRead(chnl_addr, EC_REGION_1, (uint8_t*)&ushValue, 2)) == EAPI_STATUS_SUCCESS)
	{
		*pfVoltage = ushValue;
		*pfVoltage /= 1000;
	}

	return eRet;
}

EERROR CECFunct::GetVoltDescEx(uint8_t bChannel, char* pData)
{
	char buff[25] = "=", i;

	if (bChannel > DESC_NR_CHANNEL)
	{
		return EAPI_STATUS_UNSUPPORTED;
	}

	for (i = 0; m_tbDesc[bChannel] != 0; i++)
	{
		if (m_tbDesc[bChannel][i] <= 'Z' && m_tbDesc[bChannel][i] >= 'A' || \
			m_tbDesc[bChannel][i] <= 'z' && m_tbDesc[bChannel][i] >= 'a' || \
			m_tbDesc[bChannel][i] <= '9' && m_tbDesc[bChannel][i] >= '0')
		{
			break;
		}
	}

	//EAPI expects = between no and description.
	strcat_s(buff, 25, &m_tbDesc[bChannel][i]);

	if (memcpy_s(pData, 25, buff, strlen(buff)) == 0)
	{
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}

EERROR CECFunct::GetErrorDesc(uint32_t nErrorCode, uint8_t* pData, uint32_t* pnDataSize)
{
	if (nErrorCode >= ExcepDescLength)
	{
		return EAPI_STATUS_ERROR;
	}

	if (ExcepDescList[nErrorCode] == nullptr || strcmp(ExcepDescList[nErrorCode], "INVALID") == 0)
	{
		return EAPI_STATUS_ERROR;
	}

	if (memcpy_s(pData, *pnDataSize, ExcepDescList[nErrorCode], strlen(ExcepDescList[nErrorCode])) == 0)
	{
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}

EERROR CECFunct::GetCurrentChannel(uint8_t* pChannel)
{
	int i;

	for (i = 0; i < sizeof(DescriptorList) / sizeof(DescriptorList[0]); i++)
	{
		
		if (strstr(DescriptorList[i], "Current ") != NULL)
		{
			*pChannel = i;
			return EAPI_STATUS_SUCCESS;
		}
	}

	return EAPI_STATUS_ERROR;
}

EERROR CECFunct::GetMainPowerCurrent(uint16_t* pushPower)
{
	EERROR eRet;
	unsigned char Chnl;

	if ((eRet = GetCurrentChannel(&Chnl)) == EAPI_STATUS_SUCCESS)
	{
		unsigned char chnl_addr = (Chnl * 2) + EC_RO_ADDR_HW_MON_IN;

		return m_clsECTrans.ECRead(chnl_addr, EC_REGION_1, (uint8_t*)pushPower, 2);
	}

	return eRet;
}

EERROR CECFunct::GetBootCount(uint32_t* pnBootCount)
{
	return m_clsECTrans.ECRead(EC_RO_ADDR_BOOT_CNT, EC_REGION_1, (uint8_t*)pnBootCount, 4);
}

EERROR CECFunct::GetPwrCycles(uint32_t* pnPwrCycle)
{
	return m_clsECTrans.ECRead(EC_RO_ADDR_PWR_CYC_CNT, EC_REGION_1, (uint8_t*)pnPwrCycle, 4);
}

EERROR CECFunct::GetBoardnameFromEC(uint8_t *name,int len)
{
	int retry;

	for (retry = 0; retry < 10; retry++)
	{
		if (m_clsECTrans.ECRead(BOARDNAME_INDEX, EC_REGION_1, name, len) == EAPI_STATUS_SUCCESS)
		{
			return EAPI_STATUS_SUCCESS;
		}
	}
	return EAPI_STATUS_ERROR;
}


EERROR CECFunct::GetBacklightPWM(uint8_t* pPWM)
{
	if (m_clsECTrans.ECRead(EC_RW_ADDR_BACKLIGHT_PWM, EC_REGION_1, pPWM, 1) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}
	return EAPI_STATUS_SUCCESS;
}

EERROR CECFunct::SetBacklightPWM(uint8_t bPWM)
{
	if (m_clsECTrans.ECWrite(EC_RW_ADDR_BACKLIGHT_PWM, EC_REGION_1, &bPWM, 1) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}
	return EAPI_STATUS_SUCCESS;
}

EERROR CECFunct::GetTotalUpTime(uint32_t* pnTotalUpTime)
{
	return m_clsECTrans.ECRead(EC_RO_ADDR_TOT_ON_TIME_MIN, EC_REGION_1, (uint8_t*)pnTotalUpTime, 4);
}

EERROR CECFunct::GetRestartEvent(uint8_t* pEvent)
{
	return  m_clsECTrans.ECRead(EC_RO_ADDR_LAST_SYS_RES_EVT, EC_REGION_1, (uint8_t*)pEvent, 1);
}

CECFunct::~CECFunct()
{
	m_nSemaCaps = SEMA_CAP_NONE;
	m_nSemaCapsEx = 0x00;
	m_bInit = FALSE;

	m_clsCmn.Close();
}

EERROR CECFunct::SetBIOSSource(uint8_t State)
{

	//if (m_nSemaCaps & SEMA_CAP_FAILSAFEBIOS)
	//{
		if (m_clsECTrans.ECWrite(EC_RW_ADDR_BIOS_CNTRL, EC_REGION_1, &State, 1) == EAPI_STATUS_SUCCESS)
			return EAPI_STATUS_SUCCESS;
	//}
	return EAPI_STATUS_ERROR;
}

EERROR CECFunct::GetBIOSSource(uint8_t* State)
{
	uint8_t state2;
	*State = 0;
	//if (m_nSemaCaps & SEMA_CAP_FAILSAFEBIOS)
	//{
		if (m_clsECTrans.ECRead(EC_RW_ADDR_BIOS_CNTRL, EC_REGION_1, State, 1) == EAPI_STATUS_SUCCESS)
		{
			if (m_clsECTrans.ECRead(EC_RW_ADDR_BIOS_RSC_STS, EC_REGION_1, &state2, 1) == EAPI_STATUS_SUCCESS)
			{
				*State |= (state2 & 0x7) << 2;
				return EAPI_STATUS_SUCCESS;
			}
		}
	//}
	return EAPI_STATUS_ERROR;
}

/* Not Supported by EC*/

EERROR CECFunct::GetMinMaxTemp_1(int* pchMinCPU, int* pchMaxCPU, int* pchMinBoard, int* pchMaxBoard)
{
	return EAPI_STATUS_UNSUPPORTED;
}

EERROR CECFunct::GetCurrentBoardTemp_1(int* pchBoardTemp)
{
	return EAPI_STATUS_UNSUPPORTED;
}

EERROR CECFunct::GetStartupTemp_1(int* pchStartCPU, int* pchStartBoard)
{
	return EAPI_STATUS_UNSUPPORTED;
}


EERROR CECFunct::I2CRead(uint16_t nAdr, uint8_t cmd, uint8_t* pData, uint32_t &nSize)
{
	return EAPI_STATUS_UNSUPPORTED;
}

EERROR CECFunct::I2CWrite(uint16_t nAdr, uint8_t cmd, uint8_t* pData, uint32_t nSize)
{
	return EAPI_STATUS_UNSUPPORTED;
}

EERROR CECFunct::GetScaleFactor(uint8_t bChannel, uint16_t* pushScale)
{
	return EAPI_STATUS_UNSUPPORTED;
}

EERROR CECFunct::GetBootVersion(char* szVersion, uint32_t nLen)
{
	return EAPI_STATUS_UNSUPPORTED;
}

EERROR CECFunct::GetBootVersionExt(char* szVersion, uint32_t nLen)
{
	return EAPI_STATUS_UNSUPPORTED;
}

EERROR CECFunct::GetBMCStatus(uint8_t* pBMCStatus)
{
	return EAPI_STATUS_UNSUPPORTED;
}

EERROR CECFunct::SMBusBlockTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen, uint8_t* pDataRet, uint32_t& nRetLen)
{
	return EAPI_STATUS_UNSUPPORTED;
}