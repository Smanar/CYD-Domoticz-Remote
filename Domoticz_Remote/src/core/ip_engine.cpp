#include <HTTPClient.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

#include "base64.hpp"
#include "../conf/global_config.h"
#include "ip_engine.h"

WebSocketsClient WSclient;
bool connect_ok = false;

void Update_data(JsonObject RJson2);

bool verify_ip(){
    HTTPClient client;
    String url = "http://" + String(global_config.ServerHost) + ":" + String(global_config.ServerPort) + "/info";
    int httpCode;
    try {
        Serial.println(url);
        client.setTimeout(500);
        client.begin(url.c_str());
        httpCode = client.GET();
        if  (httpCode == 200)
        {
            connect_ok = true;
            return true;
        }
    }
    catch (...) {
        Serial.println("Failed to connect");
        return false;
    }

    return false;
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

bool HttpInitDevice(Device *d, const char * id)
{
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

        if (!result) return false;

        for (auto i : result)  // Scan the array ( only 1)
        {
            d->name = (char*)malloc(strlen(i["Name"]) + 1);
            strcpy(d->name, i["Name"]);

            const char* JSondata = NULL;
            JSondata = i["Data"];
            //do some cleaning
            if (strncmp(JSondata, "Humidity ", 9) == 0) JSondata+=9;
            strncpy(d->data, JSondata,10);

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
            else if (strcmp(type, "Color Switch") == 0)
            {
                d->type = TYPE_COLOR;
            }
            else if (strcmp(type, "Temp") == 0)
            {
                d->type = TYPE_TEMPERATURE;
            }
            else if (strcmp(type, "Humidity") == 0)
            {
                d->type = TYPE_HUMIDITY;
            }
            else if (strcmp(type, "Usage") == 0)
            {
                d->type = TYPE_CONSUMPTION;
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
            if (image && strcmp(image,"Speaker") == 0)
            {
                d->type = TYPE_SPEAKER;
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
	switch(type)
    {
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



bool WS_Running(void)
{
    return connect_ok;
}


void WS_Run(void)
{
    connect_ok = false;

    Serial.println("Connecting to Webserver");

    // server address, port and URL
    WSclient.begin(global_config.ServerHost, global_config.ServerPort, "/json", "domoticz");

    // event handler
    WSclient.onEvent(webSocketEvent);

    // use HTTP Basic Authorization this is optional remove if not needed
    //WSclient.setAuthorization("user", "Password");

    // try ever 5000 again if connection has failed
    WSclient.setReconnectInterval(5000);
}

void Websocket_loop(void)
{
    WSclient.loop();
}