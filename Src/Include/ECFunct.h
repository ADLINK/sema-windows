#pragma once

#include "ECTransWin.h"
#include "EApi.h"

class  CECFunct : public IBMCFunc
{
private:

#define DEFAULT_BKLIT_BRIGHTNESS 200
#define EC_MAX_SIZE		32

#define DESC_NR_CHANNEL		7
#define DESC_LEN			0x19

#define BOARDNAME_INDEX 0xA0
#define BOARDNAME_LEN 0xA0

	CECTransport m_clsECTrans;
	Ccommon_bmc  m_clsCmn;

	uint8_t	m_bBMCAdr;
	bool	m_bDTSTemp;

	uint32_t	m_nSemaCaps;
	uint32_t	m_nSemaCapsEx;

	uint32_t	m_nTotalChannel;
	char		m_tbDesc[DESC_NR_CHANNEL][DESC_LEN];

	uint16_t	m_tbScale[DESC_NR_CHANNEL];

	uint32_t	m_n_GPIO_Cur_Id;

#define EC_RW_ADDR_BIOS_CNTRL				0x04
#define EC_RW_ADDR_BIOS_RSC_STS				0x36
#define EC_WO_ADDR_RUN_WD					0x06
#define EC_WO_ADDR_RUN_WD_MIRR				0x08
#define EC_WO_ADDR_PWRUP_WD					0x3E
#define EC_RW_ADDR_BMC_CONFIG				0x0C
#define EC_RO_ADDR_CAPABILITIES				0x10
#define EC_RO_ADDR_TOT_ON_TIME_MIN			0x20
#define EC_RO_ADDR_TOT_ON_TIME_SEC			0x24
#define EC_RO_ADDR_PWR_CYC_CNT				0x28
#define EC_RO_ADDR_BMC_FLAGS				0x2C
#define EC_RO_ADDR_LAST_SYS_RES_EVT			0x2D
#define EC_RO_ADDR_CPU_CUR_TEMP				0x2E
#define EC_RO_ADDR_BRD_CUR_TEMP				0x2F
#define EC_RO_ADDR_TEMP_LOG					0x30
#define EC_RO_ADDR_STARTUP_TEMP				0x34
#define EC_RO_ADDR_HW_STATUS				0x36
#define EC_RO_ADDR_BOOT_CNT					0x38
#define EC_RO_ADDR_HW_MON_IN				0x50
#define EC_RW_FAN_CUR_SPEED					0x60
#define EC_RW_ADDR_BACKLIGHT_PWM			0x6C
#define EC_RW_CPU_TMP_REG					0x70
#define EC_RW_SYS_TMP_REG					0x78
#define EC_RW_ADDR_GPIO_DIR					0x84
#define EC_RW_ADDR_GPIO_DIR_EXT				0X85
#define EC_RW_ADDR_GPIO_OUT  				0x86
#define EC_RW_ADDR_GPIO_IN  				0x88
#define EC_RW_ADDR_GPIO_OUT_EXT  			0x87
#define EC_RW_ADDR_GPIO_IN_EXT				0x89
#define EC_RO_ADDR_FW_VERSION				0xF0

#define EC_RW_ADDR_IIC_BMC_STATUS			0x10
#define EC_WO_ADDR_IIC_CMD_START			0x11
#define EC_RO_ADDR_IIC_TXN_STATUS			0x18
#define EC_RW_ADDR_IIC_BUFFER				0x20

#define ExcepDescLength 0x23

	char *ExcepDescList[ExcepDescLength] = { "NOERROR", "INVALID", "NO_SUSCLK", "NO_SLP_S5", "NO_SLP_S4", "NO_SLP_S3", "BIOS_FAIL", "RESET_FAIL", \
					"RESETIN_FAIL", "NO_CB_PWROK", "CRITICAL_TEMP", "POWER_FAIL", "VOLTAGE_FAIL", "RSMRST_FAIL", "NO_VDDQ_PG", "NO_V1P05A_PG", \
					"NO_VCORE_PG", "NO_SYS_GD", "NO_V5SBY", "NO_V3P3A", "NO_V5_DUAL", "NO_PWRSRC_GD", "NO_P_5V_3V3_S0_PG", "NO_SAME_CHANNEL", \
					"NO_PCH_PG", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", NULL};

	char *DescriptorList[9] = { "RTC", "5VSB", "VIN", "3.3V", /*(HW nt)*/"VMEM", /*(HW nt)*/"3.3VSB", "VCORE", "Current Input Current", NULL };

	EERROR GetRealChannel(char** tbString, uint8_t& bChannelRet);

	EERROR GetBoardnameFromEC(uint8_t*name, int len);

	virtual EERROR ReadMemL(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize);
	virtual EERROR WriteMemL(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize);

public:
	bool	m_bInit = FALSE;
	CECFunct(Ccommon_bmc &cmn);
	virtual ~CECFunct();

	virtual EERROR Init();
	virtual bool IsInitialiezed();

	virtual uint32_t GetCaps();
	virtual uint32_t GetCapsExt();
	virtual uint32_t FindCap(uint32_t cap);
	virtual uint32_t FindCapExt(uint32_t cap);

	virtual EERROR GetVersion(char* szVersion, uint32_t nSize);
	virtual EERROR GetBootVersion(char* szVersion, uint32_t nSize); 

	virtual EERROR GetBootVersionExt(char* szVersion, uint32_t nSize);
	virtual EERROR GetManufData(uint32_t nDataInfo, uint8_t* pData, uint32_t nSize);
	virtual EERROR SetManufData(uint32_t nDataInfo, uint8_t* pData, uint32_t nLen);

	virtual EERROR GetTotalUpTime(uint32_t* pnTotalUpTime);
	virtual EERROR GetPwrUpTime(uint32_t* pnPwrUpTime);
	virtual EERROR GetPwrCycles(uint32_t* pnPwrCycle);
	virtual EERROR GetBootCount(uint32_t* pnBootCount);
	virtual EERROR GetBMCFlags(uint8_t* pBMCFlag);
	virtual EERROR GetBMCStatus(uint8_t* pBMCStatus);
	virtual EERROR GetRestartEvent(uint8_t* pEvent);
	virtual EERROR GetRestartEventText(char* szVersion, uint32_t nSize);

	virtual EERROR GetFanSpeed(uint32_t nFanNr, uint16_t* pushSpeed);
	virtual EERROR GetScaleFactor(uint8_t bChannel, uint16_t* pushScale);
	virtual EERROR GetVolt(uint8_t bChannel, float* pfVoltage);
	virtual EERROR GetVoltDescExt(uint8_t bChannel, char* szDesc, uint32_t nSize, bool bTruncate);
	virtual EERROR GetVoltDescEx(uint8_t bChannel, char* pData);

	virtual EERROR GetCurrentChannel(uint8_t* pChannel);
	virtual EERROR GetMainPowerCurrent(uint16_t* pushPower);

	virtual EERROR GetCurrentCPUTemp(int32_t* pfTemp);
	virtual EERROR GetCurrentBoardTemp(int32_t* pchTemp);
	virtual EERROR GetBRDMinMaxTemp(int* pchMinBoard, int* pchMaxBoard);
	virtual EERROR GetCPUMinMaxTemp(int* pchMinCPU, int* pchMaxCPU);
	virtual EERROR GetStartupTemp(int* pchpStartBoard);
	virtual EERROR GetCPUStartupTemp(int* pchStartCPU);
	virtual EERROR GetCurrentBoardTemp_1(int* pchStartBoard);
	virtual EERROR GetMinMaxTemp_1(int* pchpMinCPU, int* pchMaxCPU, int* pchMinBoard, int* pchMaxBoard);
	virtual EERROR GetStartupTemp_1(int* pchStartCPU, int* pchStartBoard);

	virtual EERROR SetSysCtrlReg(uint32_t nValue);
	virtual EERROR GetSysCtrlReg(uint32_t* pnCtrlReg);


	//Storage
	virtual EERROR ReadMem(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize);
	virtual EERROR WriteMem(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize);
	virtual EERROR ClearMem(uint8_t Region);
	virtual EERROR StorageCap(uint8_t Region, uint32_t *RegSize, uint32_t *RegBlkSize);
	virtual EERROR LockMem(uint8_t Region);
	virtual EERROR UnlockMem(uint8_t Region, uint8_t Permission, char* passcode);

	virtual EERROR GetErrorLog(uint32_t nPos, uint8_t* pData, uint32_t* pnDataSize, bool bSetPos);

	virtual EERROR GetErrorNumDescription(uint32_t ErrorNum, uint8_t* ErrorNumDes);
	virtual EERROR GetErrorDesc(uint32_t nErrorCode, uint8_t* pData, uint32_t* pnDataSize);

	virtual EERROR GetHWMonitor(char** tbString, uint32_t* pnValue);

	//Fan 
	virtual EERROR SetSmartFanTempPoints(uint32_t nFanNr, char* pData, uint32_t nSize);
	virtual EERROR SetSmartFanPWMPoints(uint32_t nFanNr, char* pData, uint32_t nSize);
	virtual EERROR SetSmartFanMode(uint32_t nFanNr, uint8_t bMode);
	virtual EERROR SetSmartFanTempSrc(uint32_t nFanNr, uint8_t bSrc);
	virtual EERROR GetSmartFanTempPoints(uint32_t nFanNr, char* pData, uint32_t nSize);
	virtual EERROR GetSmartFanPWMPoints(uint32_t nFanNr, char* pData, uint32_t nSize);
	virtual EERROR GetSmartFanMode(uint32_t nFanNr, uint8_t* pData);
	virtual EERROR GetSmartFanTempSrc(uint32_t nFanNr, uint8_t* pData);
	virtual EERROR GetSmartFanSupport(uint32_t FanId);

	//GPIO
	virtual EERROR SetId(uint8_t GpioId);
	virtual EERROR GetGPIO(uint32_t* pnGPIO);
	virtual EERROR GetGPIOIN(uint32_t * pnGPIO, uint32_t nBitMask);
	virtual EERROR GetGPIOOUT(uint32_t * pnGPIO, uint32_t nBitMask);
	virtual EERROR GetGPIODir(uint32_t* pnGPIO , uint32_t nBitMask);
	virtual EERROR SetGPIO(uint32_t plevel, uint32_t nBitMask);
	virtual EERROR SetGPIODir(uint32_t pDirection, uint32_t nBitMask);
	virtual EERROR GetCapsGpioDir(uint32_t* pnCapsIn, uint32_t* pnCapsOut,uint32_t GpioId);
	virtual EERROR ValidateBitMask(uint32_t nBitMask);

	//Watch Dog
	virtual EERROR SetWatchdog(uint32_t nValue);
	virtual EERROR TriggerWatchdog();
	virtual EERROR SetPwrUpWatchdog(uint32_t nValue);
	virtual EERROR GetLastWDogSetValue(uint32_t *nValue);
	virtual EERROR IsWDogRunning(void);
	virtual EERROR IsPwrUpWDogRunning(void);


	//Generic I2C
	virtual EERROR I2CWaitForFree(void);
	virtual EERROR I2CGetStatus(uint32_t *p_output);
	virtual EERROR I2CTxnStatus(uint32_t *p_output);
	virtual EERROR I2CIsDeviceExist(uint32_t Id, uint32_t Addr);
	virtual EERROR I2CGetBusCap(uint32_t Id, uint32_t *pMaxBlkLen);
	virtual EERROR I2CWriteReadRaw(uint32_t Id, uint32_t Addr, void *pWBuffer, uint32_t WriteBCnt, \
		void *pRBuffer, uint32_t RBufLen, uint32_t ReadBCnt);
	virtual EERROR I2CReadTransfer(uint32_t Id, uint32_t Addr, uint32_t Cmd, void* pBuffer, \
		uint32_t BufLen, uint32_t ByteCnt);
	virtual EERROR I2CWriteTransfer(uint32_t Id, uint32_t Addr, uint32_t Cmd, void* pBuffer, \
		uint32_t ByteCnt);
	virtual EERROR I2CRawTransaction(uint32_t Id, uint32_t Addr, void* pWBuffer, uint32_t WriteBCnt, \
		void* pRBuffer, uint32_t ReadBCnt);

	virtual EERROR I2CRead(uint16_t , uint8_t , uint8_t* , uint32_t&);
	virtual EERROR I2CWrite(uint16_t , uint8_t, uint8_t* , uint32_t );
	
	// Display Backlight PWM
	virtual EERROR GetBacklightPWM(uint8_t* pPWM);
	virtual EERROR SetBacklightPWM(uint8_t bPWM);

	// BIOS Control
	virtual EERROR SetBIOSSource(uint8_t bIndex);
	virtual EERROR GetBIOSSource(uint8_t* pIndex);

};