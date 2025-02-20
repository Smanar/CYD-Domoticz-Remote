#include "lvgl.h"
#include "WiFi.h"
#include "wifi_setup.h"
#include "../conf/global_config.h"

void wifi_init_inner();

static void refresh_btn_event_handler(lv_event_t * e)
{
    wifi_init_inner();
}

static void reset_btn_event_handler(lv_event_t * e)
{
    global_config.ipConfigured = false;
    ESP.restart();
}

static void ta_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);

    if (code == LV_EVENT_READY) 
    {
        const char * txt = lv_textarea_get_text(ta);
        int len = strlen(txt);
        if (len > 0)
        {
            global_config.wifiConfigured = true;
            strcpy(global_config.wifiPassword, txt);
            //Serial.println(txt); // Don't print pass on log
            WriteGlobalConfig();
        }
    }
    else if (code == LV_EVENT_CANCEL)
    {
        wifi_init_inner();
    }
}

void wifi_pass_entry(const char* ssid){
    lv_obj_clean(lv_scr_act());

    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text_static(label, "Enter WiFi Password");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 10 + 2);

    lv_obj_t * passEntry = lv_textarea_create(lv_scr_act());
    lv_textarea_set_one_line(passEntry, true);
    lv_textarea_set_text(passEntry, "");
    lv_obj_align(passEntry, LV_ALIGN_TOP_LEFT, 10, 40);
    lv_obj_add_event_cb(passEntry, ta_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_size(passEntry, TFT_WIDTH - 20, 60);

    lv_obj_t * keyboard = lv_keyboard_create(lv_scr_act());
    lv_keyboard_set_textarea(keyboard, passEntry);
}

static void wifi_btn_event_handler(lv_event_t * e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_CLICKED) {
        delay(100);
        lv_obj_t * list_btn = (lv_obj_t *)lv_event_get_user_data(e);
        strcpy(global_config.wifiSSID, lv_list_get_btn_text(obj, list_btn));
        Serial.println(global_config.wifiSSID);
        wifi_pass_entry(lv_list_get_btn_text(obj, list_btn));
    }
}

void wifi_Connecting_screen(void)
{
    lv_obj_clean(lv_scr_act());

    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text_static(label, "Connecting to WiFi");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t * resetBtn = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(resetBtn, reset_btn_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_align(resetBtn, LV_ALIGN_CENTER, 0, 40);

    label = lv_label_create(resetBtn);
    lv_label_set_text_static(label, "Reset");
    lv_obj_center(label);
}

void wifi_init_inner(){

    lv_obj_clean(lv_scr_act());

    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text_static(label, "Scanning for networks...");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_timer_handler();
    lv_task_handler();
    lv_refr_now(NULL);

    lv_obj_clean(lv_scr_act());

    lv_obj_t * refreshBtn = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(refreshBtn, refresh_btn_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_align(refreshBtn, LV_ALIGN_TOP_RIGHT, -5, 5 - 1);

    label = lv_label_create(refreshBtn);
    lv_label_set_text(label, LV_SYMBOL_REFRESH);
    lv_obj_center(label);

    label = lv_label_create(lv_scr_act());
    lv_label_set_text_static(label, "Select a network");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 10 + 2);

    lv_obj_t * list = lv_list_create(lv_scr_act());
    lv_obj_align(list, LV_ALIGN_TOP_LEFT, 10, 40);
    lv_obj_set_size(list, TFT_WIDTH - 20, TFT_HEIGHT - 40 - 5);

    int n = WiFi.scanNetworks();

    for (int i = 0; i < n; ++i) {
        String ssid = WiFi.SSID(i);
        char* ssid_copy = (char*)malloc(ssid.length() + 1);
        int j = 0;

        for (; j < ssid.length(); ++j){
            if (ssid[j] == '\0')
                continue;

            ssid_copy[j] = ssid[j];
        }

        ssid_copy[j] = '\0';

        lv_obj_t * btn = lv_list_add_btn(list, LV_SYMBOL_WIFI, ssid_copy);
        lv_obj_add_event_cb(btn, wifi_btn_event_handler, LV_EVENT_ALL, (void*)btn);
        free(ssid_copy);
    }
}

const char* errs[] = {
    "Idle",
    "No SSID Available",
    "Scan Completed",
    "Connected",
    "Connection Failed",
    "Connection Lost",
    "Disconnected"
};

const int print_freq = 1000;
int print_timer = 0;

void wifi_init()
{

    if (!global_config.wifiConfigured)
    {
        wifi_init_inner();
        while (!global_config.wifiConfigured)
        {
            lv_timer_handler();
            lv_task_handler();
        }
    }

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    wifi_Connecting_screen();


    // Special config ?
    #if __has_include("../personnal_settings.h")
        #include "../personnal_settings.h"

        #ifdef USE_FIXEDIP
            IPAddress staticIP(ST_IP);
            IPAddress gateway(ST_GATEWAY);
            IPAddress subnet(ST_NETMASK);
            WiFi.hostname(WIFI_HOSTNAME);
            #if !defined(ST_DNS1)
                WiFi.config(staticIP, gateway, subnet);
            #else
                IPAddress dns1(ST_DNS1);
                #if !defined(ST_DNS2)
                    WiFi.config(staticIP, gateway, subnet, dns1);
                #else
                    IPAddress dns2(ST_DNS2);
                    WiFi.config(staticIP, gateway, subnet, dns1, dns2);
                #endif
            #endif
        #endif
    #endif

    Serial.print(F("Wifi connecting to SSID: "));
    Serial.println(global_config.wifiSSID);
    WiFi.begin(global_config.wifiSSID, global_config.wifiPassword);

    while (WiFi.status() != WL_CONNECTED)
    {

        if (millis() - print_timer > print_freq){
            print_timer = millis();
            Serial.printf("WiFi Status: %s\n", errs[WiFi.status()]);
        }
        
        lv_timer_handler();
        lv_task_handler();
    }
    Serial.println(F("Wifi connected"));
}

void wifi_ok(){
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("Wifi disconnection, restart"));
        ESP.restart();
    }
}

void wifi_stop(void)
{
    WiFi.disconnect();
    //wdt_reset();
}