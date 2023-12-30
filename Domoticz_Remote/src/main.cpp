#include "conf/global_config.h"
#include "core/screen_driver.h"
#include "ui/wifi_setup.h"
#include "ui/http_setup.h"
#include "lvgl.h"
#include "core/data_setup.h"
#include "ui/main_ui.h"
#include "ui/nav_buttons.h"

void wifi_init_inner();
bool CheckJson();
void Websocket_loop(void);

static void event_handler(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        global_config.version = 0;
        WriteGlobalConfig();
        ESP.restart();
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting application");
    LoadGlobalConfig();
    screen_setup();
    Serial.println("Screen init done");

    //Personnal Settings
    strcpy(global_config.wifiPassword, "xxxxxxxxxxxxxxxx");
    strcpy(global_config.wifiSSID, "xxxxxxxxxxxxxxx");
    strcpy(global_config.ServerHost, "192.168.1.1");
    global_config.ServerPort = 8080;
    global_config.wifiConfigured = true;
    global_config.ipConfigured = true;
    global_config.invertColors = true;
    WriteGlobalConfig();
    
    wifi_init(); // Wifi initialisation
    WS_init(); // Websocket initialisation
    Init_data(); // Data initialisation

    Serial.println("Application ready");


    nav_style_setup();
    main_ui_setup();

    //Start on Home panel
    nav_buttons_setup(0);

}

void loop(){

    //wifi_ok(); // Watchdog
    Websocket_loop(); // Needed for websocket event.

    lv_timer_handler();
    lv_task_handler();
}