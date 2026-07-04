#include <Preferences.h>

#include "lvgl.h"
#include "global_config.h"
#include "json_config.h"

GLOBAL_CONFIG global_config = {0};
GLOBAL_PAGE global_pages[PAGES] = {0};

COLOR_DEF color_defs[] = {
    {LV_PALETTE_BLUE, LV_PALETTE_RED},
    {LV_PALETTE_GREEN, LV_PALETTE_PURPLE},
    {LV_PALETTE_GREY, LV_PALETTE_CYAN},
    {LV_PALETTE_YELLOW, LV_PALETTE_PINK},
    {LV_PALETTE_ORANGE, LV_PALETTE_BLUE},
    {LV_PALETTE_RED, LV_PALETTE_GREEN},
    {LV_PALETTE_PURPLE, LV_PALETTE_GREY},
};


void WriteGlobalConfig() {
#if 0
    Preferences preferences;
    preferences.begin("global_config", false);
    preferences.putBytes("global_config", &global_config, sizeof(global_config));
    preferences.putBytes("global_pages", &global_pages, sizeof(global_pages));
    preferences.end();
#endif
    writeJsonConfig();
}

void VerifyOldVersion(){
    Preferences preferences;

    if (!preferences.begin("global_config", true)) return;
    // As size changes between V3 (larger) and the new version (smaller), we should read existing
    // preferences into a local buffer with appropriate size
    size_t prefLength = preferences.getBytesLength("global_config");
    char prefBuffer[prefLength];
    preferences.getBytes("global_config", &prefBuffer, prefLength);
    unsigned char version = prefBuffer[0];
    preferences.end();

    Serial.printf("Old Config version: %d\n", version);

    // Convert V3 to last version if needed
    if (version == 3) {
        Serial.println(F("Converting preferences from V3 to last version"));
        //Load old settings
        GLOBAL_CONFIG_V3 configV3 = {0};
        if (!preferences.begin("global_config", true)) return;
        preferences.getBytes("global_config", &configV3, sizeof(configV3));
        preferences.end();

        //Update from V3 to V4
        global_config.version = 4;
        global_config.screenCalibrated = configV3.screenCalibrated;
        global_config.wifiConfigured = configV3.wifiConfigured;
        global_config.ipConfigured = configV3.ipConfigured;
        global_config.lightMode = configV3.lightMode;
        global_config.invertColors = configV3.invertColors;
        global_config.rotateScreen = configV3.rotateScreen;
        global_config.notused = configV3.notused;
        global_config.screenCalXOffset = configV3.screenCalXOffset;
        global_config.screenCalXMult = configV3.screenCalXMult;
        global_config.screenCalYOffset = configV3.screenCalYOffset;
        global_config.screenCalYMult = configV3.screenCalYMult;
        strncpy(global_config.wifiSSID, configV3.wifiSSID, sizeof(global_config.wifiSSID));
        strncpy(global_config.wifiPassword, configV3.wifiPassword, sizeof(global_config.wifiPassword));
        strncpy(global_config.ServerHost, configV3.ServerHost, sizeof(global_config.ServerHost));
        global_config.ServerPort = configV3.ServerPort;

        for (uint i=0; i<TOTAL_ICONX*TOTAL_ICONY; i++) {
            global_pages[0].ListDevices[i] = configV3.ListDevices[i];
        }
        for (uint p=1; p < PAGES; p++) {                            // Clear unused pages
            for (uint i=0; i<TOTAL_ICONX*TOTAL_ICONY; i++) {
                global_pages[p].ListDevices[i] = 0;
            }
        }
        
        for (uint p=0; p < PAGES; p++) {                            // Set default names
            lv_snprintf(global_pages[p].name, sizeof(global_pages[p].name), "Page %d", p+1);
            global_pages[p].isProtected = false;
        }
        global_config.color_scheme = configV3.color_scheme;
        global_config.brightness = configV3.brightness;
        global_config.screenTimeout = configV3.screenTimeout;

        global_config.version = 5; // The fonction Updatejsonversion() will update the rest

        //Save settings using littleFS and json
        WriteGlobalConfig();

#if 0
        //Clear old data
        Serial.println(F("Clearing Global Config"));
        if (!preferences.begin("global_config", false)) return;
        preferences.clear();
        preferences.end();
#endif

    }

}

void Updatejsonversion(void)
{
    if ( global_config.version == 5)
    {
        strcpy(global_config.protectionPassword, "");
        global_config.protectSetting = true;
        global_config.protectTool = false;
        global_config.protectGroup = false;
        global_config.protectInfo = true;
    }

    global_config.version == CONFIG_VERSION;
    WriteGlobalConfig();

}

void LoadGlobalConfig() {
    global_config.version = CONFIG_VERSION;

    //Set defaut values
    global_config.brightness = 255;
    global_config.screenTimeout = 0;
    global_config.homeTimeout = 0;

    for (uint p=0; p<PAGES; p++)
    {
        lv_snprintf(global_pages[p].name, sizeof(global_pages[p].name), "Page %d", p+1);
        global_pages[p].isProtected = false;
    }

    // Load json setting
    if (!readJsonConfig(SETTINGS_FILE))
    {
        //json not present or failed to load
        //check for old version that use "preference"
        VerifyOldVersion();
    }

    // Check for version
    if (global_config.version > CONFIG_VERSION)
    {
        Serial.printf("Version missmatch: %d > %d\n", global_config.version, CONFIG_VERSION);
        Updatejsonversion();
    }

}
