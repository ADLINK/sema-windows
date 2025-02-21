#include "Driver.h"

#ifndef DRIVERINC
#define DRIVERINC

#define HST_STS_HBSY		0x01
#define HST_STS_DEVERR		0x04
#define HST_STS_BERR		0x08
#define HST_STS_FAILED		0x10
#define HST_STS_INUSE_STS	0x40

#define SMB_INUSE_POLL_RETRIES  100000  // Maximum polling retries
#define SMB_HSTS_INUSE          0x40    // "in use" status bit
#define SMB_INUSE_POLL_INTERVAL 20      // Polling interval in microseconds

#define SMB_HSTS_BUSY           0x01    // Host busy bit
#define SMB_HSTS_BYTEDONE       0x80    // Byte done status bit
#define SMB_HSTS_DEVERR         0x04    // Device error status bit
#define SMB_HSTS_BERR           0x08    // Bus error status bit
#define SMB_HSTS_FAILED         0x10    // Failed error status bit

#define SMB_HSTS_IsBusy(x)  ((x & SMB_HSTS_BUSY) == SMB_HSTS_BUSY)
#define SMB_HSTS_IsByteDone(x)  ((x & SMB_HSTS_BYTEDONE) == SMB_HSTS_BYTEDONE)
#define SMB_HSTS_IsError(x)     ((x & SMB_HSTS_ERRORS) != 0)
#define SMB_HSTS_ERRORS         (SMB_HSTS_DEVERR | SMB_HSTS_BERR | SMB_HSTS_FAILED)

typedef struct _INTEL_SMB_REGS_ {
	// Register name	// Register Offsets
	volatile UCHAR	HST_STS;			// 0x00
	volatile UCHAR	reserved_1;			// 0x01 - dummy value address/es not used!
	volatile UCHAR	HST_CNT;			// 0x02
	volatile UCHAR	HST_CMD;			// 0x03
	volatile UCHAR	XMIT_SLVA;			// 0x04
	volatile UCHAR	HST_D0;				// 0x05
	volatile UCHAR	HST_D1;				// 0x06
	volatile UCHAR	HOST_BLOCK_DB;		// 0x07 
	volatile UCHAR	PEC;				// 0x08
	volatile UCHAR	RCV_SLVA;			// 0x09
	volatile USHORT	SLV_DATA;			// 0x0A
	volatile UCHAR	AUX_STS;			// 0x0C
	volatile UCHAR	AUX_CTL;			// 0x0D
} INTEL_SMB_REGS, * PINTEL_SMB_REGS;

typedef enum
{
	TT_WB = 0x00,		// Write Byte
	TT_WBB,				// Write Byte(Cmd) Byte
	TT_WBW,				// Write Byte(Cmd) Word
	TT_WWB,				// Write Word(Cmd) Byte
	TT_RB,				// Read Byte	
	TT_RBB,				// Read Byte(Cmd) Byte
	TT_RBW,				// Read Byte(Cmd) Word
	TT_RWB,				// Read Word(Cmd) Byte
	TT_RBL,				// Read Block
	TT_WBL,				// Write Block

	TT_I2C_RBL,			// I2C Read Block
	TT_I2C_WBL			// I2C Write Block
}tTransType;

typedef enum
{
	HST_STS = 0x00,
	HST_CNT = 0x02,
	HST_CMD,
	XMIT_SLVA,
	HST_D0,
	HST_D1,
	HOST_BLOCK_DB,
	HST_PEC,
	AUX_CTL = 0x0D,
}EHST_STATE;

#define IsReadAdr(a)				(a&1)		///< Tests if address is read address
#define IsWriteAdr(a)			(!(a&1))	///< Tests if address is write address
#define TT_IsWriteAccess(Type)	(((Type==TT_WB) || (Type==TT_WBB) || (Type==TT_WBW) || (Type==TT_WWB) || (Type==TT_WBL)) ? 1:0)
#define TT_IsReadAccess(Type)	(((Type==TT_RB) || (Type==TT_RBB) || (Type==TT_RBW) || (Type==TT_RWB) || (Type==TT_RBL)) ? 1:0)

NTSTATUS SMBus_ICH_Byte_Trans(WDFDEVICE pDevice, PSMB_COMMAND pCmd, BYTE* pBufOut, UINT32 nDataOut, ULONG* pulRet);
NTSTATUS SMBus_ICH_Block_Trans(WDFDEVICE pDevice, UINT8 bAddress, BYTE bCommand, BYTE bLength, BYTE* pBuffer, ULONG* pulRet);
NTSTATUS SMBI2CTransfer(WDFDEVICE pDevice, PSMB_COMMAND pCmd, BYTE* Buffer, UINT32 nDataOut, ULONG* pulRet, WDFREQUEST request);

#endif