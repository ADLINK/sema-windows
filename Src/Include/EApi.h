#pragma once

#ifdef SEMAEAPI_EXPORTS
#define SEMAEAPI_API __declspec(dllexport)
#else
#define SEMAEAPI_API __declspec(dllimport)
#endif
#include <stdint.h>
#include <thread>
#include <chrono>
#include "Error.h"

/************************************************************************
* All version numbers
* +=========+===============================+
* | Bits | Descriptions |
* +=========+===============================+
* | 24 - 31 | Version Number |
* +---------+-------------------------------+
* | 16 - 23 | Revision Number |
* +---------+-------------------------------+
* | 0 - 15 | Build Number |
* +=========+===============================+
*/

#define EAPI_VER_MASK_VER EAPI_UINT32_C(0xFF000000)
#define EAPI_VER_MASK_REV EAPI_UINT32_C(0x00FF0000)
#define EAPI_VER_MASK_BUILD EAPI_UINT32_C(0x0000FFFF)

#define EAPI_VER_GET_VER(x) EAPI_UINT8_C (((x)>>24)&UINT8_MAX )
#define EAPI_VER_GET_REV(x) EAPI_UINT8_C (((x)>>16)&UINT8_MAX )
#define EAPI_VER_GET_BUILD(x) EAPI_UINT16_C(((x)>> 0)&UINT16_MAX)

#define EAPI_VER_CREATE(Version,Revision,Build) (\
									EAPI_UINT32_C(\
									(((Version )&UINT8_MAX )<<24)|\
									(((Revision)&UINT8_MAX )<<16)|\
									(((Build )&UINT16_MAX)<< 0))\
									)
/* Embedded API Standard Revision */
#define EAPI_VER 1
#define EAPI_REVISION 0
#define EAPI_VERSION EAPI_VER_CREATE(EAPI_VER, EAPI_REVISION, 0)

#define EAPI_MANUFACTURE "ADLINK Technology Inc."

#define EAPI_CURRENT_SEMA_VERSION	"ADLINK-SEMA-EC/BMC-WINDOWS-V4_R2_11_25_05_07"
/*
**G P I O*
*/
/* IDs */
/*
* Individual ID Per GPIO Mapping
*/
#define EAPI_GPIO_GPIO_ID(GPIO_NUM) EAPI_UINT32_C(GPIO_NUM)
#define EAPI_GPIO_GPIO_BITMASK EAPI_UINT32_C(1)
#define EAPI_ID_GPIO_GPIO00 EAPI_GPIO_GPIO_ID(0) /* (Optional) */
#define EAPI_ID_GPIO_GPIO01 EAPI_GPIO_GPIO_ID(1) /* (Optional) */
#define EAPI_ID_GPIO_GPIO02 EAPI_GPIO_GPIO_ID(2) /* (Optional) */
#define EAPI_ID_GPIO_GPIO03 EAPI_GPIO_GPIO_ID(3) /* (Optional) */
/*
* Multiple GPIOs Per ID Mapping
*/
#define EAPI_GPIO_BANK_ID(GPIO_NUM) EAPI_UINT32_C(0x10000|((GPIO_NUM)>>3))
#define EAPI_GPIO_BANK_ID_EXT(GPIO_NUM)  EAPI_UINT32_C(0x10001|((GPIO_NUM)>>3))

#define EAPI_GPIO_BANK_MASK(GPIO_NUM) EAPI_UINT32_C((1<<((GPIO_NUM)&0x7))

#define EAPI_GPIO_BANK_TEST_STATE(GPIO_NUM, TState, TValue) \
(((TValue>>((GPIO_NUM)&0x7))&1)==(TState))


#define EAPI_ID_GPIO_BANK00 EAPI_GPIO_BANK_ID( 0) /* GPIOs 0 - 31
* (optional)
*/
#define EAPI_ID_GPIO_BANK_EXT00 EAPI_GPIO_BANK_ID_EXT( 0)
#define EAPI_ID_GPIO_BANK01 EAPI_GPIO_BANK_ID(32) /* GPIOs 32 - 63
* (optional)
*/
#define EAPI_ID_GPIO_BANK02 EAPI_GPIO_BANK_ID(64) /* GPIOs 64 - 95
* (optional)
*/
#define EAPI_GPIO_BITMASK_SELECT 1
#define EAPI_GPIO_BITMASK_NOSELECT 0/*Levels */

#define EAPI_GPIO_LOW 0
#define EAPI_GPIO_HIGH 1

/*Directions */
#define EAPI_GPIO_INPUT 1
#define EAPI_GPIO_OUTPUT 0

/***
B O A R D 
I N F O M A T I O N 
S T R I N G S 
**/

#define EAPI_ID_BOARD_MANUFACTURER_STR		EAPI_UINT32_C(0) 
/* Board Manufacturer Name String */
#define EAPI_ID_BOARD_NAME_STR				EAPI_UINT32_C(1)
/* Board Name String */
#define EAPI_ID_BOARD_REVISION_STR			EAPI_UINT32_C(2) 
/* Board Name String */
#define EAPI_ID_BOARD_SERIAL_STR			EAPI_UINT32_C(3) 
 /* Board Serial Number String */
#define EAPI_ID_BOARD_BIOS_REVISION_STR		EAPI_UINT32_C(4) 
 /* Board Bios Revision String */
#define EAPI_ID_BOARD_HW_REVISION_STR		EAPI_UINT32_C(5) 
 /* Board Hardware Revision String */
#define EAPI_ID_BOARD_PLATFORM_TYPE_STR		EAPI_UINT32_C(6) 
 /* Platform ID(ETX, COM Express,etc...) */

#define EAPI_SEMA_ID_BOARD_BOOT_VERSION_STR			EAPI_UINT32_C(7)
/* Boot version string */
#define EAPI_SEMA_ID_BOARD_APPLICATION_VERSION_STR	EAPI_UINT32_C(8)
/* Firmware application version string */
#define EAPI_SEMA_ID_BOARD_CHIPSET_ID_STR			EAPI_UINT32_C(9)
/* Chipset ID string */
#define EAPI_SEMA_ID_BOARD_RESTART_EVENT_STR		EAPI_UINT32_C(10)
/* Restart Event string */
#define EAPI_SEMA_ID_BOARD_DEVICE_ID_STR			EAPI_UINT32_C(11)
/* Device ID string */
#define EAPI_SEMA_ID_BOARD_REPAIR_DATE_STR			EAPI_UINT32_C(12)
/* Last Repair Date */
#define EAPI_SEMA_ID_BOARD_MANUFACTURE_DATE_STR		EAPI_UINT32_C(13)
/* Manufacture date */
#define EAPI_SEMA_ID_BOARD_MAC_1_STRING				EAPI_UINT32_C(14)
/* MAC address 1 on module */
#define EAPI_SEMA_ID_BOARD_MAC_2_STRING				EAPI_UINT32_C(15)
/* MAC address 2 on module */
#define EAPI_SEMA_ID_BOARD_2ND_HW_REVISION_STR		EAPI_UINT32_C(16)
/*2nd HW revision string */
#define EAPI_SEMA_ID_BOARD_2ND_SERIAL_STR			EAPI_UINT32_C(17)
/*2nd HW serial string */

/***
B O A R D
I N F O M A T I O N
V A L U E S
**/

/*EAPI Specification * Revision I.E. The * EAPI Spec Version * Bits 31-24, Revision * 23-16, 15-0 always 0* Used to implement * this interface*/
/*IDS */

#define EAPI_ID_GET_EAPI_SPEC_VERSION			EAPI_UINT32_C(0)

#define EAPI_ID_BOARD_BOOT_COUNTER_VAL			EAPI_UINT32_C(1)
/*Units = Boots */

#define EAPI_ID_BOARD_RUNNING_TIME_METER_VAL	EAPI_UINT32_C(2)
/*Units = Minutes */

#define EAPI_ID_BOARD_PNPID_VAL					EAPI_UINT32_C(3)
/*Encoded PNP ID * Format * (Compressed ASCII)*/

#define EAPI_ID_BOARD_PLATFORM_REV_VAL			EAPI_UINT32_C(4) 
/*Platform Revision * I.E. The PICMG Spec * Version Bits 31-24,* Revision 23-16, * 15-0 always 0*/


#define EAPI_ID_BOARD_DRIVER_VERSION_VAL		EAPI_UINT32_C(0x10000) 
/*Vendor Specific * (Optional) */
#define EAPI_ID_BOARD_LIB_VERSION_VAL			EAPI_UINT32_C(0x10001) 
/*Vendor Specific* (Optional) */
#define EAPI_SEMA_ID_BOARD_POWER_UP_TIME		EAPI_UINT32_C(0x10002)
#define EAPI_SEMA_ID_BOARD_RESTART_EVENT		EAPI_UINT32_C(0x10003)
#define EAPI_SEMA_ID_BOARD_CAPABILITIES			EAPI_UINT32_C(0x10004)
#define EAPI_SEMA_ID_BOARD_CAPABILITIES_EX		EAPI_UINT32_C(0x10005)
#define EAPI_SEMA_ID_BOARD_SYSTEM_MIN_TEMP		EAPI_UINT32_C(0x10006)
#define EAPI_SEMA_ID_BOARD_SYSTEM_MAX_TEMP		EAPI_UINT32_C(0x10007)
#define EAPI_SEMA_ID_BOARD_SYSTEM_STARTUP_TEMP	EAPI_UINT32_C(0x10008)
#define EAPI_SEMA_ID_BOARD_CPU_MIN_TEMP			EAPI_UINT32_C(0x10009)
#define EAPI_SEMA_ID_BOARD_CPU_MAX_TEMP			EAPI_UINT32_C(0x10010)
#define EAPI_SEMA_ID_BOARD_CPU_STARTUP_TEMP		EAPI_UINT32_C(0x10011)
#define EAPI_SEMA_ID_BOARD_MAIN_CURRENT				EAPI_UINT32_C(0x10012)
#define EAPI_SEMA_ID_BOARD_2ND_SYSTEM_TEMP			EAPI_UINT32_C(0x10013)
#define EAPI_SEMA_ID_BOARD_2ND_SYSTEM_MIN_TEMP		EAPI_UINT32_C(0x10014)
#define EAPI_SEMA_ID_BOARD_2ND_SYSTEM_MAX_TEMP		EAPI_UINT32_C(0x10015)
#define EAPI_SEMA_ID_BOARD_2ND_SYSTEM_STARTUP_TEMP	EAPI_UINT32_C(0x10016)
#define EAPI_SEMA_ID_BOARD_POWER_CYCLE				EAPI_UINT32_C(0x10017)
#define EAPI_SEMA_ID_BOARD_BMC_FLAG					EAPI_UINT32_C(0x10018)
#define EAPI_SEMA_ID_BOARD_BMC_STATUS				EAPI_UINT32_C(0x10019)
#define EAPI_SEMA_ID_IO_CURRENT						EAPI_UINT32_C(0x10020)

#define EAPI_ID_HWMON_CPU_TEMP					EAPI_UINT32_C(0x20000) 
/*0.1 Kelvins */
#define EAPI_ID_HWMON_CHIPSET_TEMP				EAPI_UINT32_C(0x20001) 
/*0.1 Kelvins */
#define EAPI_ID_HWMON_SYSTEM_TEMP				EAPI_UINT32_C(0x20002)
/*0.1 Kelvins */
#define EAPI_ID_HWMON_VOLTAGE_VCORE				EAPI_UINT32_C(0x21004) 
/*millivolts */
#define EAPI_ID_HWMON_VOLTAGE_2V5				EAPI_UINT32_C(0x21008)
/*millivolts */
#define EAPI_ID_HWMON_VOLTAGE_3V3				EAPI_UINT32_C(0x2100C)
/*millivolts */
#define EAPI_ID_HWMON_VOLTAGE_VBAT				EAPI_UINT32_C(0x21010)
/*millivolts */
#define EAPI_ID_HWMON_VOLTAGE_5V				EAPI_UINT32_C(0x21014)
/*millivolts */
#define EAPI_ID_HWMON_VOLTAGE_5VSB				EAPI_UINT32_C(0x21018)
/*millivolts */
#define EAPI_ID_HWMON_VOLTAGE_12V				EAPI_UINT32_C(0x2101C)
/*millivolts */
#define EAPI_ID_HWMON_FAN_CPU					EAPI_UINT32_C(0x22000)
/*RPM */
#define EAPI_ID_HWMON_FAN_SYSTEM				EAPI_UINT32_C(0x22001)
/*RPM */
#define EAPI_SEMA_ID_HWMON_VOLTAGE_GFX_VCORE	EAPI_UINT32_C(0x22002)
#define EAPI_SEMA_ID_HWMON_VOLTAGE_1V05			EAPI_UINT32_C(0x22003)
#define EAPI_SEMA_ID_HWMON_VOLTAGE_1V5			EAPI_UINT32_C(0x22004)
#define EAPI_SEMA_ID_HWMON_VOLTAGE_VIN			EAPI_UINT32_C(0x22005)
#define EAPI_SEMA_ID_HWMON_FAN_SYSTEM_2			EAPI_UINT32_C(0x22006)
#define EAPI_SEMA_ID_HWMON_FAN_SYSTEM_3			EAPI_UINT32_C(0x22007)

#define EAPI_KELVINS_OFFSET 2731
#define EAPI_ENCODE_CELCIUS(Celsius) EAPI_UINT32_C((((Celsius)*10))+EAPI_KELVINS_OFFSET)	//To convert Celsius to Kelvin
#define EAPI_DECODE_CELCIUS(Celsius) ((Celsius)- EAPI_KELVINS_OFFSET)/10					//To convert Kelvin to Celsius


/*
*
* B A C K L I G H T
*
*/

/* IDS */
#define EAPI_ID_BACKLIGHT_1 EAPI_UINT32_C(0)
#define EAPI_ID_BACKLIGHT_2 EAPI_UINT32_C(1)
#define EAPI_ID_BACKLIGHT_3 EAPI_UINT32_C(2)

/* Backlight Values */
#define EAPI_BACKLIGHT_SET_ON		 EAPI_UINT32_C(0)
#define EAPI_BACKLIGHT_SET_OFF		 EAPI_UINT32_C(0xFFFFFFFF)
#define EAPI_BACKLIGHT_SET_DIMEST	 EAPI_UINT32_C(0)
#define EAPI_BACKLIGHT_SET_BRIGHTEST EAPI_UINT32_C(255)


/***
S T O R A G E
***/

/*IDs */
#define EAPI_ID_USER_STORAGE EAPI_UINT32_C(1)
#define EAPI_ID_SCRE_STORAGE EAPI_UINT32_C(2)
#define EAPI_ID_ODM_STORAGE	 EAPI_UINT32_C(3)
#define EAPI_ID_FP_STORAGE	 EAPI_UINT32_C(5)

#define EAPI_ID_STORAGE_STD EAPI_ID_USER_STORAGE
#define EAPI_ID_STORAGE_SCR EAPI_ID_SCRE_STORAGE
#define EAPI_ID_STORAGE_ODM EAPI_ID_ODM_STORAGE
#define EAPI_ID_STORAGE_FP EAPI_ID_FP_STORAGE


/***
F A N   C O N T R O L
***/


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

/*
**
I 2 C
**
*/


//Ext. I2C defines
#define SEMA_EXT_IIC_BUS_1		0
#define SEMA_EXT_IIC_BUS_2		1
#define SEMA_EXT_IIC_BUS_3		2
#define SEMA_EXT_IIC_BUS_4		3

#define EAPI_I2C_STD_CMD EAPI_UINT32_C(0<<30)
#define EAPI_I2C_EXT_CMD EAPI_UINT32_C(2<<30)
#define EAPI_I2C_NO_CMD EAPI_UINT32_C(1<<30)

#define EAPI_I2C_CMD_TYPE_MASK EAPI_UINT32_C(3<<30)

#define EAPI_I2C_ENC_STD_CMD(x) EAPI_UINT32_C(((x) & 0xFF) | EAPI_I2C_STD_CMD)
#define EAPI_I2C_ENC_EXT_CMD(x) EAPI_UINT32_C(((x) & 0xFFFF) | EAPI_I2C_EXT_CMD)

#define EAPI_I2C_IS_EXT_CMD(x) (EAPI_UINT32_C((x) & (EAPI_I2C_CMD_TYPE_MASK)) == EAPI_I2C_EXT_CMD)
#define EAPI_I2C_IS_STD_CMD(x) (EAPI_UINT32_C((x) & (EAPI_I2C_CMD_TYPE_MASK)) == EAPI_I2C_STD_CMD)
#define EAPI_I2C_IS_NO_CMD(x)  (EAPI_UINT32_C((x) & (EAPI_I2C_CMD_TYPE_MASK)) == EAPI_I2C_NO_CMD)



SEMAEAPI_API uint32_t EApiLibInitialize(void);
SEMAEAPI_API uint32_t EApiLibUnInitialize(void);

SEMAEAPI_API uint32_t SemaEApiLibInitialize(void);
SEMAEAPI_API uint32_t SemaEApiUnInitialize(void);


SEMAEAPI_API uint32_t EApiWDogGetCap(uint32_t *pMaxDelay, uint32_t *pMaxEventTimeout, uint32_t* Resetvalue);
SEMAEAPI_API uint32_t EApiWDogStart(uint32_t delay, uint32_t EventTimeout, uint32_t ResetTime);
SEMAEAPI_API uint32_t EApiWDogTrigger();
SEMAEAPI_API uint32_t EApiWDogStop();

SEMAEAPI_API uint32_t SemaEApiWDogGetCap(uint32_t *pMaxDelay, uint32_t *pMaxEventTimeout, uint32_t* Resetvalue);
SEMAEAPI_API uint32_t SemaEApiWDogStart(uint32_t delay, uint32_t EventTimeout, uint32_t ResetTime);
SEMAEAPI_API uint32_t SemaEApiWDogTrigger();
SEMAEAPI_API uint32_t SemaEApiWDogStop();
SEMAEAPI_API uint32_t SemaEApiWDogGetLastValue(uint32_t *LastValue);


SEMAEAPI_API uint32_t EApiPwrUpWDogStart(uint32_t ResetTime);
SEMAEAPI_API uint32_t EApiPwrUpWDogStop();

SEMAEAPI_API uint32_t SemaEApiPwrUpWDogStart(uint32_t ResetTime);
SEMAEAPI_API uint32_t SemaEApiPwrUpWDogStop();


SEMAEAPI_API uint32_t EApiStorageAreaRead(uint32_t Id, uint32_t nOffset, void* pBuffer, uint32_t nBufLen, uint32_t nByteCnt);
SEMAEAPI_API uint32_t EApiStorageAreaWrite(uint32_t Id, uint32_t nOffset, void* pBuffer, uint32_t  nByteCnt);
SEMAEAPI_API uint32_t EApiStorageCap(uint32_t Id, uint32_t* pStorageSize, uint32_t* pBlockLength);

SEMAEAPI_API uint32_t SemaEApiStorageAreaRead(uint32_t Id, uint32_t nOffset, void* pBuffer, uint32_t nBufLen, uint32_t nByteCnt);
SEMAEAPI_API uint32_t SemaEApiStorageAreaWrite(uint32_t Id, uint32_t nOffset, void* pBuffer, uint32_t  nByteCnt);
SEMAEAPI_API uint32_t SemaEApiStorageCap(uint32_t Id, uint32_t* pStorageSize, uint32_t* pBlockLength);
SEMAEAPI_API uint32_t SemaEApiStorageClear(uint32_t Id);
SEMAEAPI_API uint32_t SemaEApiStorageLock(uint32_t Id);
SEMAEAPI_API uint32_t SemaEApiStorageUnlock(uint32_t Id, uint32_t Permission, char *Password);
SEMAEAPI_API uint32_t EApiStorageHexRead(uint32_t Id, uint32_t nOffset, void* pBuffer, uint32_t nBufLen, uint32_t nByteCnt);
SEMAEAPI_API uint32_t EApiGUIDWrite(uint32_t Id, uint32_t nOffset, void* pBuffer, uint32_t nByteCnt);
SEMAEAPI_API uint32_t EApiStorageHexWrite(uint32_t Id, uint32_t nOffset, void* pBuffer, uint32_t nByteCnt);

SEMAEAPI_API uint32_t EApiBoardGetValue(uint32_t Id, uint32_t* pData);
SEMAEAPI_API uint32_t EApiBoardGetStringA(uint32_t Id, uint8_t* pData, uint32_t *pBufLen); 

SEMAEAPI_API uint32_t SemaEApiBoardGetValue(uint32_t Id, uint32_t* pData);
SEMAEAPI_API uint32_t SemaEApiBoardGetStringA(uint32_t Id, uint8_t* pData, uint32_t *pBufLen);


SEMAEAPI_API uint32_t EApiGPIOGetDirectionCaps(uint8_t Id, uint32_t* pnCapsIn, uint32_t* pnCapsOut);
SEMAEAPI_API uint32_t EApiGPIOGetDirection(uint8_t GpioId, uint32_t Bitmask, uint32_t* pDirection);
SEMAEAPI_API uint32_t EApiGPIOSetDirection(uint8_t GpioId, uint32_t Bitmask, uint32_t pDirection);
SEMAEAPI_API uint32_t EApiGPIOGetLevel(uint8_t GpioId, uint32_t Bitmask, uint32_t* plevel);
SEMAEAPI_API uint32_t EApiGPIOSetLevel(uint8_t GpioId, uint32_t Bitmask, uint32_t plevel);

SEMAEAPI_API uint32_t SemaEApiGPIOGetDirectionCaps(uint8_t Id, uint32_t* pnCapsIn, uint32_t* pnCapsOut);
SEMAEAPI_API uint32_t SemaEApiGPIOGetDirection(uint8_t GpioId, uint32_t Bitmask, uint32_t* pDirection);
SEMAEAPI_API uint32_t SemaEApiGPIOSetDirection(uint8_t GpioId, uint32_t Bitmask, uint32_t pDirection);
SEMAEAPI_API uint32_t SemaEApiGPIOGetLevel(uint8_t GpioId, uint32_t Bitmask, uint32_t* plevel);
SEMAEAPI_API uint32_t SemaEApiGPIOSetLevel(uint8_t GpioId, uint32_t Bitmask, uint32_t plevel);


SEMAEAPI_API uint32_t EApiVgaGetBacklightBrightness(uint32_t PanelId, uint8_t* PWMLevel);
SEMAEAPI_API uint32_t EApiVgaGetBacklightEnable(uint32_t PanelId, uint8_t* PWMLevel);
SEMAEAPI_API uint32_t EApiVgaSetBacklightEnable(uint32_t PanelId, uint8_t PWMLevel);
SEMAEAPI_API uint32_t EApiVgaSetBacklightBrightness(uint32_t PanelId, uint8_t PWMLevel);

SEMAEAPI_API uint32_t SemaEApiVgaGetBacklightBrightness(uint32_t PanelId, uint8_t* PWMLevel);
SEMAEAPI_API uint32_t SemaEApiVgaGetBacklightEnable(uint32_t PanelId, uint8_t* PWMLevel);
SEMAEAPI_API uint32_t SemaEApiVgaSetBacklightEnable(uint32_t PanelId, uint8_t PWMLevel);
SEMAEAPI_API uint32_t SemaEApiVgaSetBacklightBrightness(uint32_t PanelId, uint8_t PWMLevel);


SEMAEAPI_API uint32_t EApiI2CWriteReadRaw(
	uint32_t Id, uint32_t Addr, void *pWBuffer, uint32_t WriteBCnt, void *pRBuffer, \
	uint32_t RBufLen, uint32_t ReadBCnt);
SEMAEAPI_API uint32_t EApiI2CProbeDevice(uint8_t Id, uint32_t Addr);
SEMAEAPI_API uint32_t EApiI2CReadTransfer(uint32_t Id, uint32_t Addr, uint32_t Cmd, \
	void *pBuffer, uint32_t BufLen, uint32_t ByteCnt);
SEMAEAPI_API uint32_t EApiI2CWriteTransfer(uint32_t Id, uint32_t Addr, uint32_t Cmd, \
	void *pBuffer, uint32_t ByteCnt);
SEMAEAPI_API uint32_t EApiI2CGetBusCap(uint32_t Id, uint32_t *pMaxBlkLen);

SEMAEAPI_API uint32_t SemaEApiI2CWriteReadRaw(
	uint32_t Id, uint32_t Addr, void *pWBuffer, uint32_t WriteBCnt, void *pRBuffer, \
	uint32_t RBufLen, uint32_t ReadBCnt);
SEMAEAPI_API uint32_t SemaEApiI2CProbeDevice(uint8_t Id, uint32_t Addr);
SEMAEAPI_API uint32_t SemaEApiI2CReadTransfer(uint32_t Id, uint16_t Addr, uint32_t Cmd, \
	void *pBuffer, uint32_t BufLen, uint32_t ByteCnt);
SEMAEAPI_API uint32_t SemaEApiI2CWriteTransfer(uint32_t Id, uint32_t Addr, uint32_t Cmd, \
	void *pBuffer, uint32_t ByteCnt);
SEMAEAPI_API uint32_t SemaEApiI2CGetBusCap(uint32_t Id, uint32_t *pMaxBlkLen);
SEMAEAPI_API uint32_t SemaEApiI2CGetBusSts(uint32_t Id, uint32_t *pStatus);


SEMAEAPI_API uint32_t SemaEApiSmartFanSetTempSetpoints(uint32_t FanID, uint32_t Level1, uint32_t Level2, uint32_t Level3, uint32_t Level4);
SEMAEAPI_API uint32_t SemaEApiSmartFanGetTempSetpoints(uint32_t FanId, uint32_t* pLevel1, uint32_t* pLevel2, uint32_t* pLevel3, uint32_t* pLevel4);
SEMAEAPI_API uint32_t SemaEApiSmartFanSetPWMSetpoints(uint32_t FanID, uint32_t Level1, uint32_t Level2, uint32_t Level3, uint32_t Level4);
SEMAEAPI_API uint32_t SemaEApiSmartFanGetPwmSetpoints(uint32_t FanId, uint32_t* pLevel1, uint32_t* pLevel2, uint32_t* pLevel3, uint32_t* pLevel4);
SEMAEAPI_API uint32_t SemaEApiSmartFanGetTempSrc(uint32_t FanId, uint8_t* pTempSrc);
SEMAEAPI_API uint32_t SemaEApiSmartFanSetTempSrc(uint32_t FanId, uint8_t pTempSrc);
SEMAEAPI_API uint32_t SemaEApiSmartFanGetMode(uint32_t FanId, uint8_t* pFanMode);
SEMAEAPI_API uint32_t SemaEApiSmartFanSetMode(uint32_t FanId, uint8_t pFanMode);

SEMAEAPI_API uint32_t SemaEApiBoardGetVoltageMonitor(uint32_t channel, uint32_t *pValue, char *pBuffer);

SEMAEAPI_API uint32_t SemaEApiBoardGetErrorLog(uint32_t  position, uint32_t *ErrorNumber, uint8_t  *Flags, uint8_t  *RestartEvent, uint32_t *PwrCycles, uint32_t *Bootcount, uint32_t *Time, uint32_t *TotalOnTime, uint8_t *BIOSSel, uint16_t *Status, signed char *CPUtemp, signed char *Boardtemp);
SEMAEAPI_API uint32_t SemaEApiBoardGetCurErrorLog(uint32_t *ErrorNumber, uint8_t  *Flags, uint8_t  *RestartEvent, uint32_t *PwrCycles, uint32_t *Bootcount, uint32_t *Time, uint32_t *TotalOnTime, uint8_t *BIOSSel, uint16_t *Status, signed char *CPUtemp, signed char *Boardtemp);

SEMAEAPI_API uint32_t SemaEApiBoardGetErrorNumberDescription(uint32_t ErrorNum, uint8_t* pBuffer);
SEMAEAPI_API uint32_t SemaEApiBoardGetExceptionDescription(uint32_t ErrorCode, uint8_t* pBuffer);

// Bios source

SEMAEAPI_API uint32_t SemaEApiGetBIOSSource(uint8_t* bit0, uint8_t* bit1);
SEMAEAPI_API uint32_t SemaEApiSetBIOSSource(uint8_t bit0, uint8_t bit1);
SEMAEAPI_API uint32_t SemaEApiGetBIOSSourceSts(uint8_t* bit0, uint8_t* bit1, uint8_t* bit2);

#define EAPI_UINT8_C(x) ((uint8_t)(x))
#define EAPI_UINT16_C(x) ((uint16_t)(x))
#define EAPI_UINT32_C(x) ((uint32_t)(x))
uint32_t getid(uint8_t gid);
