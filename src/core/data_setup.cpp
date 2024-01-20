#include <TFT_eSPI.h>

#include "../src/conf/global_config.h"
#include "data_setup.h"
#include "../ui/http_setup.h"
#include "../ui/panels/panel.h"
#include "ip_engine.h"
#include "base64.hpp"
#include <ArduinoJson.h>

#define SIZEOF(arr) (sizeof(arr) / sizeof(*arr))

Device myDevices[9];

void RefreshHomePage(void);


void Init_data(void)
{
    for (int i = 0; i < MAXDEVICES; i = i + 1)
    {
        if (i < SIZEOF(global_config.ListDevices))
        {

            if (HttpInitDevice(&myDevices[i], global_config.ListDevices[i]))
            {
                myDevices[i].used = true;
                Serial.printf("Initialise Domoticz device id: %d , Name : %s\n", global_config.ListDevices[i], myDevices[i].name);
            }
        }
        else
        {
            myDevices[i].used = false;
        }
    }
}

void FillDeviceData(Device *d, int idx)
{
    if (HttpInitDevice(d, idx))
    {
        d->used = true;
        Serial.printf("Initialise Domoticz device id: %d , Name : %s\n", idx, d->name);
    }
}

int Get_ID_Device(int JSonidx)
{
    for (int i = 0; i < SIZEOF(myDevices); i = i + 1)
    {
        if (myDevices[i].idx == JSonidx)
        {
            return i;
        }
    }
    return -1;
}

void Update_data(JsonObject RJson2)
{

    //char buffer[4096];
    //serializeJsonPretty(RJson2, buffer);
    //Serial.println(buffer);

    const char* JSondata = NULL;
    int JSonidx = 0;
    int JSonLevel = 0;

    if (RJson2.containsKey("Level")) JSonLevel = RJson2["Level"];
    if (RJson2.containsKey("idx")) JSonidx = atoi(RJson2["idx"]);
    if (RJson2.containsKey("Data")) JSondata = RJson2["Data"];

    if (JSonidx == 0) return; // No Idx

    int ID = Get_ID_Device(JSonidx);
    if (ID < 0) return ; // Not in list

    Serial.printf("Update HP device id: %d\n", ID);

    //some cleaning
    if (strncmp(JSondata, "Humidity ", 9) == 0) JSondata+=9;

    bool NeedUpdate = false;

    if (myDevices[ID].level != JSonLevel)
    {
        myDevices[ID].level = JSonLevel;
        NeedUpdate = true;
    }
    if (strcmp(JSondata, myDevices[ID].data) != 0)
    {
        strncpy(myDevices[ID].data, JSondata,20);
        NeedUpdate = true;
    }

    if (NeedUpdate)
    {
        // If important Warning
        if (myDevices[ID].type == TYPE_WARNING)
        {
            // Force popup
            Select_deviceHP(ID);
        }
        else
        {
            RefreshHomePage();
        }
    }

}

int * GetGraphValue(int type, int idx, int *min, int *max)
{

	static int a[24];
    *min = 0xFFFF;
    *max = 0;

    String url = "/json.htm?type=command&param=graph&sensor=";
    switch (type)
    {
        case TYPE_TEMPERATURE:
        case TYPE_HUMIDITY:
            url = url + "temp";
            break;
        case TYPE_CONSUMPTION:
        case TYPE_LUX:
            url = url + "counter";
            break;
        case TYPE_PERCENT_SENSOR:
            url = url + "Percentage";
            break;
        default:
            //not supported
            return nullptr;
    }

    JsonArray JS;
    url = url + "&idx=" + String(idx) + "&range=day";
    if (HTTPGETRequestWithReturn((char *)url.c_str(), &JS))
    {
        if (!JS) return nullptr;

        double v;

        // Value are not constant so can be evry 15 mn or 20 mn
        int s = JS.size();

        int j = 0;
        for (auto i : JS)
        {
            //ATM we are using only the 24 last value every 4 values
            if ((s < 4 * 26) && (s % 4 == 0) && (j < 24))
            {
                if (type == TYPE_TEMPERATURE) v = i["te"];
                if (type == TYPE_HUMIDITY) v = i["hu"];
                if (type == TYPE_CONSUMPTION) v = i["u"];
                if (type == TYPE_PERCENT_SENSOR) v = i["v"];

                // Because of decimal values
                if (type == TYPE_TEMPERATURE) v = v *10;

                if (v > *max) *max = v;
                if (v < *min) *min = v;
                a[j] = int(v);
                j += 1;
            }
            s -= 1;
        }

        v = (*max - *min) / 10;
        *min = *min - v;
        *max = *max + v;

        return a;
    }

	return nullptr;
}


bool HttpInitDevice(Device *d, int id)
{

    JsonArray JS;
    String url = "/json.htm?type=command&param=getdevices&rid=" + String(id);

    if (!HTTPGETRequestWithReturn((char *)url.c_str(), &JS)) return false;
    if (!JS) return false;

    for (auto i : JS)  // Scan the array ( only 1)
    {
        if (d->name) free(d->name);
        d->name = (char*)malloc(strlen(i["Name"]) + 1);
        strcpy(d->name, i["Name"]);

        const char* JSondata = NULL;
        JSondata = i["Data"];

        //do some cleaning
        if (strncmp(JSondata, "Humidity ", 9) == 0) JSondata+=9;

        strncpy(d->data, JSondata,20);

        if (d->ID) free(d->ID);
        d->ID = (char*)malloc(strlen(i["ID"]) + 1);
        strcpy(d->ID, i["ID"]);
        d->idx = i["idx"];
        d->level = i["Level"];

        const char* type = i["Type"];
        const char* subtype = i["SubType"];
        const char* image = i["Image"];

        //Set a defaut value
        d->type = TYPE_VALUE_SENSOR;

        if (strcmp(type, "Light/Switch") == 0)
        {
            d->type = TYPE_LIGHT;

            if (strcmp(subtype,"Selector Switch") == 0)
            {

                d->type = TYPE_SELECTOR;
                const char *base64 = i["LevelNames"];
                if (d->levelname) free(d->levelname);
                d->levelname = (char*)malloc(strlen(i["LevelNames"]) + 1);

                unsigned int string_length = decode_base64((const unsigned char*)base64, (unsigned char *)d->levelname);
                d->levelname[string_length] = '\0';

                char *ptr = d->levelname;
                while (*ptr != '\0')
                {
                    if (*ptr == '|') { *ptr = '\n'; }
                    ptr++;
                }

            }
            else // Type "switch"
            {
                const char* switchtype = i["SwitchType"];

                if (strcmp(switchtype,"Dimmer") == 0)
                {
                    d->type = TYPE_DIMMER;
                }
                else if (strcmp(switchtype,"On/Off") == 0)
                {
                    d->type = TYPE_LIGHT;
                }
                else // Just passive sensor
                {
                    d->type = TYPE_SWITCH_SENSOR;
                }
            }


        }
        else if (strcmp(type, "Color Switch") == 0)
        {
            d->type = TYPE_COLOR;
        }
        else if (strncmp(type, "Temp",4) == 0)
        {
            d->type = TYPE_TEMPERATURE;
        }
        else if (strcmp(type, "Humidity") == 0)
        {
            d->type = TYPE_HUMIDITY;
        }
        else if ((strcmp(type, "Usage") == 0) || (strcmp(type, "P1 Smart Meter") == 0))
        {
            d->type = TYPE_CONSUMPTION;
        }
        else if (strcmp(type, "General") == 0)
        {
            d->type = TYPE_SWITCH_SENSOR;

            if (strcmp(subtype,"Alert") == 0) d->type = TYPE_WARNING;
            else if (strcmp(subtype,"Percentage") == 0) d->type = TYPE_PERCENT_SENSOR;
        }
        else if (strcmp(type, "Lux") == 0)
        {
            d->type = TYPE_LUX;
        }
        else if (strcmp(type, "Setpoint") == 0)
        {
            d->type = TYPE_SETPOINT;
        }

        // Correction by image
        if (image && strcmp(image,"WallSocket") == 0)
        {
            d->type = TYPE_PLUG;
        }
        else if (image && strcmp(image,"Speaker") == 0)
        {
            d->type = TYPE_SPEAKER;
        }
        else if (image && strcmp(image,"blinds") == 0)
        {
            d->type = TYPE_BLINDS;
        }

    }

    return true;

}