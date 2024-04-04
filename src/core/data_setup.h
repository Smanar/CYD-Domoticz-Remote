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
    TYPE_METEO,
    TYPE_SETPOINT,
    TYPE_TEXT,
    TYPE_AIR_QUALITY,
    TYPE_PUSH
};

typedef struct _Device {
    char* name = nullptr;
    char* ID = nullptr;
    char* data = nullptr;
    char* levelname = nullptr;
    unsigned short level = 0;
    unsigned short idx = 0;
    unsigned short type = TYPE_UNUSED;
    unsigned short lenData = 0;

    //lv_obj_t * label; // used for device update

} Device;

void Init_data();
void FillDeviceData(Device *d, int idx);
bool HttpInitDevice(Device *d, int id);
int * GetGraphValue(int type, int idx, int *, int *);

#endif
