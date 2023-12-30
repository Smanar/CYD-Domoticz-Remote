#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <vector>

#include "data_setup.h"
#include "../ui/http_setup.h"

#define SIZEOF(arr) (sizeof(arr) / sizeof(*arr))
#define MAXDEVICES 7
char * ListDevices[MAXDEVICES] = {"37", "81", "16", "36", "28", "35", "57"}; // 6 widgets
Device myDevices[9];
void RefreshHomePage(void);


void Init_data(void)
{
    for (int i = 0; i < MAXDEVICES; i = i + 1)
    {
        if (i < MAXDEVICES)
        {
            HttpInitDevice(&myDevices[i], ListDevices[i]);
            myDevices[i].used = true;
            Serial.printf("Updated Domoticz device id: %s , Name : %s\n", ListDevices[i], myDevices[i].name);
        }

    }

}

void Update_data(JsonObject RJson2)
{


    Serial.println("Websocket update");
    char buffer[4096];
    serializeJsonPretty(RJson2, buffer);
    Serial.println(buffer);


    const char* JSondata = NULL;
    int JSonidx = 0;
    int JSonLevel = 0;

    if (RJson2.containsKey("Level")) JSonLevel = RJson2["Level"];
    if (RJson2.containsKey("idx")) JSonidx = atoi(RJson2["idx"]);
    if (RJson2.containsKey("Data")) JSondata = RJson2["Data"];

    if (JSonidx == 0) return;
    bool NeedUpdate = false;

    for (int i = 0; i < SIZEOF(myDevices); i = i + 1)
    {
        if (myDevices[i].idx == JSonidx)
        {
            if (myDevices[i].level != JSonLevel)
            {
                myDevices[i].level = JSonLevel;
                NeedUpdate = true;
            }
            if (strcmp(JSondata, myDevices[i].data) != 0)
            {
                strncpy(myDevices[i].data, JSondata,10);
                NeedUpdate = true;
            }

        }
    }

    if (NeedUpdate)
    {
        RefreshHomePage();
    }

}
