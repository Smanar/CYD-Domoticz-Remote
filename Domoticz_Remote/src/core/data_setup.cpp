#include <TFT_eSPI.h>
#include <ArduinoJson.h>


#include "data_setup.h"
#include "../ui/http_setup.h"
#include "../ui/panels/panel.h"
#include "ip_engine.h"

#define SIZEOF(arr) (sizeof(arr) / sizeof(*arr))
#define MAXDEVICES 9
//char * ListDevices[MAXDEVICES] = {"37", "81", "16", "36", "28", "35", "57"};
static const char* const ListDevices[] = {"37", "81", "16", "36", "28", "35", "57", "14", "87"};
Device myDevices[9];

void RefreshHomePage(void);


void Init_data(void)
{
    for (int i = 0; i < MAXDEVICES; i = i + 1)
    {
        if (i < MAXDEVICES)
        {
            if (HttpInitDevice(&myDevices[i], ListDevices[i]))
            {
                myDevices[i].used = true;
                Serial.printf("Updated Domoticz device id: %s , Name : %s\n", ListDevices[i], myDevices[i].name);
            }
        }

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

    //some cleaning
    if (strncmp(JSondata, "Humidity ", 9) == 0) JSondata+=9;

    if (JSonidx == 0) return; // No Idx

    int ID = Get_ID_Device(JSonidx);
    if (ID < 0) return ; // Not in list

    bool NeedUpdate = false;

    if (myDevices[ID].level != JSonLevel)
    {
        myDevices[ID].level = JSonLevel;
        NeedUpdate = true;
    }
    if (strcmp(JSondata, myDevices[ID].data) != 0)
    {
        strncpy(myDevices[ID].data, JSondata,10);
        NeedUpdate = true;
    }

    if (NeedUpdate)
    {
        // If important Warning
        if (myDevices[ID].type == TYPE_WARNING)
        {
            // Force popup
            Select_device(ID);
        }
        else
        {
            RefreshHomePage();
        }
    }

}
