#ifndef DATA_SETUP_H
#define DATA_SETUP_H

enum {
    TYPE_UNKNOW,
    TYPE_SWITCH,
    TYPE_SPEAKER,
    TYPE_TEMPERATURE,
    TYPE_WARNING,
    TYPE_SELECTOR,
    TYPE_SWITCH_SENSOR,
    TYPE_PERCENT_SENSOR,
    TYPE_VALUE_SENSOR,
    TYPE_COLOR,
    TYPE_PLUG,
    TYPE_HUMIDITY,
    TYPE_CONSUMPTION,
    TYPE_LIGHT,
    TYPE_DIMMER,
    TYPE_BLINDS,
    TYPE_LUX,
    TYPE_SETPOINT
};

typedef struct _Device {
    char* name;
    char * ID;
    char data[20];
    int level = 0;
    int  idx = 0;
    int type = TYPE_VALUE_SENSOR;
    bool used = false;
    char * levelname;

    //lv_obj_t * label; // used for device update

    int pointer;
} Device;

void Init_data();
bool HTTPGETRequest(char * url);
void FillDeviceData(Device *d, int idx);
bool HttpInitDevice(Device *d, int id);
int * GetGraphValue(int type, int idx, int *, int *);

#endif
