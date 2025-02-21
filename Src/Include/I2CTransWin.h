#pragma once

#include "stdafx.h"
#include "common.h"
#include <windows.devices.gpio.h>
#include <windows.devices.i2c.h>
#include <windows.foundation.h>
#include "winioctl.h"
#include "timeapi.h"
#include <stdio.h>
#include <ppltasks.h> 
#include <windows.h>
#include <wrl.h>
#include <collection.h> 
#include <strsafe.h>
#include <string> 
#include <vector> 
#include <sstream>
#include <iostream>
#include <cwctype>
#pragma comment(lib, "winmm.lib")



using namespace Platform;
using namespace std;
using namespace Windows::Devices::I2c;
using namespace ABI::Windows::Foundation;
#define IsReadAdr(a)	((a&1) ? TRUE : FALSE)

typedef enum
{
	TT_WB_I2C = 0x00,		// Write Byte
	TT_WBB_I2C,		// Write Byte(Cmd) Byte
	TT_WBW_I2C,		// Write Byte(Cmd) Word
	TT_WWB_I2C,		// Write Word(Cmd) Byte
	TT_RB_I2C,		// Read Byte	
	TT_RBB_I2C,		// Read Byte(Cmd) Byte
	TT_RBW_I2C,		// Read Byte(Cmd) Word
	TT_RWB_I2C,		// Read Word(Cmd) Byte
	TT_RBL_I2C,		// Read Block
	TT_WBL_I2C,		// Write Block
	TT_I2C_RBL_I2C, // I2C Read Block
	TT_I2C_WBL_I2C  // I2C Write Block
}tTransTypeI2c;

#define SEMASMB_VERSION_ADDON_MAXSIZE		0x20
#define SEMASMB_VERSION_GETSMBSTRING_SIZE	0x40
#define SEMASMB_PEC_SIZE 0x01

class CI2CTransport
{
private:
	//HANDLE m_hSemaDrv;
	I2cDevice^ device_cls;
	
public:
	CI2CTransport();
	virtual ~CI2CTransport();
	
	EERROR Init(HANDLE);
	EERROR BlockTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen, uint8_t* pDataRet, uint32_t& nRetLen);
	EERROR BlockI2CTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen, uint8_t* pDataRet, uint32_t& nRetLen);
	EERROR ByteTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen, uint8_t* pDataRet, uint32_t& nRetLen);
	EERROR BlockTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen);
	int MakeDevice(int slaveAddress, _In_opt_ String^ friendlyName);
	int I2Cread_write(I2cDevice^ device1, std::wstring command, std::vector<BYTE> writeBuf, unsigned int bytesToRead, unsigned char* read_val);

};
