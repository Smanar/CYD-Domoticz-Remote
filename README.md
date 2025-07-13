Small project to control your home-automation using Domoticz and a CYD (Cheap touchscreen).

There is a video here https://www.youtube.com/watch?v=1Lc6MlZjePM

## Description   

To make short I was searching a small wall mounted touchscreen to control my home, with low consumption enought to be powered all the time.   
It use Wifi connection to connect direclty to domoticz, to ask information and use the websocket connexion to have real time event, so nothing to add. Just power the device using USB, give the Domoticz IP the wifi password and it's all.   
The project is still WIP so somes settings are hard coded on the device, for exemple wich one devices will be displayed on the homepage, but it already work in stand alone mode.   

<p align="center">
  <img src="https://github.com/Smanar/Ressources/blob/main/pictures/CYD_1.jpg">
</br>
  <img src="https://github.com/Smanar/Ressources/blob/main/pictures/Display_4x4_level.jpg">
</br>
  <img src="https://github.com/Smanar/Ressources/blob/main/pictures/domoticz_dashboard.jpg">
</p>

You can found more recents captures on the official topic here https://www.domoticz.com/forum/viewtopic.php?t=41232


## Installation   

There is different way to install the code on th device, but I think this one is the easier.   

- Buy a CYD on Ali Express, you can found differents one, different size, different form, different technologies, you can found somes of them here https://github.com/rzeldent/platformio-espressif32-sunton
- Install device drivers, for exemple here https://www.wch-ic.com/downloads/CH341SER_EXE.html   
- Donwload Visual Studio Code https://code.visualstudio.com/    
- Install PlatformIO on it https://docs.platformio.org/en/latest/integration/ide/vscode.html#installation   
- Download the project and open it with Visual Studio Code.
- Take a look in the platformio.ini file (you can found usefull information/setting in it).   
- Configure your device. ATM the project is configured for the 2432S028R, so you need to create a board configuration file in the "boards" folder (https://github.com/Smanar/CYD-Domoticz-Remote/tree/main/boards) and set it in the platformio.ini file (l14). You can find information to make it on google, for exemple on the first link of this part, else just ask me, I can try to make one.   
- ???
- Just launch a compilation (Build) and upload (Upload) the code on the device using the USB cable. Don't forget to press the button (Boot) on the device to autorise the action.
- On first launch, configure the Wifi, the domoticz IP and port, and on setting select the devices you want o display on the Homepage.   

## Settings   

The project is still WIP so some settings are still hard coded.   
Somes settings depend of hardware, tell me your hardware, I will search values to edit.   

There is 3 ways to configure the software settings (wifi, device id, and ect ...):   
- Can use the option FORCE_CONFIG and use hardcoded setting here https://github.com/Smanar/CYD-Domoticz-Remote/blob/main/src/main.cpp#L66
- You can start the device and configure setting using the GUI.
- You can use a personnal_settings.h file placed on the project with your settings.

By defaut, domoticz defaut devices displayed on Homepage will be 1,2,3,4,5,6,7,8,9 so you need to change them on the application setting panel.   

For the hardware part (according to your device) mains settings are in the file platformio.ini, by defaut the project is configured for the esp32-2432S028R

``` 
	#-DFORCE_CONFIG # Enable or disable the forced network configuration.
	-DFORCECALIBRATE=false # Reset the calibration at every restart, used to reset it after a config change.
	#-DOLD_DOMOTICZ # If you are using older Domoticz version < 2023.2
	-DBONUSPAGE=0 # Special page to add, similar to Homepage but not updated, from 1 to 3
	-DFASTCLIC # Enable 1 clic action for some widget
	#-DNO_INFO_PAGE # Disable the Info panel
	#-DNO_GROUP_PAGE # Disable the Group/scene panel
	#-DCUSTOM_FONT
	#-DAUTO_BRIGHTNESS
	#-DLIGHTWS # Only possible for version > 16088, decrease WS requests.
	#-DPUSHOTA # To enable PUSH OTA (Don't enable both OTA)
	#-DPULLOTA # To enable PULL OTA (Don't enable both OTA)
``` 

## OTA
There is 2 options for OTA, PUSHOTA and PULLOTA you can enable one of them or nothing (not both ofc).

- PUSHOTA Enable the OTA in PUSH mode, it mean there is a webserver that run permanently, you can access it using /update as url.
- PULLOTA can be used using the GUI, on the device itself, using the first tab, it will download itself the new firmware, but for the moment the url is hardcoded. https://github.com/Smanar/CYD-Domoticz-Remote/blob/main/src/core/ota.cpp#L15

## Tested devices, working nativley
- esp32-2432S028R
- esp32-2432S024C
- esp32-E32S3RGB43
- esp32-4848S040
- esp32-8048S043C
- esp32-8048S070C
- esp32-ZX7D00CE01S

## Actuals issues   
- Memory ^^, lot of feature are still in Beta test. For exemple Domoticz JSON are too big to be handled by the device, it cause problem for chart for exemple.
- The features availables will depend of user devices. The home page will always work, the websocket return too, for the rest IDK. Some JSON are realy impressive, can take some Mega for only 5% of data used, and not possible to reduce them, there is no setting in domoticz. I m using a filtering for some of them, it's possible too using smaller requests to have smaller JSON as return, for exemple this one [https://www.domoticz.com/wiki/Domoticz_ ... 2Fswitche](https://www.domoticz.com/wiki/Domoticz_API/JSON_URL%27s#Get_details_of_all_lights.2Fswitches) but this one don't have data, so it's possible to make a device list, but without information. I will see what to keep or not on usage.   

## troubleshooting    
- Be sure the device have its IP autorised in the Trusted Networks setting in Domoticz.   
- If you have problem with the Touchpad, you can force a calibration using the parameter -DFORCECALIBRATE=true. Else you will have it only at first launch.
     
