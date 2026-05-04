#include "LittleFS.h"
#include "ArduinoJson.h"
#include "global_config.h"
#include "json_config.h"

JsonDocument settings;

void loadJson() {
    settings["version"] = global_config.version;
    settings["screenCalibrated"] = global_config.screenCalibrated;
    settings["wifiConfigured"] = global_config.wifiConfigured;
    settings["ipConfigured"] = global_config.ipConfigured;
    settings["lightMode"] = global_config.lightMode;
    settings["invertColors"] = global_config.invertColors;
    settings["rotateScreen"] = global_config.rotateScreen;
    settings["notused"] = global_config.notused;
    settings["screenCalXOffset"] = global_config.screenCalXOffset;
    settings["screenCalXMult"] = global_config.screenCalXMult;
    settings["screenCalYOffset"] = global_config.screenCalYOffset;
    settings["screenCalYMult"] = global_config.screenCalYMult;
    settings["wifiSSID"] = global_config.wifiSSID;
    settings["wifiPassword"] = global_config.wifiPassword;
    settings["ServerHost"] = global_config.ServerHost;
    settings["ServerPort"] = global_config.ServerPort;
    for (uint i=0; i<TOTAL_ICONX*TOTAL_ICONY; i++) {
        settings["ListDevices"][i] = global_config.ListDevices[i];
    }
    settings["color_scheme"] = global_config.color_scheme;
    settings["brightness"] = global_config.brightness;
    settings["screenTimeout"] = global_config.screenTimeout;
}

void writeJsonConfig() {
    loadJson();                                                     // Load JSON data
    File settingsFile = LittleFS.open(SETTINGS_FILE, "w");          // Open settings file
    if (!settingsFile) {                                            // Error opening?
        Serial.printf("Can't open %s for write\n", SETTINGS_FILE);
        return;
    }

    uint16_t bytes = serializeJsonPretty(settings, settingsFile);   // Write JSON structure to file
    if (!bytes) {                                                   // Error writting?
        Serial.printf("Can't write %s\n", SETTINGS_FILE);
    }
    settingsFile.flush();                                           // Flush file
    settingsFile.close();                                           // Close it
    Serial.printf("Settings written in %s\n", SETTINGS_FILE);
}

void dumpConfig() {
    String buffer;
    loadJson();                                                     // Load JSON data
    serializeJsonPretty(settings, buffer);
    Serial.println(buffer);
}