#include "data_setup.h"
#include <ArduinoJson.h>

bool verify_ip(void);
bool HTTPGETRequest(char * url2);
void Websocket_loop(void);
void WS_Run(void);
bool WS_Running(void);
void InitIPEngine(void);
unsigned long total_WS_lenght(void);

bool HTTPGETRequestWithReturn(const char * url2, JsonDocument *doc, bool filter=false);
void subscribedeviceWS(short r, const char *c);