#include <HTTPClient.h>
#include <WebSocketsClient.h>
//#include <ArduinoJson.h>
#include <WebSocketsClient.h>

#include "../conf/global_config.h"
#include "ip_engine.h"

WebSocketsClient WSclient;
bool connect_ok = false;

void Update_data(JsonObject RJson2);


bool verify_ip(){
    return HTTPGETRequestWithReturn("/json.htm?type=command&param=getServerTime",NULL);
}

bool HTTPGETRequest(char * url2)
{
    return HTTPGETRequestWithReturn(url2,NULL);
}

bool HTTPGETRequestWithReturn(const char * url2, JsonArray *JS, bool NeedFilter)
{
    HTTPClient client;
    String url = "http://" + String(global_config.ServerHost) + ":" + String(global_config.ServerPort) + url2;
    int httpCode;
    try {
        Serial.println(url);
        client.setTimeout(500);
        client.begin(url);
        httpCode = client.GET();
        if (httpCode != 200)
        {
             Serial.println("Failed to connect 2");
             return false;
        }

        //if no need data return
        if (!JS) return true;

        //Some security
        double buffer = 60000;
        if (int(buffer*1.2) > ESP.getMaxAllocHeap())
        {
            //Limited by PSRAM ?
            Serial.printf("Not enought Heap memory to parse json, Available %d, needed %f\n", ESP.getMaxAllocHeap(), buffer*1.2f);
            return false;
        }

        DynamicJsonDocument doc(buffer);
        DeserializationError err;

        //Need to use filter here, domotocz is too much talkative
        // https://arduinojson.org/v6/example/filter/

        if (NeedFilter)
        {
            // The filter: it contains "true" for each value we want to keep
            StaticJsonDocument<200> filter;
            filter["result"][0]["Data"] = true;
            filter["result"][0]["idx"] = true;
            filter["result"][0]["Name"] = true;

            err = deserializeJson(doc, client.getString(), DeserializationOption::Filter(filter));

            //char buffer[4096];
            //serializeJsonPretty(doc, buffer);
            //Serial.println(buffer);
        }
        else
        {
            err = deserializeJson(doc, client.getString());
        }

        if (err)
        {
            Serial.println("Can't deserializeJson JSON\n");
            return false;
        }

        //Some debug
        //https://arduinojson.org/v6/how-to/determine-the-capacity-of-the-jsondocument/
        //Serial.printf("JSON PARSE: %s\n", a.c_str());
        //Serial.printf("xxx %d\n", JS->size());
        //Serial.printf("Heap Memory Usable (Kb) %d , Max %d, Total %d\n", ESP.getMaxAllocHeap()/1000, ESP.getFreeHeap()/1000, ESP.getHeapSize()/1000); // 45044
        //Serial.printf("RAM Memory Free (Kb) %d , Total %d\n", ESP.getFreePsram()/1000, ESP.getPsramSize()/1000); // 4 M

        *JS = doc["result"].as<JsonArray>();

        if (!*JS)
        {
            Serial.println("Json not available\n");
            return false;
        }

        return true;

    }
    catch (...) {
        Serial.println("Failed to connect");
        return false;
    }

    return false;

}



static void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
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

static void webSocketEvent(WStype_t type, uint8_t * payload, size_t length)
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
			//Serial.printf("[WSc] get text: %s\n", payload);
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