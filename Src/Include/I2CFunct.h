#pragma once

#include "I2CTransWin.h"
#include "EApi.h"

class  CI2CFunct : public IBMCFunc
{
private:

#define MAX_DESC_NR_CHANNEL		20
#define MAX_DESC_LEN			0x11

#define SEMA_CMD_PIC_RATE				0x0F		///< Get Board Error Log
#define SEMA_CMD_GET_BOARDERRLOG		0x1C		///< Get Board Error Log
#define SEMA_CMD_SET_SYSCTRLREG			0x1F		///< Set SystemControlRegister
#define SEMA_CMD_SET_WD					0x20		///< Set/Clear Watchdog-timer
//#define SEMA_CMD_CLR_RBTC				0x21		///< Clear Recovery-BIOS Timeout Counter
#define SEMA_CMD_SET_PWRUPWD			0x22		///< Set/Clear Initial Watchdog-timer
#define SEMA_CMD_GET_GPIOREG			0x24		///< Get gpio register
#define SEMA_CMD_SET_GPIOREG			0x25		///< Set gpio register
#define SEMA_CMD_SYSCFG					0x27		///< Get/Set system config register
#define SEMA_CMD_MEMSIZE				0x2D		///< Get Memsize on CRR-945GSE (1:512MB, 2:1GB, 3:2GB)
#define SEMA_CMD_CLEAR_EXCEPT			0x2E		///< Clears Exception code (bits 4..0 of BMC flags -> CMD 0x35)
#define SEMA_CMD_CAPABILITIES			0x2F		///< Get BMC Capabilities
#define SEMA_CMD_RD_VERSION1			0x30		///< Read version string 1
#define SEMA_CMD_RD_VERSION2			0x31		///< Read version string 2
#define SEMA_CMD_RD_TOM					0x32		///< Read total uptime minutes
#define SEMA_CMD_RD_PWRUPSECS			0x33		///< Read seconds since power up
#define SEMA_CMD_RD_PWRCYCLES			0x34		///< Read number of power cycles
#define SEMA_CMD_RD_BMC_FLAGS			0x35		///< Read BMC flags (internal BMC Status (bits7..5) + exception code (4..0))
#define SEMA_CMD_RD_RESTARTEVT			0x36		///< Read last system restart event
#define SEMA_CMD_RD_CPUTEMPCUR			0x37		///< Read current CPU temperature
#define SEMA_CMD_RD_BRDTEMPCUR			0x38		///< Read current board temperature
#define SEMA_CMD_RD_MINMAXTEMP			0x39		///< Read minimum/maximum CPU and board temperatures
#define SEMA_CMD_RD_STARTTEMP			0x3A		///< Read startup temperatures of CPU and board
#define SEMA_CMD_RD_BMC_STATUS			0x3D		///< Read BMC Status byte
#define SEMA_CMD_RD_BOOTCOUNT			0x3E		///< Read number of boots
#define SEMA_CMD_RD_BLVERSION			0x3F		///< Read bootloader version
#define SEMA_CMD_SET_ADDRESS			0x40		///< Set address and length for flash access
#define SEMA_CMD_WRITE_DATA				0x41		///< Write data to user flash
#define SEMA_CMD_READ_DATA				0x42		///< Read data from user flash
#define SEMA_CMD_CLEAR_DATA				0x43		///< Clear data from user flash

#define SEMA_CMD_RD_AIN0				0x60		///< Read analog input Ch0
#define SEMA_CMD_RD_AIN1				0x61		///< Read analog input Ch1
#define SEMA_CMD_RD_AIN2				0x62		///< Read analog input Ch2
#define SEMA_CMD_RD_AIN3				0x63		///< Read analog input Ch3
#define SEMA_CMD_RD_AIN4				0x64		///< Read analog input Ch4
#define SEMA_CMD_RD_AIN5				0x65		///< Read analog input Ch5
#define SEMA_CMD_RD_AIN6				0x66		///< Read analog input Ch6
#define SEMA_CMD_RD_AIN7				0x67		///< Read analog input Ch7
#define SEMA_CMD_RD_CPU_FAN				0x68		///< Read CPU fan speed
#define SEMA_CMD_RD_MPCURRENT			0x69		///< Read main power current
#define SEMA_CMD_RD_SYSTEM_FAN_1		0x6A		///< Read system fan #1 speed
#define SEMA_CMD_RD_SYSTEM_FAN_2		0x6B		///< Read system fan #2 speed
#define SEMA_CMD_RD_SYSTEM_FAN_3		0x6C		///< Read system fan #3 speed

#define SEMA_CMD_EXC_CODE_TABLE			0x6F		///< Read exception code table

#define SEMA_CMD_RD_2ND_BRDTEMPCUR		0xE0		///< Read current board 2nd temperature
#define SEMA_CMD_RD_2ND_MINMAXTEMP		0xE1		///< Read minimum/maximum 2nd CPU and 2nd board temperatures
#define SEMA_CMD_RD_2ND_STARTTEMP		0xE2		///< Read 2nd startup temperatures of CPU and board

#define SEMA_CMD_RD_MF_DATA			0x70		///< Read part number

#define SEMA_CMD_BL_SETPWM			0x80		///< Set Backlight PWM
#define SEMA_CMD_BL_GETPWM			0x81		///< Get Backlight PWM

#define SEMA_CMD_SFC_CPU_TMP_SET_PTS	0xA0		///< Get/Set Smart Fan control (CPU) temperature setpoints
#define SEMA_CMD_SFC_CPU_PWM_SET_PTS	0xA1		///< Get/Set Smart Fan control (CPU) PWM setpoints
#define SEMA_CMD_SFC_SYS_1_TMP_SET_PTS		0xA2		///< Get/Set Smart Fan control (System #1) temperature setpoints
#define SEMA_CMD_SFC_SYS_1_PWM_SET_PTS		0xA3		///< Get/Set Smart Fan control (System #1) PWM setpoints
#define SEMA_CMD_SFC_SYS_2_TMP_SET_PTS		0xA8		///< Get/Set Smart Fan control (System #2) temperature setpoints
#define SEMA_CMD_SFC_SYS_2_PWM_SET_PTS		0xA9		///< Get/Set Smart Fan control (System #2) PWM setpoints
#define SEMA_CMD_SFC_SYS_3_TMP_SET_PTS		0xAA		///< Get/Set Smart Fan control (System #3) temperature setpoints
#define SEMA_CMD_SFC_SYS_3_PWM_SET_PTS		0xAB		///< Get/Set Smart Fan control (System #3) PWM setpoints


#define SEMA_CMD_GET_ADC_SCALE		0xA4		///< Get ADC scale factors (for analog inputs Ch0..7)
#define SEMA_CMD_GET_VOLT_DESCR		0xA5		///< Get voltage descriptions (or nominal values)
#define SEMA_CMD_GET_EXT_VOLT_DESCR 		0xA6		///< Get extended voltage descriptions (or nominal values)


#define SEMA_CMD_IIC4_BLOCK			0xB0
#define SEMA_CMD_IIC1_BLOCK			0xB1		///< I2C block access on external I2C bus #1
#define SEMA_CMD_IIC2_BLOCK			0xB2		///< I2C block access on external I2C bus #2
#define SEMA_CMD_IIC3_BLOCK			0xB3
#define	SEMA_CMD_IIC4_BYTE_READ 		0xB4
#define SEMA_CMD_IIC1_BYTE_READ		0xB5		///< I2C byte access on external I2C bus #1
#define SEMA_CMD_IIC2_BYTE_READ		0xB6		///< I2C byte access on external I2C bus #2
#define SEMA_CMD_IIC3_BYTE_READ			0xB7
#define SEMA_CMD_IIC4_BYTE_WRITE		0xB8	
#define SEMA_CMD_IIC1_BYTE_WRITE	0xB9		///< I2C byte access on external I2C bus #1
#define SEMA_CMD_IIC2_BYTE_WRITE	0xBA		///< I2C byte access on external I2C bus #2
#define SEMA_CMD_IIC3_BYTE_WRITE		0xBB
#define SEMA_CMD_IIC4_COMANND_BYTE_READWRITE	0xBC
#define SEMA_CMD_IIC1_COMANND_BYTE_READWRITE    0xBD ///< I2C command byte access on external I2C bus #1
#define SEMA_CMD_IIC2_COMANND_BYTE_READWRITE    0xBE ///< I2C command byte access on external I2C bus #2
#define SEMA_CMD_IIC_GETDATA		0xBF		///< I2C get data on external I2C busses

#define SEMA_CMD_IIC1_TRANS		0xC1    		///< I2C transaction access on external I2C bus #1   
#define SEMA_CMD_IIC2_TRANS		0xC2    		///< I2C transaction access on external I2C bus #2 
#define SEMA_CMD_IIC_STATUS		0xC4    		///< I2C status on last transction
#define SMB_TRANS_BUFFER_LEN	33

#define SEMA_CMD_RD_AIN8			0xD0		///< Read analog input Ch8
#define SEMA_CMD_RD_AIN9			0xD1		///< Read analog input Ch9
#define SEMA_CMD_RD_AIN10			0xD2		///< Read analog input Ch10
#define SEMA_CMD_RD_AIN11			0xD3		///< Read analog input Ch11
#define SEMA_CMD_RD_AIN12			0xD4		///< Read analog input Ch12
#define SEMA_CMD_RD_AIN13			0xD5		///< Read analog input Ch13
#define SEMA_CMD_RD_AIN14			0xD6		///< Read analog input Ch14
#define SEMA_CMD_RD_AIN15			0xD7		///< Read analog input Ch15

#define SEMA_CMD_GET_SOFTSTRAP		0x26  		///< Get softstrap register
#define SEMA_CMD_SET_SOFTSTRAP_OTF	0x27		///< Set softstrap register without PowerCycle (OnTheFly)
#define SEMA_CMD_SET_SOFTSTRAP_PC	0x28		///< Set softstrap register with PowerCycle

#define SEMA_CMD_CLRONTIME			0x52		///< Clear Ontime Counter
#define SEMA_CMD_CLRPWRCYLES		0x53		///< Clear Power Cycle Counter
#define SEMA_CMD_CLRTEMPLOG			0x54		///< Clear Temperature log (Min- Max- stored temperatures)
#define SEMA_CMD_CLEAR_ADMIN		0x55		///< Clear data from admin flash (0x400..0x7ff resp. 0x200..0x3ff on "small" BMCs)
#define SEMA_CMD_CLRBOOTCNT			0x56		///< Clear Bootcounter

#define SEMA_CMD_BOOTSYNC			0x21		///< Synchronize to bootloader
#define SEMA_CMD_LEAVEAPP			0x51		///< Leave application (enter bootloader)
#define SEMA_CMD_SENDDATA			0x25		///< Send data (bootloader and firmware update)
#define SEMA_CMD_ENTERADMIN			0x50		///< Enter admin mode
#define SEMA_CMD_LEAVEADMIN			0x5f		///< Leave admin mode
#define SEMA_CMD_TIVA_BL_GET_STATUS		0x23
#define SEMA_CMD_TIVA_BL_DOWNLOAD		0x21
#define SEMA_CMD_TIVA_BL_SEND_DATA		0x24
#define SEMA_CMD_TIVA_BL_RUN			0x22

#define SEMA_EXT_IIC_READ		0x01
#define SEMA_EXT_IIC_BLOCK 		0x02
#define SEMA_EXT_IIC_BYTE		0x04
#define SEMA_EXT_IIC_COMMAND		0x08
#define SEMA_EXT_IIC_EXT_COMMAND		0x10
#define SEMA_EXT_IIC_EXT_COMMAND_WR		0x20
#define SEMA_EXT_IIC_SPEED_100		0x01
#define SEMA_EXT_IIC_SPEED_400		0x02


#define	ONE_WIRE_COMMAND_0XC6		0xC6
#define	ONE_WIRE_COMMAND_0XC7		0xC7
#define	ONE_WIRE_COMMAND_0XC8		0xC8
#define	ONE_WIRE_COMMAND_0XCC		0xCC

#define MAX_BIOS_INDEX			3			// max BIOS index (0:standard 1:fail-safe 2:extern(SPI0 on carrier) 3:Internal BIOS (SPI0 on module))

#if !EGW3200
char* tbBoard[16] = { "Express - IBE2",	"Express-IBER", "Express-CVC",	"Express-HL",
		"Express-HL2",		"Express-HLE",	"cExpress-HL",	"cExpress-BT",
		"cExpress-BT2",		"nanoX-BT",		"cExpress-BL ",	"Express-BL",
		"Express-BLE",		"Express-BL2 ",	"CMx-BT", NULL};
#endif
	
	CI2CTransport	m_clsI2CTrans;
	Ccommon_bmc m_clsCmn;
	uint8_t	m_bBMCAdr;

	bool	m_bDTSTemp;

	uint32_t	m_nSemaCaps;
	uint32_t	m_nSemaCapsEx;

	uint32_t	m_nTotalChannel;
	char		m_tbDesc[MAX_DESC_NR_CHANNEL][MAX_DESC_LEN];

	uint16_t	m_tbScale[MAX_DESC_NR_CHANNEL];
	uint32_t	m_nTotalScale;

	uint32_t	m_n_GPIO_Cur_Id;

	EERROR GetRealChannel(char** tbString, uint8_t& bChannelRet);


	virtual EERROR SemaApi_TransExt(uint8_t BusNumber, uint8_t Type, uint8_t Length, uint8_t *Buffer);
	virtual EERROR ReadMemL(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize);
	virtual EERROR WriteMemL(uint8_t Region, uint32_t nAdr, uint8_t* pData, uint32_t nSize);

public:
	bool	m_bInit;

	CI2CFunct(Ccommon_bmc &cmn);
	virtual ~CI2CFunct();

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
	virtual EERROR GetSysCtrlReg(uint32_t* pnCtrlReg);
	virtual EERROR GetScaleFactor(uint8_t bChannel, uint16_t* pushScale);
	virtual EERROR GetVolt(uint8_t bChannel, float* pfVoltage);
	virtual EERROR GetVoltDescExt(uint8_t bChannel, char* szDesc, uint32_t nSize, bool bTruncate);
	virtual EERROR GetVoltDescEx(uint8_t bChannel, char* pData);

	virtual EERROR GetCurrentChannel(uint8_t* pChannel);
	virtual EERROR GetMainPowerCurrent(uint16_t* pushPower);
	
	virtual EERROR GetBacklightPWM(uint8_t* pPWM);
	virtual EERROR SetBacklightPWM(uint8_t bPWM);

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
	
	virtual EERROR SetSmartFanTempPoints(uint32_t nFanNr, char* pData, uint32_t nSize);
	virtual EERROR SetSmartFanPWMPoints(uint32_t nFanNr, char* pData, uint32_t nSize);
	virtual EERROR SetSmartFanMode(uint32_t nFanNr, uint8_t bMode);
	virtual EERROR SetSmartFanTempSrc(uint32_t nFanNr, uint8_t bSrc);
	virtual EERROR GetSmartFanTempPoints(uint32_t nFanNr, char* pData, uint32_t nSize);
	virtual EERROR GetSmartFanPWMPoints(uint32_t nFanNr, char* pData, uint32_t nSize);
	virtual EERROR GetSmartFanMode(uint32_t nFanNr, uint8_t* pData);
	virtual EERROR GetSmartFanTempSrc(uint32_t nFanNr, uint8_t* pData);
	virtual EERROR GetSmartFanSupport(uint32_t FanId);

	virtual EERROR SetId(uint8_t GpioId);
	virtual EERROR GetGPIO(uint32_t* pnGPIO);
	virtual EERROR GetGPIOIN(uint32_t* pnGPIO, uint32_t nBitMask);
	virtual EERROR GetGPIOOUT(uint32_t* pnGPIO, uint32_t nBitMask);
	virtual EERROR GetGPIODir(uint32_t* pnGPIO, uint32_t nBitMask);
	virtual EERROR SetGPIO(uint32_t plevel, uint32_t nBitMask);
	virtual EERROR SetGPIODir(uint32_t pDirection, uint32_t nBitMask);
	virtual EERROR GetCapsGpioDir(uint32_t* pnCapsIn, uint32_t* pnCapsOut, uint32_t GpioId);
	virtual EERROR ValidateBitMask(uint32_t nBitMask);
	
	virtual EERROR SetWatchdog(uint32_t nValue);
	virtual EERROR TriggerWatchdog();
	virtual EERROR SetPwrUpWatchdog(uint32_t nValue);
	virtual EERROR GetLastWDogSetValue(uint32_t *nValue);
	virtual EERROR IsWDogRunning(void);
	virtual EERROR IsPwrUpWDogRunning(void);

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


	// bios

	virtual EERROR GetBIOSSource(uint8_t* pIndex);
	virtual EERROR SetBIOSSource(uint8_t bIndex);
	
	//SMBus access
	virtual EERROR SMBusBlockTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen, uint8_t* pDataRet, uint32_t& nRetLen);
	
};