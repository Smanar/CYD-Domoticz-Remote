#include "http_setup.h"
#include "../conf/global_config.h"
#include "lvgl.h"
#include <TFT_eSPI.h>
#include <HTTPClient.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include "base64.hpp"


bool connect_ok = false;
lv_obj_t * ipEntry;
lv_obj_t * portEntry;
lv_obj_t * label = NULL;

WebSocketsClient WSclient;


void WS_init_inner(void);
void Websocket_loop(void);
void Update_data(JsonObject RJson2);


bool verify_ip(){
    HTTPClient client;
    String url = "http://" + String(global_config.ServerHost) + ":" + String(global_config.ServerPort) + "/printer/info";
    int httpCode;
    try {
        Serial.println(url);
        client.setTimeout(500);
        client.begin(url.c_str());
        httpCode = client.GET();
        return httpCode == 200;
    }
    catch (...) {
        Serial.println("Failed to connect");
        return false;
    }
}

bool HTTPGETRequest(char * url2)
{
    HTTPClient client;
    String url = "http://" + String(global_config.ServerHost) + ":" + String(global_config.ServerPort) + url2;
    int httpCode;
    try {
        Serial.println(url);
        client.setTimeout(500);
        client.begin(url.c_str());
        httpCode = client.GET();
        return httpCode == 200;
    }
    catch (...) {
        Serial.println("Failed to connect");
        return false;
    }
}

bool HttpInitDevice(Device *d, char * id){
    HTTPClient client;
    String url = "http://" + String(global_config.ServerHost) + ":" + String(global_config.ServerPort) + "/json.htm?type=command&param=getdevices&rid=" + id;
    int httpCode;
    try {
        Serial.println(url);
        client.setTimeout(500);
        client.begin(url.c_str());
        httpCode = client.GET();
        if (httpCode != 200)
        {
             Serial.println("Failed to connect 2");
             return false;
        }

        String payload = client.getString();
        DynamicJsonDocument doc(60000);
        auto a = deserializeJson(doc, payload);
        Serial.printf("JSON PARSE: %s\n", a.c_str());
        auto result = doc["result"].as<JsonArray>();
        for (auto i : result)  // Scan the array ( only 1)
        {
            d->name = (char*)malloc(strlen(i["Name"]) + 1);
            strcpy(d->name, i["Name"]);
            strncpy(d->data, i["Data"],10);
            d->ID = (char*)malloc(strlen(i["ID"]) + 1);
            strcpy(d->ID, i["ID"]);
            d->HardwareID = i["HardwareID"];
            d->idx = i["idx"];
            d->level = i["Level"];

            const char* type = i["Type"];
            const char* subtype = i["SubType"];
            const char* image = i["Image"];

            if (strcmp(type, "Light/Switch") == 0)
            {
                d->type = TYPE_SWITCH;

                bool b = i["HaveDimmer"];

                if (b)
                {
                    d->type = TYPE_DIMMER;
                }
            }
            else if (strcmp(type, "Temp") == 0)
            {
                d->type = TYPE_TEMPERATURE;
            }
            else if (strcmp(type, "General") == 0)
            {
                d->type = TYPE_WARNING;
            }

            if (strcmp(subtype,"Selector Switch") == 0)
            {

                d->type = TYPE_SELECTOR;
                const char *base64 = i["LevelNames"]; 
                d->levelname = (char*)malloc(strlen(i["LevelNames"]) + 1);

                unsigned int string_length = decode_base64((const unsigned char*)base64, (unsigned char *)d->levelname);
                d->levelname[string_length] = '\0';

                char *ptr = d->levelname;
                while (*ptr != '\0')
                {
                    if (*ptr == '|') { *ptr = '\n'; }
                    ptr++;
                }

            }

            // Correction by image
            if (image && strcmp(image,"WallSocket") == 0)
            {
                d->type = TYPE_PLUG;
            }

        }

        return true;

    }
    catch (...) {
        Serial.println("Failed to connect");
        return false;
    }

    return false;
}


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
            connect_ok = true;
        }
        else
        {
            lv_label_set_text(label, "Failed to connect");
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
        lv_label_set_text(label, "Connecting to Domoticz");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

        lv_obj_t * resetBtn = lv_btn_create(lv_scr_act());
        lv_obj_add_event_cb(resetBtn, reset_btn_event_handler, LV_EVENT_ALL, NULL);
        lv_obj_align(resetBtn, LV_ALIGN_CENTER, 0, 40);

        lv_obj_t * btnLabel = lv_label_create(resetBtn);
        lv_label_set_text(btnLabel, "Reset");
        lv_obj_center(btnLabel);
        return;
    }

    lv_obj_t * keyboard = lv_keyboard_create(lv_scr_act());
    label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Enter Domoticz IP and Port");
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 10 + 2);

    ipEntry = lv_textarea_create(lv_scr_act());
    lv_textarea_set_one_line(ipEntry, true);
    lv_textarea_set_max_length(ipEntry, 63);
    lv_textarea_set_text(ipEntry, "");
    lv_obj_align(ipEntry, LV_ALIGN_TOP_LEFT, 10, 40);
    lv_obj_add_event_cb(ipEntry, ta_event_cb, LV_EVENT_ALL, keyboard);
    lv_obj_set_size(ipEntry, TFT_HEIGHT - 20 - 100, 60);

    portEntry = lv_textarea_create(lv_scr_act());
    lv_textarea_set_one_line(portEntry, true);
    lv_textarea_set_max_length(portEntry, 5);
    lv_textarea_set_text(portEntry, "80");
    lv_obj_align(portEntry, LV_ALIGN_TOP_LEFT, TFT_HEIGHT - 20 - 80, 40);
    lv_obj_add_event_cb(portEntry, ta_event_cb, LV_EVENT_ALL, keyboard);
    lv_obj_set_size(portEntry, 90, 60);
    
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_NUMBER);
    lv_keyboard_set_textarea(keyboard, ipEntry);
}

long last_data_update_ip = -10000;
const long data_update_interval_ip = 10000;
int retry_count = 0;


void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
	const uint8_t* src = (const uint8_t*) mem;
	Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		Serial.printf("%02X ", *src);
		src++;
	}
	Serial.printf("\n");
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length)
{
	switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			Serial.printf("[WSc] Connected to url: %s\n", payload);
            connect_ok = true;

			// send message to server when Connected
			WSclient.sendTXT("Connected");
			break;
		case WStype_TEXT:
			Serial.printf("[WSc] get text: %s\n", payload);
            if (length > 0)
            {
                DynamicJsonDocument doc(4096);
                DeserializationError err = deserializeJson(doc, payload, length);

                if (!err)
                {
                    // ok it's Json
                    if (doc.containsKey("event"))
                    {
                        //if (strcmp(doc["event"], "response") == 0)
                        if ((doc["event"] == "response") && (doc.containsKey("data")))
                        {
                            //On the request data is a string so need to be deserialized too
                            //StaticJsonDocument<1024> doc2;
                            DynamicJsonDocument doc2(4096);
                            deserializeJson(doc2, doc["data"].as<const char*>());
                            //JsonObject RJson = doc2.as<JsonObject>();

                            //char buffer[4096];
                            //serializeJsonPretty(doc2, buffer);
                            //Serial.println(buffer);

                           //for (JsonPair keyValue : RJson) {
                           //   Serial.println(keyValue.key().c_str());
                           //}

                            JsonArray result = doc2.as<JsonObject>()["result"];
                            if (result)
                            {

                                for (JsonObject a : result) {
                                    Update_data(a);
                                }


                            }

                        }
                    }
                }
            }

			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			Serial.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:
            Serial.printf("[WSc] error: %u\n", length);
            break;	
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}
}



void WS_init(){
    connect_ok = false;

    WS_init_inner();

    Serial.println("Connecting to Webserver");

    // server address, port and URL
    WSclient.begin(global_config.ServerHost, global_config.ServerPort, "/json", "domoticz");

    // event handler
    WSclient.onEvent(webSocketEvent);

    // use HTTP Basic Authorization this is optional remove if not needed
    //WSclient.setAuthorization("user", "Password");

    // try ever 5000 again if connection has failed
    WSclient.setReconnectInterval(5000);


    while (!connect_ok)
    {
        lv_timer_handler();
        lv_task_handler();

        if (!connect_ok && global_config.ipConfigured && (millis() - last_data_update_ip) > data_update_interval_ip)
        {

            Serial.println("Waiting");

            //connect_ok = verify_ip();
            //delay(1000);

            last_data_update_ip = millis();
            retry_count++;
            String retry_count_text = "Connecting to Domoticz (Try " + String(retry_count + 1) + ")";
            lv_label_set_text(label, retry_count_text.c_str());
        }

        Websocket_loop();

    }

}

void Websocket_loop(void)
{
    WSclient.loop();
}