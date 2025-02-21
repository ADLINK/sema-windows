#include "stdafx.h"
#include "SMBusTransWin.h"

#include "Error.h"
#include "tchar.h"

#include "winioctl.h"
#include "timeapi.h"
#pragma comment(lib, "winmm.lib")


CSemaTransport::CSemaTransport()
{
	m_hSemaDrv = INVALID_HANDLE_VALUE;
}

EERROR CSemaTransport::Init(HANDLE drvhdl)
{
	m_hSemaDrv = drvhdl;
	return (m_hSemaDrv != INVALID_HANDLE_VALUE) ? EAPI_STATUS_SUCCESS : EAPI_STATUS_NOT_INITIALIZED;
}

EERROR CSemaTransport::ByteTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen, uint8_t* pDataRet, uint32_t& nRetLen)
{
	SMB_COMMAND stCmd;
	DWORD dwRet = 0x00;

	ZeroMemory((uint8_t*)&stCmd, sizeof(SMB_COMMAND));

	if (m_hSemaDrv == INVALID_HANDLE_VALUE)
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	stCmd.bAddr = bAddr >> 0x01;
	stCmd.bTransType = bType;

	stCmd.pData[0] = pBufIn[0];
	stCmd.pData[1] = pBufIn[1];
	stCmd.pData[2] = pBufIn[2];
	stCmd.pData[3] = pBufIn[3];
	
	Sleep(0x08);

	if (DeviceIoControl(m_hSemaDrv, IOCTL_SemaSMB_READ_WRITE, &stCmd, sizeof(SMB_COMMAND), pDataRet, nRetLen, &dwRet, NULL))
	{
		nRetLen = (uint8_t)dwRet;
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}

EERROR CSemaTransport::BlockTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen)
{
	BYTE pDataRet[MAX_PATH]; uint32_t nOutLen = MAX_PATH;
	
	ZeroMemory(pDataRet, MAX_PATH);

	return BlockTrans(bAddr, bType, bCmd, pBufIn, nInLen, pDataRet, nOutLen);
}

EERROR CSemaTransport::BlockTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen, uint8_t* pDataRet, uint32_t& nRetLen)
{
	SMB_COMMAND stCmd;
	DWORD dwRet = 0x00;
	ZeroMemory((uint8_t*)&stCmd, sizeof(SMB_COMMAND));

	if (m_hSemaDrv == INVALID_HANDLE_VALUE)
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	stCmd.bAddr = bAddr >> 0x01;
	stCmd.bLength = (uint8_t)nInLen;
	stCmd.bCmd = bCmd;
	stCmd.bTransType = bType;

	memcpy_s(stCmd.pData, SMB_MAX_LENGTH, pBufIn, nInLen);

	if (DeviceIoControl(m_hSemaDrv, IOCTL_SemaSMB_READ_WRITE_BLOCK, &stCmd, sizeof(SMB_COMMAND), pDataRet, nRetLen, &dwRet, NULL))
	{
		nRetLen = (uint8_t)dwRet;
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}
EERROR CSemaTransport::BlockI2CTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen, uint8_t* pDataRet, uint32_t& nRetLen)
{
	SMB_COMMAND stCmd;
	DWORD dwRet = 0x00;

	ZeroMemory((uint8_t*)&stCmd, sizeof(SMB_COMMAND));

	if (m_hSemaDrv == INVALID_HANDLE_VALUE)
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	stCmd.bAddr = bAddr >> 0x01;
	stCmd.bLength = (uint8_t)nInLen;
	stCmd.bCmd = bCmd;
	stCmd.bTransType = bType;

	memcpy_s(stCmd.pData, SMB_MAX_LENGTH, pBufIn, nInLen);

	if (DeviceIoControl(m_hSemaDrv, IOCTL_SemaSMB_READ_WRITE_BLOCK, &stCmd, sizeof(SMB_COMMAND), pDataRet, nRetLen, &dwRet, NULL))
	{
		nRetLen = (uint8_t)dwRet;
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}

CSemaTransport::~CSemaTransport()
{
	m_hSemaDrv = INVALID_HANDLE_VALUE;
}