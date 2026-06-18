#include <HTTPClient.h>
#include <WebSocketsClient.h>

#include "../conf/global_config.h"
#include "ip_engine.h"

static WebSocketsClient WSclient;
static bool connect_ok = false;
unsigned long total_data_lengh;

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

    filter["result"][0]["Level"] = true;
    filter["result"][0]["Rain"] = true;
    filter["result"][0]["Type"] = true;

    total_data_lengh = 0;
}

bool verify_ip(){
    return HTTPGETRequestWithReturn("/json.htm?type=command&param=getServerTime",NULL);
}

bool HTTPGETRequest(const char * url2)
{
    return HTTPGETRequestWithReturn(url2,NULL);
}

// IMPORTANT https://arduinojson.org/v6/how-to/use-arduinojson-with-httpclient/#how-to-parse-a-json-document-from-an-http-response
// prevent issue DeserializationError::EmptyInput
//https://github.com/bblanchon/ArduinoJson/issues/1705
bool HTTPGETRequestWithReturn(const char * url2, JsonDocument *doc, bool NeedFilter)
{

    HTTPClient client;
    int httpCode;
    static char tmpBuffer[180];    // As routine is asynchronous, use only local data

    lv_snprintf(tmpBuffer, sizeof(tmpBuffer), "http://%s:%d%s",global_config.ServerHost, global_config.ServerPort, url2);
    //String url = "http://" + String(global_config.ServerHost) + ":" + String(global_config.ServerPort) + url2;

    try {
        Serial.println(tmpBuffer);
        client.useHTTP10(true); // Unfortunately, by using the underlying Stream, we bypass the code that handles chunked transfer encoding, so we must switch to HTTP version 1.0.
        client.setTimeout(1000);
        client.begin(tmpBuffer);
        httpCode = client.GET();
        if (httpCode != 200)
        {
             Serial.println(F("Failed to connect 2"));
             client.end();
             return false;
        }

        //if no need data return
        if (!doc)
        {
            client.end();
            return true;
        }

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

            client.end();
            return false;
        }

        client.end();

        //Serial.printf("Can't deserializeJson JSON : %s\n",err.c_str());
        //Serial.printf("content Length : %d\n", client.getSize());
        //Serial.printf("Free memory : %d\n", ESP.getMaxAllocHeap());

        // Some debug
        //char buffer2[4096];
        //serializeJsonPretty(doc, buffer2, sizeof(buffer2));
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
    if (*c) {
        char localBuffer[512];

        if (r == 0)
        {
            lv_snprintf(localBuffer, sizeof(localBuffer),"{\"event\":\"request\",\"query\":\"type=command&param=getdevices&rid=%s\"}", c);
        }
        else
        {
            lv_snprintf(localBuffer, sizeof(localBuffer), "{\"event\":\"request\",\"query\":\"type=command&param=%s\"}", c);
        }

        if (strlen(localBuffer) >= sizeof(localBuffer) - 1)
        {
            Serial.println(F("[WS] subscribedeviceWS: buffer too small !"));
        }

        Serial.printf("Special Setting to WS: %s\n", localBuffer);
        WSclient.sendTXT(localBuffer);

    }
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
                #if (0)
                // Print just received message (debug)
                char payloadText[length+1];
                strncpy(payloadText, (const char*) payload, sizeof(payloadText));
                Serial.printf("WS text %s\n", payloadText);
                #endif
                total_data_lengh += length;

                JsonDocument doc;
                doc.clear();

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

                            DeserializationError err2 = deserializeJson(doc2, doc["data"].as<const char*>());
                            if (err2) {
                                Serial.printf("[WSc] Can't deserialize doc2: %s\n", err2.c_str());
                                break;
                            }
                            //JsonObject RJson = doc2.as<JsonObject>();

                            //char buffer[4096];
                            //serializeJsonPretty(doc2, buffer, sizeof(buffer));
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

    if (WSclient.isConnected())
    {
        WSclient.disconnect();
    }

    Serial.printf("Connecting to %s:%d\n",global_config.ServerHost, global_config.ServerPort);

    // server address, port and URL
    WSclient.begin(global_config.ServerHost, global_config.ServerPort, "/json", "domoticz");
    //WSclient.beginSSL(global_config.ServerHost, global_config.ServerPort, "/json", "", "domoticz");

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
    return total_data_lengh/1024;
}
