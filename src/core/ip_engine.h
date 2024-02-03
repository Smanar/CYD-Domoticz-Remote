#include "data_setup.h"
#include <ArduinoJson.h>

bool verify_ip(void);
bool HTTPGETRequest(char * url2);
void Websocket_loop(void);
void WS_Run(void);
bool WS_Running(void);

bool HTTPGETRequestWithReturn(const char * url2, JsonArray *JS, bool filter=false);