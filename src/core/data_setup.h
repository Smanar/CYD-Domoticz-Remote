#include "Arduino.h"

#ifndef DATA_SETUP_H
#define DATA_SETUP_H

enum {
    TYPE_UNUSED,
    TYPE_UNKNOWN,
    TYPE_PLUG,
    TYPE_COLOR,
    TYPE_LIGHT,
    TYPE_DIMMER,
    TYPE_SWITCH,            // All device with on/off need to be before the TYPE_SWITCH
    TYPE_SPEAKER,
    TYPE_TEMPERATURE,
    TYPE_HUMIDITY,
    TYPE_WARNING,
    TYPE_SELECTOR,
    TYPE_SWITCH_SENSOR,
    TYPE_PERCENT_SENSOR,
    TYPE_VALUE_SENSOR,
    TYPE_CONSUMPTION,
    TYPE_POWER,
    TYPE_BLINDS,
    TYPE_LUX,
    TYPE_WEIGHT,
    TYPE_METEO,
    TYPE_SETPOINT,
    TYPE_THERMOSTAT,
    TYPE_TEXT,
    TYPE_AIR_QUALITY,
    TYPE_PUSH,
    TYPE_PAGE
};

typedef struct _Device {
    char* name = nullptr;
    char* ID = nullptr;
    char* data = nullptr;
    char* levelname = nullptr;
    unsigned short level = 0;
    short idx = 0;
    unsigned short type = TYPE_UNUSED;
    unsigned short lenData = 0;
    unsigned short maxlevel = 100;

    //lv_obj_t * label; // used for device update

} Device;

void Init_data_widget_page();
void FillDeviceData(Device *d, int idx);
bool InitDeviceRequest(Device *dd, const char* c);
int * GetGraphValue(int type, int idx, int *, int *);
const char * loadDeviceList(int page, bool displayAll = false);
void GetThermostatValue(int idx, int *min, int *max, float *step, float *setpoint);

#endif
