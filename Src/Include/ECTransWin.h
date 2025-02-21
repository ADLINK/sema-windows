#pragma once

#include "stdafx.h"
#include "common.h"

#define EC_REGION_1   ((0x62 << 8) | 0x66)
#define EC_REGION_2   ((0x68 << 8) | 0x6C)

class CECTransport
{
private:
	HANDLE m_hSemaDrv;

public:
	CECTransport();
	virtual ~CECTransport();

	EERROR Init(HANDLE);
	EERROR ECRead(uint16_t ECAddr, uint16_t RegionIndex, uint8_t* pDataRet, uint32_t nRet);
	EERROR ECWrite(uint16_t ECAddr, uint16_t RegionIndex, uint8_t* pDataIn, uint32_t nInLen);
};
