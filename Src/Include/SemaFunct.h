#pragma once

#include "combaseapi.h"
#include "Common.h"

#define SEMA_MANU_DATA_MAX			10
#define SEMA_MANU_DATA_HW_REV		0
#define SEMA_MANU_DATA_SR_NO		1
#define SEMA_MANU_DATA_LR_DATE		2
#define SEMA_MANU_DATA_MF_DATE		3
#define SEMA_MANU_DATA_2HW_REV		4
#define SEMA_MANU_DATA_2SR_NO		5
#define SEMA_MANU_DATA_MACID_1		6
#define SEMA_MANU_DATA_MACID_2		7
#define SEMA_MANU_DATA_PLAT_ID		8
#define SEMA_MANU_GET_VALUE_A		9

__interface  IBMCFunc
{
public:
	virtual EERROR Init() PURE;
	virtual bool IsInitialiezed() PURE;

	virtual uint32_t GetCaps() PURE;
	virtual uint32_t GetCapsExt() PURE;
	virtual uint32_t FindCap(uint32_t cap)PURE;
	virtual uint32_t FindCapExt(uint32_t cap)PURE;

	virtual EERROR GetVersion(char* szVersion, uint32_t nSize) PURE;
	virtual EERROR GetBootVersion(char* szVersion, uint32_t nSize) PURE;
	virtual EERROR GetBootVersionExt(char* szVersion, uint32_t nSize) PURE;
	virtual EERROR GetManufData(uint32_t nDataInfo, uint8_t* pData, uint32_t nSize) PURE;
	virtual EERROR SetManufData(uint32_t nDataInfo, uint8_t* pData, uint32_t nLen) PURE;

	virtual EERROR GetTotalUpTime(uint32_t* pnTotalUpTime) PURE;
	virtual EERROR GetPwrUpTime(uint32_t* pnPwrUpTime) PURE;
	virtual EERROR GetPwrCycles(uint32_t* pnPwrCycle) PURE;
	virtual EERROR GetBootCount(uint32_t* pnBootCount) PURE;
	virtual EERROR GetBMCFlags(uint8_t* pBMCFlag) PURE;
	virtual EERROR GetBMCStatus(uint8_t* pBMCStatus) PURE;
	virtual EERROR GetRestartEvent(uint8_t* pEvent) PURE;
	virtual EERROR GetRestartEventText(char* szVersion, uint32_t nSize) PURE;

	virtual EERROR GetFanSpeed(uint32_t nFanNr, uint16_t* pushSpeed) PURE;
	virtual EERROR GetSysCtrlReg(uint32_t* pnCtrlReg) PURE;
	virtual EERROR GetScaleFactor(uint8_t bChannel, uint16_t* pushScale) PURE;
	virtual EERROR GetVolt(uint8_t bChannel, float* pfVoltage) PURE;
	virtual EERROR GetVoltDescEx(uint8_t bChannel, char* pData) PURE;

	virtual EERROR GetVoltDescExt(uint8_t bChannel, char* szDesc, uint32_t nSize, bool bTruncate) PURE;
	virtual EERROR GetHWMonitor(char** tbString, uint32_t* pnValue) PURE;
	virtual EERROR GetCurrentChannel(uint8_t* pChannel) PURE;
	virtual EERROR GetMainPowerCurrent(uint16_t* pushPower) PURE;
	
	virtual EERROR GetBacklightPWM(uint8_t* pPWM)  PURE;
	virtual EERROR SetBacklightPWM(uint8_t bPWM)  PURE;

	virtual EERROR SetSysCtrlReg(uint32_t nValue) PURE;

	virtual EERROR GetCurrentCPUTemp(int32_t* pfTemp)PURE;
	virtual EERROR GetCurrentBoardTemp(int32_t* pchTemp)PURE;
	virtual EERROR GetBRDMinMaxTemp(int* pchMinBoard, int* pchMaxBoard)PURE;
	virtual EERROR GetCPUMinMaxTemp(int* pchMinCPU, int* pchMaxCPU)PURE;
	virtual EERROR GetStartupTemp(int* pchpStartBoard)PURE;
	virtual EERROR GetCPUStartupTemp(int* pchStartCPU)PURE;
	virtual EERROR GetCurrentBoardTemp_1(int* pchStartBoard)PURE;
	virtual EERROR GetMinMaxTemp_1(int* pchpMinCPU, int* pchMaxCPU, int* pchMinBoard, int* pchMaxBoard)PURE;
	virtual EERROR GetStartupTemp_1(int* pchStartCPU, int* pchStartBoard)PURE;

	virtual EERROR ReadMem(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize) PURE;
	virtual EERROR WriteMem(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize) PURE;
	virtual EERROR ClearMem(uint8_t Region) PURE;
	virtual EERROR StorageCap(uint8_t Region, uint32_t *RegSize, uint32_t *RegBlkSize) PURE;
	virtual EERROR LockMem(uint8_t Region) PURE;
	virtual EERROR UnlockMem(uint8_t Region, uint8_t Permission, char* passcode) PURE;
	
	virtual EERROR GetErrorLog(uint32_t nPos, uint8_t* pData, uint32_t* pnDataSize, bool bSetPos) PURE;

	virtual EERROR GetErrorNumDescription(uint32_t ErrorNum, uint8_t* ErrorNumDes) PURE;
	virtual EERROR GetErrorDesc(uint32_t nErrorCode, uint8_t* pData, uint32_t* pnDataSize) PURE;

	/*****************************FAN********************************/

	virtual EERROR SetSmartFanTempPoints(uint32_t nFanNr, char* pData, uint32_t nSize) PURE;
	virtual EERROR SetSmartFanPWMPoints(uint32_t nFanNr, char* pData, uint32_t nSize) PURE;
	virtual EERROR SetSmartFanMode(uint32_t nFanNr, uint8_t bMode) PURE;
	virtual EERROR SetSmartFanTempSrc(uint32_t nFanNr, uint8_t bSrc) PURE;
	virtual EERROR GetSmartFanTempPoints(uint32_t nFanNr, char* pData, uint32_t nSize) PURE;
	virtual EERROR GetSmartFanPWMPoints(uint32_t nFanNr, char* pData, uint32_t nSize) PURE;
	virtual EERROR GetSmartFanMode(uint32_t nFanNr, uint8_t* pData) PURE;
	virtual EERROR GetSmartFanTempSrc(uint32_t nFanNr, uint8_t* pData) PURE;
	virtual EERROR GetSmartFanSupport(uint32_t FanId) PURE;


	/********************************** GPIO *******************************************/
	virtual EERROR SetId(uint8_t GpioId) PURE;
	virtual EERROR GetGPIO(uint32_t* pnGPIO) PURE;
	virtual EERROR GetGPIOIN(uint32_t* pnGPIO, uint32_t nBitMask) PURE;
	virtual EERROR GetGPIOOUT(uint32_t* pnGPIO, uint32_t nBitMask) PURE;
	virtual EERROR GetGPIODir(uint32_t* pnGPIO, uint32_t nBitMask) PURE;
	virtual EERROR SetGPIO(uint32_t plevel, uint32_t nBitMask) PURE;
	virtual EERROR SetGPIODir(uint32_t pDirection, uint32_t nBitMask) PURE;
	virtual EERROR GetCapsGpioDir(uint32_t* pnCapsIn, uint32_t* pnCapsOut, uint32_t GpioId) PURE;
	virtual EERROR ValidateBitMask(uint32_t nBitMask) PURE;

	/*************************Watch Dog******************************************/

	virtual EERROR SetWatchdog(uint32_t nValue) PURE;
	virtual EERROR TriggerWatchdog() PURE;
	virtual EERROR SetPwrUpWatchdog(uint32_t nValue) PURE;
	virtual EERROR GetLastWDogSetValue(uint32_t *nValue) PURE;
	virtual EERROR IsWDogRunning(void) PURE;
	virtual EERROR IsPwrUpWDogRunning(void) PURE;

	/*************************Generic I2C******************************************/
	virtual EERROR I2CWaitForFree(void);
	virtual EERROR I2CGetStatus(uint32_t *p_output);
	virtual EERROR I2CIsDeviceExist(uint32_t Id, uint32_t Addr) PURE;
	virtual EERROR I2CGetBusCap(uint32_t Id, uint32_t *pMaxBlkLen) PURE;
	virtual EERROR I2CWriteReadRaw(uint32_t Id, uint32_t Addr, void *pWBuffer, uint32_t WriteBCnt, \
		void *pRBuffer, uint32_t RBufLen, uint32_t ReadBCnt) PURE;
	virtual EERROR I2CReadTransfer(uint32_t Id, uint32_t Addr, uint32_t Cmd, void* pBuffer, \
		uint32_t BufLen, uint32_t ByteCnt) PURE;
	EERROR I2CWriteTransfer(uint32_t Id, uint32_t Addr, uint32_t Cmd, void* pBuffer, \
		uint32_t ByteCnt) PURE;
	virtual EERROR I2CRawTransaction(uint32_t Id, uint32_t Addr, void* pWBuffer, uint32_t WriteBCnt, \
		void* pRBuffer, uint32_t ReadBCnt) PURE;

	virtual EERROR I2CRead(uint16_t, uint8_t, uint8_t*, uint32_t&);
	virtual EERROR I2CWrite(uint16_t, uint8_t, uint8_t*, uint32_t);


	/*******************************Bios Source ****************************************/
	virtual EERROR GetBIOSSource(uint8_t* pIndex)PURE;
	virtual EERROR SetBIOSSource(uint8_t pIndex)PURE;

	/*******************************SMBus Access ****************************************/
	virtual EERROR SMBusBlockTrans(uint8_t, uint8_t, uint8_t, uint8_t*, uint32_t, uint8_t*, uint32_t&) PURE;
};