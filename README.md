# Powermanager IoT

**Powermanager IoT** is a modification to the the Powermanager from the electronics shop www.conrad.de which allows you to switch power jacks via a HTML web server based on an arduino mega. This is my first project on GitHub and my first Arduino project, so be indulgent. :-)

See the [Project page](https://www.fam-moser.de/blog/kmosers-tech-blog/computer/iot/power-manager-iot.html) (german) for more information about the needed hardware.

## Installation
Copy the **ext_def_example.h** to **ext_def.h** and adapt it to your needs.

| Setting | Description |
| --- | --- |
| myip | Desired IP of the Powermanager |
| gwip | IP of your internet router |
| dnsip | Your DNS server |
| mymac | Desired MAC adress of the Powermanager |
| fhemIp | IP adress of the FHEM server |
| website | URL to the FHEM server. Needed for update GET requests |
| authHeader | Basic Auth credentials to access FHEM |
| devicePrefix | Devicename used in Fhem |

To build the setting for **authHeader** just put username and password for your Fhem installation in one line, seperated by a colon and use [base64](https://www.base64encode.org/) to encode the string. The setting in the example file defaults to *admin:password*.

### Used Libraries

The Sektch ueses the [EtherCard Library](https://github.com/jcw/ethercard), so you need to download and install it.

## Physical Installation

### PIN Connections

	Relais Pins - 22,23,24,25,26,27,28,29
	Button Pins - 30,31,32,33,34,35,36,37
	LED Pins    - 2,3,4,5,6,7,8,9
    LM35        - 12
    ENC28J60    - 50,51,52,53

## Compiling
Choose **Arduino/Genuino Mega or Mega 2560** as board type in the Arduino IDE.

## Support

For questions and help use the issue tracker on [GitHub](https://github.com/kmoser77/powermanager_iot/issues) or contact me via contact form on [www.fam-moser.de](https://www.fam-moser.de/navigation/service/kontakt.html)