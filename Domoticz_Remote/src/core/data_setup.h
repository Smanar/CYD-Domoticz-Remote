#ifndef DATA_SETUP_H
#define DATA_SETUP_H


enum {
    TYPE_UNKNOW = 0,
    TYPE_SWITCH = 1,
    TYPE_SPEAKER = 2,
    TYPE_TEMPERATURE = 3,
    TYPE_WARNING = 4,
    TYPE_SELECTOR = 5,
    TYPE_DIMMER = 6,
    TYPE_COLOR = 7,
    TYPE_PLUG = 8
};

typedef struct _Device {
    char* name;
    char * ID;
    int  HardwareID = 0;
    char data[10];
    int level = 0;
    int  idx = 0;
    int type = 0;
    bool used = false;
    char * levelname;

    //lv_obj_t * label; // used for device update

    int pointer;
} Device;

void Init_data();
bool HTTPGETRequest(char * url);

#endif
