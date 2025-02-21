#include "stdafx.h"
#include "SMBusFunct.h"

#define MAX_BLOCK 32

EERROR CSMBusFunct::I2CGetBusCap(uint32_t Id, uint32_t *pMaxBlkLen)
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

EERROR CSMBusFunct::I2CWaitForFree(void)
{
	return EAPI_STATUS_SUCCESS;
}

EERROR
CSMBusFunct::I2CIsDeviceExist(uint32_t Id, uint32_t Addr)
{
	unsigned char cmd;
	unsigned char tBuffer[SMB_TRANS_BUFFER_LEN] = { 0 };
	uint32_t I2CStatus;

	switch (Id)
	{
	case SEMA_EXT_IIC_BUS_1:
		cmd = SEMA_CMD_IIC1_TRANS;
		break;
	case SEMA_EXT_IIC_BUS_2:
		cmd = SEMA_CMD_IIC2_TRANS;
		break;
	default:
		return EAPI_STATUS_UNSUPPORTED;
	}

	tBuffer[0] = (uint8_t)Addr;
	tBuffer[1] = 0;
	tBuffer[2] = 0;
	I2CStatus= m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, cmd, tBuffer, 3);

	if (I2CStatus != EAPI_STATUS_SUCCESS)
	{
		return EAPI_STATUS_ERROR;
	}

	if (I2CGetStatus(&I2CStatus) == EAPI_STATUS_SUCCESS)
	{
		if (I2CStatus !=1)
		return EAPI_STATUS_ERROR;
	}
	else
		return EAPI_STATUS_SUCCESS;
	
}


EERROR CSMBusFunct::I2CWriteReadRaw(uint32_t Id, uint32_t Addr, void *pWBuffer, uint32_t WriteBCnt, \
	void *pRBuffer, uint32_t RBufLen, uint32_t ReadBCnt)
{
	unsigned int i;
	EERROR result;
	unsigned char tBuffer[SMB_TRANS_BUFFER_LEN] = { 0 };

	if (NULL != pWBuffer)
	{
		tBuffer[0] = (uint8_t)(Addr & 0xFE);
		tBuffer[2] = (byte)0;
		tBuffer[1] = WriteBCnt+1;
		for (i = 0; i < WriteBCnt; i++) {
			tBuffer[i + 3] = ((byte*)pWBuffer)[i];
		}

		result = SemaApi_TransExt(Id, SEMA_EXT_IIC_BLOCK, WriteBCnt + 3, tBuffer);
	}

	if (NULL != pRBuffer)
	{
		tBuffer[0] = Addr | 0x01;
		tBuffer[1] = ReadBCnt;
		tBuffer[2] = 0;

		result = SemaApi_TransExt(Id, SEMA_EXT_IIC_BLOCK | SEMA_EXT_IIC_READ, 3, tBuffer);
	}

	if (result != EAPI_STATUS_SUCCESS)
	{
		return result;
	}

	if (NULL != pRBuffer)
	{
		for (i = 0; i < ReadBCnt; i++)
		{
			((byte*)pRBuffer)[i] = tBuffer[i];
		}
	}

	return EAPI_STATUS_SUCCESS;
}


EERROR CSMBusFunct::I2CWriteTransfer(uint32_t Id, uint32_t Addr, uint32_t Cmd, void* pBuffer, \
	uint32_t ByteCnt)
{
	
	unsigned int i;
	EERROR result;
	unsigned char tBuffer[SMB_TRANS_BUFFER_LEN] = { 0 };

	tBuffer[0] = (uint8_t)Addr & 0xFE;

	if (EAPI_I2C_IS_STD_CMD(Cmd)) {
		tBuffer[2] = (byte)Cmd;
		tBuffer[1] = ByteCnt + 1;
		for (i = 0; i < ByteCnt; i++) {
			tBuffer[i + 3] = ((byte*)pBuffer)[i];
		}
		result = SemaApi_TransExt(Id, SEMA_EXT_IIC_BLOCK, ByteCnt + 3, tBuffer);
	}
	else if (EAPI_I2C_IS_EXT_CMD(Cmd)) {
		tBuffer[4] = (byte)(Cmd & 0xFF);
		tBuffer[3] = (byte)((Cmd >> 8) & 0xFF);
		tBuffer[2] = 0;
		tBuffer[1] = ByteCnt + 2;
		for (i = 0; i < ByteCnt; i++) {
			tBuffer[i + 5] = ((byte*)pBuffer)[i];
		}
		result = SemaApi_TransExt(Id, SEMA_EXT_IIC_EXT_COMMAND, ByteCnt + 5, tBuffer);
	}
	else if (EAPI_I2C_IS_NO_CMD(Cmd)) {
		tBuffer[2] = 0;
		tBuffer[1] = ByteCnt;
		for (i = 0; i < ByteCnt; i++) {
			tBuffer[i + 3] = ((byte*)pBuffer)[i];
		}
		result = SemaApi_TransExt(Id, SEMA_EXT_IIC_EXT_COMMAND, ByteCnt + 3, tBuffer);
	}
	else
	{
		return EAPI_STATUS_UNSUPPORTED;
	}
	return result;
}

EERROR CSMBusFunct::I2CReadTransfer(uint32_t Id, uint32_t Addr, uint32_t Cmd, void* pBuffer, \
	uint32_t BufLen, uint32_t ByteCnt)
{
	unsigned int i;
	EERROR result;
	unsigned char tBuffer[SMB_TRANS_BUFFER_LEN] = { 0 };

	tBuffer[0] = (byte)(Addr | 0x01);

	if (EAPI_I2C_IS_STD_CMD(Cmd)) {
		tBuffer[1] = (byte)ByteCnt;
		tBuffer[2] = (byte)Cmd;
		
		result = SemaApi_TransExt(Id, SEMA_EXT_IIC_BLOCK | SEMA_EXT_IIC_READ, 3, tBuffer);
	}
	else if (EAPI_I2C_IS_EXT_CMD(Cmd)) {
		tBuffer[4] = (byte)(Cmd & 0xFF);
		tBuffer[3] = (byte)((Cmd >> 8) & 0xFF);
		tBuffer[2] = ByteCnt;
		tBuffer[1] = 2;

		result = SemaApi_TransExt(Id, SEMA_EXT_IIC_EXT_COMMAND | SEMA_EXT_IIC_READ, 5, tBuffer);
	}
	else if (EAPI_I2C_IS_NO_CMD(Cmd)) {
		tBuffer[2] = ByteCnt;
		tBuffer[1] = 0;

		result = SemaApi_TransExt(Id, SEMA_EXT_IIC_EXT_COMMAND | SEMA_EXT_IIC_READ, 3, tBuffer);
	}
	else
		return EAPI_STATUS_UNSUPPORTED;

	if (result != EAPI_STATUS_SUCCESS)
	{
		return result;
	}

	for (i = 0; i < ByteCnt; i++) 
	{ 
		((byte*)pBuffer)[i] = tBuffer[i]; 
	}

	return EAPI_STATUS_SUCCESS;
}

EERROR
CSMBusFunct::SemaApi_TransExt(
	uint8_t BusNumber,
	uint8_t Type,
	uint8_t Length,
	uint8_t *Buffer
) {
	EERROR result;
	byte command = 0;
	bool read = false;
	int i = 0;
	byte expected = Buffer[2];
	uint32_t retLength = MAX_PATH;
	BYTE pDataRet[MAX_PATH] = { 0 };

	if (Type & SEMA_EXT_IIC_READ)
	{
		read = true;
	}

	if (Type & SEMA_EXT_IIC_BLOCK)
		command = SEMA_CMD_IIC4_BLOCK;
	else if (Type & SEMA_EXT_IIC_BYTE && read)
		command = SEMA_CMD_IIC4_BYTE_READ;
	else if (Type & SEMA_EXT_IIC_BYTE)
		command = SEMA_CMD_IIC4_BYTE_WRITE;
	else if (Type & SEMA_EXT_IIC_COMMAND)
		command = SEMA_CMD_IIC4_COMANND_BYTE_READWRITE;
	else if (Type & SEMA_EXT_IIC_EXT_COMMAND)
		command = SEMA_CMD_IIC1_TRANS - 1;
	else if (Type & SEMA_EXT_IIC_EXT_COMMAND_WR)
		command = SEMA_CMD_IIC1_TRANS - 1;

	if (command == 0)
	{
		return EAPI_STATUS_ERROR;
	}
	switch (BusNumber)
	{
	case SEMA_EXT_IIC_BUS_1:
		command += 1;
		break;
	case SEMA_EXT_IIC_BUS_2:
		command += 2;
		break;
	case SEMA_EXT_IIC_BUS_3:
		command += 3;
		break;
	case SEMA_EXT_IIC_BUS_4:
		break;
	default:
		return EAPI_STATUS_UNSUPPORTED;
	}

	result = m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, command, Buffer, Length, pDataRet, retLength);

	if (result == EAPI_STATUS_SUCCESS)
	{
		if (read == true || ((!!(Type & SEMA_EXT_IIC_EXT_COMMAND_WR) != 0) && (expected != 0)))
		{
			Sleep((Buffer[1] + Buffer[2] + 2) / 8 + 1); // wait until transfer on ext. bus has finished
			read = 1;
		}
	}

	if (result == EAPI_STATUS_SUCCESS && read)
	{
		result = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, SEMA_CMD_IIC_GETDATA, NULL, 0x00, Buffer, retLength);
	}

	return result;
}

EERROR CSMBusFunct::I2CGetStatus(uint32_t *p_output)
{
	EERROR result;
	uint32_t nRet = MAX_PATH;
	unsigned char buf[MAX_PATH] = { 0 };
	
	result = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x1, TT_RBL, SEMA_CMD_IIC_STATUS, NULL, 0, buf, nRet);
	
	if (EAPI_STATUS_SUCCESS != result)
	{
			return EAPI_STATUS_ERROR;
	}
	else
	{
		*p_output = (uint32_t)buf[0];
	}
	
	return result;
}

EERROR CSMBusFunct::I2CTxnStatus(uint32_t* Status)
{
	return EAPI_STATUS_UNSUPPORTED;
}

EERROR CSMBusFunct::I2CRawTransaction(uint32_t Id, uint32_t Addr, void* pWBuffer, uint32_t WriteBCnt, \
	void* pRBuffer, uint32_t ReadBCnt)
{
	return EAPI_STATUS_UNSUPPORTED;
}
