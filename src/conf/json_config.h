#ifndef _JSON_CONFIG_INIT
    #define _JSON_CONFIG_INIT

    #include "ArduinoJson.h"

    #define SETTINGS_FILE "/settings.json"
    #define TMP_FILE "/tmpFile.tmp"

    JsonDocument loadJson();
    void writeJsonConfig();
    bool checkJsonConfig(const char* jsonFile);
    bool readJsonConfig(const char* jsonFile);
    void dumpConfig();

#endif // !_JSON_CONFIG_INIT