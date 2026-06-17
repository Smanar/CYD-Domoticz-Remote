#include "helper.h"
#include "lvgl.h"
#include "LittleFS.h"
#include "WiFi.h"
#include "conf/global_config.h"
#include "ip_engine.h"
#include "../ui/navigation.h"

extern unsigned long runningTime(void);

void loadInfo(char* textChar, size_t textSize) {

    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    uint32_t used_size = mon.total_size - mon.free_size;

    size_t usedBytes = LittleFS.usedBytes();
    size_t totalBytes = LittleFS.totalBytes();
    size_t remainingBytes = totalBytes - usedBytes;
    IPAddress localIp = WiFi.localIP();

    lv_snprintf(textChar, textSize,
    "HEAP Memory Usable (Kb) %d, Max %d, Total %d\n", ESP.getMaxAllocHeap()/1024, ESP.getFreeHeap()/1024, ESP.getHeapSize()/1024);
    #ifdef BOARD_HAS_PSRAM
    lv_snprintf(textChar + strlen(textChar), textSize - strlen(textChar),
        "PSRAM Memory Free (Kb) %d, Total %d\n", ESP.getFreePsram()/1024, ESP.getPsramSize()/1024);
    #endif
    lv_snprintf(textChar + strlen(textChar), textSize - strlen(textChar),
    "LV Heap %d kB used (%d %%) %d%% frag.\n", used_size / 1024, mon.used_pct, mon.frag_pct);
    lv_snprintf(textChar + strlen(textChar), textSize - strlen(textChar),
    "Application Version : %s\n", APPLICATION_VERSION);
    lv_snprintf(textChar + strlen(textChar), textSize - strlen(textChar),
    "Running time : %d-%02d:%02d:%02d\n", runningTime()/(3600*24), (runningTime()/3600)%24 , (runningTime()/60)%60, runningTime()%60);
    lv_snprintf(textChar + strlen(textChar), textSize - strlen(textChar),
    "Total data by WS : %d ko\n", total_WS_lenght());
    lv_snprintf(textChar + strlen(textChar), textSize - strlen(textChar),
    "LittleFS %s, %d kB free (%d%%)\n", totalBytes?"ok":"*BAD*", remainingBytes / 1024, (100 * remainingBytes) / totalBytes);
    lv_snprintf(textChar + strlen(textChar), textSize - strlen(textChar),
    "IP %d.%d.%d.%d\n", localIp[0], localIp[1], localIp[2], localIp[3]);
}

void getType(unsigned short type, char* typeText, size_t textLen) {
    switch (type)
    {
        case TYPE_UNUSED: strncpy(typeText, "UNUSED", textLen); break;
        case TYPE_UNKNOWN: strncpy(typeText, "UNKNOWN", textLen); break;
        case TYPE_PLUG: strncpy(typeText, "PLUG", textLen); break;
        case TYPE_COLOR: strncpy(typeText, "COLOR", textLen); break;
        case TYPE_LIGHT: strncpy(typeText, "LIGHT", textLen); break;
        case TYPE_DIMMER: strncpy(typeText, "DIMMER", textLen); break;
        case TYPE_SWITCH: strncpy(typeText, "SWITCH", textLen); break;
        case TYPE_SPEAKER: strncpy(typeText, "SPEAKER", textLen); break;
        case TYPE_TEMPERATURE: strncpy(typeText, "TEMPERATURE", textLen); break;
        case TYPE_HUMIDITY: strncpy(typeText, "HUMIDITY", textLen); break;
        case TYPE_WARNING: strncpy(typeText, "WARNING", textLen); break;
        case TYPE_SELECTOR: strncpy(typeText, "SELECTOR", textLen); break;
        case TYPE_SWITCH_SENSOR: strncpy(typeText, "SWITCH_SENSOR", textLen); break;
        case TYPE_PERCENT_SENSOR: strncpy(typeText, "PERCENT_SENSOR", textLen); break;
        case TYPE_VALUE_SENSOR: strncpy(typeText, "VALUE_SENSOR", textLen); break;
        case TYPE_CONSUMPTION: strncpy(typeText, "CONSUMPTION", textLen); break;
        case TYPE_POWER: strncpy(typeText, "POWER", textLen); break;
        case TYPE_BLINDS: strncpy(typeText, "BLINDS", textLen); break;
        case TYPE_LUX: strncpy(typeText, "LUX", textLen); break;
        case TYPE_WEIGHT: strncpy(typeText, "WEIGHT", textLen); break;
        case TYPE_RAIN: strncpy(typeText, "RAIN", textLen); break;
        case TYPE_SETPOINT: strncpy(typeText, "SETPOINT", textLen); break;
        case TYPE_THERMOSTAT: strncpy(typeText, "THERMOSTAT", textLen); break;
        case TYPE_TEXT: strncpy(typeText, "TEXT", textLen); break;
        case TYPE_AIR_QUALITY: strncpy(typeText, "AIR_QUALITY", textLen); break;
        case TYPE_PUSH: strncpy(typeText, "PUSH", textLen); break;
        case TYPE_PAGE: strncpy(typeText, "PAGE", textLen); break;
        case TYPE_UV: strncpy(typeText, "UV", textLen); break;
        case TYPE_WIND: strncpy(typeText, "WIND", textLen); break;
        default: strncpy(typeText, "???", textLen);
    }
}

void getPanelName(int widgetPageIndex, char* pageText, size_t textLen) {
    switch (widgetPageIndex)
    {
        case TOOL_PANEL: strncpy(pageText, "TOOL", textLen); break;
        case HOMEPAGE_PANEL ... (LAST_PAGE_PANEL): strncpy(pageText, global_pages[widgetPageIndex - HOMEPAGE_PANEL].name, textLen); break;
        #ifndef NO_GROUP_PAGE
            case GROUP_PANEL: strncpy(pageText, "GROUP", textLen); break;
        #endif
        #ifndef NO_INFO_PAGE
            case INFO_PANEL: strncpy(pageText, "INFO", textLen); break;
        #endif
        case DEVICE_PANEL: strncpy(pageText, "DEVICE", textLen); break;
        case SETTING_PANEL: strncpy(pageText, "SETTING", textLen); break;
        case PASS_PANEL: strncpy(pageText, "PASS", textLen); break;
        default: strncpy(pageText, "???", textLen);
    }
}

bool isActivePanel(int widgetPageIndex) {
    return (widgetPageIndex >= HOMEPAGE_PANEL) && (widgetPageIndex <= LAST_PAGE_PANEL);
}