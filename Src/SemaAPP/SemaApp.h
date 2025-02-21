#pragma once
#ifndef SEMAAPP
#define SEMAAPP
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SEMAEAPI_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SEMAEAPI_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SEMAEAPI_EXPORTS
#define SEMAEAPI_API __declspec(dllexport)
#else
#define SEMAEAPI_API __declspec(dllimport)
#endif

#include "stdafx.h"
#include  <string.h>
#include "EApi.h"

typedef struct
{
	bool	SetWatchdog, TriggerWatchdog, StopWatchdog, WatchDogCap, IsPwrUpWDogStart, IsPwrUpWDogStop;
	struct {
		unsigned int	WatchdogTimeOut;
		unsigned int	Delay;
		unsigned int	EventTimeout;
	}WatchDogFuncArgs;

	bool	StorageCap, StorageAreaRead, StorageAreaWrite, StorageClear, StorageLock, StorageUnlock, GUIDRead, GUIDWrite, StorageHexRead, StorageHexWrite, ODM_Write;
	struct {
		uint32_t    Id;
		uint32_t	Address;
		uint32_t	Permission;
		uint32_t	Length;
		char    	Password[32];
		char*	String;
	}StorageFuncArgs;

	bool	SmartFanTempSet, SmartFanTempGet, SmartFanTempSetSrc, SmartFanTempGetSrc, SmartFanPWMSet;
	bool	SmartFanModeGet, SmartFanModeSet, SmartFanPWMGet;
	struct {
		unsigned char	FanID;
		unsigned int	Level[4];
		unsigned char	TempSrc;
		unsigned char	FanMode;
	}SmartFaneFuncArgs;

	bool	GetStringA,  GetValue, GetVoltageMonitor;
	struct {
		unsigned char cap;
		unsigned int Size;
		unsigned char pData[250] = { 0 };
		unsigned int IntData;
		unsigned int  rData;
		unsigned char channel;
		unsigned int  voltage;
		unsigned int Id;
		unsigned int outSize;
		unsigned int inSize;
	}SemaNativeFuncArgs;

	bool	VgaGetBacklightEnable, VgaSetBacklightEnable, VgaGetBacklightBrightness, VgaSetBacklightBrightness;
	struct {
		unsigned int level;
		unsigned char Id;
		unsigned int EnnDI;
	}backlightFuncArgs;


	bool	GPIOGetDirectionCaps, GPIOGetDirection, GPIOSetDirection, GPIOGetLevel, GPIOSetLevel;
	struct {
		unsigned char	GPIOID;
		unsigned char	GPIOPin;
		unsigned int	Dir;
		unsigned int	Level;
	}GPIOFuncArgs;

	bool	GetBIOSSource, SetBIOSSource, GetBIOSSourceSts;
	struct {
		unsigned char bit0;
		unsigned char bit1;
	}BIOSSourceArgs;


	bool GetErrorLog, GetErrorNumberDescription, GetCurrentPosErrorLog, GetExceptionDescription;
	struct {
		unsigned char	Pos;
		unsigned char	ErrRExcNo;
		uint32_t		ErrorNumber;
		uint8_t  Flags;
		uint8_t  RestartEvent;
		uint32_t PwrCycles;
		uint32_t Bootcount;
		uint32_t Time;
		uint32_t TotalOnTime;
		uint8_t	 BiosSel;
		uint16_t Status;
		signed char CPUtemp;
		signed char Boardtemp;
	}ErrorNExceFuncArgs;

	typedef enum {
		I2Cr,
		I2Cw,
	}I2c1Op;

	bool IsI2CCap, IsI2CProb, IsI2CSts, IsI2CReRaw, IsI2CWrRaw, IsI2CReXf, IsI2CWrXf;
	struct {
		uint32_t BusID;
		uint16_t Address;
		uint32_t CmdType;
		uint32_t cmd;
		void* pBuffer;
		void* pWrBuffer;
		uint32_t nByteCnt;
		uint32_t WByteCnt;
	}I2CFuncArgs;
}tCmdLineArgs;


typedef enum
{
	SMB = 1,
	EC,
}bmc_type;

signed int ParseArgs(int argc, char* argv[], tCmdLineArgs *Args);
#endif