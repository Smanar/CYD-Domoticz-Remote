#ifndef _JSON_CONFIG_INIT
    #define _JSON_CONFIG_INIT

    #include "global_config.h"

    extern GLOBAL_CONFIG global_config;

    #define SETTINGS_FILE "/settings.json"
    #define TMP_FILE "/tmpFile.tmp"

    void loadJson();
    void writeJsonConfig();
    bool checkJsonConfig(char* jsonFile);
    bool readJsonConfig(char* jsonFile);
    void dumpConfig();
#endif // !_JSON_CONFIG_INIT