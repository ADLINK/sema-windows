#include "SemaSMB.h"
#include "SemaSMB.tmh"

UINT32 SMBus_ICH_Lock(WDFDEVICE pDevice)
{
	SEMA_DEVICE_EXT* pDevExt = GetSEMADevExt(pDevice);
	NTSTATUS eRet = STATUS_INVALID_DEVICE_STATE;
	UINT32 nRetry = 100;
	UINT8 bTemp;

#if !DeviceGuardSupport
	LONGLONG llTimeOut = WDF_REL_TIMEOUT_IN_MS(10);
	eRet = WdfWaitLockAcquire(pDevExt->hSmbLock, &llTimeOut);
#endif
	do
	{
		KeStallExecutionProcessor(20);
		if (pDevExt->pSmbMemReg != NULL)
		{
			bTemp = pDevExt->pSmbMemReg->HST_STS;
		}
		else
		{
			bTemp = READ_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_STS)));
		}
	} while ((bTemp & HST_STS_INUSE_STS) && (nRetry--));

	if (nRetry <= 0x00)
	{
		eRet = STATUS_TIMEOUT;
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER,
			"SMBI2CTransfer failed, timeout waiting for byte done, SMB_HSTS 0x%X",
			bTemp);
	}
	else
	{
		eRet = STATUS_SUCCESS;
		if (pDevExt->pSmbMemReg != NULL)
		{
			pDevExt->pSmbMemReg->HST_STS &= ~HST_STS_INUSE_STS;
		}
		else
		{
			bTemp = READ_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_STS)));
			WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_STS)), bTemp & ~HST_STS_INUSE_STS);
		}
	}
	return eRet;
}

void SMBus_ICH_SetAddress(WDFDEVICE pDevice, UINT8 bAddress, tTransType eType)
{	
	SEMA_DEVICE_EXT*	pDevExt = GetSEMADevExt(pDevice);
	UINT8 bTemp;
	if (pDevExt->pSmbMemReg != NULL || pDevExt->pSmbPortReg != 0)
	{
		if (pDevExt->pSmbMemReg != NULL)
		{
			pDevExt->pSmbMemReg->AUX_CTL = 0x02; // Enable 32-byte-Buffer (ICH9 only)
			bTemp = pDevExt->pSmbMemReg->HST_CNT; // Reset Index counter
		}
		else
		{
			WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + AUX_CTL)), 0x02); //bTemp & ~HST_STS_INUSE_STS);
			bTemp = READ_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_CNT)));
		}

		
		for (bTemp = 0; bTemp < 0x20; bTemp++)
		{
			if (pDevExt->pSmbMemReg != NULL)
				pDevExt->pSmbMemReg->HOST_BLOCK_DB = 0x00;
			else
				WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HOST_BLOCK_DB)), 0);
		}


		if (pDevExt->pSmbMemReg != NULL)
		{
			bTemp = pDevExt->pSmbMemReg->HST_CNT;
			pDevExt->pSmbMemReg->XMIT_SLVA = (TT_IsReadAccess(eType)) ? (bAddress | 1) : (bAddress & 0xFE);
		}
		else
		{
			bTemp = READ_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_CNT)));
			WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + XMIT_SLVA)), (TT_IsReadAccess(eType)) ? (bAddress | 1) : (bAddress & 0xFE));
		}
	}
}

void SMBus_ICH_Start(WDFDEVICE pDevice, UINT8 bCtrl)
{
	SEMA_DEVICE_EXT*	pDevExt = GetSEMADevExt(pDevice);

	if (pDevExt->pSmbMemReg != NULL)
	{
		pDevExt->pSmbMemReg->HST_CNT = bCtrl;
		pDevExt->pSmbMemReg->HST_CNT = bCtrl | 0x40;
	}
	else
	{
		WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_CNT)), bCtrl);
		WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_CNT)), bCtrl | 0x40);
	}	
}

NTSTATUS SMBus_Wait_For_Complition(WDFDEVICE pDevice)
{
	volatile UINT8 hostStatus;
	UINT32 nRetry = 1000;

	NTSTATUS eRet = STATUS_SUCCESS;

	SEMA_DEVICE_EXT* pDevExt = GetSEMADevExt(pDevice);

	if (pDevExt->pSmbMemReg != NULL || pDevExt->pSmbPortReg != 0)
	{
		do
		{
			KeStallExecutionProcessor(10);
			
			if (pDevExt->pSmbMemReg != NULL)
			{
				hostStatus = pDevExt->pSmbMemReg->HST_STS;
			}
			else
			{
				hostStatus = READ_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_STS)));
			}

			if (SMB_HSTS_IsByteDone(hostStatus))
			{
				eRet = STATUS_SUCCESS;

				if (pDevExt->pSmbMemReg != NULL)
				{
					pDevExt->pSmbMemReg->HST_STS = hostStatus & ~HST_STS_INUSE_STS;
				}
				else
				{
					WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_STS)), hostStatus & ~HST_STS_INUSE_STS);
				}
				break;
			}
			else if (SMB_HSTS_IsError(hostStatus))
			{
				eRet = STATUS_UNSUCCESSFUL;
				TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER,
					"SMBI2CTransfer failed, timeout waiting for byte done, SMB_HSTS 0x%X",
					hostStatus);
				break;
			}
			--nRetry;

		} while (nRetry > 0);
	}
	return eRet;
}

void SMBus_ICH_UnLock(WDFDEVICE pDevice)
{
	SEMA_DEVICE_EXT*	pDevExt = GetSEMADevExt(pDevice);
	if (pDevExt->pSmbMemReg != NULL || pDevExt->pSmbPortReg != 0)
	{
#if !DeviceGuardSupport
		WdfWaitLockRelease(pDevExt->hSmbLock);
#endif
		if (pDevExt->pSmbMemReg != NULL)
		{
			pDevExt->pSmbMemReg->HST_STS = 0xFF;
		}
		else
		{
			WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_STS)), 0xFF);
		}
	}
}


NTSTATUS SMBus_ICH_Byte_Trans(WDFDEVICE pDevice, PSMB_COMMAND pCmd, BYTE* Buffer, UINT32 nDataOut, ULONG* pulRet)
{	
	UINT8 ctl = 0;
	UINT8 Address = pCmd->bAddr << 1;
	*pulRet = nDataOut;
	SEMA_DEVICE_EXT* pDevExt = GetSEMADevExt(pDevice);
	Address = (pCmd->bTransType < 4) ? Address : Address + 1;
	
	NTSTATUS eRet = SMBus_ICH_Lock(pDevice);
	if (NT_SUCCESS(eRet))
	{
		eRet = STATUS_INVALID_DEVICE_STATE;
		if (pDevExt->pSmbMemReg != NULL || pDevExt->pSmbPortReg != 0)
		{
			eRet = STATUS_SUCCESS;
			SMBus_ICH_SetAddress(pDevice, Address, IsWriteAdr(Address) ? TT_WBL : TT_RBL);
			
			switch (pCmd->bTransType)
			{
			case TT_WBW:
				if (pDevExt->pSmbMemReg != NULL)
					pDevExt->pSmbMemReg->HST_D1 = pCmd->pData[2];
				else
					WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_D1)), pCmd->pData[2]);
				ctl += 4;
			case TT_WBB:
				if (pDevExt->pSmbMemReg != NULL)
					pDevExt->pSmbMemReg->HST_D0 = pCmd->pData[1];
				else
					WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_D0)), pCmd->pData[1]);
				ctl += 4;
			case TT_WB:		
				if (pDevExt->pSmbMemReg != NULL)
					pDevExt->pSmbMemReg->HST_CMD = pCmd->pData[0];
				else
					WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_CMD)), pCmd->pData[0]);
				ctl += 4;
				break;

			case TT_RBW:
				ctl += 4;
			case TT_RBB:
				ctl += 4;
				if (pDevExt->pSmbMemReg != NULL)
					pDevExt->pSmbMemReg->HST_CMD = pCmd->pData[0];
				else
					WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_CMD)), pCmd->pData[0]);
			case TT_RB:
				ctl += 4;
				break;
			default:
				SMBus_ICH_UnLock(pDevice);
				return 0;
			}
		}
	}
	
	SMBus_ICH_Start(pDevice, ctl);
	
	eRet = SMBus_Wait_For_Complition(pDevice);

	if (NT_SUCCESS(eRet))
	{
		switch (pCmd->bTransType)
		{
		case TT_RB:
		case TT_RBW:
			if (pDevExt->pSmbMemReg != NULL)
				Buffer[1] = pDevExt->pSmbMemReg->HST_D1;
			else
				Buffer[1] = READ_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_D1)));
		case TT_RBB:
			if (pDevExt->pSmbMemReg != NULL)
				Buffer[0] = pDevExt->pSmbMemReg->HST_D0;
			else
				Buffer[0] = READ_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_D0)));
			break;
		}
	}

	if (NT_SUCCESS(eRet) == FALSE)
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_PNP, "SemaSMB Error : %04X", eRet);
	}
	SMBus_ICH_UnLock(pDevice);

	return eRet;
}

NTSTATUS SMBus_ICH_Block_Trans(WDFDEVICE pDevice, UINT8 bAddress, UINT8 bCommand, UINT8 bLength, UINT8 *pBuffer, ULONG* pulRet)
{
	UINT8 bTemp = 0x00;
	UINT8 i = 0x00;
	SEMA_DEVICE_EXT*	pDevExt;

	(*pulRet) = 0;

	NTSTATUS eRet = SMBus_ICH_Lock(pDevice);
	if (NT_SUCCESS(eRet))
	{
		eRet = STATUS_INVALID_DEVICE_STATE;
		pDevExt = GetSEMADevExt(pDevice);
		if (pDevExt->pSmbMemReg != NULL || pDevExt->pSmbPortReg != 0)
		{
			eRet = STATUS_SUCCESS;
			SMBus_ICH_SetAddress(pDevice, bAddress, IsWriteAdr(bAddress) ? TT_WBL : TT_RBL);

			if (pDevExt->pSmbMemReg != NULL)
			{
				pDevExt->pSmbMemReg->HST_CMD = bCommand; // Set Host Command
				pDevExt->pSmbMemReg->HST_D0 = bLength;  // Clear Number of received bytes or set Bytes to send
			}
			else
			{
				WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_CMD)), bCommand);
				WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_D0)), bLength);
			}
			
			if (IsWriteAdr(bAddress))
			{
				UINT8 pBufData[0x32];
				RtlZeroMemory(pBufData, 0x32);
				RtlCopyMemory(pBufData, pBuffer, bLength);

				for (i = 0; i < 0x20; i++)
				{
					if (pDevExt->pSmbMemReg != NULL)
						pDevExt->pSmbMemReg->HOST_BLOCK_DB = pBufData[i];
					else
						WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HOST_BLOCK_DB)), pBufData[i]);
				}
			}

			SMBus_ICH_Start(pDevice, 0x14);

			eRet = SMBus_Wait_For_Complition(pDevice);

			if (NT_SUCCESS(eRet))
			{
				if (IsWriteAdr(bAddress))
				{
					(*pulRet) = bLength;
				}

				if (IsReadAdr(bAddress))				// If read access: get buffer
				{
					UINT8 pBufData[0x20];
					RtlZeroMemory(pBufData, 0x20);

					if (pDevExt->pSmbMemReg != NULL)	// Get number of received Bytes		
						bTemp = pDevExt->pSmbMemReg->HST_D0;
					else
						bTemp = READ_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_D0)));

					for (i = 0; i < bTemp; i++)
					{
						if (pDevExt->pSmbMemReg != NULL)	// Get number of received Bytes		
							pBufData[i] = pDevExt->pSmbMemReg->HOST_BLOCK_DB;
						else
							pBufData[i] = READ_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HOST_BLOCK_DB)));
					}

					RtlCopyMemory(pBuffer, pBufData, bTemp);
					(*pulRet) = bTemp;
				}
			}
		}
	}

	if (NT_SUCCESS(eRet) == FALSE)
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_PNP, "SemaSMB Error : %04X", eRet);
	}

	SMBus_ICH_UnLock(pDevice);
	return eRet;
}

void SMBI2CDisable(void)
{
	unsigned char smbhcfg = (unsigned char)pci_read(0, 0x1F, 1, 0x40, 1);
	smbhcfg &= ~(0x4);
	pci_write(0, 0x1F, 1, 0x40, 1, smbhcfg);
}

void SMBI2CEnable(void)
{
	unsigned char smbhcfg = (unsigned char)pci_read(0, 0x1F, 1, 0x40, 1);
	smbhcfg |= 0x4;
	pci_write(0, 0x1F, 1, 0x40, 1, smbhcfg);
}

NTSTATUS
SMBI2CTransfer(WDFDEVICE pDevice, PSMB_COMMAND pCmd, BYTE* Buffer, UINT32 nDataOut, ULONG* pulRet, WDFREQUEST request)
{
	NTSTATUS status = STATUS_SUCCESS;
	UINT8 ctlCode = 0, txed = 0;
	PUINT8 data = NULL;
	UNREFERENCED_PARAMETER(nDataOut);
	UNREFERENCED_PARAMETER(Buffer);

	SEMA_DEVICE_EXT* pDevExt = GetSEMADevExt(pDevice);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "--> %!FUNC!");

	status = SMBus_ICH_Lock(pDevice);
	if (NT_SUCCESS(status))
	{
		return status;
	}

	if (pCmd->bTransType == TT_I2C_RBL)
		ctlCode = 0x18;
	else if (pCmd->bTransType == TT_I2C_WBL)
		ctlCode = 0x14;
	else
		status = STATUS_INVALID_PARAMETER;

	if (pCmd->bLength <= 0)
		status = STATUS_INVALID_PARAMETER;

	status = WdfRequestRetrieveOutputBuffer(
		request,
		pCmd->bLength,
		(PVOID)(&data),
		NULL
	);

	if (!NT_SUCCESS(status))
	{
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_DRIVER,
			"WdfRequestRetrieveOutputBuffer failed %!STATUS!", status);
		*pulRet = 0;
		status = STATUS_INVALID_PARAMETER;
	}

	if (NT_SUCCESS(status))
	{
		SMBI2CEnable();

		if (pDevExt->pSmbMemReg != NULL)
			pDevExt->pSmbMemReg->AUX_CTL = 0;
		else
			WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + AUX_CTL)), 0);
		
		if (pDevExt->pSmbMemReg != NULL)
			pDevExt->pSmbMemReg->XMIT_SLVA = ((UINT8)(pCmd->bAddr << 1));
		else
			WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + XMIT_SLVA)), ((UINT8)(pCmd->bAddr << 1)));

		if (ctlCode == 0x14)
		{
			if (pDevExt->pSmbMemReg != NULL)
			{
				pDevExt->pSmbMemReg->HST_D0 = pCmd->bLength;
				pDevExt->pSmbMemReg->HOST_BLOCK_DB = pCmd->pData[0];
			}
			else
			{
				WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_D0)), pCmd->bLength);
				WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HOST_BLOCK_DB)), pCmd->pData[0]);
			}
		}
		else if (ctlCode == 0x18)
		{
			if (pDevExt->pSmbMemReg != NULL)
				pDevExt->pSmbMemReg->HST_D1 = pCmd->bCmd;
			else
				WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_D1)), pCmd->bCmd);
			
			if (pCmd->bLength == 1)
			{
				ctlCode |= 0x20; // SMB_HCTL_LASTBYTE
			}
		}
		else
		{
			if (pDevExt->pSmbMemReg != NULL)
				pDevExt->pSmbMemReg->HST_CMD = pCmd->bCmd;
			else
				WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_CMD)), pCmd->bCmd);
		}

		if (pDevExt->pSmbMemReg != NULL)
		{
			pDevExt->pSmbMemReg->HST_CNT = ctlCode;
			pDevExt->pSmbMemReg->HST_CNT = ctlCode | 0x40;
		}
		else
		{
			WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_CNT)), ctlCode);
			WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_CNT)), ctlCode | 0x40);
		}
		if (pCmd->bTransType == TT_I2C_RBL)
			ctlCode = 0x18;
		else
			ctlCode = 0x14;

		do
		{
			if (SMBus_Wait_For_Complition(pDevice) != STATUS_SUCCESS)
			{
				status = STATUS_UNSUCCESSFUL;
				break;
			}

			if (pCmd->bTransType == TT_I2C_RBL)
			{
				if (pDevExt->pSmbMemReg != NULL)
					pCmd->pData[txed] = data[txed] = pDevExt->pSmbMemReg->HOST_BLOCK_DB;
				else
					pCmd->pData[txed] = data[txed] = READ_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HOST_BLOCK_DB)));				
			}
			else if ((txed + 1) < pCmd->bLength)
			{
				if (pDevExt->pSmbMemReg != NULL)
					pDevExt->pSmbMemReg->HOST_BLOCK_DB = pCmd->pData[txed + 1];
				else
					WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HOST_BLOCK_DB)), pCmd->pData[txed + 1]);
			}

			if (pDevExt->pSmbMemReg != NULL)
				pDevExt->pSmbMemReg->HST_STS = 0x80;
			else
				WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_STS)), pCmd->pData[txed + 1]);

			txed++;

			if (((txed + 1) == pCmd->bLength) && (pCmd->bTransType == TT_I2C_RBL))
				ctlCode |= 0x20;

			if (pDevExt->pSmbMemReg != NULL)
				pDevExt->pSmbMemReg->HST_CNT = ctlCode;
			else
				WRITE_PORT_UCHAR((PUCHAR)((USHORT)(pDevExt->pSmbPortReg + HST_CNT)), ctlCode);
		
		} while (txed < pCmd->bLength);

		SMBI2CDisable();

	}

	SMBus_ICH_UnLock(pDevice);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "<-- %!FUNC!");

	return status;
}