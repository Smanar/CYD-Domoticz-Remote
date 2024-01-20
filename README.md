WIP project to control Domoticz with a CYD.

There is a video here https://www.youtube.com/watch?v=1Lc6MlZjePM

## Description   

To make short I was searching a small wall mounted touchscreen to control my home, with low consumption enought to be powered all the time.   
It use Wifi connection to connect direclty to domoticz, to ask information and use the websocket connexion to have real time event, so nothing to add. Just power the device using USB, give the Domoticz IP the wifi password and it's all.   
In reality the project is not finished so lot of settings are hard coded on the device, for exemple wich one devices will be displayed on the homepage, but it already work in stand alone mode.   

You can found some captures here https://www.domoticz.com/forum/viewtopic.php?t=41232

ATM, I m aking tries on the 2.8'' device a ESP32-2432S028R.

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
- On first launch, configure the Wifi, the domoticz IP and port, and on setting select the devices you want o display on the Homepage. (Take care this part is hardcoded ATM, see below)   
   
   
For ther moment, need to edit code itself to set defaut devices, the edit box is here, but not possible to edit value.

## Needed settings   

The project is still WIP so some settings are still hard coded.   
- Somes settings depend of hardware, tell me your hardware, I will search values to edit.
- To go faster somes settings are hard coded here https://github.com/Smanar/CYD-Domoticz-Remote/blob/main/Domoticz_Remote/src/main.cpp#L34
- The devices you want by defaut on the homepage are here https://github.com/Smanar/CYD-Domoticz-Remote/blob/main/Domoticz_Remote/src/main.cpp#L14   

## Actuals issues   
- Memory ^^, lot of feature are still in Beta test. For exemple Domoticz JSON are too big to be handled by the device, it cause problem for chart for exemple.
- The features availables will depend of user devices. The home page will always work, the websocket return too, for the rest IDK. Some JSON are realy impressive, can take some Mega for only 5% of data used, and not possible to reduce them, there is no setting in domoticz. I m using a filtering for some of them, it's possible too using smaller requests to have smaller JSON as return, for exemple this one [https://www.domoticz.com/wiki/Domoticz_ ... 2Fswitche](https://www.domoticz.com/wiki/Domoticz_API/JSON_URL%27s#Get_details_of_all_lights.2Fswitches) but this one don't have data, so it's possible to make a device list, but without information. I will see what to keep or not on usage.   

  
