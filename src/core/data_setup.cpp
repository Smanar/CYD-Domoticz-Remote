#include "../src/conf/global_config.h"
#include "data_setup.h"
#include "../ui/http_setup.h"
#include "../ui/panels/panel.h"
#include "ip_engine.h"
#include "base64.hpp"
#include <ArduinoJson.h>

#define SIZEOF(arr) (sizeof(arr) / sizeof(*arr))

Device myDevices[TOTAL_ICONX*TOTAL_ICONY];
static char TmpBuffer[255]; // To prevent multiple re-alloc
static int tab[24]; // Tab for graph

void RefreshHomePage(void);

char * Cleandata(const char *origin, const char *bonus = nullptr) 
{
    if (!origin) return TmpBuffer;

    if (strncmp(origin, "Humidity ", 9) == 0) origin+=9;
    strncpy(TmpBuffer, origin, 255);

    if (bonus)
    {
        strncpy(TmpBuffer + strlen(origin), ";", 255 - strlen(origin));
        strncpy(TmpBuffer + strlen(origin) + 1, bonus, 254 - strlen(origin));
    }

    for (int i = 0; i<strlen(TmpBuffer); i++) { if (TmpBuffer[i] == ';') TmpBuffer[i] = '\n';}

    return TmpBuffer;
}

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

    //Special device
    char * data;
    if (RJson2.containsKey("Rain"))
    {
        data = Cleandata(JSondata, RJson2["Rain"]);
    }
    else
    {
        data = Cleandata(JSondata);
    }

    bool NeedUpdate = false;

    if (myDevices[ID].level != JSonLevel)
    {
        myDevices[ID].level = JSonLevel;
        NeedUpdate = true;
    }
    if (strcmp(data, myDevices[ID].data) != 0)
    {
        //Use dynamic array, but only 1 time
        if (strlen(data) > myDevices[ID].lenData)
        {
            if (myDevices[ID].data) free(myDevices[ID].data);
            myDevices[ID].data = (char*)malloc(strlen(data) + 1);
            myDevices[ID].lenData = strlen(data);
        }

        strncpy(myDevices[ID].data, data, myDevices[ID].lenData + 1);
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

    JsonDocument doc;
    url = url + "&idx=" + String(idx) + "&range=day";
    if (HTTPGETRequestWithReturn((char *)url.c_str(), &doc))
    {
        JsonArray JS;
        JS = doc["result"];

        if (JS.isNull())
        {
            Serial.println("Json not available\n");
            return nullptr;
        }

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
            if (type == TYPE_METEO) v = i["mm"];

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
    JsonDocument doc;
#ifdef OLD_DOMOTICZ
    String url = "/json.htm?type=devices&rid=" + String(id);
#else
    String url = "/json.htm?type=command&param=getdevices&rid=" + String(id);
#endif

    if (!HTTPGETRequestWithReturn((char *)url.c_str(), &doc)) return false;

    JsonArray JS;
    JS = doc["result"];

    if (JS.isNull())
    {
        Serial.println("Json not available\n");
        return false;
    }

    for (auto i : JS)  // Scan the array (only 1)
    {
        if (d->name) free(d->name);
        d->name = (char*)malloc(strlen(i["Name"]) + 1);
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

        //Special device
        char * data;
        if (i.containsKey("Rain"))
        {
           data = Cleandata(JSondata, i["Rain"]);
        }
        else
        {
            data = Cleandata(JSondata);
        }
        //Use dynamic array if needed, but only 1 time if needed to prevent fragmentation
        if (strlen(data) > d->lenData)
        {
            if (d->data) free(d->data);
            d->data = (char*)malloc(strlen(data) + 1);
            //Serial.printf("Re-alloc from %d to %d\n", d->lenData, strlen(data));
            d->lenData = strlen(data);
        }
        strncpy(d->data, data, d->lenData + 1);

        if (d->ID) free(d->ID);
        d->ID = (char*)malloc(strlen(i["ID"]) + 1);
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
                // Decoded string is always smaller, bytes = (string_length(encoded_string) − 814) / 1.37
                // So we loose 30% of memory for nothing but don't need to re-alloc it.
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