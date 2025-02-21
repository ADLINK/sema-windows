#include "stdafx.h"
#include "../Include/Utility.h"

CUtil::CUtil()
{

}

CUtil::~CUtil()
{

}

/*
EERROR CUtil::StrToBuf(char* szInfo, uint8_t* pData, uint32_t nSize, uint32_t* pRetSize)
{
	EERROR eRet = ER_INVALID_POINTER;
	if ((szInfo != NULL) && (pData != NULL) && (pRetSize != NULL))
	{
		uint32_t nSizeInfo = (uint32_t)strlen(szInfo);
		eRet = (nSize > nSizeInfo) ? ER_OK : ER_MORE_DATA;
		(*pRetSize) = (nSize > nSizeInfo) ? nSizeInfo : nSize;
		memcpy_s(pData, nSize, szInfo, (*pRetSize));
	}
	return eRet;
}

EERROR CUtil::ByteToBuf(uint8_t bData, uint8_t* pData, uint32_t nSize, uint32_t* pRetSize)
{
	EERROR eRet = ER_INVALID_POINTER;
	if ((pData != NULL) && (pRetSize != NULL))
	{
		eRet = ER_SIZE;
		if (nSize >= 0x01)
		{
			*pRetSize = 0x01;
			*((uint8_t*)pData) = bData;
			eRet = ER_OK;
		}
	}
	return eRet;
}

EERROR CUtil::UShortToBuf(uint16_t ushData, uint8_t* pData, uint32_t nSize, uint32_t* pRetSize)
{
	EERROR eRet = ER_INVALID_POINTER;
	if ((pData != NULL) && (pRetSize != NULL))
	{
		eRet = ER_SIZE;
		if (nSize >= 0x02)
		{
			*pRetSize = 0x02;
			*((uint16_t*)pData) = ushData;
			eRet = ER_OK;
		}
	}
	return eRet;
}

EERROR CUtil::UintToBuf(uint32_t nData, uint8_t* pData, uint32_t nSize, uint32_t* pRetSize)
{
	EERROR eRet = ER_INVALID_POINTER;
	if ((pData != NULL) && (pRetSize != NULL))
	{
		eRet = ER_SIZE;
		if (nSize >= 0x04)
		{
			*pRetSize = 0x04;
			*((uint32_t*)pData) = nData;
			eRet = ER_OK;
		}
	}
	return eRet;
}
*/