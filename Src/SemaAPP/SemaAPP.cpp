// SemaAPP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include  <string.h>
#include <stdio.h>
#include <Windows.h>

#include "SemaApp.h"
#include "EApi.h"

#pragma warning(disable : 4996)

tCmdLineArgs CmdLineArgs;
char*			ExeName;

int guid_string_to_bytes(const char* guid_string, uint8_t* guid_bytes) {

	for (int i = 0, j = 0; i < 36; i += 2) {
		char hex[3] = { guid_string[i], guid_string[i + 1], '\0' };
		sscanf(hex, "%hhX", &guid_bytes[j++]);
	}
	return 1;
}
char* FormatNumber(uint32_t number) {
	char buffer[9];
	char* result = new char[9];


	snprintf(buffer, sizeof(buffer), "%08X", number);


	char part1[2] = { buffer[1], '\0' };
	char part2[2] = { buffer[3], '\0' };
	char part3[3] = { buffer[6], buffer[7], '\0' };


	snprintf(result, 9, "%s.%s.%s", part1, part2, part3);

	return result;
}

int DispatchCMDToSEMA(tCmdLineArgs *Args)
{
	unsigned int MaxResetValue, MaxDelay, MaxEventTimeout;
	unsigned int GpioCapsIn = 0, GpioCapsOut = 0;
	unsigned int StorageSize, BlockLength;
	uint8_t PWMLevel;
	uint32_t sts = EAPI_STATUS_INVALID_PARAMETER;
	
	if (Args->VgaGetBacklightEnable)
	{
		if ((sts = EApiVgaGetBacklightEnable(Args->backlightFuncArgs.Id, &PWMLevel)) == EAPI_STATUS_SUCCESS)
		{
			if (PWMLevel == 1)
			{
				printf("\nBacklight ON\n");
			}
			else
			{
				printf("\nBacklight OFF\n");
			}
		}
		return sts;
	}

	if (Args->VgaSetBacklightEnable)
	{
		if ((sts = EApiVgaSetBacklightEnable(Args->backlightFuncArgs.Id, Args->backlightFuncArgs.EnnDI)) == EAPI_STATUS_SUCCESS)
		{
			if ((sts = EApiVgaGetBacklightEnable(Args->backlightFuncArgs.Id, &PWMLevel)) == EAPI_STATUS_SUCCESS)
			{
				if (PWMLevel == Args->backlightFuncArgs.EnnDI && PWMLevel == 1)
				{
					printf("\nBacklight ON\n");
				}
				else
				{
					printf("\nBacklight OFF\n");
				}
			}
		}
		return sts;
	}

	if (Args->VgaGetBacklightBrightness)
	{
		if ((sts = EApiVgaGetBacklightBrightness(Args->backlightFuncArgs.Id, &PWMLevel)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nCurrent backlight value is %d\n", PWMLevel);
		}
		return sts;
	}

	if (Args->VgaSetBacklightBrightness)
	{
		if ((sts = EApiVgaSetBacklightBrightness(Args->backlightFuncArgs.Id, Args->backlightFuncArgs.level)) == EAPI_STATUS_SUCCESS)
		{
			if ((sts = EApiVgaGetBacklightBrightness(Args->backlightFuncArgs.Id, &PWMLevel)) == EAPI_STATUS_SUCCESS)
			{
				printf("\nCurrent backlight value is set to %d\n", PWMLevel);
			}
		}
		return sts;
	}

	if (Args->GetValue)
	{
		if ((sts = SemaEApiBoardGetValue(Args->SemaNativeFuncArgs.Id, &(Args->SemaNativeFuncArgs.IntData))) == EAPI_STATUS_SUCCESS)
		{
			char* BoardCapabilities[32] = { "Uptime and Power Cycles", "System Restart Event", "User-Flash Size", "Runtime Watchdog", "Temperatures", "Voltage Monitor", "Storage of failure reason", "Bootloader timeout", "Display Backlight control", "Power-up Watchdog", "Power Monitor (current sense)", "Boot counter", "Input Voltage",NULL, "Rsense of Power Monitor", "Dual-BIOS", "I2C Bus 1", "I2C Bus 2", "CPU Fan", "System Fan 1", "AT/ATX Mode", "ACPI Thermal Trigger", "Power-up to last state", "Backlight Restore", "DTS Temperature", "DTS Offset Registers", "System Fan 2", "System Fan 3", "Ext-GPIO", "I2C Bus 3", "I2C Bus 4", "BMC ID 0" };
			char* values[32] = {};
			values[13] = NULL;
			char* BoardExCapabilities[15] = { "Board 2 Temperature", "PEC Protocol", NULL, "Error log", "1-Wire Bus", "Wake by EC", "GPIO Alternate Function", "Soft Fan", "Parameter Memory", "Extended I2C registers for Status", "Ext-GPIO Input Interrupt", "Hardware Monitor Input String", "Ext-GPIO Pins Count", "Power-up/Runtime Watchdog support action setting", "Switch BIOS immediately" };
			char* Extvalues[15] = {};
			Extvalues[2] = NULL;
			char* BoardBMCFlag[3] = { "Exception code", "Mode", "BIOS" };
			char* formattedNum;
			int i;

			switch (Args->SemaNativeFuncArgs.Id) {
			case EAPI_ID_GET_EAPI_SPEC_VERSION:
				formattedNum = FormatNumber(Args->SemaNativeFuncArgs.IntData);
				printf("\nEAPI Specification Version: %s\n", formattedNum);
				break;
			case EAPI_ID_BOARD_BOOT_COUNTER_VAL:
				printf("\nBoot counter: %d\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_ID_BOARD_RUNNING_TIME_METER_VAL:
				printf("\nRunning Time Meter: %d minutes\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_ID_BOARD_LIB_VERSION_VAL:
				formattedNum = FormatNumber(Args->SemaNativeFuncArgs.IntData);
				printf("\nSEMA Library Version : % s\n", formattedNum);
				break;
			case EAPI_ID_HWMON_CPU_TEMP:
				printf("\nCPU temperature: %.2lf C\n", (Args->SemaNativeFuncArgs.IntData - 2731) * 0.1);
				break;
			case EAPI_ID_HWMON_SYSTEM_TEMP:
				printf("\nSystem Temperature: %.2lf C\n", (Args->SemaNativeFuncArgs.IntData - 2731) * 0.1);
				break;
			case EAPI_ID_HWMON_VOLTAGE_VCORE:
				printf("\nCPU Core Voltage: %d mV\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_ID_HWMON_VOLTAGE_2V5:
				printf("\n2.5V Voltage: %d mV\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_ID_HWMON_VOLTAGE_3V3:
				printf("\n3.3V Voltage: %d mV\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_ID_HWMON_VOLTAGE_VBAT:
				printf("\nBattery Voltage: %d mV\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_ID_HWMON_VOLTAGE_5V:
				printf("\n5V Voltage: %d mV\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_ID_HWMON_VOLTAGE_5VSB:
				printf("\n5V Standby Voltage: %d mV\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_ID_HWMON_VOLTAGE_12V:
				printf("\n12V Voltage: %d mV\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_ID_HWMON_FAN_CPU:
				printf("\nCPU Fan speed: %d RPM\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_ID_HWMON_FAN_SYSTEM:
				printf("\nSystem Fan 1 speed: %d RPM\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_SEMA_ID_BOARD_POWER_UP_TIME:
				printf("\nBoard powerup time: %d seconds\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_SEMA_ID_BOARD_RESTART_EVENT:
				printf("\nRestart event: 0x%X\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_SEMA_ID_BOARD_CAPABILITIES:
				printf("\nBMC capabilities:\n");
				for (i = 0; i < (sizeof(BoardCapabilities) / sizeof(*BoardCapabilities)); i++) {
					if (i == 2) {
						if ((Args->SemaNativeFuncArgs.IntData & (1 << i)) != 0) {
							values[i] = " : 1024 bytes (0x0000 to 0x03FF)";
						}
						else {
							values[i] = " : 512 bytes";
						}
					}
					else if ((i == 12)) {
						values[i] = " : Not Supported";
						i++;
					}
					else if (i == 14) {
						values[i] = " : Not Supported";
					}
					else if (i == 31)
					{
						if ((Args->SemaNativeFuncArgs.IntData & (1 << i)) != 0)
						{
							values[i] = " : Tiva BMC";
						}
						else
						{
							values[i] = " : Other BMC";
						}
					}
					else
					{
						if ((Args->SemaNativeFuncArgs.IntData & (1 << i)) != 0)
						{
							values[i] = " : Supported";
						}
						else
						{
							values[i] = " : Not Supported";
						}
					}
				}
				printf("\n%-*s%-*s%-*s\t\t%-*s%-*s%-*s", 10, "Bit", 30, "Capability", 30, " Status", 10, "Bit", 30, "Capability", 30, " Status");
				printf("\n");
				for (i = 0; i < 16; i++) {
					if (i == 12) {
						printf("\n%d&%-*d%-*s%-*s\t\t%-*d%-*s%-*s", i, 5, i + 1, 30, BoardCapabilities[i], 30, values[i], 8, i + 16, 30, BoardCapabilities[i + 16], 30, values[i + 16]);
					}
					else if (i == 13) {
						printf("\n%65s\t\t%-*d%-*s%-*s", "", 8, i + 16, 30, BoardCapabilities[i + 16], 30, values[i + 16]);
					}
					else {
						printf("\n%-*d%-*s%-*s\t\t%-*d%-*s%-*s", 8, i, 30, BoardCapabilities[i], 30, values[i], 8, i + 16, 30, BoardCapabilities[i + 16], 30, values[i + 16]);
					}
				}
				printf("\n");
				break;
			case EAPI_SEMA_ID_BOARD_CAPABILITIES_EX:
				printf("\nExtended BMC capabilities:\n");
				for (i = 0; i < (sizeof(BoardExCapabilities) / sizeof(*BoardExCapabilities)); i++) {

					if (i != 2)
					{
						if (i == 12)
						{
							if ((Args->SemaNativeFuncArgs.IntData & (1 << i)) != 0)
							{
								Extvalues[i] = " : 12 Pins";
							}
							else
							{
								Extvalues[i] = " : 8 Pins";
							}
						}
						else
						{
							if ((Args->SemaNativeFuncArgs.IntData & (1 << i)) != 0)
							{
								Extvalues[i] = " : Supported";
							}
							else
							{
								Extvalues[i] = " : Not Supported";
							}
						}
					}
				}
				printf("\n%-*s%-*s%-*s\t\t%-*s%-*s%-*s", 10, "Bit", 30, "Capability", 30, " Status", 10, "Bit", 30, "Capability", 30, " Status");
				printf("\n");
				for (i = 0; i < 8; i++) {
					if (i == 7) {
						printf("\n%-*d%-*s%-*s", 8, i + 32, 30, BoardExCapabilities[i], 30, Extvalues[i]);
					}
					else if (i != 2) {
						printf("\n%-*d%-*s%-*s\t\t%-*d%-*s%-*s", 8, i + 32, 30, BoardExCapabilities[i], 30, Extvalues[i], 8, i + 32 + 8, 30, BoardExCapabilities[i + 8], 30, Extvalues[i + 8]);
					}
					else {
						printf("\n%65s\t\t%-*d%-*s%-*s", "", 8, i + 32 + 8, 30, BoardExCapabilities[i + 8], 30, Extvalues[i + 8]);
					}
				}
				printf("\n");
				break;
			case EAPI_SEMA_ID_BOARD_SYSTEM_MIN_TEMP:
				printf("\nBoard minimum temperature: %.2lf C\n", (Args->SemaNativeFuncArgs.IntData - 2731) * 0.1);
				break;
			case EAPI_SEMA_ID_BOARD_SYSTEM_MAX_TEMP:
				printf("\nBoard maximum temperature: %.2lf C\n", (Args->SemaNativeFuncArgs.IntData - 2731) * 0.1);
				break;
			case EAPI_SEMA_ID_BOARD_SYSTEM_STARTUP_TEMP:
				printf("\nBoard startup temperature: %.2lf C\n", (Args->SemaNativeFuncArgs.IntData - 2731) * 0.1);
				break;
			case EAPI_SEMA_ID_BOARD_CPU_MIN_TEMP:
				printf("\nCPU minimum temperature: %.2lf C\n", (Args->SemaNativeFuncArgs.IntData - 2731) * 0.1);
				break;
			case EAPI_SEMA_ID_BOARD_CPU_MAX_TEMP:
				printf("\nCPU maximum temperature: %.2lf C\n", (Args->SemaNativeFuncArgs.IntData - 2731) * 0.1);
				break;
			case EAPI_SEMA_ID_BOARD_CPU_STARTUP_TEMP:
				printf("\nCPU startup temperature: %.2lf C\n", (Args->SemaNativeFuncArgs.IntData - 2731) * 0.1);
				break;
			case EAPI_SEMA_ID_BOARD_MAIN_CURRENT:
				printf("\nMain power current: %d mA\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_SEMA_ID_HWMON_VOLTAGE_GFX_VCORE:
				printf("\nGFX Voltage: %d mV\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_SEMA_ID_HWMON_VOLTAGE_1V05:
				printf("\n1.05V Voltage: %d mV\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_SEMA_ID_HWMON_VOLTAGE_1V5:
				printf("\n1.5V Voltage: %d mV\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_SEMA_ID_HWMON_VOLTAGE_VIN:
				printf("\nVin Voltage: %d mV\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_SEMA_ID_HWMON_FAN_SYSTEM_2:
				printf("\nSystem Fan 2 speed: %d RPM\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_SEMA_ID_HWMON_FAN_SYSTEM_3:
				printf("\nSystem Fan 3 speed: %d RPM\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_SEMA_ID_BOARD_2ND_SYSTEM_TEMP:
				printf("\nBoard 2nd Current temperature: %.2lf C\n", (Args->SemaNativeFuncArgs.IntData - 2731) * 0.1);
				break;
			case EAPI_SEMA_ID_BOARD_2ND_SYSTEM_MIN_TEMP:
				printf("\nBoard 2nd minimum temperature: %.2lf C\n", (Args->SemaNativeFuncArgs.IntData - 2731) * 0.1);
				break;
			case EAPI_SEMA_ID_BOARD_2ND_SYSTEM_MAX_TEMP:
				printf("\nBoard 2nd maximum temperature: %.2lf C\n", (Args->SemaNativeFuncArgs.IntData - 2731) * 0.1);
				break;
			case EAPI_SEMA_ID_BOARD_2ND_SYSTEM_STARTUP_TEMP:
				printf("\nBoard 2nd startup temperature: %.2lf C\n", (Args->SemaNativeFuncArgs.IntData - 2731) * 0.1);
				break;
			case EAPI_SEMA_ID_BOARD_POWER_CYCLE:
				printf("\nPower cycle counter: %d\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_SEMA_ID_BOARD_BMC_FLAG:
				printf("\nBMC Flag:");
				for (i = 0; i < (sizeof(BoardBMCFlag) / sizeof(*BoardBMCFlag)); i++)
				{
					if (i == 0)
					{
						uint32_t deciVal = Args->SemaNativeFuncArgs.IntData & 0x11111;
						char* opstring;
						switch (deciVal)
						{
						case 0:
							opstring = "NO ERROR";
							break;
						case 2:
							opstring = "NO_SUSCLK";
							break;
						case 3:
							opstring = "NO_SLP_S5";
							break;
						case 4:
							opstring = "NO_SLP_S4";
							break;
						case 5:
							opstring = "NO_SLP_S3";
							break;
						case 6:
							opstring = "BIOS_FAIL";
							break;
						case 7:
							opstring = "RESET_FAIL";
							break;
						case 8:
							opstring = "RESETIN_FAIL";
							break;
						case 9:
							opstring = "NO_CB_PWORK";
							break;
						case 10:
							opstring = "CRITICAL_TEMP";
							break;
						case 11:
							opstring = "POWER_FAIL";
							break;
						case 12:
							opstring = "VOLTAGE_FAIL";
							break;
						case 13:
							opstring = "RSMRST_FAIL";
							break;
						case 14:
							opstring = "NO_VDDQ_PG";
							break;
						case 15:
							opstring = "NO_V1P05A_PG";
							break;
						case 16:
							opstring = "NO_VCORE_PG";
							break;
						case 17:
							opstring = "NO_SYS_GD";
							break;
						case 18:
							opstring = "NO_V5SBY";
							break;
						case 19:
							opstring = "NO_V3P3A";
							break;
						case 20:
							opstring = "NO_V5_DUAL";
							break;
						case 21:
							opstring = "NO_PWRSRC_GD";
							break;
						case 22:
							opstring = "NO_P_5V_3V3_S0_PG";
							break;
						case 23:
							opstring = "NO_SAME_CHANNEL";
							break;
						case 24:
							opstring = "NO_PCH_PG";
							break;
						default:
							opstring = "";
							break;
						}
						printf("\n%s : %s\n", BoardBMCFlag[i], opstring);

					}
					else if (i == 1)
					{
						if ((Args->SemaNativeFuncArgs.IntData & (1 << 6)) != 0)
						{
							printf("\n%s : ATX Mode\n", BoardBMCFlag[i]);
						}
						else
						{
							printf("\n%s : AT Mode\n", BoardBMCFlag[i]);
						}
					}
					else
					{
						if ((Args->SemaNativeFuncArgs.IntData & (1 << 7)) != 0)
						{
							printf("\n%s : Fail-Safe BIOS is active\n", BoardBMCFlag[i]);
						}
						else
						{
							printf("\n%s : Standard BIOS\n", BoardBMCFlag[i]);
						}
					}
				}
				break;
			case EAPI_SEMA_ID_BOARD_BMC_STATUS:
				printf("\nBoard BMC Status: 0x%X\n", Args->SemaNativeFuncArgs.IntData);
				break;
			case EAPI_SEMA_ID_IO_CURRENT:
				printf("\nIO Current: %d mA\n", Args->SemaNativeFuncArgs.IntData);
				break;
			default:
				printf("\n%d\n", Args->SemaNativeFuncArgs.IntData);
				break;
			}
		}
		return sts;
	}
	
	if (Args->WatchDogCap)
	{
		if ((sts = EApiWDogGetCap(&MaxEventTimeout,&MaxDelay,&MaxResetValue)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nMaxEventTimeout : %d seconds\n", MaxEventTimeout);
			printf("MaxDelay : %d seconds\n", MaxDelay);
			printf("MaxResetValue : %d seconds\n", MaxResetValue);
		}
		return sts;
	}

	if (Args->SetWatchdog)
	{
		if ((sts = EApiWDogStart(Args->WatchDogFuncArgs.Delay, Args->WatchDogFuncArgs.EventTimeout, Args->WatchDogFuncArgs.WatchdogTimeOut)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nRun-time Watchdog Started with : %d seconds\n", Args->WatchDogFuncArgs.WatchdogTimeOut);
		}
		else if(sts == EAPI_STATUS_RUNNING)
		{
			printf("\nRun-time Watchdog is already running\n");
			sts = EAPI_STATUS_SUCCESS;
		}
		return sts;
	}

	if (Args->TriggerWatchdog)
	{
		uint32_t LastValue;
		if ((sts = EApiWDogTrigger()) == EAPI_STATUS_SUCCESS)
		{
			SemaEApiWDogGetLastValue(&LastValue);
			printf("\nWatchdog is triggered to %d seconds.\n", LastValue);
		}
		else if(sts == EAPI_STATUS_ERROR)
		{
			printf("\nWatchdog is not running\n");
			sts = EAPI_STATUS_SUCCESS;
		}
		return sts;
	}

	if (Args->StopWatchdog)
	{
		if ((sts = EApiWDogStop()) == EAPI_STATUS_SUCCESS)
		{
			printf("\nWatchdog stopped successfully.\n");
		}
		else if(sts == EAPI_STATUS_ERROR)
		{
			printf("\nWatchdog is not running\n");
			sts = EAPI_STATUS_SUCCESS;
		}
		return sts;
	}
	
	if (Args->IsPwrUpWDogStart)				
	{
		if ((sts = EApiPwrUpWDogStart(Args->WatchDogFuncArgs.WatchdogTimeOut)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nPower-Up Watchdog is load with %d seconds\n", Args->WatchDogFuncArgs.WatchdogTimeOut);
		}
		return sts;
	}

	if (Args->IsPwrUpWDogStop)				 
	{
		uint32_t sts;

		if ((sts = EApiPwrUpWDogStop()) == EAPI_STATUS_SUCCESS)
		{
			printf("\nPower-Up Watchdog Disabled\n");
			return EAPI_STATUS_SUCCESS;
		}
		else if (sts == EAPI_STATUS_WRITE_ERROR)
		{
			printf("\nPower-Up Watchdog Not Enabled\n");
			return EAPI_STATUS_SUCCESS;
		}
	}
	
	if (Args->SmartFanTempSet)
	{
		if ((sts = SemaEApiSmartFanSetTempSetpoints(Args->SmartFaneFuncArgs.FanID, Args->SmartFaneFuncArgs.Level[0], Args->SmartFaneFuncArgs.Level[1], \
			Args->SmartFaneFuncArgs.Level[2], Args->SmartFaneFuncArgs.Level[3])) == EAPI_STATUS_SUCCESS)
		{
			printf("\nTemperature levels set successfully\n");
			return EAPI_STATUS_SUCCESS;
		}
		return sts;
	}

	if (Args->SmartFanTempGet)
	{
		if ((sts = SemaEApiSmartFanGetTempSetpoints(Args->SmartFaneFuncArgs.FanID, &(Args->SmartFaneFuncArgs.Level[0]), &(Args->SmartFaneFuncArgs.Level[1]), \
			&(Args->SmartFaneFuncArgs.Level[2]), &(Args->SmartFaneFuncArgs.Level[3]))) == EAPI_STATUS_SUCCESS)
		{
			switch (Args->SmartFaneFuncArgs.FanID)
			{
			case 0:
				printf("\nFan ID: %d (CPU Fan)\n", Args->SmartFaneFuncArgs.FanID);
				break;
			case 1:
				printf("\nFan ID: %d (Sys Fan 1)\n", Args->SmartFaneFuncArgs.FanID);
				break;
			case 2:
				printf("\nFan ID: %d (Sys Fan 2)\n", Args->SmartFaneFuncArgs.FanID);
				break;
			case 3:
				printf("\nFan ID: %d (Sys Fan 3)\n", Args->SmartFaneFuncArgs.FanID);
				break;
			}
			printf("Level 1 : %d\n", Args->SmartFaneFuncArgs.Level[0]);
			printf("Level 2 : %d\n", Args->SmartFaneFuncArgs.Level[1]);
			printf("Level 3 : %d\n", Args->SmartFaneFuncArgs.Level[2]);
			printf("Level 4 : %d\n", Args->SmartFaneFuncArgs.Level[3]);
		}
		return sts;
	}

	if (Args->SmartFanPWMSet)
	{
		if ((sts = SemaEApiSmartFanSetPWMSetpoints(Args->SmartFaneFuncArgs.FanID, Args->SmartFaneFuncArgs.Level[0], Args->SmartFaneFuncArgs.Level[1],\
			Args->SmartFaneFuncArgs.Level[2], Args->SmartFaneFuncArgs.Level[3])) == EAPI_STATUS_SUCCESS)
		{
			printf("\nPWM levels set successfully\n");
		}
		return sts;
	}

	if (Args->SmartFanPWMGet)
	{
		if ((sts = SemaEApiSmartFanGetPwmSetpoints(Args->SmartFaneFuncArgs.FanID, &(Args->SmartFaneFuncArgs.Level[0]), &(Args->SmartFaneFuncArgs.Level[1]),\
			&(Args->SmartFaneFuncArgs.Level[2]), &(Args->SmartFaneFuncArgs.Level[3]))) == EAPI_STATUS_SUCCESS)
		{
			switch (Args->SmartFaneFuncArgs.FanID)
			{
			case 0:
				printf("\nFan ID: %d (CPU Fan)\n", Args->SmartFaneFuncArgs.FanID);
				break;
			case 1:
				printf("\nFan ID: %d (Sys Fan 1)\n", Args->SmartFaneFuncArgs.FanID);
				break;
			case 2:
				printf("\nFan ID: %d (Sys Fan 2)\n", Args->SmartFaneFuncArgs.FanID);
				break;
			case 3:
				printf("\nFan ID: %d (Sys Fan 3)\n", Args->SmartFaneFuncArgs.FanID);
				break;
			}
			printf("Level 1 : %d\n", Args->SmartFaneFuncArgs.Level[0]);
			printf("Level 2 : %d\n", Args->SmartFaneFuncArgs.Level[1]);
			printf("Level 3 : %d\n", Args->SmartFaneFuncArgs.Level[2]);
			printf("Level 4 : %d\n", Args->SmartFaneFuncArgs.Level[3]);
		}
		return sts;
	}

	if (Args->SmartFanModeSet)
	{
		if ((sts = SemaEApiSmartFanSetMode(Args->SmartFaneFuncArgs.FanID, Args->SmartFaneFuncArgs.FanMode)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nFAN mode set successfully.\n");
		}
		return sts;
	}

	if (Args->SmartFanModeGet)
	{
		if ((sts = SemaEApiSmartFanGetMode(Args->SmartFaneFuncArgs.FanID, (uint8_t*)&(Args->SmartFaneFuncArgs.FanMode))) == EAPI_STATUS_SUCCESS)
		{
			switch (Args->SmartFaneFuncArgs.FanID)
			{
			case 0:
				printf("\nFan ID: %d (CPU Fan)\n", Args->SmartFaneFuncArgs.FanID);
				break;
			case 1:
				printf("\nFan ID: %d (Sys Fan 1)\n", Args->SmartFaneFuncArgs.FanID);
				break;
			case 2:
				printf("\nFan ID: %d (Sys Fan 2)\n", Args->SmartFaneFuncArgs.FanID);
				break;
			case 3:
				printf("\nFan ID: %d (Sys Fan 3)\n", Args->SmartFaneFuncArgs.FanID);
				break;
			}
			switch (Args->SmartFaneFuncArgs.FanMode)
			{
			case 0:
				printf("\nFan Mode: %d (Auto)\n", Args->SmartFaneFuncArgs.FanMode);
				break;
			case 1:
				printf("\nFan Mode: %d (Off)\n", Args->SmartFaneFuncArgs.FanMode);
				break;
			case 2:
				printf("\nFan Mode: %d (On)\n", Args->SmartFaneFuncArgs.FanMode);
				break;
			case 3:
				printf("\nFan Mode: %d (Soft)\n", Args->SmartFaneFuncArgs.FanMode);
				break;
			}
		}
		return sts;
	}

	if (Args->SmartFanTempSetSrc)
	{
		if ((sts = SemaEApiSmartFanSetTempSrc(Args->SmartFaneFuncArgs.FanID, Args->SmartFaneFuncArgs.TempSrc)) == EAPI_STATUS_SUCCESS)
		{
			switch (Args->SmartFaneFuncArgs.TempSrc)
			{
			case 0:
				printf("\nTemperature source is set to %d (CPU sensor)\n", Args->SmartFaneFuncArgs.TempSrc);
				break;
			case 1:
				printf("\nTemperature source is set to %d (Baseboard sensor)\n", Args->SmartFaneFuncArgs.TempSrc);
				break;
			}
		}
		return sts;
	}

	if (Args->SmartFanTempGetSrc)
	{
		if ((sts = SemaEApiSmartFanGetTempSrc(Args->SmartFaneFuncArgs.FanID, (uint8_t*)(&(Args->SmartFaneFuncArgs.TempSrc)))) == EAPI_STATUS_SUCCESS)
		{
			switch (Args->SmartFaneFuncArgs.TempSrc)
			{
			case 0:
				printf("\nCurrent Temperature source is %d (CPU sensor)\n", Args->SmartFaneFuncArgs.TempSrc);
				break;
			case 1:
				printf("\nCurrent Temperature source is %d (Baseboard sensor)\n", Args->SmartFaneFuncArgs.TempSrc);
				break;
			}
		}
		return sts;
	}

	// Storage Functions Start
		
	if (Args->StorageCap)
	{
		if ((sts = EApiStorageCap(Args->StorageFuncArgs.Id, &StorageSize, &BlockLength)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nStorage Size: %d bytes\nBlockSize: %d bytes\n", StorageSize, BlockLength);
		}
		return sts;
	}

	if (Args->StorageAreaRead)
	{
		uint32_t Address = Args->StorageFuncArgs.Address;
		uint32_t Length = Args->StorageFuncArgs.Length, i;

		if ((sts = EApiStorageCap(Args->StorageFuncArgs.Id, &StorageSize, &BlockLength)) == EAPI_STATUS_SUCCESS)
		{
			
			if (Args->StorageFuncArgs.Address % BlockLength)
			{
				Address -= Address % BlockLength;
			}

			if (Args->StorageFuncArgs.Length % BlockLength)
			{
				Length += (BlockLength - (Length % BlockLength));
			}
			
		}

		unsigned char* Data = (unsigned char*)calloc(sizeof(unsigned char), Length + 10 + 2 * BlockLength);
		
		if ((sts = EApiStorageAreaRead(Args->StorageFuncArgs.Id, Address, Data, Length + 2 * BlockLength, Length)) == EAPI_STATUS_SUCCESS)
		{
			
			int diff = Args->StorageFuncArgs.Address % BlockLength;
			for (i = 0; i < Args->StorageFuncArgs.Length; i++)
			{
				Data[i] = Data[i + diff];
			}
			Data[i] = 0;
			
			printf("\nThe Data is \"%s\"\n", Data);
		}

		return sts;
	}
	
	if (Args->StorageAreaWrite)
	{
		uint32_t Address = Args->StorageFuncArgs.Address;
		uint32_t diff = 0, Length = (uint32_t)strlen(Args->StorageFuncArgs.String);

		if ((sts = EApiStorageCap(Args->StorageFuncArgs.Id, &StorageSize, &BlockLength)) == EAPI_STATUS_SUCCESS)
		{
			if (Args->StorageFuncArgs.Address % BlockLength)
			{
				diff = Address % BlockLength;
				Address -= diff;
			}

			if (Length % BlockLength)
			{
				Length += (BlockLength - (Length % BlockLength));
			}
		}

		unsigned char *Data = (unsigned char*)calloc(sizeof(unsigned char), StorageSize);
		
		if ((sts = EApiStorageAreaRead(Args->StorageFuncArgs.Id, Address, Data, Length + 2 * BlockLength, Length)) == EAPI_STATUS_SUCCESS)
		{
			memcpy(&(Data[diff]), Args->StorageFuncArgs.String, strlen(Args->StorageFuncArgs.String));

			if ((sts = EApiStorageAreaWrite(Args->StorageFuncArgs.Id, Address, Data, Length)) == EAPI_STATUS_SUCCESS)
			{
				printf("\nData Written Successfully\n");
			}
		}

		return sts;
	}
	if (Args->ODM_Write)
	{
		uint32_t Address = Args->StorageFuncArgs.Address;
		uint32_t Length = (uint32_t)strlen(Args->StorageFuncArgs.String);
		if ((sts = EApiStorageCap(Args->StorageFuncArgs.Id, &StorageSize, &BlockLength)) == EAPI_STATUS_SUCCESS)
		{
			if (Length % BlockLength)
			{
				Length += (BlockLength - (Length % BlockLength));
			}
		}

		if ((sts = EApiStorageAreaWrite(Args->StorageFuncArgs.Id, Address, Args->StorageFuncArgs.String, Length)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nData Written Successfully\n");
		}
	}

	if (Args->StorageClear)
	{
		if ((sts = SemaEApiStorageClear(Args->StorageFuncArgs.Id)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nStorage Cleaned Successfully\n");
		}
		return sts;
	}

	if (Args->StorageLock)
	{
		if (Args->StorageFuncArgs.Id == EAPI_ID_STORAGE_STD)
		{
			printf("\nError : User region is not protectable\n");
			return EAPI_STATUS_SUCCESS;
		}

		if ((sts = SemaEApiStorageLock(Args->StorageFuncArgs.Id)) == EAPI_STATUS_SUCCESS)
		{
			if (Args->StorageFuncArgs.Id == EAPI_ID_STORAGE_SCR)
			{
				printf("\nSecured region is protected from read/write operation.\n");
			}
			else
			{
				printf("\nODM region is protected from write operation.\n");
			}
		}
		else if (sts == EAPI_STATUS_WRITE_ERROR)
		{
			printf("\nInvalid Passcode\n");
			sts = EAPI_STATUS_SUCCESS;
		}
		return sts;
	}

	if (Args->StorageUnlock)
	{
		if (Args->StorageFuncArgs.Id == EAPI_ID_STORAGE_STD)
		{
			printf("\nError : User region is not protected\n");
			return EAPI_STATUS_SUCCESS;
		}

		if (Args->StorageFuncArgs.Id == EAPI_ID_STORAGE_ODM && Args->StorageFuncArgs.Permission == 1)
		{
			printf("\nError : ODM region is always readable\n");
			return EAPI_STATUS_SUCCESS;
		}

		if ((sts = SemaEApiStorageUnlock(Args->StorageFuncArgs.Id, Args->StorageFuncArgs.Permission,\
			Args->StorageFuncArgs.Password)) == EAPI_STATUS_SUCCESS)
		{
			if (Args->StorageFuncArgs.Id == EAPI_ID_STORAGE_SCR)
			{
				if (Args->StorageFuncArgs.Permission == 2)
					printf("\nSecured region is unprotected from read/write\n");
				else
					printf("\nSecured region is unprotected from read\n");
			}
			else
			{
				printf("\nODM region is unprotected from write\n");
			}
		}
		else if (sts == EAPI_STATUS_WRITE_ERROR)
		{
			printf("\nInvalid Passcode\n");
			sts = EAPI_STATUS_SUCCESS;
		}
		return sts;
	}

	if (Args->StorageHexRead)
	{
		uint32_t Address = Args->StorageFuncArgs.Address;
		uint32_t Length = Args->StorageFuncArgs.Length, i;
		unsigned char* Data = (unsigned char*)calloc(sizeof(unsigned char), Length + 10 + 2 * BlockLength);
		if (Args->StorageFuncArgs.Id <1 || Args->StorageFuncArgs.Id >3)
		{
			return -1;
		}
		if ((sts = EApiStorageHexRead(Args->StorageFuncArgs.Id, Address, Data, Length + 2 * BlockLength, Length)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nRead Buffer : ");
			for (i = 0; i < Length; i++)
			{
				printf("0x%02X ", Data[i]);
			}
			printf("\n");
		}
		return sts;
	}

	if (Args->StorageHexWrite)
	{
		uint32_t Address = Args->StorageFuncArgs.Address;
		uint32_t diff = 0, Length = (uint32_t)strlen(Args->StorageFuncArgs.String), i;
		if (Args->StorageFuncArgs.Id < 1 || Args->StorageFuncArgs.Id >3)
		{
			return -1;
		}
		StorageSize = 1024;
		unsigned char* Data = (unsigned char*)calloc(sizeof(unsigned char), StorageSize);
		memcpy(&(Data[diff]), Args->StorageFuncArgs.String, strlen(Args->StorageFuncArgs.String));
		for (i = 0; i < Length - 2; i++)
		{
			if (isxdigit(Data[i]) == 0)
			{
				printf("Please provide the hex data only 'A'-'F','a'-'f' and '0'-'9'\n");
				return EAPI_STATUS_UNSUPPORTED;
			}
		}
		if ((sts = EApiStorageHexWrite(Args->StorageFuncArgs.Id, Address, Data, Length)) == EAPI_STATUS_SUCCESS)
		{
			printf("\n %d Bytes Written Successfully\n", Length / 2);
		}
		return sts;
	}

	if (Args->GPIOGetDirectionCaps)
	{
		for (uint8_t i = 0; i <= 12; i++) {
			Args->GPIOFuncArgs.GPIOID = i;
			if ((sts = EApiGPIOGetDirectionCaps(Args->GPIOFuncArgs.GPIOID, &GpioCapsIn, &GpioCapsOut)) != EAPI_STATUS_SUCCESS)
			{
				printf("\nNumber of GPIO pins supported: %d pins\n", Args->GPIOFuncArgs.GPIOID);
				sts = EAPI_STATUS_SUCCESS;
				break;
			}
		}
		return sts;
	}

	if (Args->GPIOGetDirection)
	{
		if ((sts = EApiGPIOGetDirection(Args->GPIOFuncArgs.GPIOID, (1 << Args->GPIOFuncArgs.GPIOPin), &(Args->GPIOFuncArgs.Dir))) == EAPI_STATUS_SUCCESS)
		{
			printf("\nDirection: %s\n", Args->GPIOFuncArgs.Dir == 0 ? "Output" : "Input");
		}
		return sts;
	}

	if (Args->GPIOSetDirection)
	{
		if ((sts = EApiGPIOSetDirection(Args->GPIOFuncArgs.GPIOID, (1 << Args->GPIOFuncArgs.GPIOPin), Args->GPIOFuncArgs.Dir)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nDirection updated successfully\n");
		}
		return sts;
	}

	if (Args->GPIOGetLevel)
	{
		if ((sts = EApiGPIOGetLevel(Args->GPIOFuncArgs.GPIOID, (1 << Args->GPIOFuncArgs.GPIOPin), &(Args->GPIOFuncArgs.Level))) == EAPI_STATUS_SUCCESS)
		{
			printf("\nLevel: %s\n", Args->GPIOFuncArgs.Level == 0 ? "Low" : "High");
		}
		return sts;
	}

	if (Args->GPIOSetLevel)
	{
		if ((sts = EApiGPIOSetLevel(Args->GPIOFuncArgs.GPIOID, (1 << Args->GPIOFuncArgs.GPIOPin), Args->GPIOFuncArgs.Level)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nGPIO Level updated successfully\n");
		}
		return sts;
	}

	if (Args->GetStringA)
	{
		Args->SemaNativeFuncArgs.Size = 100;
		if ((sts = EApiBoardGetStringA(Args->SemaNativeFuncArgs.cap, (Args->SemaNativeFuncArgs.pData), &Args->SemaNativeFuncArgs.Size)) == EAPI_STATUS_SUCCESS)
		{
			switch (Args->SemaNativeFuncArgs.cap)
			{

			case EAPI_ID_BOARD_MANUFACTURER_STR:
			{
				printf("\nBoard manufacturer name: %s\n", Args->SemaNativeFuncArgs.pData);
			}break;

			case EAPI_ID_BOARD_NAME_STR:
			{
				printf("\nBoard name: %s\n", Args->SemaNativeFuncArgs.pData);
			}break;

			case EAPI_ID_BOARD_SERIAL_STR:
			{
				printf("\nBoard serial number: %s\n", Args->SemaNativeFuncArgs.pData);
			}
			break;

			case EAPI_ID_BOARD_BIOS_REVISION_STR:
			{
				printf("\nBoard BIOS revision: %s\n", Args->SemaNativeFuncArgs.pData);
			}
			break;

			case EAPI_ID_BOARD_HW_REVISION_STR:
			{
				printf("\nHW revision: %s\n", Args->SemaNativeFuncArgs.pData);
			}
			break;

			case EAPI_ID_BOARD_PLATFORM_TYPE_STR:
			{
				printf("\nBoard platform type: %s\n", Args->SemaNativeFuncArgs.pData);
			}
			break;

			case EAPI_SEMA_ID_BOARD_BOOT_VERSION_STR:
			{
				printf("\nBMC Bootloader revision: %s\n", Args->SemaNativeFuncArgs.pData);
			}
			break;

			case EAPI_SEMA_ID_BOARD_APPLICATION_VERSION_STR:
			{
				printf("\nBMC application revision: %s\n", Args->SemaNativeFuncArgs.pData);
			}
			break;

			case EAPI_SEMA_ID_BOARD_RESTART_EVENT_STR:
			{
				printf("\nBoard restart event: %s\n", Args->SemaNativeFuncArgs.pData);
			}
			break;

			case EAPI_SEMA_ID_BOARD_REPAIR_DATE_STR:
			{
				printf("\nBoard repair date: %s\n", Args->SemaNativeFuncArgs.pData);
			}
			break;

			case EAPI_SEMA_ID_BOARD_MANUFACTURE_DATE_STR:
			{
				printf("\nBoard manufacturer date: %s\n", Args->SemaNativeFuncArgs.pData);
			}
			break;

			case EAPI_SEMA_ID_BOARD_MAC_1_STRING:
			{
				printf("\nBoard MAC address 1: %s\n", Args->SemaNativeFuncArgs.pData);
			}
			break;

			case EAPI_SEMA_ID_BOARD_MAC_2_STRING:
			{
				printf("\nBoard MAC address 2: %s\n", Args->SemaNativeFuncArgs.pData);
			}
			break;

			case EAPI_SEMA_ID_BOARD_2ND_HW_REVISION_STR:
			{
				printf("\nBoard 2nd HW revision number: %s\n", Args->SemaNativeFuncArgs.pData);
			}
			break;

			case EAPI_SEMA_ID_BOARD_2ND_SERIAL_STR:
			{
				printf("\nBoard 2nd serial number: %s\n", Args->SemaNativeFuncArgs.pData);
			}
			break;
			default:
				printf("\n%s\n", Args->SemaNativeFuncArgs.pData);
				break;
			}
		}
		return sts;
	}

	if (Args->GetErrorLog)
	{
		uint32_t firstErrNum, SecondErrNum, currentErrNum;

		if ((sts = SemaEApiBoardGetCurErrorLog(&(Args->ErrorNExceFuncArgs.ErrorNumber), &(Args->ErrorNExceFuncArgs.Flags), &(Args->ErrorNExceFuncArgs.RestartEvent),\
			&(Args->ErrorNExceFuncArgs.PwrCycles), &(Args->ErrorNExceFuncArgs.Bootcount), &(Args->ErrorNExceFuncArgs.Time), &(Args->ErrorNExceFuncArgs.TotalOnTime),\
			&(Args->ErrorNExceFuncArgs.BiosSel), &(Args->ErrorNExceFuncArgs.Status), &(Args->ErrorNExceFuncArgs.CPUtemp), &(Args->ErrorNExceFuncArgs.Boardtemp))) == EAPI_STATUS_SUCCESS)
		{
			currentErrNum = Args->ErrorNExceFuncArgs.ErrorNumber;
			
			if ((sts = SemaEApiBoardGetErrorLog(0, &(Args->ErrorNExceFuncArgs.ErrorNumber), &(Args->ErrorNExceFuncArgs.Flags), &(Args->ErrorNExceFuncArgs.RestartEvent),\
				&(Args->ErrorNExceFuncArgs.PwrCycles), &(Args->ErrorNExceFuncArgs.Bootcount), &(Args->ErrorNExceFuncArgs.Time), &(Args->ErrorNExceFuncArgs.TotalOnTime),\
				&(Args->ErrorNExceFuncArgs.BiosSel), &(Args->ErrorNExceFuncArgs.Status), &(Args->ErrorNExceFuncArgs.CPUtemp), &(Args->ErrorNExceFuncArgs.Boardtemp))) == EAPI_STATUS_SUCCESS)
			{
				firstErrNum = Args->ErrorNExceFuncArgs.ErrorNumber;
				if ((sts = SemaEApiBoardGetErrorLog(1, &(Args->ErrorNExceFuncArgs.ErrorNumber), &(Args->ErrorNExceFuncArgs.Flags), &(Args->ErrorNExceFuncArgs.RestartEvent)\
					, &(Args->ErrorNExceFuncArgs.PwrCycles), &(Args->ErrorNExceFuncArgs.Bootcount), &(Args->ErrorNExceFuncArgs.Time), &(Args->ErrorNExceFuncArgs.TotalOnTime),\
					&(Args->ErrorNExceFuncArgs.BiosSel), &(Args->ErrorNExceFuncArgs.Status), &(Args->ErrorNExceFuncArgs.CPUtemp), &(Args->ErrorNExceFuncArgs.Boardtemp))) == EAPI_STATUS_SUCCESS)
				{
					SecondErrNum = Args->ErrorNExceFuncArgs.ErrorNumber;
					
					if ((sts = SemaEApiBoardGetErrorLog(Args->ErrorNExceFuncArgs.Pos, &(Args->ErrorNExceFuncArgs.ErrorNumber), &(Args->ErrorNExceFuncArgs.Flags),\
						&(Args->ErrorNExceFuncArgs.RestartEvent), &(Args->ErrorNExceFuncArgs.PwrCycles), &(Args->ErrorNExceFuncArgs.Bootcount), &(Args->ErrorNExceFuncArgs.Time),\
						&(Args->ErrorNExceFuncArgs.TotalOnTime), &(Args->ErrorNExceFuncArgs.BiosSel), &(Args->ErrorNExceFuncArgs.Status), &(Args->ErrorNExceFuncArgs.CPUtemp), &(Args->ErrorNExceFuncArgs.Boardtemp))) == EAPI_STATUS_SUCCESS)
					{
						if ((firstErrNum > SecondErrNum &&
							(Args->ErrorNExceFuncArgs.ErrorNumber + Args->ErrorNExceFuncArgs.Pos) == firstErrNum) ||
							(firstErrNum < SecondErrNum &&
							(Args->ErrorNExceFuncArgs.ErrorNumber - Args->ErrorNExceFuncArgs.Pos) == firstErrNum))
						{
							printf("ErrorNumber:\t%u\n", Args->ErrorNExceFuncArgs.ErrorNumber);
							printf("Flags:\t\t0x%x\n", Args->ErrorNExceFuncArgs.Flags);
							printf("RestartEvent:\t0x%x\n", Args->ErrorNExceFuncArgs.RestartEvent);
							printf("PowerCycle:\t%u\n", Args->ErrorNExceFuncArgs.PwrCycles);
							printf("BootCount:\t%u\n", Args->ErrorNExceFuncArgs.Bootcount);
							printf("Time:\t\t%u seconds \n", Args->ErrorNExceFuncArgs.Time);
							printf("TotalOnTime:\t%u minutes\n", Args->ErrorNExceFuncArgs.TotalOnTime);
							printf("BIOS Sel:\t0x%x\n", Args->ErrorNExceFuncArgs.BiosSel);
							printf("Status:\t\t0x%x\n", Args->ErrorNExceFuncArgs.Status);
							printf("CPUTemp:\t%d C\n", Args->ErrorNExceFuncArgs.CPUtemp);
							printf("BoardTemp:\t%d C\n", Args->ErrorNExceFuncArgs.Boardtemp);
						}
						else
						{
							int pos = currentErrNum -= firstErrNum;
							if (pos < 1)
								pos = pos * -1;
							SemaEApiBoardGetErrorLog(pos - 1, &(Args->ErrorNExceFuncArgs.ErrorNumber), &(Args->ErrorNExceFuncArgs.Flags), &(Args->ErrorNExceFuncArgs.RestartEvent),\
								&(Args->ErrorNExceFuncArgs.PwrCycles),	&(Args->ErrorNExceFuncArgs.Bootcount), &(Args->ErrorNExceFuncArgs.Time), \
								&(Args->ErrorNExceFuncArgs.TotalOnTime), &(Args->ErrorNExceFuncArgs.BiosSel), &(Args->ErrorNExceFuncArgs.Status), &(Args->ErrorNExceFuncArgs.CPUtemp), &(Args->ErrorNExceFuncArgs.Boardtemp));
							printf("\nInvalid Error Number\n");
						}
					}
				}
			}
		}
		return sts;
	}

	if (Args->GetCurrentPosErrorLog)
	{
		if ((sts = SemaEApiBoardGetCurErrorLog(&(Args->ErrorNExceFuncArgs.ErrorNumber), &(Args->ErrorNExceFuncArgs.Flags), &(Args->ErrorNExceFuncArgs.RestartEvent),\
			&(Args->ErrorNExceFuncArgs.PwrCycles), &(Args->ErrorNExceFuncArgs.Bootcount), &(Args->ErrorNExceFuncArgs.Time), &(Args->ErrorNExceFuncArgs.TotalOnTime),\
			&(Args->ErrorNExceFuncArgs.BiosSel), &(Args->ErrorNExceFuncArgs.Status), &(Args->ErrorNExceFuncArgs.CPUtemp), &(Args->ErrorNExceFuncArgs.Boardtemp))) == EAPI_STATUS_SUCCESS)
		{
			printf("ErrorNumber:\t%u\n", Args->ErrorNExceFuncArgs.ErrorNumber);
			printf("Flags:\t\t0x%x\n", Args->ErrorNExceFuncArgs.Flags);
			printf("RestartEvent:\t0x%x\n", Args->ErrorNExceFuncArgs.RestartEvent);
			printf("PowerCycle:\t%u\n", Args->ErrorNExceFuncArgs.PwrCycles);
			printf("BootCount:\t%u\n", Args->ErrorNExceFuncArgs.Bootcount);
			printf("Time:\t\t%u seconds \n", Args->ErrorNExceFuncArgs.Time);
			printf("TotalOnTime:\t%u minutes\n", Args->ErrorNExceFuncArgs.TotalOnTime);
			printf("BIOS Sel:\t0x%x\n", Args->ErrorNExceFuncArgs.BiosSel);
			printf("Status:\t\t0x%x\n", Args->ErrorNExceFuncArgs.Status);
			printf("CPUTemp:\t%d C\n", Args->ErrorNExceFuncArgs.CPUtemp);
			printf("BoardTemp:\t%d C\n", Args->ErrorNExceFuncArgs.Boardtemp);
		}
		return sts;
	}

	if (Args->GetErrorNumberDescription)
	{
		unsigned char buffer[50] = { 0 };
		if ((sts = SemaEApiBoardGetErrorNumberDescription(Args->ErrorNExceFuncArgs.ErrorNumber, buffer)) == EAPI_STATUS_SUCCESS)
		{
			printf("\n%02d  -> %s\n", Args->ErrorNExceFuncArgs.ErrorNumber, buffer);
		}
		return sts;
	}
	
	if (Args->GetExceptionDescription)
	{
		int i, valid = 0;
		for (i = 0; i < 32; i++)
		{
			unsigned char buffer[50] = { 0 };
			if ((sts = SemaEApiBoardGetExceptionDescription(i, buffer)) == EAPI_STATUS_SUCCESS)
			{
				valid = 1;
				if (i == 0)
				{
					printf("\n");
				}
				printf("%02d  -> %s\n", i, buffer);
			}
			else if(sts == EAPI_STATUS_UNSUPPORTED)
			{
				return EAPI_STATUS_UNSUPPORTED;
			}
		}
		if (valid == 1)
		{
			return EAPI_STATUS_SUCCESS;
		}
		return sts;
	}

	if (Args->GetVoltageMonitor)
	{
		char buffer[50] = { 0 };
		if ((sts = SemaEApiBoardGetVoltageMonitor(Args->SemaNativeFuncArgs.channel, &(Args->SemaNativeFuncArgs.voltage), buffer)) == EAPI_STATUS_SUCCESS)
		{
			if (strstr(buffer, "Current") != 0)
				printf("\nCurrent : %d mA \nDescription : %s\n", Args->SemaNativeFuncArgs.voltage, buffer);
			else
				printf("\nVoltage : %d mv \nDescription : %s\n", Args->SemaNativeFuncArgs.voltage, buffer);
		}
		return sts;
	}

	char* list[] = { "EAPI_ID_I2C_EXTERNAL_1", "EAPI_ID_I2C_EXTERNAL_2", "EAPI_ID_I2C_EXTERNAL_3", "EAPI_ID_I2C_EXTERNAL_4" };

	if (Args->IsI2CCap)
	{
		int i;
		uint32_t pMaxBlkLen;
		printf("\n");
		for (i = 0; i < 4; i++)
		{
			if ((sts = EApiI2CGetBusCap(i, &pMaxBlkLen)) == EAPI_STATUS_SUCCESS)
			{
				printf("%s is supported. Max read size %d bytes. Max write size %d bytes.\n", list[i], pMaxBlkLen, 29);
			}
			else
			{
				printf("%s is not supported\n", list[i]);
			}
		}
		sts = EAPI_STATUS_SUCCESS;
		return sts;
	}

	if (Args->IsI2CProb)
	{
		volatile int i, once = 1, j = 1;
		uint32_t MaxBlkSize;

		if (EApiI2CGetBusCap(Args->I2CFuncArgs.BusID - 1, &MaxBlkSize) != EAPI_STATUS_SUCCESS) {
			
			return EAPI_STATUS_UNSUPPORTED;
		}
		for (i = 1; i <=127 ; i++)
		{
			if ((sts = EApiI2CProbeDevice(Args->I2CFuncArgs.BusID - 1, i)) == EAPI_STATUS_SUCCESS)
			{
				if (once == 1)
				{

					printf("\nSlave list in %s bus:\n", list[Args->I2CFuncArgs.BusID - 1]);
					once = 0;
				}

				printf("%02d. %x\n",j++, i);
			}
			fflush(stdout);
			fflush(stdin);
			fflush(stderr);
		}
		if (once == 0)
		{
			return EAPI_STATUS_SUCCESS;
		}
		else
		{
			printf("\nNo slave devices found on %s bus\n", list[Args->I2CFuncArgs.BusID - 1]);
			return EAPI_STATUS_SUCCESS;
		}
	}

	if (Args->IsI2CSts)
	{
		uint32_t status;
		if ((sts = SemaEApiI2CGetBusSts(Args->I2CFuncArgs.BusID, &status)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nThe bus status is 0x%02x\n", status);
		}
		return sts;

	}

	if (Args->IsI2CReXf)
	{
		 if ((sts = EApiI2CReadTransfer(Args->I2CFuncArgs.BusID - 1, Args->I2CFuncArgs.Address, Args->I2CFuncArgs.cmd, Args->I2CFuncArgs.pBuffer, Args->I2CFuncArgs.nByteCnt, Args->I2CFuncArgs.nByteCnt)) == EAPI_STATUS_SUCCESS)
		 {	
			unsigned int i;
			printf("Read data:\n\n");
			for (i = 0; i < Args->I2CFuncArgs.nByteCnt; i++)
			{
				printf("%02d : %02x\n", i, ((unsigned char*)(Args->I2CFuncArgs.pBuffer))[i]);
			}
		 }
		return sts;
	}

	if (Args->IsI2CWrXf)
	{
		if ((sts = EApiI2CWriteTransfer(Args->I2CFuncArgs.BusID - 1, Args->I2CFuncArgs.Address, Args->I2CFuncArgs.cmd, Args->I2CFuncArgs.pBuffer, Args->I2CFuncArgs.nByteCnt)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nThe I2C Write Transfer command is completed\n");
		}
		return sts;
	}

	if (Args->IsI2CReRaw)
	{
		if((sts = EApiI2CWriteReadRaw(Args->I2CFuncArgs.BusID - 1, Args->I2CFuncArgs.Address, Args->I2CFuncArgs.pWrBuffer, Args->I2CFuncArgs.WByteCnt, Args->I2CFuncArgs.pBuffer, Args->I2CFuncArgs.nByteCnt, Args->I2CFuncArgs.nByteCnt)) == EAPI_STATUS_SUCCESS)
		{
			unsigned int i;
			printf("Read data:\n\n");
			for (i = 0; i < Args->I2CFuncArgs.nByteCnt; i++)
			{
				printf("%02d : %02x\n", i, ((unsigned char*)(Args->I2CFuncArgs.pBuffer))[i]);
			}
		}
		return sts;
	}
	
	if (Args->IsI2CWrRaw)
	{ 
		if ((sts = EApiI2CWriteReadRaw(Args->I2CFuncArgs.BusID - 1, Args->I2CFuncArgs.Address, Args->I2CFuncArgs.pWrBuffer, Args->I2CFuncArgs.WByteCnt, Args->I2CFuncArgs.pBuffer, Args->I2CFuncArgs.nByteCnt, Args->I2CFuncArgs.nByteCnt)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nThe I2C Write command is completed\n");
		}
		return sts;
	}

	if (Args->GetBIOSSource)
	{
		uint8_t bit0, bit1;
		if ((sts = SemaEApiGetBIOSSource(&bit0, &bit1)) == EAPI_STATUS_SUCCESS)
		{
			printf("Bit0  Bit1\n");
			switch (bit1 << 1 | bit0)
			{
			case 0:
				printf(" 0     0  - By hardware configuration of currently selected BIOS\n");
				break;
			case 1:
				printf(" 0     1  - Switch to Fail - Safe BIOS\n");
				break;
			case 2:
				printf(" 1     0  - Switch to External BIOS(SPI0 on carrier)\n");
				break;
			case 3:
				printf(" 1     1  - Switch to Internal BIOS(SPI0 on module)\n");
				break;
			}
		}
	}

	if (Args->GetBIOSSourceSts)
	{
		uint8_t bit0, bit1, bit2;
		char *printlist[] = {
		" 0     0     0  - *M0 Module SPI0 / C1 Carrier SPI1 (Standard BIOS)",
		" 0     0     1  - *C0 Carrier SPI0 / M1 Module SPI1 (Fail-Safe BIOS)",
		" 0     1     0  - Not PICMG Mode / Unknown",
		" 0     1     1  - *M0/M1 (Standard BIOS",
		" 1     0     0  - Not Dual BIOS Mode / Unknown",
		" 1     0     1  - Switch to Fail - Safe BIOS",
		" 1     1     0  - Switch to External BIOS(SPI0 on carrier)",
		" 1     1     1  - Switch to Internal BIOS(SPI0 on module)",
		};

		if ((sts = SemaEApiGetBIOSSourceSts(&bit0, &bit1, &bit2)) == EAPI_STATUS_SUCCESS)
		{
			char buffer[200] = { 0 };
			int i;
			if (bit2 == 1)
			{
				printf("\n Dual BIOS Selected.\n");
			}
			else
			{
				printf("\n PICMG BIOS Selected.\n");
			}
			printf("\n    Bit2  Bit1  Bit0\n");
			for (i = 0; i < 8; i++)
			{
				memset(buffer, 0, 200);
				if (i == (bit2 << 2 | bit1 << 1 | bit0))
				{
					strcat_s(buffer, 200, " -> ");
				}
				else
				{
					strcat_s(buffer, 200, "    ");
				}
				strcat_s(buffer, 200, printlist[i]);
				printf("%s\n", buffer);
			}
		}

		if (sts == EAPI_STATUS_UNSUPPORTED)
		{
			printf("\nOperation is not supported on this platform\n");
			return 0;
		}
	}

	if (Args->SetBIOSSource)
	{
		if ((sts = SemaEApiSetBIOSSource(Args->BIOSSourceArgs.bit0, Args->BIOSSourceArgs.bit1)) == EAPI_STATUS_SUCCESS)
		{
			printf("\nBIOS Boot source mode set successfully\n");
		}
	}

	if (Args->GUIDWrite)
	{
		uint32_t Id = 3;
		uint32_t Address = 0x100;
		uint32_t permission = 2;
		uint32_t Length = 16;
		char passcode[7] = { "0xADEC" };

		GUID guid;
		CoCreateGuid(&guid);
		WCHAR guidString[40];
		char buffer[32] = { 0 };
		uint8_t guidbyte[16] = { 0 };

		StringFromGUID2(guid, guidString, sizeof(guidString) / sizeof(WCHAR));
		wprintf(L"\nGenerated GUID: %s\n", guidString);

		int j = 0;
		for (int i = 0; i < 37; i++) {
			if (isxdigit(guidString[i])) {
				buffer[j] = guidString[i];
				j = j + 1;
			}
		}
		sts = guid_string_to_bytes(buffer, guidbyte);

		if ((sts = SemaEApiStorageUnlock(Id, permission, passcode)) == EAPI_STATUS_SUCCESS)
		{
			if ((sts = EApiGUIDWrite(Id, Address, guidbyte, Length)) == EAPI_STATUS_SUCCESS) // Secure region
			{
				printf("\nGUID Written Successfully \n");
			}
			sts = SemaEApiStorageLock(Id);
		}
		return sts;
	}

	if (Args->GUIDRead)
	{
		uint32_t Id = 3;
		uint32_t Address = 0x100;
		uint32_t permission = 2;
		uint32_t Length = 16, i;
		char passcode[7] = { "0xADEC" };

		unsigned char* Data = (unsigned char*)calloc(sizeof(unsigned char), Length + 10 + 2 * BlockLength);
		if ((sts = SemaEApiStorageUnlock(Id, permission, passcode)) == EAPI_STATUS_SUCCESS)
		{
			if ((sts = EApiStorageHexRead(Id, Address, Data, Length + 2 * BlockLength, Length)) == EAPI_STATUS_SUCCESS)
			{
				printf("\nRead Buffer : ");
				for (i = 0; i < Length; i++)
				{
					printf("0x%02X ", Data[i]);
				}
				printf("\n");
			}
			sts = SemaEApiStorageLock(Id);
		}
		return sts;
	}

	return sts;
}

int main(int argc, char* argv[])
{
	int i = 0;
	uint32_t sts;
	tCmdLineArgs CmdLineArgs;

	if ((sts = EApiLibInitialize()) != EAPI_STATUS_INITIALIZED)
	{
		if (sts == EAPI_STATUS_NOT_FOUND)
		{
			printf("\nBMC or Driver not Found\n");
		}
		else
		{
			printf("\nEapi Library not initialized\n");
		}
		return -1;
	}

	memset(&CmdLineArgs, 0, sizeof(tCmdLineArgs));
	if (ParseArgs(argc, argv, &CmdLineArgs) < 0)
	{
		return -1;
	}

	sts = DispatchCMDToSEMA(&CmdLineArgs);

	switch (sts)
	{
	case EAPI_STATUS_SUCCESS:            
		break;
	case EAPI_STATUS_UNSUPPORTED:
		printf("\nFailed : Unsupported function.\n");
		break;
	case EAPI_STATUS_INVALID_PARAMETER:
		printf("\nFailed : Input data not valid.\n");
		break;
	case EAPI_STATUS_NOT_INITIALIZED:
		printf("\nFailed : Library not initialized\n");
		break;
	case EAPI_STATUS_ERROR:
		printf("\nFailed : SEMA transaction failed\n");
		break;
	case EAPI_STATUS_INVALID_BLOCK_LENGTH:
		printf("\nFailed : Invalid block size\n");
		break;
	case EAPI_STATUS_INVALID_BLOCK_ALIGNMENT:
		printf("\nFailed : Invalid block alignment\n");
		break;
	case EAPI_STATUS_NOT_FOUND:
		printf("\nFailed : Function not found\n");
		break;
	case EAPI_STATUS_MORE_DATA:
		printf("\nFailed : Unexpected more data\n");
		break;
	case EAPI_STATUS_INVALID_BITMASK:
		printf("\nFailed : Invalid bitmask\n");
		break;
	case EAPI_STATUS_INVALID_DIRECTION:
		printf("\nFailed : Invalid Direction\n");
		break;
	case EAPI_STATUS_ALLOC_ERROR:
		printf("\nFailed : Allocation error\n");
		break;
	case EAPI_STATUS_READ_ERROR:
		printf("\nFailed : Read error\n");
		break;
	case EAPI_STATUS_WRITE_ERROR:
		printf("\nFailed : Write error\n");
		break;
	case EAPI_STATUS_TIMEOUT:
		printf("\nFailed : Timeout\n");
		break;
	case EAPI_STATUS_BUSY_COLLISION:
		printf("\nFailed BMC is busy\n");
		break;
	case EAPI_STATUS_RUNNING:
		printf("\nFailed : Already running\n");
		break;
	case EAPI_STATUS_DRIVER_TIMEOUT:
		printf("\nFailed : Driver timeout\n");
		break;
	default:
		printf("\nFailed: Unknown\n");
		break;
	}	

	EApiUnInitialize();

	if (sts == EAPI_STATUS_SUCCESS)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}