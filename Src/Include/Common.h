#pragma once

#include "Error.h"
#include "SemaFunct.h"
#include "Public.h"

typedef enum
{
	UNKNOWN = -1,
	SMB = 1,
	EC,
	I2C,
}bmc_type;

// Capabilities
#define SEMA_CAP_NONE 			0x00000000			///< Only standard capabilities available
#define SEMA_CAP_UPTIME			0x00000001			///< Bit  0: Uptime & Pwr cycles counter
#define SEMA_CAP_RESTARTEVENT	0x00000002			///< Bit  1: System restart event
#define SEMA_CAP_1KB_FLASH		0x00000004			///< Bit  2: 1k USER flash memory available
#define SEMA_CAP_WATCHDOG		0x00000008			///< Bit  3: Watchdog
#define SEMA_CAP_TEMPERATURES	0x00000010			///< Bit  4: Temperature sensors installed/available
#define SEMA_CAP_VOLTAGES		0x00000020			///< Bit  5: Voltage sensors installed/available
#define SEMA_CAP_FAILREASON		0x00000040			///< Bit  6: Storage of failure reason available (exeption code)
#define SEMA_CAP_BL_TIMEOUT		0x00000080			///< Bit  7: Bootloader timeout programmable
#define SEMA_CAP_DISPLAYCTRL	0x00000100			///< Bit  8: Display control available
#define SEMA_CAP_PWRUP_WD		0x00000200			///< Bit  9: Power up watchdog available
#define SEMA_CAP_CURRENTS		0x00000400			///< Bit 10: Current sensors installed/available
#define SEMA_CAP_BOOTCOUNT		0x00000800			///< Bit 11: Bootcounter
#define SEMA_CAP_INPUTVOLT0		0x00001000			///< Bit 12: Input-Voltage0 V10:00=5V, 01=12V, 10+11=reserved
#define SEMA_CAP_INPUTVOLT1		0x00002000			///< Bit 13: Input-Voltage1 V10:00=5V, 01=12V, 10+11=reserved
#define SEMA_CAP_RSENSE			0x00004000			///< Bit 14: Rsense for Input-Voltage: 0=8mR, 1=4mR
#define SEMA_CAP_FAILSAFEBIOS	0x00008000			///< Bit 15: Fail-Safe-BIOS supported
#define SEMA_CAP_I2C1			0x00010000			///< Bit 16: Ext. I2C bus #1 available
#define SEMA_CAP_I2C2			0x00020000			///< Bit 17: Ext. I2C bus #2 available
#define SEMA_CAP_FAN_CPU		0x00040000			///< Bit 18: Programmable CPU fan available
#define SEMA_CAP_FAN_1			0x00080000			///< Bit 19: Programmable system fan available
#define SEMA_CAP_AT_ATX_MODE	0x00100000			///< Bit 20: AT/ATX mode supported
#define SEMA_CAP_THERMAL_SCI	0x00200000			///< Bit 21: Thermal SCI supported
#define SEMA_CAP_PWRLASTSTATE	0x00400000			///< Bit 22: Power up to last state
#define SEMA_CAP_BACKLGHTRES	0x00800000			///< Bit 23: Backlight restore
#define SEMA_CAP_DTS_REGISTER	0x01000000			///< Bit 24: DTS register
#define SEMA_CAP_DTS_OFFSET		0x02000000			///< Bit 25: DTS register offset
#define SEMA_CAP_FAN_2			0x04000000			///< Bit 26: Smart fan #3
#define SEMA_CAP_FAN_3			0x08000000			///< Bit 27: Smart fan #4
#define SEMA_CAP_GPIOS			0x10000000			///< Bit 28: TIVA GPIOs support (12GPIOs)
#define SEMA_CAP_I2C3			0x20000000			///< Bit 29: Ext. I2C bus #3 available
#define SEMA_CAP_I2C4			0x40000000			///< Bit 30: Ext. I2C bus #4 available
#define SEMA_CAP_BMC_TIVA		0x80000000			///< Bit 31: BMC is from TIVA type

#define SEMA_CAP_TEMPERATURES_1	0x00000001			///< Bit  0 : Extend Byte ( Bit 32 ): Temperature sensors 1 installed/available
#define SEMA_CAP_SMBUS_PEC		0x00000002			///< Bit  1 : Extend Byte ( Bit 33 ): SM-Bus PEC protocol
#define SEMA_CAP_AFU_EXIST		0x00000004			///< Bit  2 : Extend Byte ( Bit 34 ): This bit is reserved in BMC , SEMA use this bit to identify what AFU exists or not
#define SEMA_CAP_ERROR_LOG		0x00000008			///< Bit  3 : Extend Byte ( Bit 35 ): Error log
#define SEMA_CAP_1WIRE			0x00000010			///< Bit  4 : Extend Byte ( Bit 36 ): 1-wire bus
#define SEMA_CAP_BMCAP_WAKE		0x00000020			///< Bit  5 : Extend Byte ( Bit 37 ): Wake by BMC
#define SEMA_CAP_GPIO_ADC		0x00000040			///< Bit  6 : Extend Byte ( Bit 38 ): GPIO with alternate function
#define SEMA_CAP_SOFT_FAN		0x00000080			///< Bit  7 : Extend Byte ( Bit 39 ): Software fan
#define SEMA_CAP_I2C_STS		0x00000100			
#define SEMA_CAP_GPIO_INTR		0x00000200			


#define SEMA_FAN_CPU		0
#define SEMA_FAN_SYSTEM_1  	1
#define SEMA_FAN_SYSTEM_2	2
#define SEMA_FAN_SYSTEM_3	3

#define SEMA_FAN_MODE_AUTO	0	
#define SEMA_FAN_MODE_ON	2
#define SEMA_FAN_MODE_OFF 	1
#define SEMA_FAN_MODE_SOFT_FAN 	3

#define SEMA_FAN_TEMP_CPU	0
#define SEMA_FAN_TEMP_SYS	1
#define SEMA_FAN_TEMP_SYS_1	2
#define SEMA_FAN_LEVEL_MAX  4
#define SEMA_FAN_ID_MAX    4

//Ext. I2C defines
#define SEMA_EXT_IIC_BUS_1		0
#define SEMA_EXT_IIC_BUS_2		1
#define SEMA_EXT_IIC_BUS_3		2
#define SEMA_EXT_IIC_BUS_4		3


#define SCHAR_MIN   (-128)      /* minimum signed char value */
#define SCHAR_MAX     127       /* maximum signed char value */


typedef enum
{
	SEMA_SRE_UNKNOWN = 0x00,
	SEMA_SRE_SW_RESET = 0x20,
	SEMA_SRE_HW_RESET = 0x30,
	SEMA_SRE_WATCHDOG = 0x40,
	SEMA_SRE_BIOS_FAULT = 0x50,
	SEMA_SRE_PWR_DOWN = 0x60,
	SEMA_SRE_PWR_LOSS = 0x70,
	SEMA_SRE_PWR_CYCLE = 0x80,
	SEMA_SRE_VIN_DROP = 0x90,
	SEMA_SRE_PWR_FAIL = 0xA0,
	SEMA_SRE_CRIT_TEMP = 0xB0,
	SEMA_SRE_WAKEUP = 0xC0
} tSEMA_SRE;

class Ccommon_bmc
{


	typedef struct _MSR_COMMAND
	{
		ULONG ulAdr;
	} MSR_COMMAND, *PMSR_COMMAND;

#define SMB_BIOSVER_INDEX	0x05
#define SMB_LENGTH_INDEX	0x01
#define MS_SMB_OFFSET		0x08


#define SEMA_VERSION_MAJOR	0x04
#define SEMA_VERSION_MINOR	0x02
#define SEMA_VERSION_ADDON  0x14
#define SEMA_VERSION_ADDON_STR  "R0"
#define SEMA_VENDOR			"ADLINK Technology Inc."

public:
	HANDLE m_hDrv;
	bmc_type Init(void);

	HANDLE ghMutex;

	char *exetbl, *ainlist;

	virtual EERROR lock(void);
	void unlock(void);

	EERROR SaveLocalWDT(unsigned int Timeout);
	EERROR SaveLocalPwrWDT(unsigned int Timeout);
	EERROR ShowLocalWDT(unsigned int *Timeout);
	EERROR ShowLocalPwrWDT(unsigned int *Timeout);

	EERROR GetLibraryMajor(uint16_t* pushValue);
	EERROR GetLibraryMinor(uint16_t* pushValue);
	EERROR GetLibraryAddOn(char* szAddOn, uint32_t nSize);

	EERROR GetVersionMajor(uint16_t& ushMajor);
	EERROR GetVersionMinor(uint16_t& ushMinor);
	EERROR Rdmsr(uint32_t, uint32_t *, uint32_t *);
	EERROR GetChipSet(char* szAddOn, uint32_t nSize);


	EERROR GetHardwareMajor(uint16_t* pushValue);
	EERROR GetHardwareMinor(uint16_t* pushValue);
	EERROR GetHardwareChipSet(char* szChipSet, uint32_t nSize);

	bool IsWinXPOrLater();
	EERROR GetBIOSVersion(char* szVersion, uint32_t nSize);

	int Ccommon_bmc::GetPropInfoFromCfgFile(char *propname, char *prop, char **out);
	void Close();
};