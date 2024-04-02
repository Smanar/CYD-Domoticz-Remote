#include "lvgl.h"
#include "http_setup.h"
#include "../conf/global_config.h"
#include "../core/ip_engine.h"

static lv_obj_t * ipEntry;
static lv_obj_t * portEntry;
static lv_obj_t * label = NULL;

void WS_init_inner(void);


static void ta_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = (lv_obj_t *)lv_event_get_user_data(e);

    if(code == LV_EVENT_FOCUSED) {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    else if (code == LV_EVENT_READY) 
    {
        strcpy(global_config.ServerHost, lv_textarea_get_text(ipEntry));
        global_config.ServerPort = atoi(lv_textarea_get_text(portEntry));

        if (verify_ip())
        {
            global_config.ipConfigured = true;
            WriteGlobalConfig();

            ESP.restart();
        }
        else
        {
            lv_label_set_text_static(label, "Failed to connect");
        }
    }
}

static void reset_btn_event_handler(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        global_config.ipConfigured = false;
        WS_init_inner();
    }
}

void WS_init_inner(){
    lv_obj_clean(lv_scr_act());

    if (global_config.ipConfigured) {
        label = lv_label_create(lv_scr_act());
        lv_label_set_text_static(label, "Connecting to Domoticz");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

        lv_obj_t * resetBtn = lv_btn_create(lv_scr_act());
        lv_obj_add_event_cb(resetBtn, reset_btn_event_handler, LV_EVENT_ALL, NULL);
        lv_obj_align(resetBtn, LV_ALIGN_CENTER, 0, 40);

        lv_obj_t * btnLabel = lv_label_create(resetBtn);
        lv_label_set_text_static(btnLabel, "Reset");
        lv_obj_center(btnLabel);
        return;
    }

    lv_obj_t * keyboard = lv_keyboard_create(lv_scr_act());
    label = lv_label_create(lv_scr_act());
    lv_label_set_text_static(label, "Enter Domoticz IP and Port");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 10 + 2);

    ipEntry = lv_textarea_create(lv_scr_act());
    lv_textarea_set_one_line(ipEntry, true);
    lv_textarea_set_max_length(ipEntry, 63);
    lv_textarea_set_text(ipEntry, "");
    lv_obj_align(ipEntry, LV_ALIGN_TOP_LEFT, 10, 40);
    lv_obj_add_event_cb(ipEntry, ta_event_cb, LV_EVENT_ALL, keyboard);
    lv_obj_set_size(ipEntry, TFT_WIDTH - 20 - 100, 60);

    portEntry = lv_textarea_create(lv_scr_act());
    lv_textarea_set_one_line(portEntry, true);
    lv_textarea_set_max_length(portEntry, 5);
    lv_textarea_set_text(portEntry, "8080");
    lv_obj_align(portEntry, LV_ALIGN_TOP_LEFT, TFT_WIDTH - 20 - 80, 40);
    lv_obj_add_event_cb(portEntry, ta_event_cb, LV_EVENT_ALL, keyboard);
    lv_obj_set_size(portEntry, 90, 60);
    
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_NUMBER);
    lv_keyboard_set_textarea(keyboard, ipEntry);
}

long last_data_update_ip = -10000;
const long data_update_interval_ip = 10000;
int retry_count = 0;

void WS_init()
{

    WS_init_inner();
    WS_Run();

    while (!WS_Running())
    {
        lv_timer_handler();
        lv_task_handler();

        if (!WS_Running && global_config.ipConfigured && (millis() - last_data_update_ip) > data_update_interval_ip)
        {

            Serial.println(F("Waiting"));

            //connect_ok = verify_ip();
            //delay(1000);

            last_data_update_ip = millis();
            retry_count++;
            lv_label_set_text_fmt(label, "Connecting to Domoticz (Try %d)", retry_count);
        }

        Websocket_loop();

    }

}
