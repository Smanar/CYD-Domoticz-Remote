WIP project to control Domoticz with a CYD.

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
- ???
- Just launch a compilation (Build) and upload (Upload) the code on the device using the USB cable. Don't forget to short press the button (Boot) on the device to autorie tha action.
- On first launch, configure the Wifi, the domoticz IP and port, and on setting select the devices you want o display on the Homepage.
   
   
For ther moment, need to edit code itself to set defaut devices, the edit box is here, but not possible to edit value.

## Needed settings   

The project is still WIP so some settings are still hard coded.   
- Somes settings depend of hardware, tell me your hardware, I will search values to edit.
- To go faster somes settings are hard coded here https://github.com/Smanar/CYD-Domoticz-Remote/blob/main/Domoticz_Remote/src/main.cpp#L34
- The devices you want by defaut on the homepage are here https://github.com/Smanar/CYD-Domoticz-Remote/blob/main/Domoticz_Remote/src/main.cpp#L14   

## Actuals issues   
- Memory ^^, lot of feature are still in Beta test. For exemple Domoticz JSON are too big to be handled by the device, it cause problem for chart for exemple.

  
