#include "Driver.h"
#include "Driver.tmh"
#include "Public.h"

#include "SemaSMB.h"
#include "SemaEC.h"

static int timeout[2];

struct info {
	unsigned int board_type;
	char boardname[64];
};

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING szRegistryPath)
{
	NTSTATUS eRet;
	WDF_DRIVER_CONFIG stConfig;
	WDF_OBJECT_ATTRIBUTES stAttrib;
	WPP_INIT_TRACING(DriverObject, szRegistryPath);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry\n");

	WDF_OBJECT_ATTRIBUTES_INIT(&stAttrib);
	stAttrib.EvtCleanupCallback = SemaSMB_EvtDriverContextCleanup;


	WDF_DRIVER_CONFIG_INIT(&stConfig, SemaSMB_DeviceAdd);
	eRet = WdfDriverCreate(DriverObject, szRegistryPath, &stAttrib, &stConfig, WDF_NO_HANDLE);
	if (!NT_SUCCESS(eRet))
	{
		WPP_CLEANUP(DriverObject);
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit\n");

	return eRet;
}

NTSTATUS SemaSMB_DeviceAdd(_In_ WDFDRIVER pDriver, _Inout_ PWDFDEVICE_INIT pDeviceInit)
{
	NTSTATUS eRet;
	WDFQUEUE  stQueue;
	WDFDEVICE pDevice = NULL;
	WDF_IO_QUEUE_CONFIG		stQueueConfig;
	WDF_OBJECT_ATTRIBUTES	stFdoAttrib;
	SEMA_DEVICE_EXT*		pSMBDevExt = NULL;
	WDF_PNPPOWER_EVENT_CALLBACKS	pPnpPowerCallbacks;

	UNREFERENCED_PARAMETER(pDriver);

	WDF_OBJECT_ATTRIBUTES stAttrib;
	DECLARE_CONST_UNICODE_STRING(strDevName, L"\\Device\\SEMA");

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Enter\n");

	WdfDeviceInitSetDeviceType(pDeviceInit, 40000);
	eRet = WdfDeviceInitAssignName(pDeviceInit, &strDevName);

	WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pPnpPowerCallbacks);

	pPnpPowerCallbacks.EvtDevicePrepareHardware = SemaSMB_PrepareHardware;
	pPnpPowerCallbacks.EvtDeviceReleaseHardware = SemaSMB_ReleaseHardware;
	WdfDeviceInitSetPnpPowerEventCallbacks(pDeviceInit, &pPnpPowerCallbacks);

	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&stFdoAttrib, SEMA_DEVICE_EXT);
	eRet = WdfDeviceCreate(&pDeviceInit, &stFdoAttrib, &pDevice);
	if (!NT_SUCCESS(eRet))
	{
		goto end;
	}

	pSMBDevExt = GetSEMADevExt(pDevice);

	pSMBDevExt->ulMemSize = 0x00;
	pSMBDevExt->pSmbMemReg = NULL;
	pSMBDevExt->pSmbPortReg = 0;
	pSMBDevExt->pDevice = pDevice;

	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&stQueueConfig, WdfIoQueueDispatchSequential);
	stQueueConfig.EvtIoDeviceControl = Sema_DeviceIoControl;
	eRet = WdfIoQueueCreate(pDevice, &stQueueConfig, WDF_NO_OBJECT_ATTRIBUTES, &stQueue);
	if (!NT_SUCCESS(eRet))
	{
		goto end;
	}

	WDF_OBJECT_ATTRIBUTES_INIT(&stAttrib);
	eRet = WdfWaitLockCreate(&stAttrib, &pSMBDevExt->hSmbLock);
	if (!NT_SUCCESS(eRet))
	{
		goto end;
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit\n");
end:
	return eRet;
}

void GPIO_Registry_Write(ULONG value, int GPIO_Dir_Level)
{

	NTSTATUS           status = STATUS_SUCCESS;
	UNICODE_STRING     RegistryKeyName, KeyName;
	HANDLE              handleRegKey = NULL;
	OBJECT_ATTRIBUTES  ObjectAttributes;
	ULONG ValueData = value;

	RtlInitUnicodeString(&RegistryKeyName, L"\\Registry\\Machine\\Software\\SEMA_TOOL");

	InitializeObjectAttributes(&ObjectAttributes,
		&RegistryKeyName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,    // handle
		NULL);

	status = ZwOpenKey(&handleRegKey, KEY_WRITE, &ObjectAttributes);

	if(GPIO_Dir_Level ==0)
		RtlInitUnicodeString(&KeyName, L"PCA9535_GPIO0_DIR");
	else if(GPIO_Dir_Level ==1)
		RtlInitUnicodeString(&KeyName, L"PCA9535_GPIO0_Value");

	status = ZwSetValueKey(handleRegKey, &KeyName, 0, REG_DWORD, &ValueData, sizeof(ValueData));
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "ZwSetValueKey\n");

	// All done with the registry.
	if (NULL != handleRegKey)
	{
		ZwClose(handleRegKey);
	}
}

NTSTATUS SemaSMB_PrepareHardware(WDFDEVICE pDevice, WDFCMRESLIST lstResources, WDFCMRESLIST lstResourcesTranslated)
{
	SEMA_DEVICE_EXT*	pDevExt = NULL;
	PHYSICAL_ADDRESS SMBusBaseAddr;
	NTSTATUS eRet = -STATUS_ABANDONED;

	UNREFERENCED_PARAMETER(lstResources);
	UNREFERENCED_PARAMETER(lstResourcesTranslated);
	PAGED_CODE();

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Enter\n");

	pDevExt = GetSEMADevExt(pDevice);
	
	pDevExt->SmbOrEc = 0;

	if (IsECDeviceExist(pDevice) > 0)
	{
		pDevExt->SmbOrEc = EC;
		eRet = STATUS_SUCCESS;
	}
	else if (IsSMBDeviceExist(pDevice) == 1)
		{
			pDevExt->SmbOrEc = SMB;
			if (pDevExt->Chipset.RessourceType == RESSOURTYPE_MMIO)
			{
				pDevExt->pSmbPortReg = 0;

				SMBusBaseAddr.QuadPart = 0;

				/*Checking the BAR1 for 4G decoding enable in BIOS*/
				if ((ReadsupportedExistingDevice4GDecoding(pDevice)) == 0)
					SMBusBaseAddr.HighPart = 0;
				else
					SMBusBaseAddr.HighPart = ReadsupportedExistingDevice4GDecoding(pDevice);

				
				SMBusBaseAddr.LowPart = ReadsupportedExistingDeviceMMIOBase(pDevice);

				TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, " Resource type is Memory\n");

				if (SMBusBaseAddr.LowPart == 0)
				{
					goto end;
				}

				pDevExt->ulMemSize = 4096;
				pDevExt->pSmbMemReg = (INTEL_SMB_REGS*)MmMapIoSpace(SMBusBaseAddr, pDevExt->ulMemSize, MmNonCached);
				if (pDevExt->pSmbMemReg == NULL)
				{
					goto end;
				}
			}
			else if (pDevExt->Chipset.RessourceType == RESSOURTYPE_PORT)
			{
				pDevExt->pSmbMemReg = NULL;

				TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, " Resource type is IO Port\n");

				pDevExt->pSmbPortReg = ReadsupportedExistingDevicePortBase(pDevice);
				if (pDevExt->pSmbPortReg == 0)
				{
					goto end;
				}
			}

			SMB_COMMAND pBufCmdInSMB;
			UINT8 pBufOut[50] = { 0 };
			UINT8 pBuf[100] = { 0 };
			ULONG ulRet;
			pBufCmdInSMB.bAddr = 0x28;
			pBufCmdInSMB.bCmd = 0x30;
			pBufCmdInSMB.bLength = 50;

			if (SMBus_ICH_Block_Trans(pDevice, (pBufCmdInSMB.bAddr << 0x01) | 0x01, pBufCmdInSMB.bCmd, pBufCmdInSMB.bLength, (UINT8*)pBufOut, &ulRet) == STATUS_SUCCESS)
			{
				RtlCopyMemory(pBuf, pBufOut, ulRet);
				pBufCmdInSMB.bCmd = 0x31;
				RtlZeroMemory(pBufOut, pBufCmdInSMB.bLength);
				if (SMBus_ICH_Block_Trans(pDevice, (pBufCmdInSMB.bAddr << 0x01) | 0x01, pBufCmdInSMB.bCmd, pBufCmdInSMB.bLength, (UINT8*)pBufOut, &ulRet) == STATUS_SUCCESS)
				{
					RtlCopyMemory(pBuf + ulRet, pBufOut, ulRet);
					if (strstr((char*)pBuf, "LiPPERT") || strstr((char*)pBuf, "ADLINK"))
					{
						eRet = STATUS_SUCCESS;
					}
				}
			}

			if (pDevExt->Chipset.DID==0x9D23u)
			{
				UINT32 dataout=0;
				UINT8 BufOut[4] = { 0 };
				pBufCmdInSMB.bAddr = 0x20;
				pBufCmdInSMB.bTransType = TT_RBB;
				pBufCmdInSMB.pData[0] = 0x06;
				pBufCmdInSMB.pData[1] = 0x00;
				pBufCmdInSMB.pData[2] = 0x00;
				pBufCmdInSMB.pData[3] = 0x00;
			
				if (SMBus_ICH_Byte_Trans(pDevice, &pBufCmdInSMB, (BYTE*)BufOut, dataout, &ulRet) != STATUS_SUCCESS)
				{
					TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "GPIO Direction read fail \n");
				}

				GPIO_Registry_Write((ULONG)BufOut[0], 0);

				LARGE_INTEGER Interval;
				Interval.QuadPart = -1 * 1000 * 50 * 10; /* 50ms */

				KeDelayExecutionThread(KernelMode, FALSE, &Interval);
				pBufCmdInSMB.pData[0] = 0x00;
				if (SMBus_ICH_Byte_Trans(pDevice, &pBufCmdInSMB, (BYTE*)BufOut, dataout, &ulRet) != STATUS_SUCCESS)
				{
					TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "GPIO value read fail \n");
				}
				GPIO_Registry_Write((ULONG)BufOut[0], 1);
			}
		}
	
	else
	eRet = STATUS_SUCCESS;
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit\n");
end:
	return eRet;
}

NTSTATUS SemaSMB_ReleaseHardware(IN WDFDEVICE pDevice, IN WDFCMRESLIST lstResourcesTranslated)
{
	SEMA_DEVICE_EXT*	pDevExt = GetSEMADevExt(pDevice);

	UNREFERENCED_PARAMETER(lstResourcesTranslated);
	PAGED_CODE();

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry\n");

	if (pDevExt->SmbOrEc == SMB && (pDevExt->Chipset.RessourceType == RESSOURTYPE_MMIO))
	{
		if (pDevExt != NULL && pDevExt->pSmbMemReg != NULL)
		{
			MmUnmapIoSpace(pDevExt->pSmbMemReg, pDevExt->ulMemSize);
		}
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit\n");

	return STATUS_SUCCESS;
}

VOID SemaSMB_EvtDriverContextCleanup(_In_ WDFOBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry\n");
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit\n");
	WPP_CLEANUP(WdfDriverWdmGetDriverObject((WDFDRIVER)DriverObject));
}

VOID Sema_DeviceIoControl(WDFQUEUE pQueue, WDFREQUEST pRequest, size_t nOutputBufferLength, size_t nInputBufferLength, ULONG nIoControlCode)
{
	NTSTATUS eRet = STATUS_SUCCESS;
	PCHAR pBufOut = NULL;
	size_t nBufIn = 0x00, nBufOut = 0x00;
	ULONG ulRet = 0x00;
	unsigned long msr_address;
	PMSR_COMMAND inMsrBuffer;
	PULONG msrbuffer = NULL;
	WDFDEVICE pDevice;
	SEMA_DEVICE_EXT* pDevExt;
	PSMB_COMMAND pBufCmdInSMB;
	PEC_COMMAND  pBufCmdInEC;
	EC_COMMAND  BufCmdInEC;
	EC_PORT_READ sECPortRead;
	EC_GET_FWVER sECGetFWVersion;
	PEC_GET_FWVER psECGetFWVersion;
	PEC_PORT_READ PECPortRead;
	PEC_PORT_WRITE PECPortWrite;
	EC_PORT_WRITE ECPortWrite;

	UINT64	data = 0;
	struct info *out;

	UNREFERENCED_PARAMETER(nOutputBufferLength);
	UNREFERENCED_PARAMETER(nInputBufferLength);

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry\n");

	pDevice = WdfIoQueueGetDevice(pQueue);
	pDevExt = GetSEMADevExt(pDevice);
			
	switch (nIoControlCode)
	{
	case IOCTL_SemaEC_OR_SMB:
		eRet = WdfRequestRetrieveOutputBuffer(pRequest, sizeof(struct info), &out, &nBufOut);
		if (NT_SUCCESS(eRet))
		{
			ulRet = (ULONG)nBufOut;
			if (pDevExt->SmbOrEc == SMB)
			{
				out->board_type = SMB;
				
				SMB_COMMAND BufCmdInSMB;
				UINT8 BufOut[50] = { 0 };
				ULONG lRet;
				BufCmdInSMB.bAddr = 0x28;
				BufCmdInSMB.bCmd = 0x30;
				BufCmdInSMB.bLength = 50;

				if (SMBus_ICH_Block_Trans(pDevice, (BufCmdInSMB.bAddr << 0x01) | 0x01, BufCmdInSMB.bCmd, BufCmdInSMB.bLength, (UINT8*)BufOut, &lRet) == STATUS_SUCCESS)
				{
					strncpy(out->boardname, (const char*)BufOut, nBufOut);
				}
				break;
			}
			else if (pDevExt->SmbOrEc == EC)
			{
				out->board_type = EC;

				char buffer[16] = { 0 };
				EC_COMMAND  BufCmdInEC_;
				ULONG test;

				BufCmdInEC_.bAddr = 0xA0;
				BufCmdInEC_.RegionIndex = ((0x62 << 8) | 0x66);
				BufCmdInEC_.bLength = 16;
				if ((eRet = EC_Read_Transfer(pDevice, &BufCmdInEC_, (BYTE*)buffer, (UINT32)16, &test)) == STATUS_SUCCESS)
				{
					strcpy(out->boardname, buffer);
				}
				break;
			}
			out->board_type = 0;
		}
		break;

	case IOCTL_Sema_GETVERSION:

		eRet = WdfRequestRetrieveOutputBuffer(pRequest, 2 * sizeof(UINT16), &pBufOut, &nBufOut);
		if (NT_SUCCESS(eRet))
		{
			ulRet = 0x04;
			*(PUINT16)(pBufOut) = SEMA_VERSION_MAJOR;
			*((PUINT16)(pBufOut)+1) = SEMA_VERSION_MINOR;
		}
		break;

	case IOCTL_Sema_GETSMBSTRING:
		
		if (pDevExt->SmbOrEc == EC)
		{
			ulRet = (ULONG)strlen("Intel(R) ACPI EC Controller");
		}
		else
		{
			ulRet = (ULONG)strlen("Intel(R) SMBus Controller");
		}
		eRet = WdfRequestRetrieveOutputBuffer(pRequest, (ulRet + 10), &pBufOut, &nBufOut);
		if (NT_SUCCESS(eRet))
		{
			RtlZeroMemory(pBufOut, nBufOut);
			if (pDevExt->SmbOrEc == EC)
			{
				RtlCopyMemory(pBufOut, "Intel(R) SMBus Controller", nBufOut);
			}
			else
			{
				RtlCopyMemory(pBufOut, "Intel(R) ACPI EC Controller", nBufOut);
			}
		}
		break;

	case IOCTL_Sema_READ_MSR:
		nBufOut = 8;
		eRet = WdfRequestRetrieveInputBuffer(pRequest, sizeof(MSR_COMMAND), &inMsrBuffer, &nBufOut);
		if (!NT_SUCCESS(eRet))
		{
			break;
		}

		eRet = WdfRequestRetrieveOutputBuffer(pRequest, 8, &msrbuffer, &nBufOut);
		if (!NT_SUCCESS(eRet))
		{
			break;
		}

		msr_address = inMsrBuffer->address;
		__try
		{
			data = __readmsr(msr_address);
			memcpy(msrbuffer, &data, nBufOut);
			ulRet = 8;
			eRet = STATUS_SUCCESS;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			ulRet = 0;
			eRet = STATUS_UNSUCCESSFUL;
		}
		break;

	case IOCTL_SemaSMB_READ_WRITE_BLOCK:

		eRet = WdfRequestRetrieveInputBuffer(pRequest, sizeof(SMB_COMMAND), &pBufCmdInSMB, &nBufIn);
		if (NT_SUCCESS(eRet))
		{
			eRet = WdfRequestRetrieveOutputBuffer(pRequest, 0x01, &pBufOut, &nBufOut);
			if (NT_SUCCESS(eRet))
			{
				eRet = STATUS_NOT_IMPLEMENTED;
				switch (pBufCmdInSMB->bTransType)
				{
				case TT_RBL:
					
					eRet = SMBus_ICH_Block_Trans(pDevice, (pBufCmdInSMB->bAddr << 0x01) | 0x01, pBufCmdInSMB->bCmd, pBufCmdInSMB->bLength, (UINT8*)pBufOut, &ulRet);
					break;

				case TT_WBL:
					
					eRet = SMBus_ICH_Block_Trans(pDevice, (pBufCmdInSMB->bAddr << 0x01), pBufCmdInSMB->bCmd, pBufCmdInSMB->bLength, pBufCmdInSMB->pData, &ulRet);
					break;

				case TT_I2C_RBL:
				case TT_I2C_WBL:

					eRet = SMBI2CTransfer(pDevice, pBufCmdInSMB, pBufCmdInSMB->pData, pBufCmdInSMB->bLength, &ulRet, pRequest);
					break;

				default:
					eRet = STATUS_INVALID_DEVICE_REQUEST;
				}
			}
		}
		break;

	case IOCTL_SemaSMB_READ_WRITE:	

		eRet = WdfRequestRetrieveInputBuffer(pRequest, sizeof(SMB_COMMAND), &pBufCmdInSMB, &nBufIn);
		if (NT_SUCCESS(eRet))
		{
			eRet = WdfRequestRetrieveOutputBuffer(pRequest, 0x01, &pBufOut, &nBufOut);
			if (NT_SUCCESS(eRet))
			{
				eRet = SMBus_ICH_Byte_Trans(pDevice, pBufCmdInSMB, (BYTE*)pBufOut, (UINT32)nBufOut, &ulRet);
			}
		}
		break;

	case IOCTL_SemaEC_READ:

		eRet = WdfRequestRetrieveInputBuffer(pRequest, sizeof(EC_COMMAND), &pBufCmdInEC, &nBufIn);
		if (NT_SUCCESS(eRet))
		{
			memcpy_s(&BufCmdInEC, nBufIn, pBufCmdInEC, nBufIn);
			
			eRet = WdfRequestRetrieveOutputBuffer(pRequest, 0x01, &pBufOut, &nBufOut);
			if (NT_SUCCESS(eRet))
			{
				eRet = EC_Read_Transfer(pDevice, &BufCmdInEC, (BYTE*)pBufOut, (UINT32)nBufOut, &ulRet);
			}
		}
		break;

	case IOCTL_SemaEC_WRITE:

		eRet = WdfRequestRetrieveInputBuffer(pRequest, sizeof(EC_COMMAND), &pBufCmdInEC, &nBufIn);
		if (NT_SUCCESS(eRet))
		{
			eRet = WdfRequestRetrieveOutputBuffer(pRequest, 0x01, &pBufOut, &nBufOut);
			if (NT_SUCCESS(eRet))
			{
				eRet = EC_Write_Transfer(pDevice, pBufCmdInEC, (BYTE*)pBufCmdInEC->pData,(UINT32)nBufOut, &ulRet);
				eRet = STATUS_SUCCESS;
			}
		}
		break;

	case IOCTL_Sema_Set_WDT :
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry IOCTL_Sema_Set_WDT\n");
		int *ptimeout;
		eRet = WdfRequestRetrieveInputBuffer(pRequest, 2 * sizeof(int), &ptimeout, &nBufIn);
		if (NT_SUCCESS(eRet))
		{
			timeout[0] = ptimeout[0];
			timeout[1] = ptimeout[1];
			TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry timeout %x %x\n", timeout[0], timeout[1]);
			eRet = STATUS_SUCCESS;
		}
	}
		break;
	case IOCTL_Sema_Get_WDT:
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry IOCTL_Sema_Get_WDT\n");
		int *ptimeout;
		eRet = WdfRequestRetrieveOutputBuffer(pRequest, 2 * sizeof(int), &ptimeout, &nBufOut);
		if (NT_SUCCESS(eRet))
		{
			ptimeout[0] = timeout[0];
			ptimeout[1] = timeout[1];
			ulRet = 2 * sizeof(int);
			TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry ptimeout %x %x\n", ptimeout[0], ptimeout[1]);
			eRet = STATUS_SUCCESS;
		}
	}
		break;
	case IOCTL_SemaEC_PORT_READ:
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry IOCTL_SemaEC_PORT_READ\n");
		eRet = WdfRequestRetrieveInputBuffer(pRequest, sizeof(EC_PORT_READ), &PECPortRead, &nBufIn);
		if (NT_SUCCESS(eRet))
		{
			memcpy_s(&sECPortRead, sizeof(EC_PORT_READ), PECPortRead, sizeof(EC_PORT_READ));
			eRet = WdfRequestRetrieveOutputBuffer(pRequest, 0x01, &pBufOut, &nBufOut);
			if (NT_SUCCESS(eRet))
			{
				eRet = Adl_PortRead(sECPortRead.m_usPortAddr, sECPortRead.m_pucData);
				ulRet = 0; /* always make zero output data already updated in structure*/
				*sECPortRead.m_iRetVal = (int)eRet;
			}
		}
	}
	break;
	case IOCTL_SemaEC_PORT_WRITE:
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry IOCTL_SemaEC_PORT_WRITE\n");
		eRet = WdfRequestRetrieveInputBuffer(pRequest, sizeof(EC_PORT_WRITE), &PECPortWrite, &nBufIn);
		if (NT_SUCCESS(eRet))
		{
			memcpy_s(&ECPortWrite, sizeof(EC_PORT_WRITE), PECPortWrite, sizeof(EC_PORT_WRITE));
			eRet = WdfRequestRetrieveOutputBuffer(pRequest, 0x01, &pBufOut, &nBufOut);
			if (NT_SUCCESS(eRet))
			{				
				eRet = Adl_PortWrite(ECPortWrite.m_usPortAddr, ECPortWrite.m_ucData);
				ulRet = 0; /* always make zero output data already updated in structure*/
				*ECPortWrite.m_iRetVal = (int)eRet;
			}
		}
	}
	break;
	case IOCTL_SemaEC_GET_FW_VERSION:
	{
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry IOCTL_SemaEC_GET_FW_VERSION\n");
		eRet = WdfRequestRetrieveInputBuffer(pRequest, sizeof(EC_GET_FWVER), &psECGetFWVersion, &nBufIn);
		if (NT_SUCCESS(eRet))
		{
			memcpy_s(&sECGetFWVersion, sizeof(EC_GET_FWVER), psECGetFWVersion, sizeof(EC_GET_FWVER));
			eRet = WdfRequestRetrieveOutputBuffer(pRequest, 0x01, &pBufOut, &nBufOut);
			if (NT_SUCCESS(eRet))
			{
				eRet = Adl_EcFirmwareVersion(sECGetFWVersion.m_pcFWVerBuff, sECGetFWVersion.m_usBuffLength);
				ulRet = 0; /* always make zero output data already updated in structure*/
				*sECGetFWVersion.m_iRetVal = (int)eRet;
			}
		}
	}
	break;
	
	default:
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "SemaSMB default IOCTL.\n");
		break;
	
	}
	
	WdfRequestCompleteWithInformation(pRequest, eRet, ulRet);
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit\n");
}
