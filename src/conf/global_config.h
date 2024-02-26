#ifndef _GLOBAL_CONFIG_INIT
#define _GLOBAL_CONFIG_INIT

#include "lvgl.h"

#define CONFIG_VERSION 3

typedef struct _GLOBAL_CONFIG {
    unsigned char version;
    union {
        unsigned char raw;
        struct {
            bool screenCalibrated : 1;
            bool wifiConfigured : 1;
            bool ipConfigured : 1;
            bool lightMode : 1;
            bool invertColors : 1;
            bool rotateScreen : 1;
            bool notused : 1;
        };
    };
    float screenCalXOffset;
    float screenCalXMult;
    float screenCalYOffset;
    float screenCalYMult;

    char wifiSSID[32];
    char wifiPassword[64];

    char ServerHost[64];
    unsigned short ServerPort;

    int ListDevices[TOTAL_ICONX*TOTAL_ICONY];
    
    unsigned char color_scheme;
    unsigned char brightness;
    unsigned char screenTimeout;

} GLOBAL_CONFIG;
    
typedef struct _COLOR_DEF {
    lv_palette_t primary_color;
    lv_palette_t secondary_color;
} COLOR_DEF;

extern GLOBAL_CONFIG global_config;
extern COLOR_DEF color_defs[];

void WriteGlobalConfig();
void VerifyVersion();
void LoadGlobalConfig();

#endif // !_GLOBAL_CONFIG_INIT