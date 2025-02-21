#include "stdafx.h"
#include "ECTransWin.h"

#include "Error.h"
#include "tchar.h"

#include "winioctl.h"
#include "timeapi.h"
#pragma comment(lib, "winmm.lib")


typedef struct _EC_Data
{
	uint16_t	bAddr;					// EC RAM address (start). Driver will read until the bLength
	uint16_t	RegionIndex;					// Command data (not used right now)
	uint32_t	bLength;				// Length of data to write OR Read
	uint8_t     Data[32];				// Data Array for writing
} EC_Data, * PEC_Data;


#define SEMASMB_VERSION_ADDON_MAXSIZE		0x20
#define SEMASMB_VERSION_GETSMBSTRING_SIZE	0x40
#define SEMASMB_PEC_SIZE 0x01

CECTransport::CECTransport()
{
	m_hSemaDrv = INVALID_HANDLE_VALUE;
}

EERROR CECTransport::Init(HANDLE Drvhdl)
{
	m_hSemaDrv = Drvhdl;
	return (m_hSemaDrv != INVALID_HANDLE_VALUE) ? EAPI_STATUS_SUCCESS : EAPI_STATUS_NOT_INITIALIZED;
}

EERROR CECTransport::ECRead(uint16_t ECAddr, uint16_t ECRegion, uint8_t* pDataRet, uint32_t nRead)
{
	EC_Data ec_read;
	DWORD dwRet;

	if (m_hSemaDrv == INVALID_HANDLE_VALUE)
	{
		return EAPI_STATUS_NOT_FOUND;
	}

	ec_read.bAddr = ECAddr;
	ec_read.RegionIndex = ECRegion;
	ec_read.bLength = nRead;

	if (DeviceIoControl(m_hSemaDrv, IOCTL_SemaEC_READ, &ec_read, sizeof(ec_read), pDataRet, nRead, &dwRet, NULL))
	{
		return EAPI_STATUS_SUCCESS;
	}
	return EAPI_STATUS_ERROR;
}

EERROR CECTransport::ECWrite(uint16_t ECAddr, uint16_t ECRegion, uint8_t* pDataIn, uint32_t nInLen)
{
	EC_Data ec_wr;
	DWORD dwRet;

	ec_wr.bAddr = ECAddr;
	ec_wr.RegionIndex = ECRegion;
	ec_wr.bLength = nInLen;
	memcpy(ec_wr.Data, pDataIn, nInLen);

	if (DeviceIoControl(m_hSemaDrv, IOCTL_SemaEC_WRITE, &ec_wr, sizeof(ec_wr), ec_wr.Data, nInLen, &dwRet, NULL))
	{
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}

CECTransport::~CECTransport()
{
	m_hSemaDrv = INVALID_HANDLE_VALUE;
}