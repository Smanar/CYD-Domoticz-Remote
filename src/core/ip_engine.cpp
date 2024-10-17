#include <HTTPClient.h>
#include <WebSocketsClient.h>

#include "../conf/global_config.h"
#include "ip_engine.h"

static WebSocketsClient WSclient;
static bool connect_ok = false;
unsigned long total_data_lenght;

extern char TmpBuffer[];

void Update_device_data(JsonObject RJson2);
void Update_scene_data(void);

//Filter json
JsonDocument filter;
void InitIPEngine(void)
{
    // The filter: it contains "true" for each value we want to keep
    filter["result"][0]["Data"] = true;
    filter["result"][0]["idx"] = true;
    filter["result"][0]["Name"] = true;

    total_data_lenght = 0;
}

bool verify_ip(){
    return HTTPGETRequestWithReturn("/json.htm?type=command&param=getServerTime",NULL);
}

bool HTTPGETRequest(char * url2)
{
    return HTTPGETRequestWithReturn(url2,NULL);
}

// IMPORTANT https://arduinojson.org/v6/how-to/use-arduinojson-with-httpclient/#how-to-parse-a-json-document-from-an-http-response
// prevent issue DeserializationError::EmptyInput
//https://github.com/bblanchon/ArduinoJson/issues/1705
bool HTTPGETRequestWithReturn(const char * url2, JsonDocument *doc, bool NeedFilter)
{
    HTTPClient client;
    lv_snprintf(TmpBuffer, 150, "http://%s:%d%s",global_config.ServerHost, global_config.ServerPort, url2);
    //String url = "http://" + String(global_config.ServerHost) + ":" + String(global_config.ServerPort) + url2;
    int httpCode;
    try {
        Serial.println(TmpBuffer);
        client.useHTTP10(true); // Unfortunately, by using the underlying Stream, we bypass the code that handles chunked transfer encoding, so we must switch to HTTP version 1.0.
        client.setTimeout(1000);
        client.begin(TmpBuffer);
        httpCode = client.GET();
        if (httpCode != 200)
        {
             Serial.println(F("Failed to connect 2"));
             return false;
        }

        //if no need data return
        if (!doc) return true;

        DeserializationError err;

        //Need to use filter here, domotocz is too much talkative
        // https://arduinojson.org/v6/example/filter/

        if (NeedFilter)
        {
            err = deserializeJson(*doc, client.getStream(), DeserializationOption::Filter(filter));
        }
        else
        {
            //err = deserializeJson(doc, client.getString());
            err = deserializeJson(*doc, client.getStream());
        }

        if (err)
        {
            //Serial.printf("JSON PARSE: %s\n", client.getString());
            Serial.printf("Can't deserializeJson JSON : %s\n",err.c_str());
            Serial.printf("content Length : %d\n", client.getSize());
            Serial.printf("Free memory : %d\n", ESP.getMaxAllocHeap());
            return false;
        }

        //Serial.printf("Can't deserializeJson JSON : %s\n",err.c_str());
        //Serial.printf("content Length : %d\n", client.getSize());
        //Serial.printf("Free memory : %d\n", ESP.getMaxAllocHeap());

        // Some debug
        //char buffer2[4096];
        //serializeJsonPretty(doc, buffer2);
        //Serial.println(buffer2);

        //Some debug
        //https://arduinojson.org/v6/how-to/determine-the-capacity-of-the-jsondocument/
        //Serial.printf("JSON PARSE: %s\n", a.c_str());
        //Serial.printf("xxx %d\n", JS->size());
        //Serial.printf("Heap Memory Usable (Kb) %d , Max %d, Total %d\n", ESP.getMaxAllocHeap()/1000, ESP.getFreeHeap()/1000, ESP.getHeapSize()/1000); // 45044
        //Serial.printf("RAM Memory Free (Kb) %d , Total %d\n", ESP.getFreePsram()/1000, ESP.getPsramSize()/1000); // 4 M

        return true;

    }
    catch (...) {
        Serial.println(F("Failed to connect"));
        return false;
    }

    return false;

}


#if 0
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
#endif

// Send special configuration
// On recent Domoticz version it's possible to send a special request to receive Websocket notification
// For only somes devices
// WSclient.sendTXT("{\"event\":\"request\",\"query\":\"type=command&param=getdevices&rid=1,2,3,4,5\"}");
// Use getscenes for scene tab
//
void subscribedeviceWS(short r, const char *c)
{
    if (r == 0)
    {
        lv_snprintf(TmpBuffer,200,"{\"event\":\"request\",\"query\":\"type=command&param=getdevices&rid=");
        lv_snprintf(TmpBuffer,200, "%s%s", TmpBuffer, c);
    }
    else
    {
        lv_snprintf(TmpBuffer,200,"{\"event\":\"request\",\"query\":\"type=command&param=");
        lv_snprintf(TmpBuffer,200, "%s%s", TmpBuffer, c);
    }
    lv_snprintf(TmpBuffer,200, "%s%s", TmpBuffer, "\"}");
    Serial.printf("Special Setting to WS: %s\n", TmpBuffer);
    WSclient.sendTXT(TmpBuffer);
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

                total_data_lenght += length;

                JsonDocument doc;

                DeserializationError err = deserializeJson(doc, payload, length);

                if (!err)
                {
                    // ok it's Json
                    //if (doc.containsKey("event") && (doc["event"] == "response"))
                    if (doc["event"].is<const char*>() && (doc["event"] == "response"))
                    {
                        //if (doc.containsKey("data"))
                        if (doc["data"].is<const char*>())
                        {
                            //On the request data is a string so need to be deserialized too
                            JsonDocument doc2;
                            deserializeJson(doc2, doc["data"].as<const char*>());
                            //JsonObject RJson = doc2.as<JsonObject>();

                            //char buffer[4096];
                            //serializeJsonPretty(doc2, buffer);
                            //Serial.println(buffer);

                           //for (JsonPair keyValue : RJson) {
                           //   Serial.println(keyValue.key().c_str());
                           //}

                            JsonArray result = doc2["result"];
                            if (result)
                            {
                                for (JsonObject a : result) {

                                    // Scene or group WS
                                    if (doc["request"] == "scene_request")
                                    {
                                        Serial.printf("[WSc] Scene/group WS\n");
                                        if (a["Type"] == "Group")
                                        {
                                            Serial.printf("[WSc] Group WS\n");
                                            Update_scene_data();
                                        }
                                    }
                                    // Device WS
                                    else
                                    {
                                        Update_device_data(a);
                                    }
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
			//hexdump(payload, length);

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

    Serial.println(F("Connecting to Webserver"));
    //Serial.printf("Connecting to %s%s:%d\n",global_config.ServerHost, "/json", global_config.ServerPort);

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

unsigned long total_WS_lenght(void)
{
    return total_data_lenght/1024;
}