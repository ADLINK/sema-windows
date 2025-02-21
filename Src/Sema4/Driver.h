#include <ntddk.h>
#include <wdf.h>

#include "wchar.h"
#include "ChipInfo.h"
#include "Trace.h"
#include <initguid.h>

EXTERN_C_START

#define SEMA_VERSION_MAJOR	4
#define SEMA_VERSION_MINOR	0

#define SEMASMB_VERSION_GETSMBSTRING_SIZE 0x40

#define SEMA_VERSION_ADDON_MAXSIZE 32

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD SemaSMB_DeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP SemaSMB_EvtDriverContextCleanup;
EVT_WDF_DEVICE_PREPARE_HARDWARE SemaSMB_PrepareHardware;
EVT_WDF_DEVICE_RELEASE_HARDWARE SemaSMB_ReleaseHardware;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL Sema_DeviceIoControl;

EXTERN_C_END