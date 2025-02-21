#pragma once
#ifndef PUBLIC
#define PUBLIC

#define SMB_MAX_LENGTH 32

typedef struct {
	UINT8	bAddr;				// 7! Bit SMB Address
	UINT8	bTransType;			// One of tTransType
	UINT8	bCmd;				// Command Byte for Block Read/Write
	UINT8	bLength;				// Length of data to write
	UINT8   pData[SMB_MAX_LENGTH];	// Data Array for receive/send data
} SMB_COMMAND, *PSMB_COMMAND;


typedef struct {
	UINT16	bAddr;				// 7! Bit SMB Address
	UINT16	RegionIndex;				// Command Byte for Block Read/Write
	UINT32	bLength;				// Length of data to write
	UINT8   pData[32];	// Data Array for receive/send data
} EC_COMMAND, *PEC_COMMAND;


typedef struct _MSR_COMMAND {
	unsigned long address;
} MSR_COMMAND, *PMSR_COMMAND;

typedef struct _EC_PORT_WRITE {
		unsigned char m_ucData;
		unsigned short m_usPortAddr;
		int *m_iRetVal;
}EC_PORT_WRITE, *PEC_PORT_WRITE;

typedef struct _EC_PORT_READ {
		unsigned short m_usPortAddr;
		int *m_iRetVal;
		unsigned char *m_pucData;
}EC_PORT_READ, *PEC_PORT_READ;

typedef struct _EC_GET_FWVER {
		unsigned short m_usBuffLength;
		char *m_pcFWVerBuff;
		int *m_iRetVal;
}EC_GET_FWVER, *PEC_GET_FWVER;

#define FILEIO_TYPE 40001

#define IOCTL_Sema_GETVERSION			CTL_CODE(FILEIO_TYPE,	0x902, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Sema_GETSMBSTRING			CTL_CODE(FILEIO_TYPE,	0x904, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Sema_READ_MSR				CTL_CODE(FILEIO_TYPE,	0x905, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SemaEC_OR_SMB				CTL_CODE(FILEIO_TYPE,	0x908, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Sema_Get_WDT				CTL_CODE(FILEIO_TYPE,   0x909, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Sema_Set_WDT				CTL_CODE(FILEIO_TYPE,	0x910, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SemaSMB_READ_WRITE_BLOCK	CTL_CODE(FILEIO_TYPE,	0x900, METHOD_BUFFERED, FILE_ANY_ACCESS )
#define IOCTL_SemaSMB_READ_WRITE		CTL_CODE(FILEIO_TYPE,	0x901, METHOD_BUFFERED, FILE_ANY_ACCESS )

#define IOCTL_SemaEC_READ				CTL_CODE(FILEIO_TYPE,	0x906, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SemaEC_WRITE				CTL_CODE(FILEIO_TYPE,	0x907, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SemaEC_PORT_READ				CTL_CODE(FILEIO_TYPE,	0x913, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SemaEC_PORT_WRITE				CTL_CODE(FILEIO_TYPE,	0x914, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SemaEC_GET_FW_VERSION			CTL_CODE(FILEIO_TYPE,	0x915, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define DEV_PATH _T("\\\\?\\GLOBALROOT\\Device\\SEMA")

#endif
