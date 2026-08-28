#include "stdafx.h"
#include <fstream>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "winioctl.h"

#include "Common.h"
#include "SemaFunct.h"

struct info {
	uint32_t board_type;
	char boardname[64];
};

using namespace std;

bmc_type Ccommon_bmc::Init()
{
	unsigned long outlen;
	bmc_type TypeRet = UNKNOWN;
	struct info ReadData;

	m_hDrv = CreateFile(DEV_PATH, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (m_hDrv == INVALID_HANDLE_VALUE)
	{
		return UNKNOWN;
	}

	if (DeviceIoControl(m_hDrv, IOCTL_SemaEC_OR_SMB, NULL, 0, &ReadData, sizeof(ReadData), &outlen, NULL))
	{
		if (ReadData.board_type == SMB)
		{
			TypeRet = SMB;
		}
		else if (ReadData.board_type == EC)
		{
			TypeRet = EC;
		}
	}

	return TypeRet;
}

EERROR Ccommon_bmc::lock(void)
{
	DWORD dwWaitResult;

	dwWaitResult = WaitForSingleObject(
		ghMutex,    // handle to mutex
		10);  // 10ms time-out interval

	if(dwWaitResult == WAIT_FAILED)
	{
		return EAPI_STATUS_ERROR;
	}

	return EAPI_STATUS_SUCCESS;
}

void Ccommon_bmc::unlock(void)
{
	ReleaseMutex(ghMutex);
}

EERROR Ccommon_bmc::Rdmsr(uint32_t nIndex, uint32_t* pEax, uint32_t* pEdx)
{
	DWORD dwRet = 0x00;
	uint32_t pDataRet[0x02];
	UINT64 data_msr;
	MSR_COMMAND  stCmd;

	ZeroMemory((uint8_t*)&stCmd, sizeof(MSR_COMMAND));
	ZeroMemory(pDataRet, sizeof(uint32_t) * 0x02);

	if (m_hDrv == INVALID_HANDLE_VALUE)
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	stCmd.ulAdr = nIndex;

	if (DeviceIoControl(m_hDrv, IOCTL_Sema_READ_MSR, &stCmd, sizeof(MSR_COMMAND), &data_msr, sizeof(UINT64), &dwRet, NULL))

	{
		*pEax = (UINT32)(data_msr & 0xFFFFFFFF);
		*pEdx = ((data_msr >> 32) & 0xFFFFFFFF);

		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}

EERROR Ccommon_bmc::GetVersionMajor(uint16_t& ushMajor)
{
	uint16_t pushBuf[0x02]; DWORD dwRet = 0x00;

	if (m_hDrv != INVALID_HANDLE_VALUE)
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (DeviceIoControl(m_hDrv, IOCTL_Sema_GETVERSION, NULL, 0x00, pushBuf, 0x02 * sizeof(uint16_t), &dwRet, NULL))
	{
		ushMajor = pushBuf[0x00];
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}

EERROR Ccommon_bmc::GetVersionMinor(uint16_t& ushMinor)
{
	uint16_t pushBuf[0x02]; DWORD dwRet = 0x00;

	if (m_hDrv == INVALID_HANDLE_VALUE)
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (DeviceIoControl(m_hDrv, IOCTL_Sema_GETVERSION, NULL, 0x00, pushBuf, 0x02 * sizeof(uint16_t), &dwRet, NULL))
	{
		ushMinor = pushBuf[0x00];
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}

EERROR Ccommon_bmc::GetChipSet(char* szAddOn, uint32_t nSize)
{
	EERROR eRet;
	char szData[MAX_PATH];
	DWORD dwRet = 0x00;

	if (m_hDrv == INVALID_HANDLE_VALUE)
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (DeviceIoControl(m_hDrv, IOCTL_Sema_GETSMBSTRING, NULL, 0x00, szData, MAX_PATH, &dwRet, NULL))
	{
		eRet = (nSize - 0x01) > dwRet ? EAPI_STATUS_SUCCESS : EAPI_STATUS_MORE_DATA;
		memcpy_s(szAddOn, nSize - 0x01, szData, (nSize - 0x01) > dwRet ? dwRet : (nSize - 0x01));
		return eRet;
	}

	return EAPI_STATUS_ERROR;
}

void Ccommon_bmc::Close()
{
	if (ghMutex != NULL)
	{
		CloseHandle(ghMutex);
		ghMutex = NULL;
	}

	if (m_hDrv != INVALID_HANDLE_VALUE)
		CloseHandle(m_hDrv);
	m_hDrv = INVALID_HANDLE_VALUE;
}

EERROR Ccommon_bmc::SaveLocalWDT(unsigned int Timeout)
{
	EERROR eRet;
	DWORD dwRet = 0x00;
	uint32_t pushBuf[2] = { 0 }, pushBufTe[2] = { 0 };

	if ((eRet = ShowLocalPwrWDT(&pushBuf[1])) != EAPI_STATUS_SUCCESS)
	{
		return eRet;
	}

	pushBuf[0] = Timeout;

	if (DeviceIoControl(m_hDrv, IOCTL_Sema_Set_WDT, pushBuf, sizeof(pushBuf), pushBufTe, sizeof(pushBufTe), &dwRet, NULL))
	{
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}

EERROR Ccommon_bmc::ShowLocalWDT(unsigned int *Timeout)
{
	DWORD dwRet = 0x02;
	uint32_t pushBuf[2] = { 0 }, pushBufTe[2] = { 0 };

	if (m_hDrv == INVALID_HANDLE_VALUE)
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (DeviceIoControl(m_hDrv, IOCTL_Sema_Get_WDT, pushBufTe, sizeof(pushBufTe), pushBuf, sizeof(pushBuf), &dwRet, NULL))
	{
		*Timeout = pushBuf[0];
		return EAPI_STATUS_SUCCESS;
	}
	
	return EAPI_STATUS_ERROR;
}

EERROR Ccommon_bmc::SaveLocalPwrWDT(unsigned int Timeout)
{
	EERROR eRet;
	DWORD dwRet = 0x00;
	uint32_t pushBuf[2] = { 0 }, pushBufTe[2] = { 0 };
	
	if ((eRet = ShowLocalWDT(&pushBuf[0])) != EAPI_STATUS_SUCCESS)
	{
		return eRet;
	}

	pushBuf[1] = Timeout;

	if (DeviceIoControl(m_hDrv, IOCTL_Sema_Set_WDT,  pushBuf, sizeof(pushBuf), pushBufTe, sizeof(pushBufTe), &dwRet, NULL))
	{
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}

EERROR Ccommon_bmc::ShowLocalPwrWDT(unsigned int *Timeout)
{
	DWORD dwRet = 0x02;
	uint32_t pushBuf[2] = { 0 }, pushBufTe[2] = { 0 };

	if (m_hDrv == INVALID_HANDLE_VALUE)
	{
		return EAPI_STATUS_NOT_INITIALIZED;
	}

	if (DeviceIoControl(m_hDrv, IOCTL_Sema_Get_WDT,  pushBufTe, sizeof(pushBufTe), pushBuf, sizeof(pushBuf), &dwRet, NULL))
	{
		*Timeout = pushBuf[1];
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_ERROR;
}


bool Ccommon_bmc::IsWinXPOrLater()
{
	OSVERSIONINFOEX stVerInfo;
	DWORDLONG dwlConditionMask = 0;
	int op = VER_GREATER_EQUAL;

	// Initialize the OSVERSIONINFOEX structure.

	ZeroMemory(&stVerInfo, sizeof(OSVERSIONINFOEX));
	stVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	stVerInfo.dwMajorVersion = 5;
	stVerInfo.dwMinorVersion = 2;
	stVerInfo.wServicePackMajor = 0;
	stVerInfo.wServicePackMinor = 0;

	// Initialize the condition mask.

	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMINOR, op);

	return VerifyVersionInfo(&stVerInfo, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR, dwlConditionMask) ? true : false;
}


EERROR Ccommon_bmc::GetBIOSVersion(char* szVersion, uint32_t nSize)
{
	EERROR eRet = EAPI_STATUS_ERROR;

	if (IsWinXPOrLater())
	{
		uint32_t nDataSize = GetSystemFirmwareTable('RSMB', 0x00, NULL, 0x00);
		char* pMem = (char*)malloc(nDataSize);
		if (GetSystemFirmwareTable('RSMB', 0x00, pMem, nDataSize) > 0x00)
		{
			uint32_t nVerIndex = pMem[SMB_BIOSVER_INDEX + MS_SMB_OFFSET];
			char* szSmb = pMem + pMem[SMB_LENGTH_INDEX + MS_SMB_OFFSET] + MS_SMB_OFFSET;

			while (nVerIndex > 1 && *szSmb)
			{
				szSmb += strlen(szSmb);
				szSmb++;
				nVerIndex--;
			}

			if (strlen(szSmb) <= 14)
			{
				strcpy_s(szVersion, nSize, szSmb);
				eRet = EAPI_STATUS_SUCCESS;
			}
		}
		free(pMem);
	}
	return eRet;
}

EERROR Ccommon_bmc::GetLibraryMajor(uint16_t* pushValue)
{
	*pushValue = SEMA_VERSION_MAJOR;

	return EAPI_STATUS_SUCCESS;
}

EERROR Ccommon_bmc::GetLibraryMinor(uint16_t* pushValue)
{
	*pushValue = SEMA_VERSION_MINOR;

	return EAPI_STATUS_SUCCESS;
}

EERROR Ccommon_bmc::GetLibraryAddOn(char* szAddOn, uint32_t nSize)
{
	uint32_t nSizeL = (uint32_t)(strlen(SEMA_VERSION_ADDON_STR) > nSize ? nSize : strlen(SEMA_VERSION_ADDON_STR));

	if (nSizeL > nSize)
	{
		return EAPI_STATUS_MORE_DATA;
	}

	memcpy_s(szAddOn, nSize, SEMA_VERSION_ADDON_STR, nSizeL);

	return EAPI_STATUS_SUCCESS;
}


EERROR Ccommon_bmc::GetHardwareMajor(uint16_t* pushValue)
{
	return GetVersionMajor(*pushValue);
}

EERROR Ccommon_bmc::GetHardwareMinor(uint16_t* pushValue)
{
	return GetVersionMinor(*pushValue);
}

EERROR Ccommon_bmc::GetHardwareChipSet(char* szChipSet, uint32_t nSize)
{
	return GetChipSet(szChipSet, nSize);
}