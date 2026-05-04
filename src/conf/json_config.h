#ifndef _JSON_CONFIG_INIT
    #define _JSON_CONFIG_INIT

    #include "global_config.h"

    extern GLOBAL_CONFIG global_config;

    #define SETTINGS_FILE "/settings.json"

    void loadJson();
    void writeJsonConfig();
    void dumpConfig();
#endif // !_JSON_CONFIG_INIT