#include <Esp.h>
#include "conf/global_config.h"
#include "core/screen_driver.h"
#include "ui/wifi_setup.h"
#include "ui/http_setup.h"
#include "lvgl.h"
#include "core/data_setup.h"
#include "core/ip_engine.h"
#include "ui/main_ui.h"
#include "ui/navigation.h"
#include "ui/panels/panel.h"

void Websocket_loop(void);

static void event_handler(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        global_config.version = 0;
        WriteGlobalConfig();
        ESP.restart();
    }
}

static void scr_event_cb(lv_event_t * e)
{
    int p = GetActivePanel();

    if (p < (3+BONUSPAGE))
    {

        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        if (dir == LV_DIR_LEFT) p +=1;
        if (dir == LV_DIR_RIGHT) p -=1;

        if (p < 0) p = 0;
        if (p > (2+BONUSPAGE)) p = (2+BONUSPAGE);

        lv_indev_wait_release(lv_indev_get_act());

        //Serial.printf("Dir: %d\n", dir);
        //Serial.printf("Page: %d\n", p);
        
        navigation_screen(p);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting application");
    LoadGlobalConfig();
    screen_setup();
    Serial.println("Screen init done");

    //Personnal Settings to don't have to set them at every reset, need to be removed
    #if FORCE_CONFIG
        strcpy(global_config.wifiPassword, "xxxxxxxxxxxxxxxxxxx");
        strcpy(global_config.wifiSSID, "xxxxxxxxxxxxxxx");
        strcpy(global_config.ServerHost, "192.168.1.1");
        global_config.ServerPort = 8080;
        global_config.wifiConfigured = true;
        global_config.ipConfigured = true;

        const static int t[] = {37, 75, 16, 36, 28, 35, 57, 89, 45};
        for (int i=0; i<TOTAL_ICONX*TOTAL_ICONY; i++)
            global_config.ListDevices[i] = t[i];
        WriteGlobalConfig();
    #endif

#ifdef BOARD_HAS_RGB_LED

pinMode(RGB_LED_R, OUTPUT);
pinMode(RGB_LED_G, OUTPUT);
pinMode(RGB_LED_B, OUTPUT);

digitalWrite(RGB_LED_R, true);
digitalWrite(RGB_LED_G, true);
digitalWrite(RGB_LED_B, true);

#endif
    
    wifi_init(); // Wifi initialisation
    WS_init(); // Websocket initialisation
    Init_data(); // Data initialisation
    InitIPEngine(); // IP stuff

    Serial.println("Application ready");

    //Set defaut Style
    nav_style_setup();
    Init_Info_Style();
    
    main_ui_setup();

    //Set base display
    lv_obj_add_event_cb(lv_scr_act(), scr_event_cb , LV_EVENT_GESTURE , NULL);
    lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);

    //Start on Home panel
    navigation_screen(1);

}

void loop(){

    //wifi_ok(); // Watchdog
    Websocket_loop(); // Needed for websocket event.

    lv_timer_handler();
    lv_task_handler();
}
