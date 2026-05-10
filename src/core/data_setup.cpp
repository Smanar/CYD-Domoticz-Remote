#include <ArduinoJson.h>
#include "../conf/global_config.h"
#include "../ui/http_setup.h"
#include "../ui/panels/panel.h"
#include "../ui/navigation.h"
#include "ip_engine.h"
#include "base64.hpp"
#include "data_setup.h"


#define SIZEOF(arr) (sizeof(arr) / sizeof(*arr))

Device myDevices[TOTAL_ICONX*TOTAL_ICONY]; // Home page
Device myDevices2[PAGES][TOTAL_ICONX*TOTAL_ICONY]; //Others pages


char TmpBuffer[255]; // To prevent multiple re-alloc
static int tab[24]; // Tab for graph

void RefreshHomePage(void);

char * Cleandata(unsigned short t, const char *origin, const char *bonus = nullptr)
{
    if (!origin) return TmpBuffer;
    
    if (strncmp(origin, "Humidity ", 9) == 0) origin += 9;

    if (bonus)
    {
        snprintf(TmpBuffer, sizeof(TmpBuffer), "%s;%s", origin, bonus);
    }
    else
    {
        snprintf(TmpBuffer, sizeof(TmpBuffer), "%s", origin);
    }

    if (t == TYPE_SETPOINT || t == TYPE_THERMOSTAT) {
        strncat(TmpBuffer, "°C", sizeof(TmpBuffer) - strlen(TmpBuffer) - 1);
    }

    // Remplacement des séparateurs par des sauts de ligne
    for (char *p = TmpBuffer; *p; p++) {
        if (*p == ';' || *p == ',') *p = '\n';
    }

    return TmpBuffer;
}

void Init_data(void)
{
    int idx;

    //For home page
    for (int i = 0; i < (TOTAL_ICONX*TOTAL_ICONY); i = i + 1)
    {
        myDevices[i].type = TYPE_UNUSED;

        if (global_pages[0].ListDevices[i])
        {
            idx = global_pages[0].ListDevices[i];
            myDevices[i].idx = idx;

            if (idx < 0) //Fake device or buggy
            {
                if (-idx <= PAGES)                                           // Pk, page index
                {                                                            // Check if given page is within range
                    int pagePtr = -idx - 1;                                  // Page pointer (0 to PAGES - 1)
                    if (myDevices[i].name ) free(myDevices[i].name);
                    myDevices[i].name = (char*)malloc(strlen(global_pages[pagePtr].name) + 1);
                    if (!myDevices[i].name) return; // malloc failed
                    strncpy(myDevices[i].name, global_pages[pagePtr].name, strlen(global_pages[pagePtr].name) + 1);
                    
                    myDevices[i].type = TYPE_PAGE;
                }
            } 
            else
            {
                // Do nothing, simple request for all real devices in same time later.
            }
        }
    }

    HttpInitDevice(myDevices, GetListdevice(0));
    delay(50); // During tests, the device spam Domoticz to get data too fast, and Domoticz "skip" some answers, 50ms is not a big delay.


//For the moment only enabled for HomePage, need tests
#if defined(LIGHTWS)
    subscribedeviceWS(0, GetListdevice(0, false));
#endif

    //For other pages
    for (uint p=1; p<=PAGES; p++)
    {
        for (int i = 0; i < (TOTAL_ICONX*TOTAL_ICONY); i = i + 1)
        {
            myDevices2[p-1][i].type = TYPE_UNUSED;

            if (global_pages[p].ListDevices[i])
            {
                idx = global_pages[p].ListDevices[i];
                myDevices2[p-1][i].idx = idx;
    
            }
        }

        HttpInitDevice(myDevices2[p-1], GetListdevice(p));
        delay(50); // During tests, the device spam Domoticz to get data too fast, and Domoticz "skip" some answers, 50ms is not a big delay.

    }

}

void FillDeviceData(Device *d, int idx)
{
    char tmp8[3];
    snprintf(tmp8, sizeof(tmp8), "%d" , idx);
    if (HttpInitDevice(d, tmp8))
    {
        Serial.printf("Initialize Domoticz device id: %d , Name : %s\n", idx, d->name);
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

//Static or not ? TODO
char tmp7[TOTAL_ICONX*TOTAL_ICONY * 4] = ""; // 3 digits + comma

const char *GetListdevice(int page, bool displayAll)
{
    int idx;
    int offset = 0;

    for (int i = 0; i < (TOTAL_ICONX*TOTAL_ICONY); i = i + 1)
    {
        idx = global_pages[page].ListDevices[i];
        if (displayAll || idx > 0)
        {
            offset += snprintf(tmp7 + offset, sizeof(tmp7) - offset, (i == 0) ? "%d" : ",%d", global_pages[0].ListDevices[i]);
        }
    }
    return tmp7;
}

void Update_device_data(JsonObject RJson2)
{
    //char buffer[4096];
    //serializeJsonPretty(RJson2, buffer);
    //Serial.println(buffer);

    int JSonidx = 0;

    //if (RJson2.containsKey("idx")) JSonidx = atoi(RJson2["idx"]);
    if (RJson2["idx"].is<const char*>()) JSonidx = atoi(RJson2["idx"]);
    if (JSonidx == 0) return; // No Idx

    int ID = Get_ID_Device(JSonidx);
    if (ID < 0) return ; // Not in list

    const char* JSondata = NULL;
    int JSonLevel = 0;

    //if (RJson2.containsKey("Data")) JSondata = RJson2["Data"];
    //if (RJson2.containsKey("Level")) JSonLevel = RJson2["Level"];
    if (RJson2["Data"].is<const char*>()) JSondata = RJson2["Data"];
    if (RJson2["Level"].is<int>()) JSonLevel = RJson2["Level"];

    Serial.printf("Update HP device Id: %d, Domo Idx: %d\n", ID, JSonidx);

    bool NeedUpdate = false;

    //Special device
    char * data;

    if (RJson2["Rain"].is<const char*>())
    {
        data = Cleandata(myDevices[ID].type, RJson2["Rain"]);
    }
    else if ((myDevices[ID].type == TYPE_TEXT) || (myDevices[ID].type == TYPE_WARNING))
    {
        //Keep it unchanged
        data = (char *)JSondata;
    }
    else if (myDevices[ID].type == TYPE_THERMOSTAT)
    {
        char t[10];
        lv_snprintf(t, 10, "%.1f", RJson2["Temp"].as<float>());
        data = Cleandata(myDevices[ID].type, t);
    }
    else
    {
        data = Cleandata(myDevices[ID].type, JSondata);
    }

    if (myDevices[ID].data && strcmp(data, myDevices[ID].data) != 0)
    {
        //Use dynamic array, but only 1 time
        size_t dataLen = strlen(data);
        if (dataLen >= myDevices[ID].lenData)
        {
            if (myDevices[ID].data) free(myDevices[ID].data);
            myDevices[ID].data = (char*)malloc(dataLen + 1);
            if (!myDevices[ID].data) return; // malloc failed
            myDevices[ID].lenData = dataLen;
        }

        strncpy(myDevices[ID].data, data, myDevices[ID].lenData + 1);
        NeedUpdate = true;
    }
    else if (!myDevices[ID].data)
    {
        // First update: allocate and store initial data
        myDevices[ID].data = (char*)malloc(strlen(data) + 1);
        if (!myDevices[ID].data) return; // malloc failed
        myDevices[ID].lenData = strlen(data);
        strncpy(myDevices[ID].data, data, myDevices[ID].lenData + 1);
        NeedUpdate = true;
    }

    if (myDevices[ID].level != JSonLevel)
    {
        myDevices[ID].level = JSonLevel;
        NeedUpdate = true;
    }

    if (NeedUpdate)
    {
        // If important Warning
        if (myDevices[ID].type == TYPE_WARNING)
        {
            // Force popup
            Select_deviceMemorised((void *)&myDevices[ID]);
        }
        else
        {
            // The device is actully displayed ?
            if ((GetActivePanel() == DEVICE_PANEL) && (GetSelectedDeviceIdx() == JSonidx))
            {
                RefreshDevicePanel();
            }
            else
            {
                RefreshHomePage();
            }
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
        case TYPE_AIR_QUALITY:
        case TYPE_WEIGHT:
            url = url + "counter";
            break;
        case TYPE_PERCENT_SENSOR:
        case TYPE_VALUE_SENSOR:
            url = url + "Percentage";
            break;
        case TYPE_METEO:
            url = url + "rain";
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
            Serial.println(F("Json not available\n"));
            return nullptr;
        }

        std::fill_n(tab, 24, 0);

        double v = 0.0; // value
        short hour = 0;
        const char * c = NULL;

        // Value are not constant so can be evry 15 mn or 20 mn
        //int s = JS.size();

        short j = 0, k = 0;
        short actualhour = 0;
        short diff = 0;

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

            switch (type)
            {
                case TYPE_TEMPERATURE:
                    v = i["te"];
                    break;
                case TYPE_HUMIDITY:
                    v = i["hu"];
                    break;
                case TYPE_CONSUMPTION:
                    v = i["eu"];
                    break;
                case TYPE_POWER:
                    v = i["u"];
                    break;
                case TYPE_PERCENT_SENSOR:
                case TYPE_VALUE_SENSOR:
                case TYPE_WEIGHT:
                    v = i["v"];
                    break;
                case TYPE_METEO:
                    v = i["mm"];
                    break;
                default:
                    v = 0;
            }

            // Because of decimal values
            if (type == TYPE_TEMPERATURE || type == TYPE_METEO) v = v *10;

            if (type == TYPE_WEIGHT) v = v *1000;

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

        // Scale calcul
        for (k = 0; k < 24; k++)
        {
            if (tab[k] > *max) *max = tab[k];
            if (tab[k] < *min) *min = tab[k];
        }
        v = (*max - *min) / 10;
        if (*min != 0) *min = *min - v;
        *max = *max + v;

        return tab;
    }

	return nullptr;
}


bool HttpInitDevice(Device *dd, const char* c)
{
    JsonDocument doc;

    #ifdef OLD_DOMOTICZ
        String url = "/json.htm?type=devices&rid=" + String(c);
    #else
        String url = "/json.htm?type=command&param=getdevices&rid=" + String(c);
    #endif

    if (!HTTPGETRequestWithReturn((char *)url.c_str(), &doc)) return false;

    JsonArray JS;
    JS = doc["result"];

    if (JS.isNull())
    {
        Serial.println(F("Json not available\n"));
        return false;
    }

    for (auto i : JS)
    {

        Device *d = nullptr;

        //First need the idx
        int idx = 0;

        if (i["idx"].is<int>())
        {
            idx = i["idx"].as<int>();
        }
        else if (i["idx"].is<const char*>())
        {
            idx = atoi(i["idx"].as<const char*>());
        }

        for (int i = 0; i < (TOTAL_ICONX*TOTAL_ICONY); i = i + 1)
        {
            if (dd[i].idx == idx)
            {
                d = &dd[i];
            }
        }

        // Not present ? skip it
        if (!d) continue;

        if (d->name) free(d->name);
        d->name = (char*)malloc(strlen(i["Name"]) + 1);
        if (!d->name) return false; // malloc failed
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
            Serial.println(F("Json incomplete"));
            return false;
        }

        if (d->ID) free(d->ID);
        d->ID = (char*)malloc(strlen(i["ID"]) + 1);
        if (!d->ID) return false; // malloc failed
        strncpy(d->ID, i["ID"],strlen(i["ID"]) + 1);

        if (i["idx"].is<int>())
        {
            d->idx = i["idx"].as<int>();
        }
        else if (i["idx"].is<const char*>())
        {
            d->idx = atoi(i["idx"].as<const char*>());
        }

        Serial.printf("Initialize Domoticz devices id: %d, name %s\n", d->idx, d->name);

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
                if (!d->levelname) return false; // malloc failed

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

                    // some device don't have 0/100 values
                    if (i["MaxDimLevel"].is<double>()) d->maxlevel = i["MaxDimLevel"];

                }
                else if (strcmp(switchtype,"On/Off") == 0)
                {
                    d->type = TYPE_LIGHT;
                }
                else if ((strcmp(switchtype,"Push On Button") == 0) || (strcmp(switchtype,"Push Off Button") == 0))
                {
                    d->type = TYPE_PUSH;
                }
                else if ((strcmp(switchtype,"Venetian Blinds EU") == 0) || (strcmp(switchtype,"Venetian Blinds US") == 0)
                || (strcmp(switchtype,"Blinds Percentage") == 0) || (strcmp(switchtype,"Blinds % + Stop") == 0))
                {
                    d->type = TYPE_BLINDS;
                }
                else // Just passive sensor
                {
                    d->type = TYPE_SWITCH_SENSOR;
                }
            }

        }
        else if (strncmp(type, "Lighting", 8) == 0)
        {
            d->type = TYPE_LIGHT;
            
            const char* switchtype = i["SwitchType"];
            if (strcmp(switchtype,"Dimmer") == 0)
            {
                d->type = TYPE_DIMMER;

                // some device don't have 0/100 values
                if (i["MaxDimLevel"].is<double>()) d->maxlevel = i["MaxDimLevel"];

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
        else if (strcmp(type, "Rain") == 0)
        {
            d->type = TYPE_METEO;
        }
        else if (strcmp(type, "Usage") == 0)
        {
            d->type = TYPE_POWER;
        }
        else if ((strcmp(type, "P1 Smart Meter") == 0) || (strcmp(type, "RFXMeter") == 0))
        {
            d->type = TYPE_CONSUMPTION;
        }
        else if (strcmp(type, "Weight") == 0)
        {
            d->type = TYPE_WEIGHT;
        }
        else if (strcmp(type, "Air Quality") == 0)
        {
            d->type = TYPE_AIR_QUALITY;
        }
        else if (strcmp(type, "General") == 0)
        {
            d->type = TYPE_SWITCH_SENSOR;

            if (strcmp(subtype,"Alert") == 0) d->type = TYPE_WARNING;
            else if (strcmp(subtype,"Percentage") == 0) d->type = TYPE_PERCENT_SENSOR;
            else if (strcmp(subtype,"Text") == 0) d->type = TYPE_TEXT;
            else if (strcmp(subtype,"kWh") == 0) d->type = TYPE_CONSUMPTION;
            else if (strcmp(subtype,"Custom Sensor") == 0) d->type = TYPE_VALUE_SENSOR;
        }
        else if (strcmp(type, "Lux") == 0)
        {
            d->type = TYPE_LUX;
        }
        else if ((strcmp(type, "Setpoint") == 0) || (strcmp(type, "Thermostat") == 0))
        {
            d->type = TYPE_SETPOINT;
        }
        else if (strcmp(type, "Thermostat 6") == 0)
        {
            d->type = TYPE_THERMOSTAT;
        }
        if (image)
        {
            // Correction by image
            if (strcmp(image,"WallSocket") == 0)
            {
                d->type = TYPE_PLUG;
            }
            else if (strcmp(image,"Speaker") == 0)
            {
                d->type = TYPE_SPEAKER;
            }
        }

        //Special device
        char * data;

        if (i["Rain"].is<const char*>())
        {
        data = Cleandata(d->type, i["Rain"]);
        }
        else if ((d->type == TYPE_TEXT) || (d->type == TYPE_WARNING))
        {
            //Keep it unchanged
            data = (char *)JSondata;
        }
        else if (d->type == TYPE_THERMOSTAT)
        {
            char t[10];
            lv_snprintf(t, 10, "%.1f", i["Temp"].as<float>());
            data = Cleandata(d->type, t);
        }
        else
        {
            data = Cleandata(d->type, JSondata);
        }

        //Use dynamic array if needed, but only 1 time if needed to prevent fragmentation
        if (strlen(data) > d->lenData)
        {
            if (d->data) free(d->data);
            d->data = (char*)malloc(strlen(data) + 1);
            if (!d->data) return false; // malloc failed
            //Serial.printf("Re-alloc from %d to %d\n", d->lenData, strlen(data));
            d->lenData = strlen(data);
        }

        if (d->data) strncpy(d->data, data, d->lenData + 1);
    }

    return true;


}

void GetThermostatValue(int idx, int *min, int *max, float *step, float *setpoint)
{

    *min = 0;
    *max = 50;
    *step = 0.5f;
    *setpoint= 20.0f;

#ifdef OLD_DOMOTICZ
    String url = "/json.htm?type=devices&rid=" + String(idx);
#else
    String url = "/json.htm?type=command&param=getdevices&rid=" + String(idx);
#endif

    JsonDocument doc;

    if (HTTPGETRequestWithReturn((char *)url.c_str(), &doc))
    {
        JsonArray JS;
        JS = doc["result"];

        if (JS.isNull())
        {
            Serial.println(F("Json not available\n"));
            return;
        }

        for (auto i : JS)
        {
            if (i["min"].is<double>()) *min = i["min"];
            if (i["max"].is<double>()) *max = i["max"];
            if (i["step"].is<double>()) *step = i["step"];
            if (i["SetPoint"].is<double>()) *setpoint = i["SetPoint"];
            if (i["SetPoint"].is<const char*>()) *setpoint = atof(i["SetPoint"]);

        }
    }
}