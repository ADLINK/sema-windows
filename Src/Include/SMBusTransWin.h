#pragma once

#include "stdafx.h"
#include "common.h"

#define IsReadAdr(a)	((a&1) ? TRUE : FALSE)

typedef enum
{
	TT_WB = 0x00,		// Write Byte
	TT_WBB,		// Write Byte(Cmd) Byte
	TT_WBW,		// Write Byte(Cmd) Word
	TT_WWB,		// Write Word(Cmd) Byte
	TT_RB,		// Read Byte	
	TT_RBB,		// Read Byte(Cmd) Byte
	TT_RBW,		// Read Byte(Cmd) Word
	TT_RWB,		// Read Word(Cmd) Byte
	TT_RBL,		// Read Block
	TT_WBL,		// Write Block
	TT_I2C_RBL, // I2C Read Block
	TT_I2C_WBL  // I2C Write Block
}tTransType;

#define SEMASMB_VERSION_ADDON_MAXSIZE		0x20
#define SEMASMB_VERSION_GETSMBSTRING_SIZE	0x40
#define SEMASMB_PEC_SIZE 0x01

class CSemaTransport
{
private:
	HANDLE m_hSemaDrv;

public:
	CSemaTransport();
	virtual ~CSemaTransport();
	
	EERROR Init(HANDLE);
	EERROR BlockTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen, uint8_t* pDataRet, uint32_t& nRetLen);
	EERROR BlockI2CTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen, uint8_t* pDataRet, uint32_t& nRetLen);
	EERROR ByteTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen, uint8_t* pDataRet, uint32_t& nRetLen);
	EERROR BlockTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen);

};
