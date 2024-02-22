#include "../src/conf/global_config.h"
#include "data_setup.h"
#include "../ui/http_setup.h"
#include "../ui/panels/panel.h"
#include "ip_engine.h"
#include "base64.hpp"
#include <ArduinoJson.h>

#define SIZEOF(arr) (sizeof(arr) / sizeof(*arr))

Device myDevices[TOTAL_ICONX*TOTAL_ICONY];

void RefreshHomePage(void);

void Init_data(void)
{
    for (int i = 0; i < (TOTAL_ICONX*TOTAL_ICONY); i = i + 1)
    {
        if (i < SIZEOF(global_config.ListDevices))
        {

            if (HttpInitDevice(&myDevices[i], global_config.ListDevices[i]))
            {
                myDevices[i].used = true;
                Serial.printf("Initialise Domoticz device id: %d , Name : %s\n", global_config.ListDevices[i], myDevices[i].name);
                delay(100);
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
    if (JSondata && strncmp(JSondata, "Humidity ", 9) == 0) JSondata+=9;

    bool NeedUpdate = false;

    if (myDevices[ID].level != JSonLevel)
    {
        myDevices[ID].level = JSonLevel;
        NeedUpdate = true;
    }
    if (strcmp(JSondata, myDevices[ID].data) != 0)
    {
        //Use dynamic array, but only 1 time
        if (strlen(JSondata) > myDevices[ID].lenData)
        {
            if (myDevices[ID].data) free(myDevices[ID].data);
            myDevices[ID].data = (char*)malloc(strlen(JSondata) + 1);
            myDevices[ID].lenData = strlen(JSondata);
        }

        strncpy(myDevices[ID].data, JSondata, myDevices[ID].lenData + 1);
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


static int tab[24];
int * GetGraphValue(int type, int idx, int *min, int *max)
{

    *min = 0xFFFF;
    *max = 0;

#ifdef OLD_DOMOTICZ
    String url = "/json.htm?type=graph&sensor=";
#else
    String url = "/json.htm?type=command&param=graph&sensor=";
#endif

    switch (type)
    {
        case TYPE_TEMPERATURE:
        case TYPE_HUMIDITY:
            url = url + "temp";
            break;
        case TYPE_CONSUMPTION:
        case TYPE_POWER:
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

        double v; // value
        int hour;
        const char * c = NULL;

        // Value are not constant so can be evry 15 mn or 20 mn
        int s = JS.size();

        int j,k = 0;
        int actualhour = 0;
        int diff = 0;

        //Just need to memorise the last 24 values

        for (auto i : JS)
        {
            //"d" : "2024-02-16 19:45",
            c = i["d"];
            sscanf(c+11, "%2d", &hour);

            if (hour>23) hour = 23;
        
            //How many value different 
            if (hour >= actualhour) diff = hour - actualhour;
            else diff = hour - actualhour + 24;

            //if already value set for this hour, skip
            if (diff == 0) continue;

            actualhour = hour;

            if (type == TYPE_TEMPERATURE) v = i["te"];
            if (type == TYPE_HUMIDITY) v = i["hu"];
            if (type == TYPE_CONSUMPTION) v = i["u"];
            if (type == TYPE_POWER) v = i["u"];
            if (type == TYPE_PERCENT_SENSOR) v = i["v"];

            // Because of decimal values
            if (type == TYPE_TEMPERATURE) v = v *10;

            if (v > *max) *max = v;
            if (v < *min) *min = v;

            //swift the table
            for (j = 0; j < diff; j++)
            {
                for (k = 0; k < 23; k++)
                {
                    tab[k] = tab[k+1];
                }
                tab[23] = v;
            }

        }

        v = (*max - *min) / 10;
        *min = *min - v;
        *max = *max + v;

        return tab;
    }

	return nullptr;
}


bool HttpInitDevice(Device *d, int id)
{
    JsonArray JS;
#ifdef OLD_DOMOTICZ
    String url = "/json.htm?type=devices&rid=" + String(id);
#else
    String url = "/json.htm?type=command&param=getdevices&rid=" + String(id);
#endif

    if (!HTTPGETRequestWithReturn((char *)url.c_str(), &JS)) return false;

    if (!JS) return false;

    for (auto i : JS)  // Scan the array (only 1)
    {
        if (d->name) free(d->name);
        d->name = (char*)malloc(strlen(i["Name"]) + 1 + 1);
        strncpy(d->name, i["Name"],strlen(i["Name"]) + 1);

        const char* JSondata = NULL;
        const char* type = NULL;
        const char* subtype = NULL;
        const char* image = NULL;

        type = i["Type"];
        subtype = i["SubType"];
        image = i["Image"];
        JSondata = i["Data"];

        if (!type || !subtype || !JSondata)
        {
            Serial.println("Json incomplete");
            return false;
        }

        //do some cleaning
        if (JSondata && strncmp(JSondata, "Humidity ", 9) == 0) JSondata+=9; // <<< Guru Meditation

        //Use dynamic array, but only 1 time
        if (strlen(JSondata) > d->lenData)
        {
            if (d->data) free(d->data);
            d->data = (char*)malloc(strlen(JSondata) + 1 + 1); // +1 for null char
            //Serial.printf("Re-alloc from %d to %d\n", d->lenData, strlen(JSondata));
            d->lenData = strlen(JSondata);
           
        }
        strncpy(d->data, JSondata, d->lenData + 1);
        //d->data[d->lenData] = '\0';

        if (d->ID) free(d->ID);
        d->ID = (char*)malloc(strlen(i["ID"]) + 1 + 1);
        strncpy(d->ID, i["ID"],strlen(i["ID"]) + 1);

        d->idx = i["idx"];
        d->level = i["Level"];

        //Set a defaut value
        d->type = TYPE_UNKNOWN;

        if (strcmp(type, "Light/Switch") == 0)
        {
            d->type = TYPE_LIGHT;

            if (strcmp(subtype,"Selector Switch") == 0)
            {

                d->type = TYPE_SELECTOR;
                const char *base64 = i["LevelNames"];
                if (d->levelname) free(d->levelname);
                d->levelname = (char*)malloc(strlen(i["LevelNames"]) + 1 + 1);

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
                else if ((strcmp(switchtype,"Push On Button") == 0) || (strcmp(switchtype,"Push Off Button") == 0))
                {
                    d->type = TYPE_PUSH;
                }
                else if ((strcmp(switchtype,"Venetian Blinds EU") == 0) || (strcmp(switchtype,"Venetian Blinds US") == 0) || (strcmp(switchtype,"Blinds Percentage") == 0))
                {
                    d->type = TYPE_BLINDS;
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
        else if (type && strncmp(type, "Temp",4) == 0)
        {
            d->type = TYPE_TEMPERATURE;
        }
        else if (strcmp(type, "Humidity") == 0)
        {
            d->type = TYPE_HUMIDITY;
        }
        else if (strcmp(type, "Rain") == 0)
        {
            d->type = TYPE_METEO;
        }
        else if (strcmp(type, "Usage") == 0)
        {
            d->type = TYPE_POWER;
        }
        else if (strcmp(type, "P1 Smart Meter") == 0)
        {
            d->type = TYPE_CONSUMPTION;
        }
        else if (strcmp(type, "General") == 0)
        {
            d->type = TYPE_SWITCH_SENSOR;

            if (strcmp(subtype,"Alert") == 0) d->type = TYPE_WARNING;
            else if (strcmp(subtype,"Percentage") == 0) d->type = TYPE_PERCENT_SENSOR;
            else if (strcmp(subtype,"Text") == 0) d->type = TYPE_TEXT;
            else if (strcmp(subtype,"kWh") == 0) d->type = TYPE_CONSUMPTION;
        }
        else if (strcmp(type, "Lux") == 0)
        {
            d->type = TYPE_LUX;
        }
        else if ((strcmp(type, "Setpoint") == 0) || (strcmp(type, "Thermostat") == 0))
        {
            d->type = TYPE_SETPOINT;
        }

        if (image)
        {
            // Correction by image
            if (image && strcmp(image,"WallSocket") == 0)
            {
                d->type = TYPE_PLUG;
            }
            else if (image && strcmp(image,"Speaker") == 0)
            {
                d->type = TYPE_SPEAKER;
            }
        }

    }

    return true;

}