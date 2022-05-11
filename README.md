## Introduction



<img src="https://cdn.adlinktech.com/webupd/en/Upload/ProductNews/logo_sema.png" alt="sema_logo" width="30%" align="right"  />

### What is SEMA 4.0?

**The Smart Embedded Management Agent (SEMA®)** 

Downtime of devices or systems is not acceptable in today's industries. To help customers to analyze their 
systems and take counter measures for preventive maintenance, ADLINK Technology Inc. has developed a tool which is able to monitor and collect system performance and status information from the hardware in a timely, flexible and precise manner. A Board Management Controller collects all relevant technical information from the chipset and other sources.

Using the System Management Bus driver, an application layer fetches the data and presents it to the user. 
SEMA® provides a ready-made application that shows the data in user-friendly graphic interfaces, suitable 
for supervision and troubleshooting.


<br>

Release Update
--------------

* v4.2.0 (https://github.com/ADLINK/sema-windows/releases/tag/v4.2.0)
  * This release is only supported for the following hardware:
    - cExpress-TL / cExpress-EL / cExpress-AR
    - Express-ID7 / Express-ADP / Express-TL
    - COM-HPC-cADP / COM-HPC-sIDH
    - LEC-EL
    - nanoX-EL

Note: Working on the integration of the different the board controllers (EC, TivaBMC, LiteBMC) into the same framework.

 


<br>


Feature Set
----------

SEMA® is designed to be:

* Power Consumption
* User Area Access
* I2C Control 
* Temperatures (CPU and Board)
* Board Information (Serial Number, Part Number, Firmware Version...)
* Fan Control
* GPIO Control (only support PCA9535 I/O expander)
* Watch Dog  


Detailed forensic information is available after system or module failures. The BMC Power-Up Error Log function provides detailed information about history of failures that may have occurred during power-up sequences. Log information includes e.g. error number, flags, restart event, power cycles, boot count, status, CPU temperature and board temperature. Moreover minimum and maximum temperature of the CPU and system is available to analyze system or module failure in detail.


<br>

## how to install:
* see [documentation](https://adlinktech.github.io/sema-doc/#/source/HowToInstallSEMA?id=windows-10-64bit) for more details


## Supported Hardware List:
* see [the hardware list](https://adlinktech.github.io/sema-doc/#/source/SupportedHardware) for more details


## Developer's Guide: 
* See [documentation](https://adlinktech.github.io/sema-doc/#/source/DeveloperGuide) for more details.



   
