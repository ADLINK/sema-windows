# sema-windows 4.2.x
This is the common repository for SEMA Windows supporting EC and TIVA BMC platforms

## SEMA Overview
* **SEMA (Smart Embedded Management Agent)** is ADLINK’s embedded system management framework that provides a unified interface for monitoring and controlling hardware features on supported embedded platforms.

* Implements the **PICMG Embedded API (EAPI)** standard to ensure consistent hardware access across different ADLINK boards and modules.

* Serves as an abstraction layer between the operating system and low-level board controllers such as the **Embedded Controller (EC)** or **Board Management Controller (BMC)**.

* Enables standardized access to platform hardware resources including thermal sensors, GPIO, I²C/SMBus devices, watchdog timers, fan controllers, voltage monitoring, and platform information.

* Includes drivers, EAPI libraries and utility for hardware monitoring, system health management, peripheral communication, and embedded platform control on Windows systems.


## Repo Files Overview

* Bin – Includes compiled binaries for the 64-bit environment such as the SEMA driver, Dynamic Link Library (SemaEAPI.dll) and sample utility (semautil.exe).
* Installer – Contains the Windows installer package for deploying SEMA drivers, libraries, and utilities on supported ADLINK embedded platforms.
* Src – Contains the source code for the SEMA framework, hardware access APIs, and supporting components.
* LICENSE.BSD3 – BSD 3-Clause License file.
* README.md – Documentation describing the project overview, build steps, and usage instructions.

## Supported Hardware List
* cExpress-TL , cExpress-EL, cExpress-AR, cExpress-ALN/ASL, cExpress-MTL, cExpress-RLP, cExpress-R8
* cExpress-AL, cExpress-SL, cExpress-KL, cExpress-WL
* Express-ID7, Express-ADP, Express-TL
* Express-CF, Express-BD7, Express-DN7, Express-CFR, Express-SL2, Express-SL/KL
* COM-HPC-cRLS, COM-HPC-sIDH, COM-HPC-mMTL, COM-HPC-cADP
* NanoX-EL, NanoX-BT, NanoX-ASL
* Q7-EL, Q7-AL, ETX-BT
* Express-VR7
* LEC-EL, LEC-AL, LEC-ASL/ALN
  
## Supported Operating System
* Windows 10 (64-bit)
* Windows 11 (64-bit)

## Documents
Refer to this ([link](https://www.adlinktech.com/Products/DownloadMDownload?lang=en&pdNo=1274&MainCategory=Industrial_IoT_and_Cloud_solutions&kind=M)) for the following guides,
- SEMA Installation Guide
- SEMA App User Guide
- SEMA EAPI Guide





