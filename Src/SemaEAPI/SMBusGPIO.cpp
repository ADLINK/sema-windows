#include "stdafx.h"
#include "SMBusFunct.h"


#define PCA9535_ADDRESS				0x40		///< GPIO PCA9535 chip Address
#define PCA9535_GPIO_PORT0_INPUT_REG		0x00		///< GPIO Configuration registers
#define PCA9535_GPIO_PORT0_OUTPUT_REG		0x02		///< GPIO Output registers
#define PCA9535_GPIO_PORT0_CON_REG			0x06		///< GPIO Direction registers
#define PCA9535_GPIO_PORT1_INPUT_REG		0x01		///< GPIO Configuration registers
#define PCA9535_GPIO_PORT1_OUTPUT_REG		0x03		///< GPIO Input registers
#define PCA9535_GPIO_PORT1_CON_REG			0x07		///< GPIO Direction registers

#define BMC_BITMASK				0x000000FF

#define PCA9535_INPUT_CAP		0x000000FF
#define PCA9535_OUTPUT_CAP		0x000000FF
#define PCA9535_BITMASK			0x000000FF

#define SEMA_C_GPIOS			0x10000000

/* Directions */
#define PCA9535_GPIO_INPUT   1
#define PCA9535_GPIO_OUTPUT  0

EERROR CSMBusFunct::SetId(uint8_t GpioId)
{
	m_n_GPIO_Cur_Id = EAPI_GPIO_BANK_ID(GpioId);

	return EAPI_STATUS_SUCCESS;
}

void GPIO_Registry_Write(ULONG value, int GPIO_Dir_Level)
{
	DWORD dwRet, cbData = sizeof(DWORD);
	HKEY hkey;

	dwRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		L"Software\\SEMA_TOOL",
		0,
		KEY_WRITE,
		&hkey);

	if (GPIO_Dir_Level == 0)
		dwRet = RegSetValueExA(hkey, "PCA9535_GPIO0_DIR", 0, REG_DWORD, (LPBYTE)&value, cbData);
	else if (GPIO_Dir_Level == 1)
		dwRet = RegSetValueExA(hkey, "PCA9535_GPIO0_Value", 0, REG_DWORD, (LPBYTE)&value, cbData);

	RegCloseKey(hkey);

}
EERROR CSMBusFunct::GetGPIO(uint32_t* pnGPIO)
{

	EERROR eRet;
	uint8_t bDataRet[4] = { 0x00 };
	uint32_t nRet = 0x04;

	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}

	Sleep(50);

	if (m_nSemaCaps & SEMA_C_GPIOS)
	{
		uint8_t pDataIn[] = { SEMA_CMD_GPIO_READ, 0x00, 0x00, 0x00,0x00 };

		if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, pDataIn[0], NULL, \
			0x00, (uint8_t*)&bDataRet, nRet)) == EAPI_STATUS_SUCCESS)
		{
			*pnGPIO = bDataRet[0];
		}
	}
	else
	{
		uint8_t pDataIn[] = { PCA9535_GPIO_PORT0_INPUT_REG, 0x00, 0x00, 0x00,0x00 };

		if ((eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_RBB, pDataIn[0], pDataIn, 0x04, (uint8_t*)&bDataRet, nRet)) == EAPI_STATUS_SUCCESS)
		{
			*pnGPIO = bDataRet[0];
			/*
			nRet = 0x04;
			pDataIn[0x00] = PCA9535_GPIO_PORT1_INPUT_REG;

			Sleep(50);
			if ((eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_RBB, pDataIn[0], pDataIn, 0x02, (uint8_t*)&bDataRet, nRet)) == EAPI_STATUS_SUCCESS)
			{
				*pnGPIO |= (bDataRet[0] << 0x08);
			}
			*/
		}
	}
	return eRet;
}

EERROR CSMBusFunct::GetGPIOIN(uint32_t * pnGPIO, uint32_t nBitMask)
{
	return EAPI_STATUS_ERROR;
}
EERROR CSMBusFunct::GetGPIOOUT(uint32_t * pnGPIO, uint32_t nBitMask)
{
	return EAPI_STATUS_ERROR;
}

EERROR CSMBusFunct::GetGPIODir(uint32_t* pnGPIO, uint32_t nBitMask)
{
	
	EERROR eRet;
	uint8_t bDataRet[4] = { 0x00 };
	uint32_t nRet = 0x04;

	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}

	Sleep(50);

	if (m_nSemaCaps & SEMA_C_GPIOS)
	{
		uint8_t pDataIn[] = { SEMA_CMD_GPIO_DIR, 0x00, 0x00, 0x00,0x00 };
		if ((eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr | 0x01, TT_RBL, pDataIn[0], NULL, \
			0x00, (uint8_t*)&bDataRet, nRet)) == EAPI_STATUS_SUCCESS)
		{
			*pnGPIO = bDataRet[0];
		}
	}
	else
	{
		uint8_t pDataIn[] = { PCA9535_GPIO_PORT0_CON_REG, 0x00, 0x00, 0x00,0x00 };
		if ((eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_RBB, pDataIn[0], pDataIn, 0x04, \
			(uint8_t*)&bDataRet, nRet)) == EAPI_STATUS_SUCCESS)
		{
			*pnGPIO = bDataRet[0];
			/*
			nRet = 0x04;
			pDataIn[0x00] = PCA9535_GPIO_PORT1_CON_REG;
			if ((eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_RBB, pDataIn[0], pDataIn, 0x02, \
				(uint8_t*)&bDataRet, nRet)) == EAPI_STATUS_SUCCESS)
			{
				*pnGPIO |= (bDataRet[0] << 0x08);
			}
			*/
		}
	}
	return eRet;
}

EERROR CSMBusFunct::SetGPIO(uint32_t plevel, uint32_t nBitMask)
{
	EERROR eRet;
	uint32_t nRet = 0x04;

	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}

	Sleep(50);

	if (m_nSemaCaps & SEMA_C_GPIOS)
	{
		uint8_t pDataIn[] = { SEMA_CMD_GPIO_WRITE, (uint8_t)plevel };

		eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, pDataIn[0], &pDataIn[1], 0x01);
	}
	else
	{
		uint8_t pDataIn[] = { PCA9535_GPIO_PORT0_OUTPUT_REG, (uint8_t)plevel,0,0,0 }, bDataRet[4] = { 0x00 };

		if ((eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_WBB, pDataIn[0x00], pDataIn, \
			0x02, (uint8_t*)&bDataRet, nRet)) == EAPI_STATUS_SUCCESS)
		{
			//write registry
			GPIO_Registry_Write(plevel, 1);
			/*
			Sleep(50);
			pDataIn[0x00] = PCA9535_GPIO_PORT1_OUTPUT_REG;
			pDataIn[0x01] = (uint8_t)(plevel >> 0x08);
			eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_WBB, pDataIn[0x00],\
				pDataIn, 0x02, (uint8_t*)&bDataRet, nRet);
				*/
		}
	}
	return eRet;
}

EERROR CSMBusFunct::SetGPIODir(uint32_t pDirection, uint32_t nBitMask)
{
	EERROR eRet;
	uint32_t nRet = 0x04;

	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}

	Sleep(50);

	if (m_nSemaCaps & SEMA_C_GPIOS)
	{
		uint8_t pDataIn[] = { SEMA_CMD_GPIO_DIR, (uint8_t)pDirection };

		eRet = m_clsSemaTrans.BlockTrans(m_bBMCAdr, TT_WBL, pDataIn[0], &pDataIn[1], 0x01);
	}
	else
	{
		uint8_t pDataIn[] = { PCA9535_GPIO_PORT0_CON_REG, (uint8_t)pDirection }, bDataRet[4] = { 0x00 };

		if ((eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_WBB, pDataIn[0], pDataIn, 0x02, \
			(uint8_t*)&bDataRet, nRet)) == EAPI_STATUS_SUCCESS)
		{
			//write registry
			GPIO_Registry_Write(pDirection, 0);

			/*
			pDataIn[0x00] = PCA9535_GPIO_PORT1_CON_REG;
			pDataIn[0x01] = (uint8_t)(pDirection >> 0x08);
			Sleep(50);
			eRet = m_clsSemaTrans.ByteTrans(PCA9535_ADDRESS, TT_WBB, pDataIn[0], pDataIn, 0x02, \
				(uint8_t*)&bDataRet, nRet);
				*/
		}
	}
	return eRet;
}

EERROR CSMBusFunct::GetCapsGpioDir(uint32_t* pnCapsIn, uint32_t* pnCapsOut, uint32_t GpioId)
{
	if (m_n_GPIO_Cur_Id != EAPI_GPIO_BANK_ID(0))
	{
		return EAPI_STATUS_ERROR;
	}

	*pnCapsIn = PCA9535_INPUT_CAP;
	*pnCapsOut = PCA9535_OUTPUT_CAP;

	return EAPI_STATUS_SUCCESS;
}


EERROR CSMBusFunct::ValidateBitMask(uint32_t nBitMask)
{
	if (m_nSemaCaps & SEMA_C_GPIOS)
	{
		if (nBitMask >= BMC_BITMASK)
		{
			return EAPI_STATUS_ERROR;
		}
	}
	else
	{
		if (nBitMask >= PCA9535_BITMASK)
		{
			return EAPI_STATUS_ERROR;
		}
	}
	return EAPI_STATUS_SUCCESS;
}