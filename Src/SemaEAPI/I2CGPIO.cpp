#include "stdafx.h"
#include "I2CFunct.h"


#define PCA9535_ADDRESS				0x40		///< GPIO PCA9535 chip Address
#define PCA9535_GPIO_PORT0_INPUT_REG		0x00		///< GPIO Configuration registers
#define PCA9535_GPIO_PORT0_OUTPUT_REG		0x02		///< GPIO Output registers
#define PCA9535_GPIO_PORT0_CON_REG			0x06		///< GPIO Direction registers
#define PCA9535_GPIO_PORT1_INPUT_REG		0x01		///< GPIO Configuration registers
#define PCA9535_GPIO_PORT1_OUTPUT_REG		0x03		///< GPIO Input registers
#define PCA9535_GPIO_PORT1_CON_REG			0x07		///< GPIO Direction registers

#define PCA9535_INPUT_CAP		0x000000FF
#define PCA9535_OUTPUT_CAP		0x000000FF
#define PCA9535_BITMASK			0x00000FFF

/* Directions */
#define PCA9535_GPIO_INPUT   1
#define PCA9535_GPIO_OUTPUT  0

EERROR CI2CFunct::SetId(uint8_t GpioId)
{
	m_n_GPIO_Cur_Id = EAPI_GPIO_BANK_ID(GpioId);

	return EAPI_STATUS_SUCCESS;
}

EERROR CI2CFunct::GetGPIO(uint32_t* pnGPIO)
{
#if 0
	EERROR eRet;
	uint8_t pDataIn[] = { PCA9535_GPIO_PORT0_INPUT_REG, 0x00, 0x00, 0x00,0x00 };
	uint8_t bDataRet[4] = { 0x00 }; 
	uint32_t nRet = 0x04;

	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}

	Sleep(50);

	if ((eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_RBB, pDataIn[0], pDataIn, 0x02, (uint8_t*)&bDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		*pnGPIO = bDataRet[0];
		nRet = 0x04;
		pDataIn[0x00] = PCA9535_GPIO_PORT1_INPUT_REG;
		
		Sleep(50);
		if ((eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_RBB, pDataIn[0], pDataIn, 0x02, (uint8_t*)&bDataRet, nRet)) == EAPI_STATUS_SUCCESS)
		{
			*pnGPIO |= (bDataRet[0] << 0x08);
		}
	}

	return eRet;
#endif
	return EAPI_STATUS_ERROR;
}

EERROR CI2CFunct::GetGPIOOUT(uint32_t * pnGPIO, uint32_t nBitMask)
{
	return EAPI_STATUS_ERROR;
}

EERROR CI2CFunct::GetGPIOIN(uint32_t * pnGPIO, uint32_t nBitMask)
{
	return EAPI_STATUS_ERROR;
}
EERROR CI2CFunct::GetGPIODir(uint32_t* pnGPIO, uint32_t nBitMask)
{
#if 0
	EERROR eRet;
	uint8_t pDataIn[] = { PCA9535_GPIO_PORT0_CON_REG, 0x00, 0x00, 0x00,0x00 };
	uint8_t bDataRet[4] = { 0x00 };
	uint32_t nRet = 0x04;

	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}

	Sleep(50);
	if ((eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_RBB, pDataIn[0], pDataIn, 0x02, \
		(uint8_t*)&bDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		*pnGPIO = bDataRet[0];
		nRet = 0x04;
		pDataIn[0x00] = PCA9535_GPIO_PORT1_CON_REG;
		if ((eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_RBB, pDataIn[0], pDataIn, 0x02, \
			(uint8_t*)&bDataRet, nRet)) == EAPI_STATUS_SUCCESS)
		{
			*pnGPIO |= (bDataRet[0] << 0x08);
		}
		Sleep(50);
	}

	return eRet;
#endif
	return EAPI_STATUS_ERROR;
}

EERROR CI2CFunct::SetGPIO(uint32_t plevel, uint32_t nBitMask)
{
#if 0
	EERROR eRet;
	
	uint8_t pDataIn[] = { PCA9535_GPIO_PORT0_OUTPUT_REG, (uint8_t)plevel }, bDataRet[4] = { 0x00 };
	uint32_t nRet = 0x04;

	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}

	Sleep(50);
	if ((eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_WBB, pDataIn[0x00], pDataIn,\
		0x02, (uint8_t*)&bDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		Sleep(50);
		pDataIn[0x00] = PCA9535_GPIO_PORT1_OUTPUT_REG;
		pDataIn[0x01] = (uint8_t)(plevel >> 0x08);
		eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_WBB, pDataIn[0x00],\
			pDataIn, 0x02, (uint8_t*)&bDataRet, nRet);
	}

	return eRet;
#endif
	return EAPI_STATUS_ERROR;
}

EERROR CI2CFunct::SetGPIODir(uint32_t pDirection, uint32_t nBitMask)
{
#if 0
	EERROR eRet;
	
	uint8_t pDataIn[] = { PCA9535_GPIO_PORT0_CON_REG, (uint8_t)pDirection}, bDataRet[4] = { 0x00 };
	uint32_t nRet = 0x04;

	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}

	Sleep(50);
	if ((eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_WBB, pDataIn[0], pDataIn, 0x02, \
		(uint8_t*)&bDataRet, nRet)) == EAPI_STATUS_SUCCESS)
	{
		pDataIn[0x00] = PCA9535_GPIO_PORT1_CON_REG;
		pDataIn[0x01] = (uint8_t)(pDirection >> 0x08);
		Sleep(50);
		eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_WBB, pDataIn[0], pDataIn, 0x02, \
			(uint8_t*)&bDataRet, nRet);
	}

	return eRet;
#endif
	return EAPI_STATUS_ERROR;
}

EERROR CI2CFunct::GetCapsGpioDir(uint32_t* pnCapsIn, uint32_t* pnCapsOut, uint32_t GpioId)
{
	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}

	*pnCapsIn = PCA9535_INPUT_CAP;
	*pnCapsOut = PCA9535_OUTPUT_CAP;

	return EAPI_STATUS_SUCCESS;
}


EERROR CI2CFunct::ValidateBitMask(uint32_t nBitMask)
{
	if (nBitMask >= PCA9535_BITMASK)
	{
		return EAPI_STATUS_ERROR;
	}
	return EAPI_STATUS_SUCCESS;
}