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
	long inlen;
	char *BoardList = NULL, *AIN_list = NULL, *ExeTbl = NULL;
	uint32_t board_type = 0;

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

	if ((inlen = GetPropInfoFromCfgFile("BoardName", NULL, &BoardList)) < 0)
	{
		char test[20] = "Invalidboard";
		inlen = 20;
		BoardList = test;
	}
	else
	{
		BoardList[inlen] = 0;
	}

	if (TypeRet != UNKNOWN && TypeRet == EC)
	{
		char *next, *string = strtok_s(BoardList, ";", &next);
		int count = 0, i, j;

		while (string != NULL && strcmp(string, ReadData.boardname) != 0)
		{
			count++;
			string = strtok_s(NULL, ";", &next);
		}

		if (string == NULL)
		{
			return TypeRet;
		}

		if (GetPropInfoFromCfgFile("AIN", NULL, &AIN_list) > 0)
		{
			for (j = 0, i = 0; AIN_list[i] != 0 && j < count; i++)
			{
				if (AIN_list[i] == ';')
				{
					j++;
				}
			}

			ainlist = &AIN_list[i];

			for (i = 0; ainlist[i] != 0; i++)
			{
				if (ainlist[i] == ';')
				{
					ainlist[i] = 0;
					break;
				}
			}
			if (i < 5)
			{
				ainlist = NULL;
			}
		}


		if (GetPropInfoFromCfgFile("ExeTbl", NULL, &ExeTbl) > 0)
		{
			for (j = 0, i = 0; ExeTbl[i] != 0 && j < count; i++)
			{
				if (ExeTbl[i] == ';')
				{
					j++;
				}
			}

			exetbl = &ExeTbl[i];

			for (i = 0; exetbl[i] != 0; i++)
			{
				if (exetbl[i] == ';')
				{
					exetbl[i] = 0;
					break;
				}
			}
			if (i < 5)
			{
				exetbl = NULL;
			}
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

struct prop_ {
	char prop_name[200];
	char prop[4000];
	struct prop_ *next;
};

struct node {
	char node_name[400];
	struct prop_ *prop_list;
};

void filter(char *in, char *out)
{
	int start = 0, k = 0, j;
	for (j = 0; in[j] != 0; j++)
	{
		if (in[j] != '"' && start == 0)
		{
			continue;
		}
		if (in[j] == '"' && start == 0)
		{
			start = 1;
			continue;
		}
		if (start == 1 && in[j] != '"')
		{
			out[k++] = in[j];
		}
		if (start == 1 && in[j] == '"')
		{
			out[k] = 0;
			break;
		}
	}
}


// Fetch a little endian DWORD from the registry
//(see platform SDK "Registry Value Types")
bool Get_DWORDRegistryValue(HKEY hKeyRoot, LPCTSTR pszSubKey, LPCTSTR pszValue, char* buffer)
{
	HKEY hKey;
	DWORD dwType = REG_MULTI_SZ;
	DWORD dwSize = 200;
	LONG  lRes;
	lRes = RegOpenKeyEx(hKeyRoot, pszSubKey, 0, KEY_READ, &hKey);
	if (lRes != ERROR_SUCCESS)
	{
		return false;
	}

	lRes = RegQueryValueEx(hKey, pszValue, 0, &dwType, (LPBYTE)buffer, &dwSize);
	RegCloseKey(hKey);
	if (lRes != ERROR_SUCCESS)
	{
		return false;
	}
	return true;
}

int Ccommon_bmc::GetPropInfoFromCfgFile(char *propname, char *prop, char **out)
{
	std::fstream file;
	std::streampos size;
	char buffer[200], cbuffer[200];
	int i, j, k, start, list_count = 0, list_open_count = 0, list_close_count = 0, open, close;
	char *boardlist_in_raw, *board_name, *Next = NULL;


	char *filepath[] = { "SuppBoardList", "C:\\Program Files\\Adlink\\SEMA\\Application\\SuppBoardList",\
								"C:\\Program Files (x86)\\Adlink\\SEMA\\Application\\SuppBoardList" };
	int count = sizeof(filepath) / sizeof(filepath[0]);

	for (i = 0; i < count; i++)
	{
		file.open(filepath[i], ios::in | ios::out | ios::_Nocreate);
		if (file)
			break;
	}

	if ((prop == NULL && out == NULL) || propname == NULL)
	{
		return -1;
	}

	if (i == count)
	{
		if (Get_DWORDRegistryValue(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Adlink\\SEMA"), L"SEMAInstallationPath", buffer) == true)
		{
			snprintf(cbuffer,200, "%wsSuppBoardList", (wchar_t*)buffer);
			file.open(cbuffer, ios::in | ios::out | ios::_Nocreate);
			if (!file)
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}
	}

	file.seekg(0, ios_base::end);
	size = file.tellg();
	file.seekg(0, ios_base::beg);

	boardlist_in_raw = (char*)calloc(size, sizeof(char));
	if (boardlist_in_raw == NULL)
	{
		return -1;
	}

	file.read(boardlist_in_raw, size);

	open = close = 0;
	for (i = 0; i < size; i++)
	{
		if (boardlist_in_raw[i] == '{')
		{
			open++;
		}
		if (boardlist_in_raw[i] == '}')
		{
			close++;
		}
	}


	if (open != close)
	{
		return -1;
	}

	open = close = 0;
	for (i = 0; i < size; i++)
	{
		if (boardlist_in_raw[i] == '[')
		{
			open++;
		}
		if (boardlist_in_raw[i] == ']')
		{
			close++;
		}
	}

	if (open != close)
	{
		free(boardlist_in_raw);
		return -1;
	}

	board_name = strtok_s(boardlist_in_raw, "[", &Next);
	if (board_name != NULL)
	{
		board_name = strtok_s(board_name, "]", &Next);
	}

	if (board_name == NULL)
	{
		free(boardlist_in_raw);
		return -1;
	}

	for (i = 0; board_name[i + 1] != 0; i++)
	{
		if (board_name[i] == '}')
			list_close_count++;
		if (board_name[i] == '{')
			list_open_count++;
	}
	
	if (list_open_count == list_close_count)
		list_count = list_open_count;
	else
	{
		return -1;
	}

	char **index_list = (char**)calloc(list_count, sizeof(char*));
	if (index_list == NULL)
	{
		free(boardlist_in_raw);
		return -1;
	}
	
	start = k = j = 0;
	for (i = 0; board_name[i + 1] != 0; i++)
	{
		if (isprint(board_name[i]) && start == 0)
		{
			index_list[k++] = &(board_name[i]);
			start = 1;
		}
		if (board_name[i] == '}')
		{
			board_name[i + 1] = start = 0;
			i += 2;
		}
	}


	for (i = 0; i < list_count; i++)
	{
		for (k = 0, j = 0; index_list[i][k] != 0; k++)
		{
			if (isprint(index_list[i][k]))
			{
				index_list[i][j++] = index_list[i][k];
			}
		}
		index_list[i][j] = 0;
	}

	struct node *node_list = (struct node*)calloc(list_count + 1, sizeof(struct node));

	if (node_list == NULL)
	{
		free(boardlist_in_raw);
		return -1;
	}

	for (i = 0; i < list_count; i++)
	{
		start = 0;
		for (k = 0, j = 0; index_list[i][k] != 0; k++)
		{
			if (start == 1)
			{
				if (index_list[i][k] == '{')
				{
					node_list[i].node_name[j] = 0;
					for (k, j = 0; index_list[i][k] != 0; k++)
					{
						index_list[i][j++] = index_list[i][k];
					}
					index_list[i][j] = 0;
					break;
				}
				else
				{
					if (isprint(index_list[i][k]) && index_list[i][k] != '"' &&  index_list[i][k] != '{' &&  index_list[i][k] != ','\
						&&  index_list[i][k] != ':' &&  index_list[i][k] != ';')
					{
						node_list[i].node_name[j++] = index_list[i][k];
					}
				}
			}
			if (isprint(index_list[i][k]) && start == 0)
			{
				if (index_list[i][k] != '"' &&  index_list[i][k] != '{' &&  index_list[i][k] != ','\
					&&  index_list[i][k] != ':' &&  index_list[i][k] != ';')
					node_list[i].node_name[j++] = index_list[i][k];
				if (index_list[i][k] == '{')
				{
					k -= 1;
				}
				start = 1;
			}
		}
	}


	for (i = 0; i < list_count; i++)
	{
		char *ptr = strtok_s(index_list[i], "}", &Next);
		ptr = strtok_s(index_list[i], ",", &Next);
		while (ptr != NULL)
		{
			for (j = 0, k = 0; ptr[j] != 0; j++)
			{
				if (ptr[j] != '{')
				{
					ptr[k++] = ptr[j];
				}
			}
			ptr[k] = 0;

			struct prop_ *prop_ptr = node_list[i].prop_list;
			while (prop_ptr != NULL && prop_ptr->next != NULL) prop_ptr = prop_ptr->next;
			if (prop_ptr == NULL)
			{
				node_list[i].prop_list = (struct prop_*)calloc(1, sizeof(struct prop_));
				prop_ptr = node_list[i].prop_list;
			}
			else
			{
				prop_ptr->next = (struct prop_*)calloc(1, sizeof(struct prop_));
				prop_ptr = prop_ptr->next;
			}

			char *ptr_t = _strdup(ptr), *Next_t;
			if (ptr_t == NULL)
			{
				free(boardlist_in_raw);
				return -1;
			}

			char *prop_n = strtok_s(ptr_t, ":", &Next_t);
			char *prop = strtok_s(NULL, ":", &Next_t);
			
			if (prop_n == NULL || prop == NULL)
			{
				free(boardlist_in_raw);
				return -1;
			}

			filter(prop_n, prop_ptr->prop_name);
			filter(prop, prop_ptr->prop);

			ptr = strtok_s(NULL, ",", &Next);
		}
	}

	struct node *ptr = node_list;
	int ret = -1;

	if (out != NULL)
	{
		*out = (char*)calloc(list_count, 4000);
		if (*out != NULL)
		{
			for (i = 0; i < list_count; i++)
			{
				int flag = 0;
				struct prop_ *ptr1 = ptr[i].prop_list;
				while (ptr1)
				{
					if (strcmp(ptr1->prop_name, propname) == 0)
					{
						strcat_s(*out, 4000, ptr1->prop);
						strcat_s(*out, 4000, ";");
						flag = 1;
					}
					ptr1 = ptr1->next;
				}
				if (flag == 0)
				{
					strcat_s(*out, 4000, ";");
				}
			}
			ret = (int)strlen(*out);
		}
	}


	if (prop != NULL)
	{
		for (i = 0; i < list_count; i++)
		{
			struct prop_ *ptr1 = ptr[i].prop_list;
			while (ptr1)
			{
				if (strcmp(ptr1->prop_name, propname) == 0)
				{
					if (strcmp(ptr1->prop, prop) == 0)
					{
						free(boardlist_in_raw);
						return 0;
					}
				}
				ptr1 = ptr1->next;
			}
		}
	}
	free(boardlist_in_raw);
	return ret;
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