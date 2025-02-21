#include "stdafx.h"
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
#include <windows.devices.gpio.h>
#include <windows.devices.i2c.h>
#include <windows.foundation.h>
#include "I2CTransWin.h"
#include "Error.h"
#include "tchar.h"
#include "winioctl.h"
#include "timeapi.h"
#pragma comment(lib, "winmm.lib")

using namespace Platform;
using namespace Windows::Devices::I2c;
using namespace ABI::Windows::Foundation;

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;



class wexception
{
public:
	explicit wexception(const std::wstring& msg) : msg_(msg) { }
	virtual ~wexception() { /*empty*/ }

	virtual const wchar_t* wwhat() const
	{
		return msg_.c_str();
	}

private:
	std::wstring msg_;
};

int CI2CTransport::MakeDevice(int slaveAddress, _In_opt_ String^ friendlyName)
{
	using namespace Windows::Devices::Enumeration;
	CoInitialize(nullptr); // NULL if using older VC++

	String^ aqs;
	if (friendlyName)
	{
		aqs = I2cDevice::GetDeviceSelector(friendlyName);
	}
	else
	{
		aqs = I2cDevice::GetDeviceSelector();
	}
	auto dis = concurrency::create_task(DeviceInformation::FindAllAsync(aqs)).get();
	if (dis->Size < 1)
	{
		throw wexception(L"I2C controller not found"); //
		return 0;
	}

	String^ id = dis->GetAt(0)->Id;

	device_cls = concurrency::create_task(I2cDevice::FromIdAsync(
		id,
		ref new I2cConnectionSettings(slaveAddress))).get();
	if (!device_cls) {
		std::wostringstream msg;
		msg << L"Slave address 0x" << std::hex << slaveAddress << L" on bus " << id->Data() <<
			L" is in use. Please ensure that no other applications are using I2C.";
		throw wexception(msg.str());
	}
	return 0;
}

int CI2CTransport::I2Cread_write(I2cDevice^ device1, std::wstring command, std::vector<BYTE> writeBuf, unsigned int bytesToRead, unsigned char* read_val)
{
	I2cTransferResult result;
	if (command == L"write")
	{
		result = device_cls->WritePartial(
			ArrayReference<BYTE>(
				writeBuf.data(),
				static_cast<unsigned int>(writeBuf.size())));
	}
	else if (command == L"read")
	{
		auto readBuf = ref new Platform::Array<BYTE>(bytesToRead);
		result = device_cls->ReadPartial(readBuf);
	}
	else if (command == L"writeread")
	{
		auto readBuf = ref new Array<BYTE>(bytesToRead);
		result = device_cls->WriteReadPartial(
			ArrayReference<BYTE>(
				writeBuf.data(),
				static_cast<unsigned int>(writeBuf.size())),
			readBuf);
		for(int i=0;i<=31;i++)
		   read_val[i] = readBuf[i];
		
	}
	switch (result.Status)
	{
	case I2cTransferStatus::FullTransfer:
		return 0;
	case I2cTransferStatus::PartialTransfer:
		return -1;
	case I2cTransferStatus::SlaveAddressNotAcknowledged:
		return -2;
	case I2cTransferStatus::ClockStretchTimeout:
		return -3;
	case I2cTransferStatus::UnknownError:
		return -4;
	default:
		return -4;
	}
}

CI2CTransport::CI2CTransport()
{
	
}

EERROR CI2CTransport::Init(HANDLE drvhdl)
{
	int i = 0;
	std::vector<BYTE> Buf;
	String^ friendlyName[5] = { "I2C0", "I2C1", "I2C2", "I2C3", "I2C4" };
	for (i = 0; i < 5; i++)
	{
		try {
			auto ret = MakeDevice(0x28, friendlyName[i]);
		}
		catch (const wexception & ex) {
			//std::wcerr << L"Error : " << ex.wwhat() << L"\n";
			continue;
		}
		catch (Platform::Exception ^ ex) {
			//std::wcerr << L"Error: " << ex->Message->Data() << L"\n";
			continue;
		}
		if ((I2Cread_write(device_cls, L"read", Buf, 1, NULL)) != 0)
		{
			//printf("Error in I2Cread_write\n");
			continue;
		}
		
		break;		
	}
	if (i >= 5)
		return EAPI_STATUS_ERROR;

	return EAPI_STATUS_SUCCESS;

}

EERROR CI2CTransport::ByteTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen, uint8_t* pDataRet, uint32_t& nRetLen)
{
	return EAPI_STATUS_ERROR;
}

EERROR CI2CTransport::BlockTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen)
{
	BYTE pDataRet[MAX_PATH]; uint32_t nOutLen = MAX_PATH;
	ZeroMemory(pDataRet, MAX_PATH);
	return BlockTrans(bAddr, bType, bCmd, pBufIn, nInLen, pDataRet, nOutLen);
}

EERROR CI2CTransport::BlockTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen, uint8_t* pDataRet, uint32_t& nRetLen)
{
	int status=0;
	std::vector<BYTE> writeBuf;
	unsigned char data_read[32];
	if (bType == TT_RBL_I2C)//read block
	{
		writeBuf.push_back(bCmd);
		if ((status = I2Cread_write(device_cls, L"writeread", writeBuf, 33, data_read)) != 0)
		{
			return EAPI_STATUS_ERROR;
		}
		memcpy_s(pDataRet, SMB_MAX_LENGTH, data_read+1, data_read[0]);
		writeBuf.clear();
		nRetLen = data_read[0];
		return EAPI_STATUS_SUCCESS;
	}
	else if (bType == TT_WBL_I2C)
	{
		writeBuf.push_back(bCmd);
		writeBuf.push_back(nInLen);
		writeBuf.insert(writeBuf.end(), pBufIn, pBufIn + nInLen);
		if ((status = I2Cread_write(device_cls, L"write", writeBuf, 0, NULL)) != 0)
		{
			return EAPI_STATUS_ERROR;
		}
		return EAPI_STATUS_SUCCESS;
	}
	return EAPI_STATUS_ERROR;
}
EERROR CI2CTransport::BlockI2CTrans(uint8_t bAddr, uint8_t bType, uint8_t bCmd, uint8_t* pBufIn, uint32_t nInLen, uint8_t* pDataRet, uint32_t& nRetLen)
{

	return EAPI_STATUS_ERROR;
}

CI2CTransport::~CI2CTransport()
{

}