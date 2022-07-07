<br>

What is SEMA?
-----
<img src="https://cdn.adlinktech.com/webupd/en/Upload/ProductNews/logo_sema.png" alt="sema_logo" width="20%" align="right"  />


**The Smart Embedded Management Agent (SEMA®)**

Downtime of devices or systems is not acceptable in today's industries. To help customers to analyze their systems and take counter measures for preventive maintenance. We provide the solution which is able to monitor and collect system performance and status information from the hardware in a timely.

A Board Management Controller is embedded on our hardware and collects all relevant technical information from the chipset through the different communication interfaces such as eSPI, I2C and SMBus.

SEMA® middleware is on the top of Board controller that provides a ready-made application that shows the data in user-friendly graphic interfaces, suitable for supervision and troubleshooting.

Important Notice
-----
SEMA4.0 would be designed the abstraction layer to integrate the different controllers (EC, TivaBCC and LiteBMC) and this integration will be ready in Q4 2022.


Architecture Overview
-----


<img src="https://adlinktech.github.io/sema/index.assets/image-20220701234314926.png"  width="500" />

SEMA EAPI Library provides you the interface which can be accessed to the drivers and controllers
<br>
    <strong>Note:</strong> EAPI (Embedded API) library: PICMG® organization defined the software specification on COM Express for the industrial applications. Here is the available specification https://www.picmg.org/wp-content/uploads/COM_EAPI_R1_0.pdf for your reference.


Release Update
--------------

* **v4.2.1: click [here](https://github.com/ADLINK/sema-windows/releases/tag/v4.2.1) to download the latest release !**
  * This release is only supported for the following hardware:
    - cExpress-TL / cExpress-EL / cExpress-AR
    - Express-ID7 / Express-ADP / Express-TL
    - COM-HPC-cADP / COM-HPC-sIDH
    - LEC-EL
    - nanoX-EL

Note: Working on the integration of the different the board controllers (EC, TivaBMC, LiteBMC) into the same framework.

how to install:
----
* see [documentation](https://adlinktech.github.io/sema/HowToInstallonWindows.html) for more details

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

Developer's Guide: 
----
* See [documentation](https://adlinktech.github.io/sema/DeveloperGuide.html) for more details.



   
