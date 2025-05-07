#include "stdafx.h"
#include  <string.h>
#include "EApi.h"
#include "SemaApp.h"


unsigned int GetValuesMap[41] = 
{   0,
	EAPI_ID_GET_EAPI_SPEC_VERSION,
	EAPI_ID_BOARD_BOOT_COUNTER_VAL,
	EAPI_ID_BOARD_RUNNING_TIME_METER_VAL,
	EAPI_ID_BOARD_LIB_VERSION_VAL,
	EAPI_ID_HWMON_CPU_TEMP,
	EAPI_ID_HWMON_SYSTEM_TEMP,
	EAPI_ID_HWMON_VOLTAGE_VCORE,
	EAPI_ID_HWMON_VOLTAGE_2V5,
	EAPI_ID_HWMON_VOLTAGE_3V3,
	EAPI_ID_HWMON_VOLTAGE_VBAT,
	EAPI_ID_HWMON_VOLTAGE_5V,
	EAPI_ID_HWMON_VOLTAGE_5VSB,
	EAPI_ID_HWMON_VOLTAGE_12V,
	EAPI_ID_HWMON_FAN_CPU,
	EAPI_ID_HWMON_FAN_SYSTEM,
	EAPI_SEMA_ID_BOARD_POWER_UP_TIME,
	EAPI_SEMA_ID_BOARD_RESTART_EVENT,
	EAPI_SEMA_ID_BOARD_CAPABILITIES,
	EAPI_SEMA_ID_BOARD_CAPABILITIES_EX,
	EAPI_SEMA_ID_BOARD_SYSTEM_MIN_TEMP,
	EAPI_SEMA_ID_BOARD_SYSTEM_MAX_TEMP,
	EAPI_SEMA_ID_BOARD_SYSTEM_STARTUP_TEMP,
	EAPI_SEMA_ID_BOARD_CPU_MIN_TEMP,
	EAPI_SEMA_ID_BOARD_CPU_MAX_TEMP,
	EAPI_SEMA_ID_BOARD_CPU_STARTUP_TEMP,
	EAPI_SEMA_ID_BOARD_MAIN_CURRENT,
	EAPI_SEMA_ID_HWMON_VOLTAGE_GFX_VCORE,
	EAPI_SEMA_ID_HWMON_VOLTAGE_1V05,
	EAPI_SEMA_ID_HWMON_VOLTAGE_1V5,
	EAPI_SEMA_ID_HWMON_VOLTAGE_VIN,
	EAPI_SEMA_ID_HWMON_FAN_SYSTEM_2,
	EAPI_SEMA_ID_HWMON_FAN_SYSTEM_3,
	EAPI_SEMA_ID_BOARD_2ND_SYSTEM_TEMP,
	EAPI_SEMA_ID_BOARD_2ND_SYSTEM_MIN_TEMP,
	EAPI_SEMA_ID_BOARD_2ND_SYSTEM_MAX_TEMP,
	EAPI_SEMA_ID_BOARD_2ND_SYSTEM_STARTUP_TEMP,
	EAPI_SEMA_ID_BOARD_POWER_CYCLE,
	EAPI_SEMA_ID_BOARD_BMC_FLAG,
	EAPI_SEMA_ID_BOARD_BMC_STATUS,
	EAPI_SEMA_ID_IO_CURRENT
};


unsigned GetStringMap[17] = {
	0,
	EAPI_ID_BOARD_MANUFACTURER_STR,
	EAPI_ID_BOARD_NAME_STR,
	EAPI_ID_BOARD_SERIAL_STR,
	EAPI_ID_BOARD_BIOS_REVISION_STR,
	EAPI_ID_BOARD_HW_REVISION_STR,
	EAPI_ID_BOARD_PLATFORM_TYPE_STR,
	EAPI_SEMA_ID_BOARD_BOOT_VERSION_STR,		
	EAPI_SEMA_ID_BOARD_APPLICATION_VERSION_STR,
	EAPI_SEMA_ID_BOARD_RESTART_EVENT_STR,
	EAPI_SEMA_ID_BOARD_REPAIR_DATE_STR,
	EAPI_SEMA_ID_BOARD_MANUFACTURE_DATE_STR,
	EAPI_SEMA_ID_BOARD_MAC_1_STRING,
	EAPI_SEMA_ID_BOARD_MAC_2_STRING,
	EAPI_SEMA_ID_BOARD_2ND_HW_REVISION_STR,
	EAPI_SEMA_ID_BOARD_2ND_SERIAL_STR,
	NULL
};

unsigned int string_to_hex(char *string)
{
	int data = 0;
	if (string != NULL)
	{
		sscanf_s(string, "%x", &data);
	}
	return data;
}

void ShowHelp(int condition)
{
	if (condition == 0)
	{
		printf("\nUsage:\n");
		printf("- Display this screen:\n");
		printf("	semautil /h\n");
		printf("\n- Version:\n");
		printf("	semautil version for more help related to SEMA version\n");
		
		printf("\n- System monitor-Board Info:\n");
		printf("	semautil /i    for more help related to system monitor-board information functions\n");
		printf("\n- Board values:\n");
		printf("	semautil /d    for more help related to board values functions\n");
		printf("\n- Watch Dog:\n");
		printf("	semautil /w    for more help related to watchdog functions\n");
		printf("\n- Storage:\n");
		printf("	semautil /s    for more help related to storage functions\n");
		printf("\n- Voltage monitor:\n");
		printf("	semautil /v    for more help related to voltage monitor functions\n");
		printf("\n- GPIO:\n");
		printf("	semautil /g    for more help related to GPIO functions\n");
		printf("\n- Generic I2C Read/Write:\n");
		printf("	semautil /i2c  for more help related to generic I2C read/write functions\n");
		printf("\n- Error log:\n");
		printf("	semautil /e    for more help related to error log functions\n");
		printf("\n- BIOS source:\n");
		printf("	semautil /src  for more help related to BIOS source functions\n");
		printf("\n- UUID (type 4):\n");
		printf("	semautil /c  for more help related to UUID (type4) functions\n");
		printf("\n- Smart FAN control:\n");
		printf("	semautil /f    for more help related to fan control functions\n");
		printf("\n- LVDS Backlight control:\n");
		printf("	semautil /b    for more help related to LVDS backlight control functions\n");
		printf("\n- Exception Description:\n");
		printf("	semautil /x    for more help related to exception description functions\n");

	}
	if (condition == 1)
	{
		printf("\n- Watch Dog:\n");
		printf("  1. semautil /w get_cap\n");
		printf("  2. semautil /w start [sec (1-65535)] \n");
		printf("  3. semautil /w trigger\n");
		printf("  4. semautil /w stop\n");
		printf("  5. semautil /w pwrup_enable [sec (60-65535)]\n");
		printf("  6. semautil /w pwrup_disable\n");
		printf("Note:	Start time will be different for the different platforms.\n");
		printf("        Please go to BIOS menu to check it\n");
	}
	if (condition == 2)
	{
		printf("\n- Storage:\n");
		printf("  1. semautil /s get_cap   [Region]\n");
		printf("  2. semautil /s read      [Region] [Address] [Length]\n");
		printf("  3. semautil /s write     [Region] [Address] [string/value]\n");
		printf("  4. semautil /s lock      [Region] \n");
		printf("  5. semautil /s unlock    [Region] [Permission] [Passcode]\n");
		printf("  6. semautil /s hex_write [Region] [Address] [value] \n");
		printf("  7. semautil /s hex_read  [Region] [Address] [Length] \n");
		printf("  8. semautil /s odm_write [ODM Id] [string/value]\n\n");

		printf("       Region:\n");
		printf("       \t1. User\n");
		printf("       \t2. Secured\n");
		printf("       \t3. ODM\n\n");

		printf("       Permission\n");
		printf("       \t1. Read-only\n");
		printf("       \t2. Read/Write\n\n");

		printf("       ODM Id:\n");
		printf("       \t1.Hardware Revision\n");
		printf("       \t2.Serial Number\n");
		printf("       \t3.Last Repair date\n");
		printf("       \t4.Manufacturing date\n");
		printf("       \t5.2nd Hardware Revision\n");
		printf("       \t6.2nd Serial Number\n");
		printf("       \t7.MAC Id\n");
		printf("       \t8.MAC Id 2\n\n");

		printf("     Example: semautil /s write 1020 Aaaa 4\n          It will be written to 1020, 1021, 1022, 1023\n\n");
		printf("     Unlock the ODM region to perform odm_write operation\n\n ");
		//printf("    Note: Hexa decimal values are not valid\n     Note: Locking of ODM region will only make ODM is read-only.\n          lock function will not protect User region.\n          read and write function will not work on ODM region\n");
		printf("     Note : hex_write operation should be provided as below\n	Example: semautil /s hex_write 1 128 aa bb c d \n");
	}
	if (condition == 3)
	{
		printf("\n-Smart FAN control:\n");
		printf("  1. semautil /f set_temp_points [FanID] [Level 1] [Level 2] [Level 3] [Level 4]\n");
		printf("  2. semautil /f set_pwm_points  [FanID] [PWM Level 1] [PWM Level 2] [PWM Level 3] [PWM Level 4]\n");
		printf("  3. semautil /f get_temp_points [FanID]\n");
		printf("  4. semautil /f get_pwm_points  [FanID]\n");
		printf("  5. semautil /f set_temp_source [FanID] [TempSrc]\n");
		printf("  6. semautil /f get_temp_source [FanID]\n");
		printf("  7. semautil /f get_mode        [FanID] \n");
		printf("  8. semautil /f set_mode        [FanID] [Mode]\n");
		printf("\n  TempSrc\n\t0 - CPU sensor\n\t1 - Board seensor\n");
		printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n");
		printf("  Mode\n\t0 : Auto\n\t1 : Off\n\t2 : On\n\t3 : Soft\n");
	}
	if (condition == 4)
	{
		printf("\n- System monitor-Board Info:\n");
		printf("  1. semautil /i get_bd_info [EApi ID] \n");
		printf("       1  : Board manufacturer name \n");
		printf("       2  : Board name \n");
		printf("       3  : Board serial number\n");
		printf("       4  : Board BIOS revision\n");
		printf("       5  : HW revision \n");
		printf("       6  : Board platform type\n");
		printf("       7  : BMC Bootloader revision\n");
		printf("       8  : BMC application revision\n");
		printf("       9  : Board restart event\n");
		printf("       10 : Board repair date\n");
		printf("       11 : Board manufacturer date\n");
		printf("       12 : Board MAC address 1\n");
		printf("       13 : Board MAC address 2\n");
		printf("       14 : Board 2nd HW revision number\n");
		printf("       15 : Board 2nd serial\n");
	}
	if (condition == 5)
	{
		printf("\n- Voltage monitor:\n");
		printf("  1. semautil /v get_voltage [Channel (0 - 15)] \n");
	}
	if (condition == 6)
	{
		printf("\n- Error log:\n");
		printf("  1. semautil /e get_error_log  [Position(0-31)]\n");
		printf("  2. semautil /e get_cur_error_log\n");
		printf("  3. semautil /e get_bmc_error_code [Error Number]\n");
	}
	if (condition == 7)
	{
		printf("\n- Exception Description :\n");
		printf("  1. semautil /x get_excep_desc\n");
	}
	if (condition == 8)
	{
		printf("\n- GPIO:\n");
		printf("  1. semautil /g get_direction_cap\n");
		printf("  2. semautil /g get_direction     [ID]  [GPIO Bit]\n");
		printf("  3. semautil /g set_direction     [ID]  [GPIO Bit] [0 - Output or 1 - Input]\n");
		printf("  4. semautil /g get_level         [ID]  [GPIO Bit]\n");
		printf("  5. semautil /g set_level         [ID]  [GPIO Bit] [0 - Low or 1 - High]\n");
		printf("\n  GPIO set/write parameters:\n");
		printf("     GPIO Bit   0-15   \n");
		printf("     Note: GPIO function is not available in all platform\n");
	}
	if (condition == 9)
	{
		printf("\n- Board values:\n");
		printf("  1. semautil /d  get_value [EAPI ID] \n");
		printf("       1	:  EAPI Specification Version \n");
		printf("       2	:  Boot Counter\n");
		printf("       3	:  Running Time Meter\n");
		printf("       4	:  Vendor Specific Library Version\n");
		printf("       5	:  CPU Temperature\n");
		printf("       6	:  System Temperature\n");
		printf("       7 	:  CPU Core Voltage\n");
		printf("       8	:  2.5V Voltage\n");
		printf("       9	:  3.3V Voltage\n");
		printf("       10	:  Battery Voltage\n");
		printf("       11	:  5V Voltage\n");
		printf("       12	:  5V Standby Voltage\n");
		printf("       13	:  12V Voltage\n");
		printf("       14	:  CPU Fan\n");
		printf("       15	:  System Fan 1\n");
		printf("       16	:  Get power uptime\n");
		printf("       17	:  Get restart event\n");
		printf("       18	:  Get BMC capabilities\n");
		printf("       19	:  Get extended BMC capabilities\n");
		printf("       20	:  Board Min Temperature\n");
		printf("       21	:  Board Max Temperature\n");
		printf("       22	:  Board Startup Temperature\n");
		printf("       23	:  CPU Min Temperature\n");
		printf("       24	:  CPU Max Temperature\n");
		printf("       25	:  CPU Startup Temperature\n");
		printf("       26	:  Get main power current\n");
		printf("       27	:  GFX Voltage\n");
		printf("       28	:  1.05V Voltage\n");
		printf("       29	:  1.5V Voltage\n");
		printf("       30	:  Vin Voltage\n");
		printf("       31	:  System Fan 2\n");
		printf("       32	:  System Fan 3\n");
		printf("       33	:  Board 2nd Current Temperature\n");
		printf("       34	:  Board 2nd Min temperature\n");
		printf("       35	:  Board 2nd Max Temperature\n");
		printf("       36	:  Board 2nd Startup Temperature\n");
		printf("       37	:  Get Power cycle counter\n");
		printf("       38	:  Get Board BMC Flag\n");
		printf("       39	:  Get Board BMC Status\n");
		printf("       40	:  IO current\n");
	}
	if (condition == 10)
	{
		printf("\n- LVDS Backlight control:\n");
		printf("  1. semautil /b  set_bkl_value   [ID] [Level (0-255)]\n");
		printf("  2. semautil /b  set_bkl_enable  [ID] [0 - Disable or 1 - Enable]\n");
		printf("  3. semautil /b  get_bkl_value   [ID]\n");
		printf("  4. semautil /b  get_bkl_enable  [ID]\n");
		printf("       [ID]       LCD\n");
		printf("        0    EAPI_ID_BACKLIGHT_1\n");
		printf("        1    EAPI_ID_BACKLIGHT_2\n");
		printf("        2    EAPI_ID_BACKLIGHT_3\n");
	}

	if (condition == 11)
	{
		printf("\n- Get BIOS Source:\n");
		printf("  1. semautil /src  get_src\n");
		printf("  2. semautil /src  set_src bit-0 bit-1\n");
		printf("  3. semautil /src  get_src_sts\n");
		
		printf("  \nBit-0 Bit-1:\n");
		printf("  0     0      -   By hardware configuration of currently selected BIOS\n");
		printf("  0     1      -   Switch to Fail-Safe BIOS\n");
		printf("  1     0      -   Switch to External BIOS (SPI0 on carrier)\n");
		printf("  1     1      -   Switch to Internal BIOS (SPI0 on module)\n");
	}

	if (condition == 12)
	{
		printf("\n- UUID:\n");
		printf("  1. semautil /c guid_generate_write\n");
		printf("  2. semautil /c guid_read\n");
	}
}

signed int ParseArgs(int argc, char* argv[], tCmdLineArgs *Args)
{
	int help_condition = 0;
	int eRet = 1;
	uint32_t BlockLength, StorageSize;

	if (argc == 1)
	{
		ShowHelp(help_condition);
		return -1;
	}

	if (_stricmp(argv[1], "/h") == 0)
	{
		ShowHelp(help_condition);
		return -1;
	}
	/* Watch Dog*/
	else if (_stricmp(argv[1], "/w") == 0)
	{
		if (argc == 3 && (_stricmp(argv[2], "get_cap") == 0))
		{
			Args->WatchDogCap = TRUE;
		}
		else if (argc == 4 && _stricmp(argv[2], "start") == 0)
		{
			Args->SetWatchdog = TRUE;
			Args->WatchDogFuncArgs.WatchdogTimeOut = atoi(argv[3]);
		}
		else if (argc == 3 && (_stricmp(argv[2], "trigger") == 0))
		{
			Args->TriggerWatchdog = TRUE;
		}
		else if (argc == 3 && (_stricmp(argv[2], "stop") == 0))
		{
			Args->StopWatchdog = TRUE;
		}
		else if (argc > 3 && (_stricmp(argv[2], "pwrup_enable") == 0))
		{
			Args->IsPwrUpWDogStart = TRUE;
			Args->WatchDogFuncArgs.WatchdogTimeOut = atoi(argv[3]);
			if (argc == 4 && Args->WatchDogFuncArgs.WatchdogTimeOut < 60 && Args->WatchDogFuncArgs.WatchdogTimeOut > 0)
			{
				printf("\n\nNote: The allowed range for Powerup Watchdog timer is 60-65535 seconds\n");
				return -1;
			}

		}
		else if (argc == 3 && (_stricmp(argv[2], "pwrup_disable") == 0))
		{
			Args->IsPwrUpWDogStop = TRUE;
		}
		else
		{
			help_condition = 1;
		}
	}
	else if (_stricmp(argv[1], "/s") == 0)
	{
		if (argc == 4  && (_stricmp(argv[2], "get_cap") == 0))
		{
			int i;

			Args->StorageCap = TRUE;
			Args->StorageFuncArgs.Id = atoi(argv[3]);

			for (i = 0; argv[3][i]; i++)
			{
				if (!isdigit(argv[3][i]))
				{
					Args->StorageCap = FALSE;
					help_condition = 2;
				}
			}

			if (Args->StorageFuncArgs.Id < 1 || Args->StorageFuncArgs.Id > 3)
			{
				Args->StorageCap = FALSE;
				help_condition = 2;
			}
		}
		else if (argc == 6 && (_stricmp(argv[2], "read") == 0))
		{
			int i;
			uint32_t Status;
			Args->StorageAreaRead = TRUE;
			Args->StorageFuncArgs.Id = atoi(argv[3]);
			for (i = 0; argv[3][i]; i++)
			{
				if (!isdigit(argv[3][i]))
				{
					Args->StorageAreaRead = FALSE;
					help_condition = 2;
				}
			}
			if (Args->StorageFuncArgs.Id < 1 || Args->StorageFuncArgs.Id > 3)
			{
				Args->StorageAreaRead = FALSE;
				help_condition = 2;
			}
			Args->StorageFuncArgs.Address = atoi(argv[4]);
			for (i = 0; argv[4][i]; i++)
			{
				if (!isdigit(argv[4][i]))
				{
					Args->StorageAreaRead = FALSE;
					help_condition = 2;
				}
			}
			Args->StorageFuncArgs.Length = atoi(argv[5]);
			for (i = 0; argv[5][i]; i++)
			{
				if (!isdigit(argv[5][i]))
				{
					Args->StorageAreaRead = FALSE;
					help_condition = 2;
				}
			}
			if (help_condition != 2)
			{
				/*
				if (Args->StorageFuncArgs.Id == EAPI_ID_STORAGE_ODM)
				{
					Args->StorageAreaRead = FALSE;
					printf("\nRead/Write function will not work in ODM region\n");
					eRet = -3;
				}
				*/
			}
			
			if ((Status = EApiStorageCap(Args->StorageFuncArgs.Id, &StorageSize, &BlockLength)) == EAPI_STATUS_SUCCESS)
			{
				if (Args->StorageFuncArgs.Address > StorageSize || Args->StorageFuncArgs.Address < 0)
				{
					Args->StorageAreaRead = FALSE;
					help_condition = 2;
				}
			}

		}
		else if (argc == 6 && (_stricmp(argv[2], "write") == 0))
		{
			int i;
			Args->StorageAreaWrite = TRUE;
			Args->StorageFuncArgs.Id = atoi(argv[3]);
			for (i = 0; argv[3][i]; i++)
			{
				if (!isdigit(argv[3][i]))
				{
					Args->StorageAreaWrite = FALSE;
					help_condition = 2;
				}
			}
			if (Args->StorageFuncArgs.Id < 1 || Args->StorageFuncArgs.Id > 3)
			{
				Args->StorageAreaWrite = FALSE;
				help_condition = 2;
			}

			Args->StorageFuncArgs.Address = atoi(argv[4]);
			for (i = 0; argv[4][i]; i++)
			{
				if (!isdigit(argv[4][i]))
				{
					Args->StorageAreaWrite = FALSE;
					help_condition = 2;
				}
			}

			Args->StorageFuncArgs.String = argv[5];

			if (help_condition != 2)
			{
				/*
				if (Args->StorageFuncArgs.Id == EAPI_ID_STORAGE_ODM)
				{
					Args->StorageAreaWrite = FALSE;
					printf("\n\tRead/Write function will not work in ODM region\n");
					eRet = -3;
				}
				*/

				if (EApiStorageCap(Args->StorageFuncArgs.Id, &StorageSize, &BlockLength) == EAPI_STATUS_SUCCESS)
				{
					if (Args->StorageFuncArgs.Address > StorageSize || Args->StorageFuncArgs.Address < 0)
					{
						Args->StorageAreaWrite = FALSE;
						help_condition = 2;
					}
				}
			}
		}
		else if (argc == 4 && (_stricmp(argv[2], "lock") == 0))
		{
			int i;
			Args->StorageLock = TRUE;
			Args->StorageFuncArgs.Id = atoi(argv[3]);
			for (i = 0; argv[3][i]; i++)
			{
				if (!isdigit(argv[3][i]))
				{
					Args->StorageLock = FALSE;
					help_condition = 2;
				}
			}
			if (Args->StorageFuncArgs.Id < 1 || Args->StorageFuncArgs.Id > 3)
			{
				Args->StorageLock = FALSE;
				help_condition = 2;
			}
		}
		else if (argc == 6 && (_stricmp(argv[2], "unlock") == 0))
		{
			int i;
			Args->StorageUnlock = TRUE;
			Args->StorageFuncArgs.Id = atoi(argv[3]);
			for (i = 0; argv[3][i]; i++)
			{
				if (!isdigit(argv[3][i]))
				{
					Args->StorageUnlock = FALSE;
					help_condition = 2;
				}
			}
			if (Args->StorageFuncArgs.Id < 1 || Args->StorageFuncArgs.Id > 3)
			{
				Args->StorageUnlock = FALSE;
				help_condition = 2;
			}
			Args->StorageFuncArgs.Permission = atoi(argv[4]);
			for (i = 0; argv[4][i]; i++)
			{
				if (!isdigit(argv[4][i]))
				{
					Args->StorageUnlock = FALSE;
					help_condition = 2;
				}
			}

			strcpy_s(Args->StorageFuncArgs.Password, sizeof(Args->StorageFuncArgs.Password), argv[5]);
		}
		else if (argc >= 6 && (_stricmp(argv[2], "hex_read") == 0))
		{
		int i;
		uint32_t Status;
		Args->StorageHexRead = TRUE;
		Args->StorageFuncArgs.Id = atoi(argv[3]);
		for (i = 0; argv[3][i]; i++)
		{
			if (!isdigit(argv[3][i]))
			{
				Args->StorageHexRead = FALSE;
				help_condition = 2;
			}
		}
		if (Args->StorageFuncArgs.Id < 1 || Args->StorageFuncArgs.Id > 3)
		{
			Args->StorageHexRead = FALSE;
			help_condition = 2;
		}
		Args->StorageFuncArgs.Address = atoi(argv[4]);
		for (i = 0; argv[4][i]; i++)
		{
			if (!isdigit(argv[4][i]))
			{
				Args->StorageHexRead = FALSE;
				help_condition = 2;
			}
		}
		Args->StorageFuncArgs.Length = atoi(argv[5]);
		for (i = 0; argv[5][i]; i++)
		{
			if (!isdigit(argv[5][i]))
			{
				Args->StorageHexRead = FALSE;
				help_condition = 2;
			}
		}
		if ((Status = EApiStorageCap(Args->StorageFuncArgs.Id, &StorageSize, &BlockLength)) == EAPI_STATUS_SUCCESS)
		{
			if (Args->StorageFuncArgs.Address > StorageSize || Args->StorageFuncArgs.Address < 0)
			{
				Args->StorageHexRead = FALSE;
				help_condition = 2;
			}
		}
		}
		else if (argc >= 6 && (_stricmp(argv[2], "hex_write") == 0))
		{
		int i, j = 0;
		Args->StorageHexWrite = TRUE;
		Args->StorageFuncArgs.Id = atoi(argv[3]);
		for (i = 0; argv[3][i]; i++)
		{
			if (!isdigit(argv[3][i]))
			{
				Args->StorageHexWrite = FALSE;
				help_condition = 2;
			}
		}
		if (Args->StorageFuncArgs.Id < 1 || Args->StorageFuncArgs.Id > 3)
		{
			Args->StorageHexWrite = FALSE;
			help_condition = 2;
		}

		Args->StorageFuncArgs.Address = atoi(argv[4]);
		for (i = 0; argv[4][i]; i++)
		{
			if (!isdigit(argv[4][i]))
			{
				Args->StorageHexWrite = FALSE;
				help_condition = 2;
			}
		}

		char buffer[2048];
		for (i = 5; i < argc; i++)
		{
			if (strlen(argv[i]) == 1)
			{
				buffer[j] = '0';
				j++;
				buffer[j] = argv[i][0];
			}
			else
			{
				buffer[j] = argv[i][0];
				buffer[++j] = argv[i][1];
			}
			j++;
		}
		buffer[j] = '\0';

		Args->StorageFuncArgs.String = buffer;

		if (EApiStorageCap(Args->StorageFuncArgs.Id, &StorageSize, &BlockLength) == EAPI_STATUS_SUCCESS)
		{
			if (Args->StorageFuncArgs.Address > StorageSize || Args->StorageFuncArgs.Address < 0)
			{
				Args->StorageHexWrite = FALSE;
				help_condition = 2;
			}
		}
		}
		else if (argc == 5 && (_stricmp(argv[2], "odm_write") == 0))
		{
		int i;
		Args->ODM_Write = TRUE;
		Args->StorageFuncArgs.Id = 3;
		int ODM_Id = atoi(argv[3]);
		for (i = 0; argv[3][i]; i++)
		{
			if (!isdigit(argv[3][i]))
			{
				Args->ODM_Write = FALSE;
				help_condition = 2;
			}
		}
		switch (ODM_Id)
		{
		case 1:
			Args->StorageFuncArgs.Address = 0x10;
			break;
		case 2:
			Args->StorageFuncArgs.Address = 0x20;
			break;
		case 3:
			Args->StorageFuncArgs.Address = 0x30;
			break;
		case 4:
			Args->StorageFuncArgs.Address = 0x40;
			break;
		case 5:
			Args->StorageFuncArgs.Address = 0x50;
			break;
		case 6:
			Args->StorageFuncArgs.Address = 0x60;
			break;
		case 7:
			Args->StorageFuncArgs.Address = 0x70;
			break;
		case 8:
			Args->StorageFuncArgs.Address = 0x80;
			break;
		default:
			printf("\nInvalid ODM_Id");
			break;
		}
		Args->StorageFuncArgs.Length = strlen(argv[4]);
		if (Args->StorageFuncArgs.Length > 16) {
			printf("\nString should not exceed 16 characters");
			help_condition = 2;
		}
		Args->StorageFuncArgs.String = argv[4];
		if (help_condition != 2)
		{
			if (EApiStorageCap(Args->StorageFuncArgs.Id, &StorageSize, &BlockLength) == EAPI_STATUS_SUCCESS)
			{
				if (Args->StorageFuncArgs.Address > StorageSize || Args->StorageFuncArgs.Address < 0)
				{
					Args->ODM_Write = FALSE;
					help_condition = 2;
				}
			}
		}
		}
		else
		{
			help_condition = 2;
		}
	}
	else if (_stricmp(argv[1], "/f") == 0)
	{
		if (argc > 2 && (_stricmp(argv[2], "set_temp_points") == 0))
		{
			if (argc == 8)
			{
				int i, j, k = 0;
				Args->SmartFanTempSet = TRUE;
				Args->SmartFaneFuncArgs.FanID = atoi(argv[3]);

				for (j = 3; j < 8; j++)
				{
					for (i = 0; argv[j][i]; i++)
					{
						if (!isdigit(argv[j][i]))
						{
							Args->SmartFanTempSet = FALSE;
							printf("\nsemautil / f set_temp_points [FanID] [Level 1] [Level 2] [Level 3] [Level 4]\n");
							printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n\n");
							eRet = -3;
							k = 1;
							j = 9;
							break;
						}
					}
				}

				if (k != 1)
				{
					Args->SmartFaneFuncArgs.Level[0] = atoi(argv[4]);
					Args->SmartFaneFuncArgs.Level[1] = atoi(argv[5]);
					Args->SmartFaneFuncArgs.Level[2] = atoi(argv[6]);
					Args->SmartFaneFuncArgs.Level[3] = atoi(argv[7]);

					if (Args->SmartFaneFuncArgs.FanID > 3 || Args->SmartFaneFuncArgs.FanID < 0 || Args->SmartFaneFuncArgs.Level[0] > 100 || Args->SmartFaneFuncArgs.Level[0] < 0 ||
						Args->SmartFaneFuncArgs.Level[1] > 100 || Args->SmartFaneFuncArgs.Level[1] < 0 ||
						Args->SmartFaneFuncArgs.Level[2] > 100 || Args->SmartFaneFuncArgs.Level[2] < 0 ||
						Args->SmartFaneFuncArgs.Level[3] > 100 || Args->SmartFaneFuncArgs.Level[3] < 0)
					{
						Args->SmartFanTempSet = FALSE;
						printf("\nsemautil / f set_temp_points [FanID] [Level 1] [Level 2] [Level 3] [Level 4]\n");
						printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n\n");
						eRet = -3;
					}
				}
			}
			else
			{
				printf("\nsemautil / f set_temp_points [FanID] [Level 1] [Level 2] [Level 3] [Level 4]\n");
				printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n\n");
				eRet = -3;
			}
		}
		else if (argc > 2 && (_stricmp(argv[2], "set_pwm_points") == 0))
		{
			if (argc == 8)
			{

				int i, j, k = 0;
				Args->SmartFanPWMSet = TRUE;

				for (j = 3; j < 8; j++)
				{
					for (i = 0; argv[j][i]; i++)
					{
						if (!isdigit(argv[j][i]))
						{
							Args->SmartFanPWMSet = FALSE;
							printf("\nsemautil /f set_pwm_points  [FanID] [PWM Level 1] [PWM Level 2] [PWM Level 3] [PWM Level 4]\n");
							printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n");
							eRet = -3;
							k = 1; j = 9;
							break;
						}
					}
				}
				if (k != 1)
				{
					Args->SmartFaneFuncArgs.FanID = atoi(argv[3]);
					Args->SmartFaneFuncArgs.Level[0] = atoi(argv[4]);
					Args->SmartFaneFuncArgs.Level[1] = atoi(argv[5]);
					Args->SmartFaneFuncArgs.Level[2] = atoi(argv[6]);
					Args->SmartFaneFuncArgs.Level[3] = atoi(argv[7]);
					if (Args->SmartFaneFuncArgs.FanID > 3 || Args->SmartFaneFuncArgs.FanID < 0 || Args->SmartFaneFuncArgs.Level[0] > 100 || Args->SmartFaneFuncArgs.Level[0] < 0 ||
						Args->SmartFaneFuncArgs.Level[1] > 100 || Args->SmartFaneFuncArgs.Level[1] < 0 ||
						Args->SmartFaneFuncArgs.Level[2] > 100 || Args->SmartFaneFuncArgs.Level[2] < 0 ||
						Args->SmartFaneFuncArgs.Level[3] > 100 || Args->SmartFaneFuncArgs.Level[3] < 0)
					{
						Args->SmartFanPWMSet = FALSE;
						printf("\nsemautil /f set_pwm_points  [FanID] [PWM Level 1] [PWM Level 2] [PWM Level 3] [PWM Level 4]\n");
						printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n");
						eRet = -3;
					}
				}
			}
			else
			{
				Args->SmartFanPWMSet = FALSE;
				printf("\nsemautil /f set_pwm_points  [FanID] [PWM Level 1] [PWM Level 2] [PWM Level 3] [PWM Level 4]\n");
				printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n");
				eRet = -3;
			}
		}
		else if (argc > 2 && (_stricmp(argv[2], "get_temp_points") == 0))
		{
			if (argc == 4)
			{
				int i, k = 0;
				Args->SmartFanTempGet = TRUE;
				Args->SmartFaneFuncArgs.FanID = atoi(argv[3]);

				for (i = 0; argv[3][i]; i++)
				{
					if (!isdigit(argv[3][i]))
					{
						Args->SmartFanTempGet = FALSE;
						printf("\nsemautil /f get_temp_points [FanID]\n");
						printf("  FanID\n\t 0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n");
						eRet = -3;
						k = 1;
						break;
					}
				}

				if (k == 0 && (Args->SmartFaneFuncArgs.FanID > 3 || Args->SmartFaneFuncArgs.FanID < 0))
				{
					Args->SmartFanTempGet = FALSE;
					printf("\nsemautil /f get_temp_points [FanID]\n");
					printf("  FanID\n\t 0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n");
					eRet = -3;
				}
			}
			else
			{
				Args->SmartFanTempGet = FALSE;
				printf("\nsemautil /f get_temp_points [FanID]\n");
				printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n");
				eRet = -3;
			}
		}
		else if (argc > 2 && (_stricmp(argv[2], "get_pwm_points") == 0))
		{
			if (argc == 4)
			{
				int i, k = 0;
				Args->SmartFanPWMGet = TRUE;
				Args->SmartFaneFuncArgs.FanID = atoi(argv[3]);

				for (i = 0; argv[3][i]; i++)
				{
					if (!isdigit(argv[3][i]))
					{
						Args->SmartFanPWMGet = FALSE;
						printf("\nsemautil /f get_pwm_points  [FanID]\n");
						printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n");
						eRet = -3;
						k = 1;
						break;
					}
				}

				if (k != 1 && (Args->SmartFaneFuncArgs.FanID > 3 || Args->SmartFaneFuncArgs.FanID < 0))
				{
					Args->SmartFanPWMGet = FALSE;
					printf("\nsemautil /f get_pwm_points  [FanID]\n");
					printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n");
					eRet = -3;
				}
			}
			else
			{
				Args->SmartFanPWMGet = FALSE;
				printf("\nsemautil /f get_pwm_points  [FanID]\n");
				printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n\n");
				eRet = -3;
			}
		}
		else if (argc > 2 && (_stricmp(argv[2], "get_temp_source") == 0))
		{
			if (argc == 4)
			{
				int i, k = 0;
				Args->SmartFanTempGetSrc = TRUE;
				Args->SmartFaneFuncArgs.FanID = atoi(argv[3]);

				for (i = 0; argv[3][i]; i++)
				{
					if (!isdigit(argv[3][i]))
					{
						Args->SmartFanTempGetSrc = FALSE;
						printf("\nsemautil /f get_temp_source [FanID]\n");
						printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n\n");
						printf("  TempSrc\n\t0 - CPU sensor\n\t1 - Board seensor\n");
						eRet = -3;
						k = 1;
						break;
					}
				}

				if (k == 0 && (Args->SmartFaneFuncArgs.FanID > 3 || Args->SmartFaneFuncArgs.FanID < 0))
				{
					Args->SmartFanTempGetSrc = FALSE;
					printf("\nsemautil /f get_temp_source [FanID]\n");
					printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n\n");
					printf("  TempSrc\n\t0 - CPU sensor\n\t1 - Board seensor\n");
					eRet = -3;
				}
			}
			else
			{
				Args->SmartFanTempGetSrc = FALSE;
				printf("\nsemautil /f get_temp_source [FanID]\n");
				printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n\n");
				printf("  TempSrc\n\t0 - CPU sensor\n\t1 - Board seensor\n");
				eRet = -3;
			}
		}
		else if (argc > 2 && (_stricmp(argv[2], "set_temp_source") == 0))
		{
			int i, k = 0;
			if (argc == 5)
			{
				Args->SmartFanTempSetSrc = TRUE;
				Args->SmartFaneFuncArgs.FanID = atoi(argv[3]);
				Args->SmartFaneFuncArgs.TempSrc = atoi(argv[4]);

				for (i = 0; argv[3][i] && argv[4][i]; i++)
				{
					if (!isdigit(argv[3][i]) || !isdigit(argv[4][i]))
					{
						Args->SmartFanTempGetSrc = FALSE;
						printf("\nsemautil /f set_temp_source [FanID] [TempSrc]\n");
						printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n");
						printf("  TempSrc\n\t0 - CPU sensor\n\t1 - Board seensor\n");
						eRet = -3;
						k = 1;
						break;
					}
				}
				
				if (k == 0 && (Args->SmartFaneFuncArgs.FanID > 3 || Args->SmartFaneFuncArgs.FanID < 0\
					|| Args->SmartFaneFuncArgs.TempSrc < 0 || Args->SmartFaneFuncArgs.TempSrc > 1))
				{
					Args->SmartFanTempGetSrc = FALSE;
					printf("\nsemautil /f set_temp_source [FanID] [TempSrc]\n");
					printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n");
					printf("  TempSrc\n\t0 - CPU sensor\n\t1 - Board seensor\n");
					eRet = -3;
				}
			}
			else
			{
				Args->SmartFanTempGetSrc = FALSE;
				printf("\nsemautil /f set_temp_source [FanID] [TempSrc]\n");
				printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n");
				printf("  TempSrc\n\t0 - CPU sensor\n\t1 - Board seensor\n");
				eRet = -3;
			}
		}
		else if (argc > 2 && (_stricmp(argv[2], "get_mode") == 0))
		{
			if (argc == 4)
			{
				int i, k = 0;
				Args->SmartFanModeGet = TRUE;
				Args->SmartFaneFuncArgs.FanID = atoi(argv[3]);

				for (i = 0; argv[3][i]; i++)
				{
					if (!isdigit(argv[3][i]))
					{
						Args->SmartFanTempGetSrc = FALSE;
						printf("\nsemautil /f get_mode  [FanID] \n");
						printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n");
						printf("  Mode\n\t0 : Auto\n\t1 : Off\n\t2 : On\n\t3 : Soft\n");
						eRet = -3;
						k = 1;
						break;
					}
				}

				if (k == 0 && (Args->SmartFaneFuncArgs.FanID > 3 || Args->SmartFaneFuncArgs.FanID < 0))
				{
					Args->SmartFanTempGetSrc = FALSE;
					printf("\nsemautil /f get_mode  [FanID] \n");
					printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n");
					printf("  Mode\n\t0 : Auto\n\t1 : Off\n\t2 : On\n\t3 : Soft\n");
					eRet = -3;
				}
			}
			else
			{
				Args->SmartFanTempGetSrc = FALSE;
				printf("\nsemautil /f get_mode  [FanID] \n");
				printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n\n");
				eRet = -3;
			}
		}
		else if (argc > 2 && _stricmp(argv[2], "set_mode") == 0)
		{
			int k = 0, i;
			if (argc == 5)
			{
				Args->SmartFanModeSet = TRUE;

				for (i = 0; argv[3][i] && argv[4][i]; i++)
				{
					if (!isdigit(argv[3][i]) || !isdigit(argv[4][i]))
					{
						Args->SmartFanTempGetSrc = FALSE;
						printf("\nsemautil /f set_mode  [FanID] [Mode]\n");
						printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n\n");
						printf("  Mode\n\t0 : Auto\n\t1 : Off\n\t2 : On\n\t3 : Soft\n");
						eRet = -3;
						k = 1;
						break;
					}
				}

				if (k != 1)
				{
					Args->SmartFaneFuncArgs.FanID = atoi(argv[3]);
					Args->SmartFaneFuncArgs.FanMode = atoi(argv[4]);
					if (Args->SmartFaneFuncArgs.FanID > 3 || Args->SmartFaneFuncArgs.FanID < 0 || Args->SmartFaneFuncArgs.FanMode < 0 || Args->SmartFaneFuncArgs.FanMode > 3)
					{
						Args->SmartFanTempGetSrc = FALSE;
						printf("\nsemautil /f set_mode  [FanID] [Mode]\n");
						printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n\n");
						printf("  Mode\n\t0 : Auto\n\t1 : Off\n\t2 : On\n\t3 : Soft\n");
						eRet = -3;
					}
				}
			}
			else
			{
				Args->SmartFanTempGetSrc = FALSE;
				printf("\nsemautil /f set_mode  [FanID] [Mode]\n");
				printf("  FanID\n\t0 : CPU fan\n\t1 : System fan 1\n\t2 : System fan 2\n\t3 : System fan 3\n\n");
				printf("  Mode\n\t0 : Auto\n\t1 : Off\n\t2 : On\n\t3 : Soft\n");
				eRet = -3;
			}
		}
		else
		{
			help_condition = 3;
		}
	}
	else if (_stricmp(argv[1], "/i") == 0)
	{
		if (argc == 4 && (_stricmp(argv[2], "get_bd_info") == 0))
		{
			Args->GetStringA = TRUE;
			Args->SemaNativeFuncArgs.cap = atoi(argv[3]);
			if (Args->SemaNativeFuncArgs.cap < 1 || Args->SemaNativeFuncArgs.cap > 15)
			{
				Args->GetStringA = FALSE;
				help_condition = 4;
			}
			else
			{
				Args->SemaNativeFuncArgs.cap = GetStringMap[Args->SemaNativeFuncArgs.cap];
			}
		}
		else
		{
			help_condition = 4;
		}
	}
	else if (_stricmp(argv[1], "/v") == 0)
	{
		if (argc == 4 && (_stricmp(argv[2], "get_voltage") == 0))
		{
			Args->GetVoltageMonitor = TRUE;
			Args->SemaNativeFuncArgs.channel = atoi(argv[3]);
			if (Args->SemaNativeFuncArgs.channel < 0 || Args->SemaNativeFuncArgs.channel > 15)
			{
				help_condition = 5;
			}
		}
		else
		{
			help_condition = 5;
		}
	}
	else if (_stricmp(argv[1], "/e") == 0)
	{
		if (argc == 4 && (_stricmp(argv[2], "get_error_log") == 0))
		{
			Args->GetErrorLog = TRUE;
			Args->ErrorNExceFuncArgs.Pos = atoi(argv[3]);
		}
		else if (argc == 3 && (_stricmp(argv[2], "get_cur_error_log") == 0))
		{
			Args->GetCurrentPosErrorLog = TRUE;
		}
		else if (argc == 4 && (_stricmp(argv[2], "get_bmc_error_code") == 0))
		{
			Args->GetErrorNumberDescription = TRUE;
			Args->ErrorNExceFuncArgs.ErrorNumber = atoi(argv[3]);
		}
		else
		{
			help_condition = 6;
		}
	}
	else if (_stricmp(argv[1], "/x") == 0)
	{
		if (argc == 3 && (_stricmp(argv[2], "get_excep_desc") == 0))
		{
			Args->GetExceptionDescription = TRUE;
		}
		else
		{
			help_condition = 7;
		}
	}
	else if (_stricmp(argv[1], "/g") == 0)
	{
		if (argc == 3 && (_stricmp(argv[2], "get_direction_cap") == 0))
		{
			Args->GPIOGetDirectionCaps = TRUE;
		}
		else if (argc == 5 && (_stricmp(argv[2], "get_direction") == 0))
		{
			Args->GPIOGetDirection = TRUE;
			Args->GPIOFuncArgs.GPIOID = atoi(argv[3]);
			Args->GPIOFuncArgs.GPIOPin = atoi(argv[4]);
		}
		else if (argc == 6 && (_stricmp(argv[2], "set_direction") == 0))
		{
			Args->GPIOSetDirection = TRUE;
			Args->GPIOFuncArgs.GPIOID = atoi(argv[3]);
			Args->GPIOFuncArgs.GPIOPin = atoi(argv[4]);
			Args->GPIOFuncArgs.Dir = atoi(argv[5]);
			if (Args->GPIOFuncArgs.Dir != 0 && Args->GPIOFuncArgs.Dir != 1)
			{
				Args->GPIOSetDirection = FALSE;
				help_condition = 8;
			}
		}
		else if (argc == 5 && (_stricmp(argv[2], "get_level") == 0))
		{
			Args->GPIOGetLevel = TRUE;
			Args->GPIOFuncArgs.GPIOID = atoi(argv[3]);
			Args->GPIOFuncArgs.GPIOPin = atoi(argv[4]);
		}
		else if (argc == 6 && (_stricmp(argv[2], "set_level") == 0))
		{
			Args->GPIOSetLevel = TRUE;
			Args->GPIOFuncArgs.GPIOID = atoi(argv[3]);
			Args->GPIOFuncArgs.GPIOPin = atoi(argv[4]);
			Args->GPIOFuncArgs.Level = atoi(argv[5]);
			if (Args->GPIOFuncArgs.Level != 0 && Args->GPIOFuncArgs.Level != 1)
			{
				Args->GPIOSetLevel = FALSE;
				help_condition = 8;
			}
		}
		else
		{
			help_condition = 8;
		}
	}
	else if (_stricmp(argv[1], "/d") == 0)
	{
		if (argc == 4 && (_stricmp(argv[2], "get_value") == 0))
		{
			Args->GetValue = TRUE;
			Args->SemaNativeFuncArgs.Id = atoi(argv[3]);
			Args->SemaNativeFuncArgs.inSize = 20;

			if (Args->SemaNativeFuncArgs.Id < 1 || Args->SemaNativeFuncArgs.Id > 40)
			{
				Args->GetValue = FALSE;
				help_condition = 9;
			}
			else
			{
				Args->SemaNativeFuncArgs.Id = GetValuesMap[Args->SemaNativeFuncArgs.Id];
			}
		}
		else
		{
			help_condition = 9;
		}
	}
	else if (_stricmp(argv[1], "/b") == 0)
	{
		if (argc == 4 && (_stricmp(argv[2], "get_bkl_enable") == 0))
		{
			Args->VgaGetBacklightEnable = TRUE;
			Args->backlightFuncArgs.Id = atoi(argv[3]);
		}
		else if (argc == 5 && (_stricmp(argv[2], "set_bkl_enable") == 0))
		{
			Args->VgaSetBacklightEnable = TRUE;
			Args->backlightFuncArgs.Id = atoi(argv[3]);
			Args->backlightFuncArgs.EnnDI = atoi(argv[4]);
			if (Args->backlightFuncArgs.EnnDI != 0 && Args->backlightFuncArgs.EnnDI != 1)
			{
				Args->VgaSetBacklightEnable = FALSE;
				help_condition = 10;
			}
		}
		else if (argc == 4 && (_stricmp(argv[2], "get_bkl_value") == 0))
		{
			Args->VgaGetBacklightBrightness = TRUE;
			Args->backlightFuncArgs.Id = atoi(argv[3]);
		}
		else if (argc == 5 && (_stricmp(argv[2], "set_bkl_value") == 0))
		{
			Args->VgaSetBacklightBrightness = TRUE;
			Args->backlightFuncArgs.Id = atoi(argv[3]);
			Args->backlightFuncArgs.level = atoi(argv[4]);
			if (Args->backlightFuncArgs.level > 255 || Args->backlightFuncArgs.level < 0)
			{
				Args->VgaSetBacklightBrightness = FALSE;
				help_condition = 10;
			}
		}
		else
		{
			help_condition = 10;
		}
	}

	else if (_stricmp(argv[1], "/src") == 0)
	{
		if (argc == 3 && (_stricmp(argv[2], "get_src") == 0))
		{
			Args->GetBIOSSource = TRUE;
		}
		else if (argc == 3 && (_stricmp(argv[2], "get_src_sts") == 0))
		{
			Args->GetBIOSSourceSts = TRUE;
		}
		else if (argc == 5 && (_stricmp(argv[2], "set_src") == 0))
		{
			Args->SetBIOSSource = TRUE;
			Args->BIOSSourceArgs.bit0 = atoi(argv[4]);
			Args->BIOSSourceArgs.bit1 = atoi(argv[3]);
			if ((Args->BIOSSourceArgs.bit0 | Args->BIOSSourceArgs.bit1) > 1)
			{
				Args->SetBIOSSource = FALSE;
				help_condition = 11;
			}
		}
		else
		{
			help_condition = 11;
		}
	}
	else if (_stricmp(argv[1], "/i2c") == 0)
	{
		if (argc == 3 && (_stricmp(argv[2], "bus_cap") == 0))
		{
			Args->IsI2CCap = TRUE;
		}
		else if (argc > 2 && (_stricmp(argv[2], "probe_device") == 0))
		{
			if (argc == 4)
			{
				Args->IsI2CProb = TRUE;
				Args->I2CFuncArgs.BusID = atoi(argv[3]);

				if (Args->I2CFuncArgs.BusID < 1 || Args->I2CFuncArgs.BusID > 4)
				{
					printf("Invalid BusID\n");
					printf("  \n[Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
					printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");
					eRet = -3;
				}

			}
			else
			{
				printf("\nUsage :\n");
				printf("  semautil /i2c  probe_device [bus id]\n");

				printf("\n  [Bus Id]:\n");
				printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
				printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
				printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
				printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
				printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");
				eRet = -3;
			}
		}
		else if ((argc > 2) && (_stricmp(argv[2], "write_raw") == 0))
		{
			if (argc >= 7)
			{
				unsigned int i;
				Args->IsI2CWrRaw = TRUE;
				Args->I2CFuncArgs.BusID = atoi(argv[3]);
				Args->I2CFuncArgs.Address = string_to_hex(argv[4]);
				Args->I2CFuncArgs.WByteCnt = atoi(argv[5]);
				Args->I2CFuncArgs.pWrBuffer = calloc(Args->I2CFuncArgs.nByteCnt, sizeof(unsigned char));
				if (Args->I2CFuncArgs.pWrBuffer == NULL)
				{
					return EAPI_STATUS_ERROR;
				}
				if (Args->I2CFuncArgs.BusID < 1 || Args->I2CFuncArgs.BusID > 4)
				{
					printf("Invalid BusID\n");
					printf("  \n[Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
					printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");
					eRet = -3;
				}

				if (argc != Args->I2CFuncArgs.WByteCnt + 6)
				{
					printf("\nUsage :\n");
					printf(" EAPI_I2C_NO_CMD:\n");
					printf("  semautil /i2c  write_raw	 [bus id] [address] [wr length] byte0 byte1 ...\n");
					printf(" EAPI_I2C_ENC_STD_CMD and EAPI_I2C_ENC_EXT_CMD:\n");
					printf("  semautil /i2c  write_raw	 [bus id] [address] [wr length] [cmd] byte0 byte1 ...\n");

					printf("\n  [Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
					printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");
					eRet = -3;
				}
				else
				{
					if (Args->I2CFuncArgs.WByteCnt > 29 || Args->I2CFuncArgs.WByteCnt <= 0)
					{
						printf("\nInvalid Size maximum write size is 29 bytes, min 1 byte.\n");
						eRet = -3;
						return eRet;
					}
					for (i = 0; i < Args->I2CFuncArgs.WByteCnt; i++)
					{
						((unsigned char*)(Args->I2CFuncArgs.pWrBuffer))[i] = string_to_hex(argv[6 + i]);
					}
				}
			}
			else
			{
				printf("\nUsage :\n");
				printf(" EAPI_I2C_NO_CMD:\n");
				printf("  semautil /i2c  write_raw	 [bus id] [address] [wr length] byte0 byte1 ...\n");
				printf(" EAPI_I2C_ENC_STD_CMD and EAPI_I2C_ENC_EXT_CMD:\n");
				printf("  semautil /i2c  write_raw	 [bus id] [address] [wr length] [cmd] byte0 byte1 ...\n");

				printf("\n  [Bus Id]:\n");
				printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
				printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
				printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
				printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
				printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");
				eRet = -3;
			}
		}
		else if (argc > 2 && (_stricmp(argv[2], "read_raw") == 0))
		{
			if (argc >= 6)
			{
				Args->IsI2CReRaw = TRUE;
				Args->I2CFuncArgs.BusID = atoi(argv[3]);
				Args->I2CFuncArgs.Address = string_to_hex(argv[4]);
				Args->I2CFuncArgs.pWrBuffer = malloc(32);
				int i, j;
				for (i = 5, j = 0; i < (argc - 1); i++, j++)
				{
					((unsigned char*)(Args->I2CFuncArgs.pWrBuffer))[j] = string_to_hex(argv[i]);
				}
				Args->I2CFuncArgs.WByteCnt = j;
				Args->I2CFuncArgs.nByteCnt = atoi(argv[argc - 1]);
			
				if (Args->I2CFuncArgs.nByteCnt > 32 || Args->I2CFuncArgs.nByteCnt <= 0)
				{
					printf("\nInvalid Size maximum read size is 32 bytes, min 1 byte.\n");
					eRet = -3;
					return eRet;
				}
				Args->I2CFuncArgs.pBuffer = calloc(Args->I2CFuncArgs.nByteCnt, sizeof(unsigned char));
				if (Args->I2CFuncArgs.pBuffer == NULL)
				{
					return EAPI_STATUS_ERROR;
				}
				if (Args->I2CFuncArgs.BusID < 1 || Args->I2CFuncArgs.BusID > 4)
				{
					printf("Invalid BusID\n");
					printf("  \n[Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
					printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");
					eRet = -3;
				}
			}
			else
			{
				printf("\nUsage :\n");
				printf(" EAPI_I2C_NO_CMD:\n");
				printf("  semautil /i2c  read_raw	 [bus id] [address] [re length]\n");
				printf(" EAPI_I2C_ENC_STD_CMD and EAPI_I2C_ENC_EXT_CMD:\n");
				printf("  semautil /i2c  read_raw	 [bus id] [address] [cmd] [re length]\n");

				printf("\n  [Bus Id]:\n");
				printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
				printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
				printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
				printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
				printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");
				eRet = -3;
			}
		}
		else if (argc > 2 && (_stricmp(argv[2], "raw_xfer") == 0))
				{
				if (argc >= 7)
				{
					int i;
					Args->I2CFuncArgs.BusID = atoi(argv[3]);
					if (Args->I2CFuncArgs.BusID < 1 || Args->I2CFuncArgs.BusID > 4)
					{
						printf("Invalid BusID\n");
						printf("  \n[Bus Id]:\n");
						printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
						printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
						printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
						printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
						printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");
						eRet = -3;
					}
					Args->I2CFuncArgs.Address = string_to_hex(argv[4]);
					if (Args->I2CFuncArgs.Address < 0 || Args->I2CFuncArgs.Address > 127)
					{
						return -3;
					}
					
					Args->I2CFuncArgs.WByteCnt = atoi(argv[5]);
					if (Args->I2CFuncArgs.WByteCnt > 32 || Args->I2CFuncArgs.WByteCnt < 0)
					{
						printf("\nInvalid Size maximum write size is 32 bytes, min 1 byte.\n");
						eRet = -3;
						return eRet;
					}

					Args->I2CFuncArgs.nByteCnt = atoi(argv[6]);

					if (Args->I2CFuncArgs.nByteCnt > 32 || Args->I2CFuncArgs.nByteCnt < 0)
					{
						printf("\nInvalid Size maximum read size is 32 bytes, min 1 byte.\n");
						eRet = -3;
						return eRet;
					}

					if (Args->I2CFuncArgs.nByteCnt == 0)
					{
						Args->IsI2CWrRaw = TRUE;
						Args->I2CFuncArgs.pWrBuffer = calloc(Args->I2CFuncArgs.WByteCnt, sizeof(unsigned char));
						if (Args->I2CFuncArgs.pWrBuffer == NULL)
						{
							return -3;
						}
						if (argc != Args->I2CFuncArgs.WByteCnt + 7)
						{
							printf("Wrong arguments \n");
							printf("\nUsage :\n");
							printf("  semautil /i2c  raw_xfer [bus id] [address] [wr length] [rd length] byte0 byte1 byte2...\n");

							printf("\n  [Bus Id]:\n");
							printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
							printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
							printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
							printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");
							eRet = -3;
						}
						else
						{
							for (i = 0; i < Args->I2CFuncArgs.WByteCnt; i++)
							{
								((unsigned char*)(Args->I2CFuncArgs.pWrBuffer))[i] = string_to_hex(argv[7 + i]);
							}
						}
					}
					else
					{
						Args->IsI2CReRaw = TRUE;
						Args->I2CFuncArgs.pWrBuffer = calloc(Args->I2CFuncArgs.WByteCnt, sizeof(unsigned char));
						if (Args->I2CFuncArgs.pWrBuffer == NULL)
						{
							return -3;
						}
						for (i = 0; i < Args->I2CFuncArgs.WByteCnt; i++)
						{
							((unsigned char*)(Args->I2CFuncArgs.pWrBuffer))[i] = string_to_hex(argv[7 + i]);
						}

						if (argc != Args->I2CFuncArgs.WByteCnt + 7)
						{
							printf("Wrong arguments \n");
							printf("\nUsage :\n");
							printf("  semautil /i2c  raw_xfer [bus id] [address] [wr length] [rd length] byte0 byte1 byte2...\n");

							printf("  [Bus Id]:\n");
							printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
							printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
							printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
							printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");
							eRet = -3;
						}
						else
						{
							Args->I2CFuncArgs.pBuffer = calloc(Args->I2CFuncArgs.nByteCnt, sizeof(unsigned char));
							if (Args->I2CFuncArgs.pBuffer == NULL)
							{
								return -3;
							}
						}
					}
				}
				else
				{
					printf("Wrong arguments \n");
					printf("\nUsage :\n");
					printf("  semautil /i2c  raw_xfer [bus id] [address] [wr length] [rd length] byte0 byte1 byte2...\n");

					printf("\n  [Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL_1\t\tBaseboard I2C Interface 1\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL_2\t\tBaseboard I2C Interface 2\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL_3\t\tBaseboard I2C Interface 3\n");
					eRet = -3;
				}

				}
		else if (argc > 2 && (_stricmp(argv[2], "read_xfer") == 0))
		{
			if (argc == 8 || argc == 7)
			{
				Args->IsI2CReXf = TRUE;
				Args->I2CFuncArgs.BusID = atoi(argv[3]);
				Args->I2CFuncArgs.Address = string_to_hex(argv[4]);
				Args->I2CFuncArgs.CmdType = atoi(argv[5]);
				Args->I2CFuncArgs.cmd = string_to_hex(argv[6]);
				Args->I2CFuncArgs.nByteCnt = atoi(argv[7 + argc - 8]);

				if (Args->I2CFuncArgs.BusID < 1 || Args->I2CFuncArgs.BusID > 4)
				{
					printf("Invalid BusID\n");
					printf("  \n[Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
					printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");
					eRet = -3;
				}

				switch (Args->I2CFuncArgs.CmdType)
				{
				case 1:
					Args->I2CFuncArgs.cmd = Args->I2CFuncArgs.cmd | (1 << 30);
					break;
				case 2:
					Args->I2CFuncArgs.cmd = Args->I2CFuncArgs.cmd;
					break;
				case 3:
					Args->I2CFuncArgs.cmd = Args->I2CFuncArgs.cmd | (2 << 30);
					break;
				default:
					printf("Invalid Command type\n");

					printf("  [Command Type]:\n");
					printf("    ID\tENCODED CMD ID\t\tDescription\n");
					printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
					printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
					printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 10 bits CMD\n");
					eRet = -3;
					return eRet;
				}
				if (argc == 7 && Args->I2CFuncArgs.CmdType != 1)
				{
					printf("\nUsage :\n");
					printf(" EAPI_I2C_NO_CMD:\n");
					printf("  semautil /i2c  read_xfer	 [bus id] [address] [cmd type] [read length]\n");
					printf(" EAPI_I2C_ENC_STD_CMD and EAPI_I2C_ENC_EXT_CMD:\n");
					printf("  semautil /i2c  read_xfer	 [bus id] [address] [cmd type] [cmd] [read length]\n");

					printf("\n  [Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
					printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");

					printf("  [Command Type]:\n");
					printf("    ID\tENCODED CMD ID\t\tDescription\n");
					printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
					printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
					printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 10 bits CMD\n");
					eRet = -3;
					return eRet;
				}
				if (Args->I2CFuncArgs.nByteCnt > 32 || Args->I2CFuncArgs.nByteCnt <= 0)
				{
					printf("\nInvalid Size maximum read size is 32 bytes, min 1 byte.\n");
					eRet = -3;
					return eRet;
				}
				Args->I2CFuncArgs.pBuffer = calloc(Args->I2CFuncArgs.nByteCnt, sizeof(unsigned char));
				if (Args->I2CFuncArgs.pBuffer == NULL)
				{
					return EAPI_STATUS_ERROR;
				}
			}
			else
			{
				printf("\nUsage :\n");
				printf(" EAPI_I2C_NO_CMD:\n");
				printf("  semautil /i2c  read_xfer	 [bus id] [address] [cmd type] [read length]\n");
				printf(" EAPI_I2C_ENC_STD_CMD and EAPI_I2C_ENC_EXT_CMD:\n");
				printf("  semautil /i2c  read_xfer	 [bus id] [address] [cmd type] [cmd] [read length]\n");

				printf("\n  [Bus Id]:\n");
				printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
				printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
				printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
				printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
				printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");

				printf("  [Command Type]:\n");
				printf("    ID\tENCODED CMD ID\t\tDescription\n");
				printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
				printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
				printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 10 bits CMD\n");
				eRet = -3;
			}
		}
		else if (argc > 2 && (_stricmp(argv[2], "write_xfer") == 0))
		{
			int count = 8;
			if (argc > 5)
			{
				Args->I2CFuncArgs.BusID = atoi(argv[3]);
				Args->I2CFuncArgs.Address = string_to_hex(argv[4]);
				Args->I2CFuncArgs.CmdType = atoi(argv[5]);

				if (Args->I2CFuncArgs.BusID < 1 || Args->I2CFuncArgs.BusID > 4)
				{
					printf("Invalid BusID\n");
					printf("  \n[Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
					printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");
					eRet = -3;
					return eRet;
				}
				if (Args->I2CFuncArgs.CmdType == 1)
				{
					count = 7;
				}
				else if (Args->I2CFuncArgs.CmdType != 3 && Args->I2CFuncArgs.CmdType != 2)
				{
					printf("Invalid Command type\n");

					printf("  [Command Type]:\n");
					printf("    ID\tENCODED CMD ID\t\tDescription\n");
					printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
					printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
					printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 10 bits CMD\n");
					eRet = -3;
					return eRet;
				}
			}

			if (argc > count)
			{
				unsigned int i;
				Args->I2CFuncArgs.cmd = string_to_hex(argv[6]);
				switch (Args->I2CFuncArgs.CmdType)
				{
				case 1:
					Args->I2CFuncArgs.cmd = (Args->I2CFuncArgs.cmd | (1 << 30));
					break;
				case 2:
					Args->I2CFuncArgs.cmd = Args->I2CFuncArgs.cmd;
					break;
				case 3:
					Args->I2CFuncArgs.cmd = (Args->I2CFuncArgs.cmd | (2 << 30));
					break;
				}
				Args->I2CFuncArgs.nByteCnt = atoi(argv[7 + count - 8]);

				Args->I2CFuncArgs.pBuffer = calloc(Args->I2CFuncArgs.nByteCnt, sizeof(unsigned char));
				if (Args->I2CFuncArgs.pBuffer == NULL)
				{
					return EAPI_STATUS_ERROR;
				}
				if (argc != Args->I2CFuncArgs.nByteCnt + count)
				{
					printf("\nUsage :\n");
					printf(" EAPI_I2C_NO_CMD:\n");
					printf("  semautil /i2c  write_xfer	 [bus id] [address] [cmd type] [length] byte0 byte1 byte2 ...\n");
					printf(" EAPI_I2C_ENC_STD_CMD and EAPI_I2C_ENC_EXT_CMD:\n");
					printf("  semautil /i2c  write_xfer	 [bus id] [address] [cmd type] [cmd] [length] byte0 byte1 byte2 ...\n");

					printf("  \n[Bus Id]:\n");
					printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
					printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
					printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
					printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
					printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");

					printf("  [Command Type]:\n");
					printf("    ID\tENCODED CMD ID\t\tDescription\n");
					printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
					printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
					printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 10 bits CMD\n");
					eRet = -3;
				}
				else
				{
					if (Args->I2CFuncArgs.nByteCnt > 29 || Args->I2CFuncArgs.nByteCnt <= 0)
					{
						printf("\nInvalid Size maximum write size is 29 bytes, min 1 byte.\n");
						eRet = -3;
						return eRet;
					}
					Args->IsI2CWrXf = TRUE;
					for (i = 0; i < Args->I2CFuncArgs.nByteCnt; i++)
					{
						((unsigned char*)(Args->I2CFuncArgs.pBuffer))[i] = string_to_hex(argv[count + i]);
					}
				}
			}
			else
			{
				printf("\nUsage :\n");
				printf(" EAPI_I2C_NO_CMD:\n");
				printf("  semautil /i2c  write_xfer	 [bus id] [address] [cmd type] [length] byte0 byte1 byte2 ...\n");
				printf(" EAPI_I2C_ENC_STD_CMD and EAPI_I2C_ENC_EXT_CMD:\n");
				printf("  semautil /i2c  write_xfer	 [bus id] [address] [cmd type] [cmd] [length] byte0 byte1 byte2 ...\n");

				printf("  \n[Bus Id]:\n");
				printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
				printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
				printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
				printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
				printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");

				printf("  [Command Type]:\n");
				printf("    ID\tENCODED CMD ID\t\tDescription\n");
				printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
				printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
				printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 10 bits CMD\n");
				eRet = -3;
			}
		}
		else if (argc == 3 && (_stricmp(argv[2], "get_status") == 0))
		{
			Args->IsI2CSts = TRUE;
		}
		else
		{
			printf("\n- Generic I2C Read/Write:\n");
			printf("  1. semautil /i2c  bus_cap\n");
			printf("  2. semautil /i2c  probe_device [bus id]\n");
			printf("  3. semautil /i2c  write_raw	 [bus id] [address] [wr length] [cmd] byte0 byte1 ...\n");
			printf("  3. semautil /i2c  read_raw	 [bus id] [address] [cmd] [re length]\n");
			printf("  4. semautil /i2c  raw_xfer	 [bus id] [address] [wr length] [rd length] byte0 byte1 byte2...\n");
			printf("  5. semautil /i2c  read_xfer	 [bus id] [address] [cmd type] [cmd] [read lentgh]\n");
			printf("  6. semautil /i2c  write_xfer	 [bus id] [address] [cmd type] [cmd] [length] byte0 byte1 byte2 ...\n");
			printf("  7. semautil /i2c  get_status\n");

			printf("  \n[Bus Id]:\n");
			printf("    ID\tSEMA EAPI ID\t\t\tDescription\n");
			printf("    1\tEAPI_ID_I2C_EXTERNAL\t1st External I2C Interface\n");
			printf("    2\tEAPI_ID_I2C_EXTERNAL\t2nd External I2C Interface\n");
			printf("    3\tEAPI_ID_I2C_EXTERNAL\t3rd External I2C Interface\n");
			printf("    4\tEAPI_ID_I2C_EXTERNAL\t4th External I2C Interface\n");

			printf("  [Command type]:\n");
			printf("    ID\tENCODED CMD ID\t\tDescription\n");
			printf("    1\tEAPI_I2C_NO_CMD\t\tSpecify no command/index is used\n");
			printf("    2\tEAPI_I2C_ENC_STD_CMD\tExtended standard 8 bits CMD\n");
			printf("    3\tEAPI_I2C_ENC_EXT_CMD\tExtended standard 10 bits CMD\n");
			eRet = -3;
		}
	}
	else if (_stricmp(argv[1], "/c") == 0)
	{
		if (argc == 3 && (_stricmp(argv[2], "guid_generate_write") == 0)) {
			Args->GUIDWrite = TRUE;
		}
		else if (argc == 3 && (_stricmp(argv[2], "guid_read") == 0)) {
		Args->GUIDRead = TRUE;
		}
		else
		{
			help_condition = 12;
		}
	}
	else if (argc == 2 && (_stricmp(argv[1], "version") == 0))
	{
		Args->GtVersion = TRUE;
	}
	else
	{
		ShowHelp(help_condition);
		eRet = -3;
	}
	if (help_condition != 0)
	{
		ShowHelp(help_condition);
		eRet = -3;
	}
	return eRet;
}