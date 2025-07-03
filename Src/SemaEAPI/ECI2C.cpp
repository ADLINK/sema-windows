#include "iostream"
#include "stdafx.h"
#include "ECFunct.h"
#include "EApi.h"

using namespace std::chrono;
using namespace std::this_thread;

#pragma pack(1)

#define MAX_BLOCK 32

#define EAPI_I2C_STD_CMD EAPI_UINT32_C(0<<30)
#define EAPI_I2C_EXT_CMD EAPI_UINT32_C(2<<30)
#define EAPI_I2C_NO_CMD EAPI_UINT32_C(1<<30)

#define EAPI_I2C_CMD_TYPE_MASK EAPI_UINT32_C(3<<30)

#define EAPI_I2C_ENC_STD_CMD(x) EAPI_UINT32_C(((x) & 0xFF) | EAPI_I2C_STD_CMD)
#define EAPI_I2C_ENC_EXT_CMD(x) EAPI_UINT32_C(((x) & 0xFFFF) | EAPI_I2C_EXT_CMD)

#define EAPI_I2C_IS_EXT_CMD(x) (EAPI_UINT32_C((x) & (EAPI_I2C_CMD_TYPE_MASK)) == EAPI_I2C_EXT_CMD)
#define EAPI_I2C_IS_STD_CMD(x) (EAPI_UINT32_C((x) & (EAPI_I2C_CMD_TYPE_MASK)) == EAPI_I2C_STD_CMD)
#define EAPI_I2C_IS_NO_CMD(x)  (EAPI_UINT32_C((x) & (EAPI_I2C_CMD_TYPE_MASK)) == EAPI_I2C_NO_CMD)

struct I2CTrxn {
	uint8_t IFType;
	uint8_t ReWrFu;
	uint8_t Length;
	uint8_t SelOrBusID;
	uint8_t MSB_Addr;
	uint8_t LSB_Addr;
	uint8_t Reserved1;
	uint8_t Status;
	uint8_t Reserved[9];
	uint8_t Buffer[0x80];
};

#define CommandFrameLength 15

EERROR CECFunct::I2CGetBusCap(uint32_t Id, uint32_t *pMaxBlkLen)
{

	uint32_t m_nSemaCaps = GetCaps();
	
	*pMaxBlkLen = MAX_BLOCK;

	switch (Id)
	{
	case SEMA_EXT_IIC_BUS_1:

		if (m_nSemaCaps & SEMA_CAP_I2C1)
		{
			return EAPI_STATUS_SUCCESS;
		}
		break;

	case SEMA_EXT_IIC_BUS_2:
		
		if (m_nSemaCaps & SEMA_CAP_I2C2)
		{
			return EAPI_STATUS_SUCCESS;
		}
		break;

	case SEMA_EXT_IIC_BUS_3:

		if (m_nSemaCaps &  SEMA_CAP_I2C3)
		{
			return EAPI_STATUS_SUCCESS;
		}
		break;

	case SEMA_EXT_IIC_BUS_4:

		if (m_nSemaCaps &  SEMA_CAP_I2C4)
		{
			return EAPI_STATUS_SUCCESS;
		}
		break;
	}
	*pMaxBlkLen = 0;
	return EAPI_STATUS_ERROR;
}

EERROR CECFunct::I2CWaitForFree(void)
{
	int i; 
	uint8_t Status;
    
	for (i = 0; i < 100000; i++)
	{
		if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, &Status, 1) == EAPI_STATUS_SUCCESS)
		{	
			if((!!(Status & 0x4)!=1) && ((Status & 0x1)==0))  
			{		 
				return EAPI_STATUS_SUCCESS;
			}		
		}
		//sleep_for(microseconds(50));
	}
	
	return EAPI_STATUS_ERROR;
}


EERROR
CECFunct::I2CIsDeviceExist(uint32_t Id, uint32_t Addr)
{
	
	struct I2CTrxn Trxn;
	uint8_t Status;
	uint32_t I2CStatus;

	memset(&Trxn, 0, sizeof(Trxn));

	

	switch (Id)
	{
	case SEMA_EXT_IIC_BUS_1:
		Trxn.SelOrBusID = 0x1;
		break;
	case SEMA_EXT_IIC_BUS_2:
		Trxn.SelOrBusID = 0x2;
		break;
	case SEMA_EXT_IIC_BUS_3:
		Trxn.SelOrBusID = 0x3;
		break;
	case SEMA_EXT_IIC_BUS_4:
		Trxn.SelOrBusID = 0x4;
		break;
	default:
		return EAPI_STATUS_UNSUPPORTED;
	}

	Trxn.IFType = 0x4;
	Trxn.ReWrFu = 0x2;
	Trxn.Length = 0;
	Trxn.MSB_Addr = (Addr >> 8) & 0x3;
	Trxn.LSB_Addr = (uint8_t)Addr;

	if (I2CWaitForFree() != EAPI_STATUS_SUCCESS)
	{
		
		return EAPI_STATUS_ERROR;
	}

	if (m_clsECTrans.ECWrite(EC_WO_ADDR_IIC_CMD_START, EC_REGION_2, (uint8_t*)(&Trxn), \
		CommandFrameLength) != EAPI_STATUS_SUCCESS)
	{
		
		return EAPI_STATUS_ERROR;
	}

	Status = 1 << 2;

	if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, &Status, 1) != EAPI_STATUS_SUCCESS)
	{
		
		return EAPI_STATUS_ERROR;
	}

	if (I2CWaitForFree() != EAPI_STATUS_SUCCESS)
	{
		
		return EAPI_STATUS_ERROR;
	}

	if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, &Status, 1) != EAPI_STATUS_SUCCESS)
	{
		
		return EAPI_STATUS_ERROR;
	}
	
	if (!!(Status & 0x80) == 1)  //Need change as per the new spec
	{
		
		
		return EAPI_STATUS_ERROR;
	}

	if (I2CGetStatus(&I2CStatus) != EAPI_STATUS_SUCCESS)
	{
		
		return EAPI_STATUS_ERROR;
	}

	if (I2CStatus == 0)
	{
		return EAPI_STATUS_SUCCESS;
	}

	return EAPI_STATUS_NOT_FOUND;
}

EERROR CECFunct::I2CWriteReadRaw(uint32_t Id, uint32_t Addr, void *pWBuffer, uint32_t WriteBCnt, \
	void *pRBuffer, uint32_t RBufLen, uint32_t ReadBCnt)
{
	uint8_t i, Status;
	uint32_t I2CStatus;
	struct I2CTrxn Trxn;

	memset(&Trxn, 0, sizeof(Trxn));

	switch (Id)
	{
	case SEMA_EXT_IIC_BUS_1:
		Trxn.SelOrBusID = 0x1;
		break;
	case SEMA_EXT_IIC_BUS_2:
		Trxn.SelOrBusID = 0x2;
		break;
	case SEMA_EXT_IIC_BUS_3:
		Trxn.SelOrBusID = 0x3;
		break;
	case SEMA_EXT_IIC_BUS_4:
		Trxn.SelOrBusID = 0x4;
		break;
	default:
		return EAPI_STATUS_UNSUPPORTED;
	}


	if ((NULL != pWBuffer) && (NULL != pRBuffer))
	{
		return EAPI_STATUS_INVALID_PARAMETER;
	}
	
	if (I2CWaitForFree() != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (NULL != pWBuffer)
	{
		Trxn.IFType   = 0x4;
		Trxn.ReWrFu   = 0x2;
		Trxn.Length	  = WriteBCnt;
		Trxn.MSB_Addr = (Addr >> 8) & 0x7;
		Trxn.LSB_Addr = (uint8_t)Addr;

		for (i = 0; i < WriteBCnt; i++) {
			Trxn.Buffer[i] = ((byte*)pWBuffer)[i];
		}

		if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BUFFER, EC_REGION_2, (uint8_t*)(&Trxn.Buffer), \
			WriteBCnt) != EAPI_STATUS_SUCCESS)
		{
			return EAPI_STATUS_WRITE_ERROR;
		}
	}

	if (NULL != pRBuffer)
	{
		Trxn.IFType = 0x4;
		Trxn.ReWrFu = 0x1;
		Trxn.Length = ReadBCnt;
		Trxn.MSB_Addr = (Addr >> 8) & 0x7;
		Trxn.LSB_Addr = (uint8_t)Addr;
	}

	if (m_clsECTrans.ECWrite(EC_WO_ADDR_IIC_CMD_START, EC_REGION_2, (uint8_t*)(&Trxn),\
		CommandFrameLength) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	Status = 0x05;
	if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, &Status, 1) != EAPI_STATUS_SUCCESS)
	{	
		return EAPI_STATUS_ERROR;
	}
	
	sleep_for(microseconds(Trxn.Length));

	if (I2CTxnStatus(&I2CStatus) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (NULL != pRBuffer)
	{
		if (m_clsECTrans.ECRead(EC_RW_ADDR_IIC_BUFFER, EC_REGION_2, (uint8_t*)pRBuffer, ReadBCnt) \
			!= EAPI_STATUS_SUCCESS)
		{
			return EAPI_STATUS_READ_ERROR;
		}
	}

	return EAPI_STATUS_SUCCESS;
}

EERROR CECFunct::I2CReadTransfer(uint32_t Id, uint32_t Addr, uint32_t Cmd, void* pBuffer, \
	uint32_t BufLen, uint32_t ByteCnt)
{
	EERROR Status;
	
	if (EAPI_I2C_IS_STD_CMD(Cmd))
	{
		uint8_t WBuffer[2] = { 0 };
		WBuffer[0] = ((uint8_t)Cmd);
		if ((Status = I2CWriteReadRaw(Id, Addr, WBuffer, 1, NULL, 0, 0)) != EAPI_STATUS_SUCCESS)
		{
			return Status;
		}
	}
	else if (EAPI_I2C_IS_EXT_CMD(Cmd))
	{
		uint8_t WBuffer[2] = { 0 };
		WBuffer[1] = ((uint8_t)Cmd);
		WBuffer[0] = ((uint8_t)(Cmd >> 8));
		if ((Status = I2CWriteReadRaw(Id, Addr, WBuffer, 2, NULL, 0, 0)) != EAPI_STATUS_SUCCESS)
		{
			return Status;
		}
	}

	return I2CWriteReadRaw(Id, Addr, NULL, 0, pBuffer, BufLen, ByteCnt);
}

EERROR CECFunct::I2CWriteTransfer(uint32_t Id, uint32_t Addr, uint32_t Cmd, void* pBuffer, \
	uint32_t ByteCnt)
{

	if (EAPI_I2C_IS_STD_CMD(Cmd))
	{
		uint8_t *WBuffer = (uint8_t*)calloc(ByteCnt + 1, sizeof(uint8_t));

		if (WBuffer == NULL)
		{
			return EAPI_STATUS_ALLOC_ERROR;
		}

		WBuffer[0] = ((uint8_t)Cmd);
		memcpy(&(WBuffer[1]), pBuffer, ByteCnt);

		return I2CWriteReadRaw(Id, Addr, WBuffer, ByteCnt + 1, NULL, 0, 0);
	}
	else if (EAPI_I2C_IS_NO_CMD(Cmd))
	{
		return I2CWriteReadRaw(Id, Addr, pBuffer, ByteCnt, NULL, 0, 0);
	}
	else if (EAPI_I2C_IS_EXT_CMD(Cmd))
	{
		uint8_t *WBuffer = (uint8_t*)calloc(ByteCnt + 2, sizeof(uint8_t));

		if (WBuffer == NULL)
		{
			return EAPI_STATUS_ALLOC_ERROR;
		}

		WBuffer[1] = ((uint8_t)Cmd);
		WBuffer[0] = ((uint8_t)(Cmd >> 8));
		memcpy(&(WBuffer[2]), pBuffer, ByteCnt);

		return I2CWriteReadRaw(Id, Addr, WBuffer, ByteCnt + 2, NULL, 0, 0);
	}
	
	return EAPI_STATUS_ERROR;
}

EERROR CECFunct::I2CRawTransaction(uint32_t Id, uint32_t Addr, void* pWBuffer, uint32_t WriteBCnt, \
	void* pRBuffer, uint32_t ReadBCnt)
{
	unsigned char start = 0x04;
	unsigned char buffer[32] = { 0 };
	volatile int i;
	uint32_t I2CStatus;

	switch (Id)
	{
	case SEMA_EXT_IIC_BUS_1:
		Id = 0x1;
		break;
	case SEMA_EXT_IIC_BUS_2:
		Id = 0x2;
		break;
	case SEMA_EXT_IIC_BUS_3:
		Id = 0x3;
		break;
	case SEMA_EXT_IIC_BUS_4:
		Id = 0x4;
		break;
	default:
		return EAPI_STATUS_UNSUPPORTED;
	}

	if (I2CWaitForFree() != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	buffer[0] = 0x04;
	buffer[1] = 0x03;
	buffer[2] = Id;
	buffer[3] = Addr;
	buffer[4] = WriteBCnt;
	buffer[5] = ReadBCnt;

	if (m_clsECTrans.ECWrite(0x11, EC_REGION_2, &(buffer[0]), 1) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}
	if (m_clsECTrans.ECWrite(0x12, EC_REGION_2, &(buffer[1]), 1) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (m_clsECTrans.ECWrite(0x14, EC_REGION_2, &(buffer[2]), 1) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (m_clsECTrans.ECWrite(0x16, EC_REGION_2, &(buffer[3]), 1) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (m_clsECTrans.ECWrite(0x19, EC_REGION_2, &(buffer[4]), 1) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (m_clsECTrans.ECWrite(0x1A, EC_REGION_2, &(buffer[5]), 1) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	for (i = 0; i < buffer[4]; i++)
	{
		buffer[i + 6] = ((byte*)pWBuffer)[i];
	}

	if (WriteBCnt != 0)
	{
		if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BUFFER, EC_REGION_2, &(buffer[6]), buffer[4]) != EAPI_STATUS_SUCCESS)
		{
			return EAPI_STATUS_ERROR;
		}
	}

	if (m_clsECTrans.ECWrite(EC_RW_ADDR_IIC_BMC_STATUS, EC_REGION_2, &start, 1) != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	sleep_for(microseconds(buffer[4] + buffer[5]));

	if (buffer[5] == 0)
	{
		if (I2CTxnStatus(&I2CStatus) != EAPI_STATUS_SUCCESS)
		{
			return EAPI_STATUS_ERROR;
		}
	}
	else
	{
		if (I2CWaitForFree() != EAPI_STATUS_SUCCESS)
		{
			return EAPI_STATUS_ERROR;
		}
	}

	if (ReadBCnt != 0x00)
	{
		if (m_clsECTrans.ECRead(0x40, EC_REGION_2, (uint8_t*)pRBuffer, ReadBCnt) \
			!= EAPI_STATUS_SUCCESS)
		{
			return EAPI_STATUS_READ_ERROR;
		}
	}

	return EAPI_STATUS_SUCCESS;
}

EERROR CECFunct::I2CGetStatus(uint32_t *p_output)
{
	int i;
	EERROR ECStatus;
	uint8_t Status;

	if (I2CWaitForFree() != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	for (i = 0; i < 100; i++)
	{
		if ((ECStatus = m_clsECTrans.ECRead(EC_RO_ADDR_IIC_TXN_STATUS, EC_REGION_2, &Status, 1)) == EAPI_STATUS_SUCCESS)
		{
				*p_output = Status;
				break;
		}
	}
	
	return ECStatus;
}

EERROR CECFunct::I2CTxnStatus(uint32_t * p_output)
{
	int i;
	EERROR ECStatus;
	uint8_t Status;

	for (i = 0; i < 100; i++)
	{
		if ((ECStatus = m_clsECTrans.ECRead(EC_RO_ADDR_IIC_TXN_STATUS, EC_REGION_2, &Status, 1)) == EAPI_STATUS_SUCCESS)
		{
			if ((Status & 0x80) != 0)
			{
				*p_output = Status;
				return ECStatus;
			}
		}
		sleep_for(microseconds(50));
	}

	return ECStatus;
}
