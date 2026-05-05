#include "LittleFS.h"
#include "ArduinoJson.h"
#include "global_config.h"
#include "json_config.h"

extern GLOBAL_CONFIG global_config;

//
//  Load settings from global config
//
JsonDocument loadJson() {

    JsonDocument settings;

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

    return settings;
}

//
//  Write current preferences to settings file
//
void writeJsonConfig() {
    JsonDocument settings = loadJson();                                  // Load JSON data
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

//
//  Check json config file
//      If file is ok, preferences will be modified, saved, and a new settings.json will be written.
//      Returns true if file is ok, false else
//
bool checkJsonConfig(const char* jsonFile) {
    if (readJsonConfig(jsonFile)) {
        WriteGlobalConfig();
        return true;
    } else {
        return false;
    }
}

//
//  Load a global config structure from a JSON file
//      Returns true if file is ok, false else
//

bool readJsonConfig(const char* jsonFile) {

    JsonDocument settings;

    File settingsFile = LittleFS.open(jsonFile, "r");               // Open settings file
    if (!settingsFile) {                                            // Error opening?
        Serial.printf("Can't open %s for read\n", jsonFile);
        return false;
    }

    auto error = deserializeJson(settings, settingsFile);           // Read settings
    settingsFile.close();                                           // Close file
    if (error) {                                                    // Error reading JSON?
        Serial.printf("Failed to parse %s", jsonFile);
        return false;
    }

    const char* charPtr;                                            // Pointer for char data

    // Load all settings into corresponding variables
    global_config.version = settings["version"].as<unsigned char>();
    global_config.screenCalibrated = settings["screenCalibrated"].as<bool>();
    global_config.wifiConfigured = settings["wifiConfigured"].as<bool>();
    global_config.ipConfigured = settings["ipConfigured"].as<bool>();
    global_config.lightMode = settings["lightMode"].as<bool>();
    global_config.invertColors = settings["invertColors"].as<bool>();
    global_config.rotateScreen = settings["rotateScreen"].as<bool>();
    global_config.notused = settings["notused"].as<bool>();
    global_config.screenCalXOffset = settings["screenCalXOffset"].as<float>();
    global_config.screenCalXMult = settings["screenCalXMult"].as<float>();
    global_config.screenCalYOffset = settings["screenCalYOffset"].as<float>();
    global_config.screenCalYMult = settings["screenCalYMult"].as<float>();
    charPtr = settings["wifiSSID"].as<const char*>();
    if (charPtr) strncpy(global_config.wifiSSID, charPtr, sizeof(global_config.wifiSSID));
    charPtr = settings["wifiPassword"].as<const char*>();
    if (charPtr) strncpy(global_config.wifiPassword, charPtr, sizeof(global_config.wifiPassword));
    charPtr = settings["ServerHost"].as<const char*>();
    if (charPtr) strncpy(global_config.ServerHost, charPtr,sizeof(global_config.ServerHost));
    Serial.printf("ServerHost:%s-%s-%s\n", charPtr, global_config.ServerHost, global_config.ServerHost);
    global_config.ServerPort = settings["ServerPort"].as<unsigned short>();
    for (uint i=0; i<TOTAL_ICONX*TOTAL_ICONY; i++) {
        global_config.ListDevices[i] = settings["ListDevices"][i].as<int>();
    }
    global_config.color_scheme = settings["color_scheme"].as<unsigned char>();
    global_config.brightness = settings["brightness"].as<unsigned char>();
    global_config.screenTimeout = settings["screenTimeout"].as<unsigned char>();
    
    return true;
}

//
//  Dump global config on Serial
//

void dumpConfig() {
    String buffer;
    JsonDocument settings = loadJson();                                          // Load JSON data
    serializeJsonPretty(settings, buffer);
    Serial.println(buffer);
}