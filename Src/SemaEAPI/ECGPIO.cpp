#include "stdafx.h"
#include "ECFunct.h"
#include "EApi.h"
#include<iostream>
using namespace std;

#define EC_INPUT_CAP		0x000000FF
#define EC_OUTPUT_CAP		0x000000FF
#define EC_BITMASK			0x00000FFF
#define EC_BITMASK_EXT		0x000000FF
#define EC_CAP				0x15

/* Directions */
#define PCA9535_GPIO_INPUT   1
#define PCA9535_GPIO_OUTPUT  0


EERROR CECFunct::SetId(uint8_t GpioId)
{
	m_n_GPIO_Cur_Id = EAPI_GPIO_BANK_ID(GpioId);

	return EAPI_STATUS_SUCCESS;
}

EERROR CECFunct::GetGPIO(uint32_t* pnGPIO)
{
	/*if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}

	*pnGPIO = 0;

	return m_clsECTrans.ECRead(EC_RW_ADDR_GPIO_IN, EC_REGION_1, (uint8_t*)pnGPIO, 1);
	*/
	return EAPI_STATUS_ERROR;
}

EERROR CECFunct::GetGPIOIN(uint32_t* pnGPIO, uint32_t nBitMask)
{
	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}

	*pnGPIO = 0;

	if (nBitMask > EC_BITMASK_EXT)
	{
		return m_clsECTrans.ECRead(EC_RW_ADDR_GPIO_IN_EXT, EC_REGION_1, (uint8_t*)pnGPIO, 1);
	}
	else
	{
		return m_clsECTrans.ECRead(EC_RW_ADDR_GPIO_IN, EC_REGION_1, (uint8_t*)pnGPIO, 1);
	}
}
EERROR CECFunct::GetGPIOOUT(uint32_t* pnGPIO, uint32_t nBitMask)
{
	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}

	*pnGPIO = 0;

	if (nBitMask > EC_BITMASK_EXT)
	{
		return m_clsECTrans.ECRead(EC_RW_ADDR_GPIO_OUT_EXT, EC_REGION_1, (uint8_t*)pnGPIO, 1);
	}
	else
	{
		return m_clsECTrans.ECRead(EC_RW_ADDR_GPIO_OUT, EC_REGION_1, (uint8_t*)pnGPIO, 1);
	}
}

EERROR CECFunct::GetGPIODir(uint32_t* pnGPIO , uint32_t nBitMask)
{
	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}

	*pnGPIO = 0;

	if (nBitMask > EC_BITMASK_EXT)
	{
		return m_clsECTrans.ECRead(EC_RW_ADDR_GPIO_DIR_EXT, EC_REGION_1, (uint8_t*)pnGPIO, 1);
	}
	else
	{
		return m_clsECTrans.ECRead(EC_RW_ADDR_GPIO_DIR, EC_REGION_1, (uint8_t*)pnGPIO, 1);
	}
}

EERROR CECFunct::SetGPIO(uint32_t plevel, uint32_t nBitMask)
{
	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}
	if (nBitMask > EC_BITMASK_EXT)
	{
		return m_clsECTrans.ECWrite(EC_RW_ADDR_GPIO_OUT_EXT, EC_REGION_1, (uint8_t*)&plevel, 1);
	}
	else
	{
		return m_clsECTrans.ECWrite(EC_RW_ADDR_GPIO_OUT, EC_REGION_1, (uint8_t*)&plevel, 1);
	}
}

EERROR CECFunct::SetGPIODir(uint32_t pDirection , uint32_t nBitMask)
{
	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}
	if (nBitMask > EC_BITMASK_EXT)
	{
		return m_clsECTrans.ECWrite(EC_RW_ADDR_GPIO_DIR_EXT, EC_REGION_1, (uint8_t*)&pDirection, 1);
	}
	else
	{
		return m_clsECTrans.ECWrite(EC_RW_ADDR_GPIO_DIR, EC_REGION_1, (uint8_t*)&pDirection, 1);
	}

}

EERROR CECFunct::GetCapsGpioDir(uint32_t* pnCapsIn, uint32_t* pnCapsOut, uint32_t GpioId)
{

	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID_EXT(0))
		{
			return EAPI_STATUS_UNSUPPORTED;
		}
	}
	if (GpioId > 0x0B)
		return EAPI_STATUS_UNSUPPORTED;

	if (m_n_GPIO_Cur_Id == EAPI_ID_GPIO_BANK00 || m_n_GPIO_Cur_Id == EAPI_ID_GPIO_BANK_EXT00) {
		*pnCapsIn = EC_INPUT_CAP;
		*pnCapsOut = EC_OUTPUT_CAP;
		return EAPI_STATUS_SUCCESS;
	}
	return EAPI_STATUS_INVALID_PARAMETER;

}

EERROR CECFunct::ValidateBitMask(uint32_t nBitMask)
{
	uint32_t len=16,eRet=1,i;
	int retry;
	for (retry = 0; retry < 10; retry++)
	{
		if (m_clsECTrans.ECRead(EC_CAP, EC_REGION_1, (uint8_t*)&i, len) == EAPI_STATUS_SUCCESS)
		{
			eRet = 0;
		}
	}

	i = i & (1<<4);

    if (eRet == EAPI_STATUS_SUCCESS)
	{
		if (i != 0)
		{
			if (nBitMask >= EC_BITMASK)
			{
				return EAPI_STATUS_ERROR;
			}
		}
		else
		{
			if (nBitMask >= EC_BITMASK_EXT)
			{
				return EAPI_STATUS_ERROR;
			}
		}
	}
	return EAPI_STATUS_SUCCESS;
}

