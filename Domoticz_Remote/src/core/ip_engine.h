#include "data_setup.h"

bool verify_ip(void);
bool HTTPGETRequest(char * url2);
bool HttpInitDevice(Device *d, const char * id);
void Websocket_loop(void);
void WS_Run(void);
bool WS_Running(void);
