#ifndef _GLOBAL_CONFIG_INIT
#define _GLOBAL_CONFIG_INIT

#include "lvgl.h"

// USED for memorised settings
#define CONFIG_VERSION 4
// USED for OTA
#define APPLICATION_VERSION "26.5.9-1"

#if PAGES > 15
    #error Too much pages asked
#endif

typedef struct _GLOBAL_PAGE {
    char name[32];
    int ListDevices[TOTAL_ICONX*TOTAL_ICONY];
} GLOBAL_PAGE;

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

    unsigned char color_scheme;
    unsigned char brightness;
    unsigned char screenTimeout;

    unsigned int savedPageCount;
    unsigned int totalIconX;
    unsigned int totalIconY;

} GLOBAL_CONFIG;

typedef struct _GLOBAL_CONFIG_V3 {
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

    int ListDevices[TOTAL_ICONX*TOTAL_ICONY]; // No more used

    unsigned char color_scheme;
    unsigned char brightness;
    unsigned char screenTimeout;

} GLOBAL_CONFIG_V3;

    
typedef struct _COLOR_DEF {
    lv_palette_t primary_color;
    lv_palette_t secondary_color;
} COLOR_DEF;

extern GLOBAL_CONFIG global_config;
extern GLOBAL_PAGE global_pages[];

extern COLOR_DEF color_defs[];
extern void writeJsonConfig();

void WriteGlobalConfig();
void VerifyVersion();
void LoadGlobalConfig();

#endif // !_GLOBAL_CONFIG_INIT
