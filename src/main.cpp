#include <Esp.h>

#include "conf/global_config.h"
#include "conf/json_config.h"
#include "core/screen_driver.h"
#include "core/ota.h"
#include "ui/wifi_setup.h"
#include "ui/http_setup.h"
#include "lvgl.h"
#include "core/data_setup.h"
#include "core/ip_engine.h"
#include "ui/main_ui.h"
#include "ui/navigation.h"
#include "ui/panels/panel.h"
#include "LittleFS.h"
//#include "core/sound.h"

extern int currentPage;

unsigned long now;
void Websocket_loop(void);

static void scr_event_cb(lv_event_t * e)
{
    int p = GetActivePanel();

    if (p < MAX_PANEL_SCROLL)
    {

        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        if (dir == LV_DIR_LEFT) {
            if (p == HOMEPAGE_PANEL) {
                if (currentPage < PAGES - 1) {
                    currentPage +=1;
                    Init_data();
                    RefreshHomePage();                                  // Reload page as widget changed
                } else {
                    p +=1;
                }
            } else {
                p +=1;
            }
        }
        if (dir == LV_DIR_RIGHT) {
            if (p == HOMEPAGE_PANEL) {
                if (currentPage > 0) {
                    currentPage -=1;
                    Init_data();
                    RefreshHomePage();                                  // Reload page as widget changed
                } else {
                    p -=1;
                }
            } else {
                p -=1;
            }
        }

        if (p < 0) p = 0;
        if (p >= MAX_PANEL_SCROLL) p = MAX_PANEL_SCROLL - 1;

        lv_indev_wait_release(lv_indev_get_act());

        //Serial.printf("Dir: %d, page: %d, currentPage: %d\n", dir, p, currentPage);
        
        navigation_screen(p);
    }
}

void setup() {

    now = millis();

    Serial.begin(115200);
    delay(500);    // add a delay to be sure the serial is ready, while(!Serial) has, to my knowledge. no effect on a nodeMcu
    Serial.println(F("Starting application"));
    LoadGlobalConfig(); // Loading setting
    screen_setup(); // Set display
    Serial.println(F("Screen init done"));
    bool lastStatus;
    lastStatus = LittleFS.begin();
    if (!lastStatus) {
        Serial.println(F("Formattiong LittleFS!"));
        lastStatus = LittleFS.begin(true);
    }
    size_t usedBytes = LittleFS.usedBytes();
    size_t totalBytes = LittleFS.totalBytes();
    size_t remainingBytes = totalBytes - usedBytes;
    Serial.printf("LittleFS %s, %d kB free (%d%%)\n", lastStatus?"ok":"*BAD*", remainingBytes / 1024, (100 * remainingBytes) / totalBytes);

    // List content of LittleFS
    #ifdef DUMP_FILE_SYSTEM
        String path = "/";
        File dir = LittleFS.open(path);
        Serial.println("Files saved:");
        File entry = dir.openNextFile();
        short fileCount = 0;
        while(entry){
            String fileName = String(entry.name());
            fileName = path + fileName;
            Serial.printf("\t%s\n", fileName.c_str());
            fileCount++;
            entry = dir.openNextFile();
        }
        Serial.printf("\t%d file%s found\n", fileCount, fileCount != 1?"s":"");
        dir.close();
    #endif

    
    // Personal Settings to don't have to set them at every reset.
    // They are saved after have been set, so the flag FORCE_CONFIG can be disabled after
    #if FORCE_CONFIG

        #if __has_include("personal_settings.h")
            #include "personal_settings.h"

            strcpy(global_config.wifiPassword, WIFIPASSWORD);
            strcpy(global_config.wifiSSID, WIFISSID);
            strcpy(global_config.ServerHost, SERVERHOST);
            global_config.ServerPort = SERVERPORT;
            const static short t[] = DEVICELIST;

            strcpy(global_config.wifiPassword, "xxxxxxxxxxxxxxxxxxx");
            strcpy(global_config.wifiSSID, "xxxxxxxxxxxxxxx");
            strcpy(global_config.ServerHost, "192.168.1.1");
            global_config.ServerPort = 8080;
            const static short t[] = {122, 75, 16, 36, 28, -2, 63, 90, 145};
            const static short t2[] = {12, 75, 16, 36, 28, -2, 63, 0, 0};
        #endif

        global_config.wifiConfigured = true;
        global_config.ipConfigured = true;

        short v;

        for (uint i=0; i<TOTAL_ICONX*TOTAL_ICONY; i++)
        {
            if (i < sizeof(t) / sizeof(t[0])) { v = t[i]; } else { v = 0; }
            global_pages[0].ListDevices[i] = v;
        }

        // To test
        for (uint i=0; i<TOTAL_ICONX*TOTAL_ICONY; i++)
        {
            if (i < sizeof(t) / sizeof(t[0])) { v = t2[i]; } else { v = 0; }
            global_pages[1].ListDevices[i] = v;
        }

        WriteGlobalConfig();
    #endif

//Disable LED
#ifdef BOARD_HAS_RGB_LED
pinMode(RGB_LED_R, OUTPUT);
pinMode(RGB_LED_G, OUTPUT);
pinMode(RGB_LED_B, OUTPUT);

digitalWrite(RGB_LED_R, true);
digitalWrite(RGB_LED_G, true);
digitalWrite(RGB_LED_B, true);
#endif

#if defined (BOARD_HAS_CDS)
analogReadMilliVolts(CDS);
#endif



//Enable light sensor
#if defined (BOARD_HAS_CDS) && defined (AUTO_BRIGHTNESS)
pinMode(CDS, INPUT);
analogSetAttenuation(ADC_0db); // 0dB(1.0x) 0~800mV
//analogSetPinAttenuation(CDS, ADC_0db); // 0dB(1.0) 0~800mV
#endif

    //Some settings log
    Serial.printf("Brightness value: %d\n", global_config.brightness);
    
    InitIPEngine(); // IP stuff
    wifi_init(); // Wifi initialisation
    WS_init(); // Websocket initialisation
    Init_data(); // Data initialisation

#ifdef PUSHOTA
    //Webserver for OTA
    OTA_init();
#endif

    //make_sound(500,500);
    dumpConfig();

    Serial.println(F("Application ready"));

    //Set defaut Style
    nav_style_setup();
    Init_Info_Style();

    main_ui_setup();

    //Set base display
    lv_obj_add_event_cb(lv_scr_act(), scr_event_cb , LV_EVENT_GESTURE , NULL);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);

    //Start on Home panel
    navigation_screen(HOMEPAGE_PANEL);

}

void loop(){

    //wifi_ok(); // Watchdog
    Websocket_loop(); // Needed for websocket event.
#ifdef PUSHOTA
    //Webserver for OTA
    OTA_loop();
#endif

    //sound_loop();

    lv_timer_handler();
    lv_task_handler();

}

unsigned long runningTime(void)
{
    return (millis() - now) / 1000;
}
